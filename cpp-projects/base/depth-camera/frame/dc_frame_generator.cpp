

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
// #include "libobsensor/ObSensor.hpp"

// local
// # utility
// #include "utility/types.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"
// # data
#include "data/jpeg_encoding.hpp"
#include "data/fastpfor_encoding.hpp"

#include "thirdparty/taskflow/taskflow.hpp"
// #include "thirdparty/taskflow/core/executor.hpp"
// #include "thirdparty/taskflow/core/taskflow.hpp"

#include "dc_depth_indices.hpp"


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
    data::JpegDecoder depthSizedColorDecoder;
    // data::JpegDecoder bodiesIdDecoder;
    data::FastPForDecoder depthDecoder;
    data::FastPForDecoder infraDecoder;
    data::FastPForDecoder cloudDecoder;

    // processing data
    DCDepthIndices dIndices;
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

        auto initDataT                  = taskFlow.emplace([&](){init_data();}).name("init_data");
        auto resetTransformationT       = taskFlow.emplace([&](){reset_transformation();}).name("reset_transformation");
        auto cRgbaColorimageT           = taskFlow.emplace([&](){compute_color_image();}).name("compute_color_image");
        auto cRgbaDepthSizedColorImageT = taskFlow.emplace([&](){compute_depth_sized_color_image();}).name("compute_depth_sized_color_image");
        auto cBodiesIdDataT             = taskFlow.emplace([&](){compute_bodies_id_data();}).name("compute_bodies_id_data");
        auto cBodiesIdImageT            = taskFlow.emplace([&](){compute_bodies_id_image();}).name("compute_bodies_id_image");
        auto cDepthDataT                = taskFlow.emplace([&](){compute_depth_data();}).name("compute_depth_data");
        auto cDepthImageT               = taskFlow.emplace([&](){compute_depth_image();}).name("compute_depth_image");
        auto cInfraDataT                = taskFlow.emplace([&](){compute_infra_data();}).name("compute_infra_data");
        auto cInfraImageT               = taskFlow.emplace([&](){compute_infra_image();}).name("compute_infra_image");
        auto cCloudT                    = taskFlow.emplace([&](){
            compute_cloud_from_depth();
        }).name("compute_cloud");
        auto cCreateColoredCloudT       = taskFlow.emplace([&](){
            create_colored_cloud();
        }).name("create_colored_cloud");

        initDataT.priority(tf::TaskPriority::HIGH);
        resetTransformationT.priority(tf::TaskPriority::HIGH);
        cDepthDataT.priority(tf::TaskPriority::HIGH);
        cRgbaDepthSizedColorImageT.priority(tf::TaskPriority::HIGH);
        cCloudT.priority(tf::TaskPriority::NORMAL);
        cCreateColoredCloudT.priority(tf::TaskPriority::NORMAL);
        cBodiesIdDataT.priority(tf::TaskPriority::NORMAL);

        cRgbaColorimageT.priority(tf::TaskPriority::LOW);

        cDepthImageT.priority(tf::TaskPriority::LOW);
        cInfraDataT.priority(tf::TaskPriority::LOW);
        cInfraImageT.priority(tf::TaskPriority::LOW);
        cBodiesIdImageT.priority(tf::TaskPriority::LOW);

        // pipeline
        initDataT.precede(resetTransformationT, cRgbaColorimageT, cRgbaDepthSizedColorImageT, cBodiesIdDataT, cDepthDataT, cInfraDataT);
        cCloudT.succeed(resetTransformationT, cDepthDataT);
        cCreateColoredCloudT.succeed(cCloudT, cRgbaDepthSizedColorImageT);
        cDepthImageT.succeed(cDepthDataT);
        cInfraImageT.succeed(cInfraDataT);
        cBodiesIdImageT.succeed(cBodiesIdDataT);
    }

    auto initialize(DCDataFrame *initCFrame) -> void{
        
        // Log::message("I1");
        // if(initCFrame->imagesB.contains(DCImageBufferType::Depth16)){
        //     auto &dImage = std::get<1>(initCFrame->imagesB[DCImageBufferType::Depth16]);
        //     dIndices.initialize(true, dImage.width, dImage.height);
        // }else{
        //     dIndices.initialize(false, 0, 0);
        // }

        // Log::message("I2");
        // dFrame = initCFrame;
        // DCFrame initFrame;
        // frame = &initFrame;
        // reset_transformation();
        // Log::message("I3");
    }

    auto reset_timings() -> void{
        tResetCalibration = {};
        tComputeColorImage = {};
        tComputeDephtSizedColorImage = {};
        tComputeBodiesIdImage = {};
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
                  << "cbimi [" << tComputeBodiesIdImage.count() << "] "
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
        
        if(dFrame->imagesB.contains(DCImageBufferType::Depth16)){
            auto &dImage = std::get<1>(dFrame->imagesB[DCImageBufferType::Depth16]);
            dIndices.initialize(true, dImage.width, dImage.height);
        }else{
            dIndices.initialize(false, 0, 0);
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

        // start computing
        init_data();
        reset_transformation();
        compute_color_image();
        compute_depth_sized_color_image();
        compute_depth_data();
        compute_depth_image();
        compute_infra_data();
        compute_infra_image();
        compute_bodies_id_data();
        compute_bodies_id_image();
        compute_cloud_from_depth();
        create_colored_cloud();

        tTotal = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    std::chrono::microseconds tResetCalibration;
    std::chrono::microseconds tComputeColorImage;
    std::chrono::microseconds tComputeDephtSizedColorImage;
    std::chrono::microseconds tComputeBodiesIdData;
    std::chrono::microseconds tComputeBodiesIdImage;
    std::chrono::microseconds tComputeDepthData;
    std::chrono::microseconds tComputeDepthImage;
    std::chrono::microseconds tComputeInfraData;
    std::chrono::microseconds tComputeInfraImage;
    std::chrono::microseconds tComputeCloud;
    std::chrono::microseconds tComputeColoredCloud;
    std::chrono::microseconds tTotal;

private:

    auto init_data() -> void{

        // retrieve sizes
        // # original color
        size_t originalColorWidth = 0;
        size_t originalColorHeight = 0;
        if(dFrame->imagesB.contains(DCImageBufferType::OriginalColorRGBA8)){
            auto imageB = dFrame->image_buffer(DCImageBufferType::OriginalColorRGBA8);
            originalColorWidth  = imageB->width;
            originalColorHeight = imageB->height;
        }
        // # depth
        size_t depthWidth = 0;
        size_t depthHeight = 0;
        size_t depthSize = 0;
        if(dFrame->imagesB.contains(DCImageBufferType::Depth16)){
            auto imageB = dFrame->image_buffer(DCImageBufferType::Depth16);
            depthWidth  = imageB->width;
            depthHeight = imageB->height;
            depthSize   = imageB->size();
        }
        // # infrared
        size_t infraredWidth  = 0;
        size_t infraredHeight = 0;
        if(dFrame->imagesB.contains(DCImageBufferType::Infrared16)){
            auto imageB = dFrame->image_buffer(DCImageBufferType::Infrared16);
            infraredWidth  = imageB->width;
            infraredHeight = imageB->height;
        }


        // resize indices
        if(depthSize != 0){
            dIndices.initialize(true, depthWidth, depthHeight);
        }else{
            dIndices.initialize(false, 0, 0);
        }

        // insert
        // # data
        if(dFrame->datasB.contains(DCDataBufferType::Calibration)){
            frame->insert_data_buffer(DCDataBufferType::Calibration, dFrame->data_buffer(DCDataBufferType::Calibration)->byte_span());
        }
        // # images
        if(dFrame->imagesB.contains(DCImageBufferType::Depth16)){
            frame->insert_image_buffer<std::uint16_t>(DCImageBufferType::Depth16, depthWidth, depthHeight);
        }
        if(dFrame->imagesB.contains(DCImageBufferType::Infrared16)){
            frame->insert_image_buffer<std::uint16_t>(DCImageBufferType::Infrared16, infraredWidth, infraredHeight);
        }
        if(dFrame->imagesB.contains(DCImageBufferType::BodiesIdMap8)){
            frame->insert_image_buffer<ColorGray8>(DCImageBufferType::BodiesIdMap8, depthWidth, depthHeight);
        }
        if(dFrame->imagesB.contains(DCImageBufferType::OriginalColorRGBA8) && gSettings.originalSizeColorImage){
            frame->insert_image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8, originalColorWidth, originalColorHeight);
        }
        if(dFrame->imagesB.contains(DCImageBufferType::DepthSizedColorRGBA8) && gSettings.depthSizedColorImage){
            frame->insert_image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8, depthWidth, depthHeight);
        }
        if(frame->imagesB.contains(DCImageBufferType::Depth16) && gSettings.depthImage){
            frame->insert_image_buffer<ColorRGB8>(DCImageBufferType::DepthRGB8, depthWidth, depthHeight, ColorRGB8{0,0,0});
        }
        if(frame->imagesB.contains(DCImageBufferType::Infrared16) && gSettings.infraImage){
            frame->insert_image_buffer<ColorRGB8>(DCImageBufferType::InfraredRGB8, infraredWidth, infraredHeight, ColorRGB8{0,0,0});
        }
        if(frame->imagesB.contains(DCImageBufferType::BodiesIdMap8) && gSettings.bodiesIdImage){
            frame->insert_image_buffer<ColorRGB8>(DCImageBufferType::BodiesIdMapRGB8, depthWidth, depthHeight, ColorRGB8{0,0,0});
        }

        // # volume
        if(frame->datasB.contains(DCDataBufferType::Calibration) && frame->imagesB.contains(DCImageBufferType::Depth16) && gSettings.cloud){
            auto cloud = frame->insert_volume_buffer(DCVolumeBufferType::ColoredCloud);
            cloud->resize(dFrame->validVerticesCount, true);
        }
    }

    auto reset_transformation() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->datasB.contains(DCDataBufferType::Calibration)){                
            if((dFrame->idDevice != lastidDevice) || (dFrame->mode != lastMode) || (k4aTransformation == nullptr)){

                lastidDevice = dFrame->idDevice;
                lastMode     = dFrame->mode;

                // device specifics here (TODO)
                k4a::calibration calibration = *reinterpret_cast<k4a::calibration*>(frame->data_buffer(DCDataBufferType::Calibration)->get_data());
                // specify color-depth alignment
                // calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[0] += 100.f;
                // calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[1] += 0.f;
                // calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[2] += 0.f;
                k4aTransformation = std::make_unique<k4a::transformation>(calibration);
            }
        }
        tResetCalibration = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_color_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCImageBufferType::OriginalColorRGBA8) && gSettings.originalSizeColorImage){
            auto &[cMode, buffer] = dFrame->imagesB[DCImageBufferType::OriginalColorRGBA8];
            if(cMode == DCCompressionMode::JPEG){
                colorDecoder.decode(buffer, *frame->image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8));
            }else if(cMode == DCCompressionMode::None){
                std::copy(buffer.begin(), buffer.end(), frame->image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8)->get_byte_data());
            }
        }
        tComputeColorImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());        
    }

    auto compute_depth_sized_color_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCImageBufferType::DepthSizedColorRGBA8) && gSettings.depthSizedColorImage){            
            auto &[cMode, buffer] = dFrame->imagesB[DCImageBufferType::DepthSizedColorRGBA8];
            if(cMode == DCCompressionMode::JPEG){
                depthSizedColorDecoder.decode(buffer, *frame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8));
            }else if(cMode == DCCompressionMode::None){
                std::copy(buffer.begin(), buffer.end(), frame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8)->get_byte_data());
            }
        }
        tComputeDephtSizedColorImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_data() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCImageBufferType::Depth16)){
            auto &[cMode, buffer] = dFrame->imagesB[DCImageBufferType::Depth16];
            if(cMode == DCCompressionMode::FastPFor){
                depthDecoder.decode(buffer, *frame->image_buffer<std::uint16_t>(DCImageBufferType::Depth16));
            }else if(cMode == DCCompressionMode::None){
                std::copy(buffer.begin(), buffer.end(), frame->image_buffer<std::uint16_t>(DCImageBufferType::Depth16)->get_byte_data());
            }
        }
        tComputeDepthData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_bodies_id_data() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCImageBufferType::BodiesIdMap8)){
            auto &[cMode, buffer] = dFrame->imagesB[DCImageBufferType::BodiesIdMap8];
            std::copy(buffer.begin(), buffer.end(), frame->image_buffer<ColorGray8>(DCImageBufferType::BodiesIdMap8)->get_byte_data());
        }
        tComputeBodiesIdData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }



    auto compute_infra_data() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(dFrame->imagesB.contains(DCImageBufferType::Infrared16)){

            auto &[cMode, buffer] = dFrame->imagesB[DCImageBufferType::Infrared16];
            if(cMode == DCCompressionMode::FastPFor){
                infraDecoder.decode(buffer, *frame->image_buffer<std::uint16_t>(DCImageBufferType::Infrared16));
            }else if(cMode == DCCompressionMode::None){
                std::copy(buffer.begin(), buffer.end(), frame->image_buffer<std::uint16_t>(DCImageBufferType::Infrared16)->get_byte_data());
            }
        }
        tComputeInfraData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(frame->imagesB.contains(DCImageBufferType::Depth16) && gSettings.depthImage){

            static constexpr std::array<Pt3f, 5> depthGradient ={
                Pt3f{0.f,0.f,1.f},
                {0.f,1.f,1.f},
                {0.f,1.f,0.f},
                {1.f,1.f,0.f},
                {1.f,0.f,0.f},
            };

            auto depth  = frame->image_buffer<std::uint16_t>(DCImageBufferType::Depth16);
            auto imageB = frame->image_buffer<ColorRGB8>(DCImageBufferType::DepthRGB8);

            const auto dRange = dc_depth_range(frame->mode)*1000.f;
            const auto diff = dRange(1) - dRange(0);

            // convert data
            for(size_t id = 0; id < depth->size(); ++id){

                if((*depth)[id] == dc_invalid_unsigned_depth_value){
                    (*imageB)[id] = ColorRGB8{0,0,0};
                    continue;;
                }

                float vF = (static_cast<float>((*depth)[id]) - dRange(0))/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);

                auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                (*imageB)[id] = ColorRGB8{
                    static_cast<uint8_t>(255*col.x()),
                    static_cast<uint8_t>(255*col.y()),
                    static_cast<uint8_t>(255*col.z())
                };
            }
        }

        tComputeDepthImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }


    auto compute_bodies_id_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(frame->imagesB.contains(DCImageBufferType::BodiesIdMap8) && gSettings.bodiesIdImage){

            static constexpr std::array<ColorRGB8,10> bodiesColor ={
                ColorRGB8
                {255,0,0},
                {0,255,0},
                {0,0,255},
                {255,255,0},
                {255,0,255},
                {0,255,255},
                {122,122,0},
                {122,0,122},
                {0,122,122},
                {255,255,255}
            };

            auto bodiesId  = frame->image_buffer<ColorGray8>(DCImageBufferType::BodiesIdMap8);
            auto imageB    = frame->image_buffer<ColorRGB8>(DCImageBufferType::BodiesIdMapRGB8);

            std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1D), std::end(dIndices.depths1D), [&](size_t id){
                if((*bodiesId)[id] != 255){ // K4ABT_BODY_INDEX_MAP_BACKGROUND
                    (*imageB)[id] = bodiesColor[(*bodiesId)[id]%10];
                }
            });
        }
        tComputeBodiesIdImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_infra_image() -> void{

        auto tStart = Time::nanoseconds_since_epoch();
        if(frame->imagesB.contains(DCImageBufferType::Infrared16) && gSettings.infraImage){

            auto infra  = frame->image_buffer<std::uint16_t>(DCImageBufferType::Infrared16);
            auto imageB = frame->image_buffer<ColorRGB8>(DCImageBufferType::InfraredRGB8);

            const float max = 2000;
            for(size_t id = 0; id < infra->size(); ++id){

                float vF = static_cast<float>((*infra)[id]);
                if(vF > max){
                    vF = max;
                }
                vF/=max;
                (*imageB)[id] = ColorRGB8{
                    static_cast<uint8_t>(255*vF),
                    static_cast<uint8_t>(255*vF),
                    static_cast<uint8_t>(255*vF)
                };
            }
        }
        tComputeInfraImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_cloud_from_depth() -> void{

        if(!frame->datasB.contains(DCDataBufferType::Calibration) || !frame->imagesB.contains(DCImageBufferType::Depth16) || !gSettings.cloud){
            return;
        }

        auto depth = frame->image_buffer<std::uint16_t>(DCImageBufferType::Depth16);
        auto tStart = Time::nanoseconds_since_epoch();
        auto k4aDepthImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_DEPTH16,
            static_cast<int>(depth->width), static_cast<int>(depth->height),
            static_cast<int>(depth->width * depth->element_bytes_size()),
            reinterpret_cast<std::uint8_t*>(depth->get_data()),
            depth->bytes_size(),
            nullptr,nullptr
        );

        if(pointCloud.size() < depth->size()){
            pointCloud.resize(depth->size());
        }

        auto k4aPointCloudImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_CUSTOM,
            static_cast<int>(depth->width),
            static_cast<int>(depth->height),
            static_cast<int>(depth->width * 3 * sizeof(std::int16_t)),
            reinterpret_cast<std::uint8_t*>(pointCloud.get_data()),
            depth->size() * 3 * sizeof(std::int16_t),
            nullptr, nullptr
        );

        k4aTransformation->depth_image_to_point_cloud(
            k4aDepthImage,
            K4A_CALIBRATION_TYPE_DEPTH,
            &k4aPointCloudImage
        );

        int currentValidId = 0;
        for(size_t id = 0; id < depth->size(); ++id){
            if((*depth)[id] != dc_invalid_unsigned_depth_value){
                dIndices.depthVertexCorrrespondance[id] = {static_cast<int>(id), currentValidId++};
            }else{
                dIndices.depthVertexCorrrespondance[id] = {static_cast<int>(id), 0};
            }
        }

        tComputeCloud = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto create_colored_cloud() -> void{

        if(!frame->volumesB.contains(DCVolumeBufferType::ColoredCloud) || !frame->imagesB.contains(DCImageBufferType::Depth16) || !gSettings.cloud){
            return;
        }

        auto cloud              = frame->volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
        auto depth              = frame->image_buffer<std::uint16_t>(DCImageBufferType::Depth16);
        auto depthSizedColor    = frame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8);
        auto useColors          = (depthSizedColor != nullptr) && (gSettings.cloudColorMode == CloudColorMode::FromDepthSizedColorImage);

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
        std::for_each(std::execution::par_unseq, std::begin(dIndices.depthVertexCorrrespondance), std::begin(dIndices.depthVertexCorrrespondance) + depth->size(), [&](auto idC){

            auto idD = std::get<0>(idC);
            if((*depth)[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);

            Pt3f currentP = pointCloud[idD].template conv<float>();
            cloud->vertices[idV] = currentP * 0.001f;

            if(useColors){
                cloud->colors[idV] = geo::Pt3f{
                    static_cast<float>((*depthSizedColor)[idD].r()),
                    static_cast<float>((*depthSizedColor)[idD].g()),
                    static_cast<float>((*depthSizedColor)[idD].b())
                }/255.f;
            }else{
                float vF = (static_cast<float>((*depth)[idD]) - dRange(0))/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                cloud->colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            }

            // A B C
            // D I E
            // F G H

            // 0 1 2
            // 3 X 4
            // 5 6 7

            const auto &idN  = dIndices.neighbours8Depth1D[idD];
            float cD = (*depth)[idD];
            float dB = (idN[1] != -1) ? (((*depth)[idN[1]] != dc_invalid_depth_value) ? (*depth)[idN[1]] : cD) : cD;
            float dD = (idN[3] != -1) ? (((*depth)[idN[3]] != dc_invalid_depth_value) ? (*depth)[idN[3]] : cD) : cD;
            float dE = (idN[4] != -1) ? (((*depth)[idN[4]] != dc_invalid_depth_value) ? (*depth)[idN[4]] : cD) : cD;
            float dG = (idN[6] != -1) ? (((*depth)[idN[6]] != dc_invalid_depth_value) ? (*depth)[idN[6]] : cD) : cD;

            auto hV = normalize(Vec3f(dE-dD,0,-2));
            auto vV = normalize(Vec3f(0,dG-dB,-2));
            cloud->normals[idV] = normalize(hV+vV);
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

    // i->do_work_task(gSettings, dFrame, frame); // TOO SLOW
    i->do_work(gSettings, dFrame, frame);

    return true;
}

