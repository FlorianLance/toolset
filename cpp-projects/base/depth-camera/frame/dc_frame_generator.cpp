

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "dc_frame_generator.hpp"

// std
#include <execution>

// kinect
#include "k4a/k4a.hpp"

// orbbec
#include "libobsensor/ObSensor.hpp"

// local
// # utility
#include "utility/types.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"
// # data
#include "data/jpeg_encoding.hpp"
#include "data/fastpfor_encoding.hpp"

#include "thirdparty/taskflow/taskflow.hpp"

#include "dc_frame_indices.hpp"


using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

struct DCFrameGenerator::Impl{
    
    // parameters
    bool generateColorRGB = false;
    bool generateDepthRGB = false;
    bool generateInfraRGB = false;
    bool generateBodiesGray = false;

    // decoders
    data::JpegDecoder colorDecoder;
    data::JpegDecoder jpegDepthSizedColorDecoder;
    data::JpegDecoder bodiesIdDecoder;
    data::FastPForDecoder depthDecoder;
    data::FastPForDecoder infraDecoder;
    data::FastPForDecoder cloudDecoder;

    // processing data
    DCModeInfos mInfos;
    DCFrameIndices indices;
    std::uint8_t lastidDevice = 0;
    DCMode lastMode = DCMode::Invalid;
    std::unique_ptr<k4a::transformation> k4aTransformation = nullptr;
    Buffer<geo::Pt3<std::int16_t>> pointCloud;
    tf::Taskflow taskFlow;
    tf::Executor executor;


    // local task data
    DCDataFrame *dFrame = nullptr;
    DCFrame *frame = nullptr;
    DCFrameGenerationSettings gSettings;

    Impl(){

        auto resetCalibrationT          = taskFlow.emplace([&](){reset_calibration();}).name("reset_calibration");
        auto cRgbaColorimageT           = taskFlow.emplace([&](){compute_color_image();}).name("compute_color_image");
        auto cRgbaDepthSizedColorImageT = taskFlow.emplace([&](){compute_depth_sized_color_image();}).name("compute_depth_sized_color_image");
        auto cGrayBodiesIdImageT        = taskFlow.emplace([&](){compute_bodies_id_map_image();}).name("compute_bodies_id_map_image");
        auto cDepthDataT                = taskFlow.emplace([&](){compute_depth_data();}).name("compute_depth_data");
        auto cDepthImageT               = taskFlow.emplace([&](){compute_depth_image();}).name("compute_depth_image");
        auto cInfraDataT                = taskFlow.emplace([&](){compute_infra_data();}).name("compute_infra_data");
        auto cInfraImageT               = taskFlow.emplace([&](){compute_infra_image();}).name("compute_infra_image");
        auto cCloudT                    = taskFlow.emplace([&](){
            compute_cloud_from_depth();
            // if(gSettings.cloudGenMode == CloudGenerationMode::FromDepth){

            // }else{
            //     // ...
            // }
        }).name("compute_cloud");
        auto cCreateColoredCloudT       = taskFlow.emplace([&](){
            create_colored_cloud();
            // if(gSettings.cloudGenMode == CloudGenerationMode::FromDepth){

            // }else{
            //     // ...
            // }
        }).name("create_colored_cloud");

        resetCalibrationT.priority(tf::TaskPriority::HIGH);
        cDepthDataT.priority(tf::TaskPriority::HIGH);
        cRgbaDepthSizedColorImageT.priority(tf::TaskPriority::HIGH);

        cCloudT.priority(tf::TaskPriority::NORMAL);
        cCreateColoredCloudT.priority(tf::TaskPriority::NORMAL);

        cRgbaColorimageT.priority(tf::TaskPriority::LOW);
        cGrayBodiesIdImageT.priority(tf::TaskPriority::LOW);
        cDepthImageT.priority(tf::TaskPriority::LOW);
        cInfraDataT.priority(tf::TaskPriority::LOW);
        cInfraImageT.priority(tf::TaskPriority::LOW);

        // pipeline
        cCloudT.succeed(resetCalibrationT, cDepthDataT);
        cCreateColoredCloudT.succeed(cCloudT, cRgbaDepthSizedColorImageT);

        cDepthImageT.succeed(cDepthDataT);
        cInfraImageT.succeed(cInfraDataT);
    }

    auto initialize(DCDataFrame *initCFrame) -> void{
        mInfos.initialize(initCFrame->mode);
        indices.initialize(mInfos);
        dFrame = initCFrame;
        DCFrame initFrame;
        frame = &initFrame;
        reset_calibration();
    }

    auto reset_timings() -> void{
        tResetCalibration = {};
        tComputeColorImage = {};
        tComputeDephtSizedColorImage = {};
        tComputeBodiesIdMapImage = {};
        tComputeDepthData = {};
        tComputeDepthImage = {};
        tComputeInfraData = {};
        tComputeInfraImage = {};
        tComputeCloud = {};
        tComputeColoredCloud = {};
        tTotal = {};
    }

    auto display_timings() -> void{

        std::cout << "TIMES: "
                  << "rc [" << tResetCalibration.count() << "] "
                  << "cci [" << tComputeColorImage.count() << "] "
                  << "cdsci [" << tComputeDephtSizedColorImage.count() << "] "
                  << "cbimi [" << tComputeBodiesIdMapImage.count() << "] "
                  << "cdd [" << tComputeDepthData.count() << "] "
                  << "cdi [" << tComputeDepthImage.count() << "] "
                  << "cid [" << tComputeInfraData.count() << "] "
                  << "cii [" << tComputeInfraImage.count() << "] "
                  << "cc [" << tComputeCloud.count() << "] "
                  << "ccc [" << tComputeColoredCloud.count() << "] "
                  << "tot [" << tTotal.count() << "]\n";
    }

    auto do_work_task(const DCFrameGenerationSettings &gSettings, DCDataFrame *dFrame, DCFrame &frame) -> void{

        // frame
        frame.idCapture         = dFrame->idCapture;
        frame.afterCaptureTS    = dFrame->afterCaptureTS;
        frame.receivedTS        = dFrame->receivedTS;

        // info
        frame.mode              = dFrame->mode;

        // set data
        this->gSettings         = gSettings;
        this->dFrame            = dFrame;
        this->frame             = &frame;

        // run task
        auto tStart = Time::nanoseconds_since_epoch();

        if(mInfos.mode() != dFrame->mode){
            mInfos.initialize(dFrame->mode);
            indices.initialize(mInfos);
        }

        // start computing
        executor.run(taskFlow).wait();

        tTotal = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto do_work(const DCFrameGenerationSettings &gSettings, DCDataFrame *dFrame, DCFrame &frame) -> void{

        // frame
        frame.idCapture         = dFrame->idCapture;
        frame.afterCaptureTS    = dFrame->afterCaptureTS;
        frame.receivedTS        = dFrame->receivedTS;

        // info
        frame.mode              = dFrame->mode;

        // set data
        this->gSettings         = gSettings;
        this->dFrame            = dFrame;
        this->frame             = &frame;

        auto tStart = Time::nanoseconds_since_epoch();

        if(mInfos.mode() != dFrame->mode){
            mInfos.initialize(dFrame->mode);
            indices.initialize(mInfos);
        }

        // start computing
        reset_calibration();
        compute_color_image();
        compute_depth_sized_color_image();
        compute_bodies_id_map_image();
        compute_depth_data();
        compute_depth_image();
        compute_infra_data();
        compute_infra_image();
        compute_cloud_from_depth();
        create_colored_cloud();

        tTotal = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    std::chrono::microseconds tResetCalibration;
    std::chrono::microseconds tComputeColorImage;
    std::chrono::microseconds tComputeDephtSizedColorImage;
    std::chrono::microseconds tComputeBodiesIdMapImage;
    std::chrono::microseconds tComputeDepthData;
    std::chrono::microseconds tComputeDepthImage;
    std::chrono::microseconds tComputeInfraData;
    std::chrono::microseconds tComputeInfraImage;
    std::chrono::microseconds tComputeCloud;
    std::chrono::microseconds tComputeColoredCloud;
    std::chrono::microseconds tTotal;


private:

    auto reset_calibration() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->datasB.contains(DCDataBufferType::Calibration)){

            frame->calibration = std::get<1>(dFrame->datasB[DCDataBufferType::Calibration]);

            if((dFrame->idDevice != lastidDevice) || (dFrame->mode != lastMode) || (k4aTransformation == nullptr)){

                lastidDevice = dFrame->idDevice;
                lastMode     = dFrame->mode;

                // device specifics here (TODO)
                k4a::calibration calibration = *reinterpret_cast<k4a::calibration*>(frame->calibration.get_data());

                // specify color-depth alignment
                // calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[0] += 100.f;
                // calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[1] += 0.f;
                // calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[2] += 0.f;

                k4aTransformation = std::make_unique<k4a::transformation>(calibration);//*reinterpret_cast<k4a::calibration*>(frame->calibration.get_data()));
            }
        }
        tResetCalibration = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_color_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCDataImageBufferType::OriginalColorRGBA8) && gSettings.originalSizeColorImage){
            const auto &image   = dFrame->imagesB[DCDataImageBufferType::OriginalColorRGBA8];
            const auto cMode    = std::get<0>(image);
            const auto &buffer  = std::get<1>(image);
            if(cMode == DCCompressionMode::JPEG){
                colorDecoder.decode(buffer, frame->rgbaColor);
            }else if(cMode == DCCompressionMode::None){
                frame->rgbaColor.resize_image(buffer.width, buffer.height);
                std::copy(buffer.begin(), buffer.end(), frame->rgbaColor.get_byte_data());
            }

        }
        tComputeColorImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_sized_color_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCDataImageBufferType::DepthSizedColorRGBA8) && gSettings.depthSizedColorImage){
            const auto &image   = dFrame->imagesB[DCDataImageBufferType::DepthSizedColorRGBA8];
            const auto cMode    = std::get<0>(image);
            const auto &buffer  = std::get<1>(image);
            if(cMode == DCCompressionMode::JPEG){
                // Logger::message("[DS-JPEG]");
                jpegDepthSizedColorDecoder.decode(buffer, frame->rgbaDepthSizedColor);
            }else if(cMode == DCCompressionMode::None){
                // Logger::message("[DS-NONE]");
                frame->rgbaDepthSizedColor.resize_image(buffer.width, buffer.height);
                std::copy(buffer.begin(), buffer.end(), frame->rgbaDepthSizedColor.get_byte_data());
            }
        }
        tComputeDephtSizedColorImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_bodies_id_map_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        // if(!cFrame->jpegG8BodiesIdMap.empty() && gSettings.bodyTracking){
        //     bodiesIdDecoder.decode(
        //         cFrame->jpegG8BodiesIdMap, frame->grayBodiesIdMap
        //     );
        // }
        tComputeBodiesIdMapImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_data() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCDataImageBufferType::Depth16)){
            const auto &image   = dFrame->imagesB[DCDataImageBufferType::Depth16];
            const auto cMode    = std::get<0>(image);
            const auto &buffer  = std::get<1>(image);
            if(cMode == DCCompressionMode::FastPFor){
                depthDecoder.decode(buffer, frame->depth);
            }else if(cMode == DCCompressionMode::None){
                frame->depth.resize_image(buffer.width, buffer.height);
                std::copy(buffer.begin(), buffer.end(), frame->depth.get_byte_data());
            }
        }
        tComputeDepthData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_infra_data() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCDataImageBufferType::Infrared16)){
            const auto &image   = dFrame->imagesB[DCDataImageBufferType::Infrared16];
            const auto cMode    = std::get<0>(image);
            const auto &buffer  = std::get<1>(image);
            if(cMode == DCCompressionMode::FastPFor){
                infraDecoder.decode(buffer, frame->infra);
            }else if(cMode == DCCompressionMode::None){
                frame->infra.resize_image(buffer.width, buffer.height);
                std::copy(buffer.begin(), buffer.end(), frame->infra.get_byte_data());
            }
        }
        tComputeInfraData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!frame->depth.empty() && gSettings.depthImage){
            frame->compute_rgb_depth_image(frame->rgbDepth);            
        }
        tComputeDepthImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_infra_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!frame->infra.empty() && gSettings.infraImage){
            frame->compute_rgb_infra_image(frame->rgbInfra);
        }
        tComputeInfraImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_cloud_from_depth() -> void{

        if(frame->calibration.empty() || frame->depth.empty() || !gSettings.cloud){
            return;
        }

        auto tStart = Time::nanoseconds_since_epoch();
        auto k4aDepthImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_DEPTH16,
            static_cast<int>(frame->depth.width), static_cast<int>(frame->depth.height),
            static_cast<int>(frame->depth.width * frame->depth.element_bytes_size()),
            reinterpret_cast<std::uint8_t*>(frame->depth.get_data()),
            frame->depth.bytes_size(),
            nullptr,nullptr
        );

        if(pointCloud.size() < frame->depth.size()){
            pointCloud.resize(frame->depth.size());
        }

        auto k4aPointCloudImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_CUSTOM,
            static_cast<int>(frame->depth.width),
            static_cast<int>(frame->depth.height),
            static_cast<int>(frame->depth.width * 3 * sizeof(std::int16_t)),
            reinterpret_cast<std::uint8_t*>(pointCloud.get_data()),
            frame->depth.size() * 3 * sizeof(std::int16_t),
            nullptr, nullptr
        );

        k4aTransformation->depth_image_to_point_cloud(
            k4aDepthImage,
            K4A_CALIBRATION_TYPE_DEPTH,
            &k4aPointCloudImage
        );

        frame->cloud.resize(dFrame->validVerticesCount, true);

        int currentValidId = 0;
        for(size_t id = 0; id < frame->depth.size(); ++id){
            if(frame->depth[id] != dc_invalid_depth_value){
                indices.depthVertexCorrrespondance[id] = {static_cast<int>(id), currentValidId++};
            }else{
                indices.depthVertexCorrrespondance[id] = {static_cast<int>(id), 0};
            }
        }

        tComputeCloud = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_cloud_from_decoded_data() -> void{
        // ...
    }

    auto create_colored_cloud() -> void{        

        if(frame->cloud.empty() || frame->depth.empty() || !gSettings.cloud){
            return;
        }

        auto useColors  = !frame->rgbaDepthSizedColor.empty() && (gSettings.cloudColorMode == CloudColorMode::FromDepthSizedColorImage);


        static constexpr std::array<Pt3f, 5> depthGradient ={
            Pt3f{0.f,0.f,1.f},
            {0.f,1.f,1.f},
            {0.f,1.f,0.f},
            {1.f,1.f,0.f},
            {1.f,0.f,0.f},
        };

        const auto dRange = dc_depth_range(dFrame->mode)*1000.f;
        const auto diff = dRange(1) - dRange(0);

        auto tStart = Time::nanoseconds_since_epoch();
        std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::begin(indices.depthVertexCorrrespondance) + frame->depth.size(), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(frame->depth[idD] == dc_invalid_depth_value){
                return;
            }            

            auto idV = std::get<1>(idC);

            Pt3f currentP = pointCloud[idD].template conv<float>();
            frame->cloud.vertices[idV] = currentP * 0.001f;

            if(useColors){
                frame->cloud.colors[idV] = geo::Pt3f{
                    static_cast<float>(frame->rgbaDepthSizedColor[idD].r()),
                    static_cast<float>(frame->rgbaDepthSizedColor[idD].g()),
                    static_cast<float>(frame->rgbaDepthSizedColor[idD].b())
                }/255.f;
            }else{
                float vF = (static_cast<float>(frame->depth[idD]) - dRange(0))/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                frame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            }

            // A B C
            // D I E
            // F G H

            // 0 1 2
            // 3 X 4
            // 5 6 7

            const auto &idN  = indices.neighbours8Depth1D[idD];

            float cD = frame->depth[idD];
            float dB = (idN[1] != -1) ? ((frame->depth[idN[1]] != dc_invalid_depth_value) ? frame->depth[idN[1]] : cD) : cD;
            float dD = (idN[3] != -1) ? ((frame->depth[idN[3]] != dc_invalid_depth_value) ? frame->depth[idN[3]] : cD) : cD;
            float dE = (idN[4] != -1) ? ((frame->depth[idN[4]] != dc_invalid_depth_value) ? frame->depth[idN[4]] : cD) : cD;
            float dG = (idN[6] != -1) ? ((frame->depth[idN[6]] != dc_invalid_depth_value) ? frame->depth[idN[6]] : cD) : cD;

            auto hV = normalize(Vec3f(dE-dD,0,-2));
            auto vV = normalize(Vec3f(0,dG-dB,-2));
            frame->cloud.normals[idV] = normalize(hV+vV);
        });


        tComputeColoredCloud = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }
};

DCFrameGenerator::DCFrameGenerator() : i(std::make_unique<Impl>()){}

DCFrameGenerator::~DCFrameGenerator(){
}

auto DCFrameGenerator::initialize(DCDataFrame *dFrame) -> void{
    i->initialize(dFrame);
}

auto DCFrameGenerator::generate(const DCFrameGenerationSettings &gSettings, DCDataFrame *dFrame, DCFrame &frame) -> bool{

    i->reset_timings();
    i->do_work_task(gSettings, dFrame,frame);
    // i->do_work(gSettings, cFrame, frame);
    // i->display_timings();
    return true;
}

