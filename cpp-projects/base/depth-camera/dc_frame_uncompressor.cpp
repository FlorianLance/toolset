

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

#include "dc_frame_uncompressor.hpp"

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


using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

struct DCFrameUncompressor::Impl{
    
    // parameters
    bool generateColorRGB = false;
    bool generateDepthRGB = false;
    bool generateInfraRGB = false;
    bool generateBodiesGray = false;

    // decoders
    data::JpegDecoder colorDecoder;
    data::JpegDecoder depthSizedColorDecoder;
    data::JpegDecoder bodiesIdDecoder;
    data::FastPForDecoder depthDecoder;
    data::FastPForDecoder infraDecoder;
    data::FastPForDecoder cloudDecoder;

    // processing data
    std::uint8_t lastidDevice = 0;
    DCMode lastMode = DCMode::Invalid;
    std::unique_ptr<k4a::transformation> k4aTransformation = nullptr;
    Buffer<geo::Pt3<std::int16_t>> pointCloud;
    Buffer<geo::Pt2<int>> depthVertexCorrrespondance;
    tf::Taskflow taskFlow;
    tf::Executor executor;

    // local task data
    DCCompressedFrame *cFrame = nullptr;
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
            if(gSettings.cloudGenMode == CloudGenerationMode::FromDepth){
                                       compute_cloud_from_depth();
            }else{
                // ...
            }
        }).name("compute_cloud");
        auto cCreateColoredCloudT       = taskFlow.emplace([&](){
            if(gSettings.cloudGenMode == CloudGenerationMode::FromDepth){
                create_colored_cloud();
            }else{
                // ...
            }
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


        // copy audio
        // copy imu
        // copy bodies id map
        // copy

    }

    auto do_work_task(const DCFrameGenerationSettings &gSettings, DCCompressedFrame *cFrame, DCFrame &frame) -> void{

        // frame
        frame.idCapture      = cFrame->idCapture;
        frame.afterCaptureTS = cFrame->afterCaptureTS;
        frame.receivedTS     = cFrame->receivedTS;

        // info
        frame.mode = cFrame->mode;

        // set data
        this->gSettings  = gSettings;
        this->cFrame    = cFrame;
        this->frame     = &frame;

        // run task
        auto tStart = Time::nanoseconds_since_epoch();
        executor.run(taskFlow).wait();
        tTotal = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());

        // std::cout << "TIMES: "
        //           << "rc [" << tResetCalibration.count() << "] "
        //           << "cci [" << tComputeColorImage.count() << "] "
        //           << "cdsci [" << tComputeDephtSizedColorImage.count() << "] "
        //           << "cbimi [" << tComputeBodiesIdMapImage.count() << "] "
        //           << "cdd [" << tComputeDepthData.count() << "] "
        //           << "cdi [" << tComputeDepthImage.count() << "] "
        //           << "cid [" << tComputeInfraData.count() << "] "
        //           << "cii [" << tComputeInfraImage.count() << "] "
        //           << "cc [" << tComputeCloud.count() << "] "
        //           << "ccc [" << tComputeColoredCloud.count() << "] "
        //           << "tot [" << tTotal.count() << "]\n";
    }

    auto do_work(DCCompressedFrame *ncFrame, DCFrame &nFrame) -> void{

        // frame
        nFrame.idCapture      = ncFrame->idCapture;
        nFrame.afterCaptureTS = ncFrame->afterCaptureTS;
        nFrame.receivedTS     = ncFrame->receivedTS;

        // info
        nFrame.mode = ncFrame->mode;

        // set frames
        cFrame = ncFrame;
        frame  = &nFrame;

        auto tStart = Time::nanoseconds_since_epoch();

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

        // std::cout << "TIMES: "
        //           << "rc [" << tResetCalibration.count() << "] "
        //           << "cci [" << tComputeColorImage.count() << "] "
        //           << "cdsci [" << tComputeDephtSizedColorImage.count() << "] "
        //           << "cbimi [" << tComputeBodiesIdMapImage.count() << "] "
        //           << "cdd [" << tComputeDepthData.count() << "] "
        //           << "cdi [" << tComputeDepthImage.count() << "] "
        //           << "cid [" << tComputeInfraData.count() << "] "
        //           << "cii [" << tComputeInfraImage.count() << "] "
        //           << "cc [" << tComputeCloud.count() << "] "
        //           << "ccc [" << tComputeColoredCloud.count() << "] "
        //           << "tot [" << tTotal.count() << "]\n";
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
        if(!cFrame->calibration.empty()){

            frame->calibration = cFrame->calibration;

            if((cFrame->idDevice != lastidDevice) || (cFrame->mode != lastMode) || (k4aTransformation == nullptr)){

                lastidDevice = cFrame->idDevice;
                lastMode     = cFrame->mode;

                // device specifics here (TODO)                                
                k4aTransformation = std::make_unique<k4a::transformation>(*reinterpret_cast<k4a::calibration*>(frame->calibration.get_data()));
            }
        }
        tResetCalibration = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_color_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();                
        if(!cFrame->jpegRGBA8Color.empty() && gSettings.colorImage){
            colorDecoder.decode(
                cFrame->jpegRGBA8Color, frame->rgbaColor
            );
        }
        tComputeColorImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_sized_color_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();

        if(!cFrame->jpegRGBA8DepthSizedColor.empty() && gSettings.depthSizedColorImage){
            depthSizedColorDecoder.decode(
                cFrame->jpegRGBA8DepthSizedColor, frame->rgbaDepthSizedColor
            );
        }
        tComputeDephtSizedColorImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_bodies_id_map_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!cFrame->jpegG8BodiesIdMap.empty() && gSettings.bodyTracking){
            bodiesIdDecoder.decode(
                cFrame->jpegG8BodiesIdMap, frame->grayBodiesIdMap
            );
        }
        tComputeBodiesIdMapImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_data() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!cFrame->fpfDepth.empty() && gSettings.depth){
            depthDecoder.decode(
                cFrame->fpfDepth, frame->depth
            );            
        }
        tComputeDepthData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_depth_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!frame->depth.empty() && gSettings.depthImage){
            frame->compute_rgb_depth_image(frame->rgbDepth);            
        }
        tComputeDepthImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_infra_data() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!cFrame->fpfInfra.empty() && gSettings.infra){
            infraDecoder.decode(
                cFrame->fpfInfra, frame->infra
            );
        }
        tComputeInfraData = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_infra_image() -> void{
        auto tStart = Time::nanoseconds_since_epoch();
        if(!frame->infra.empty() && gSettings.infraImage){
            frame->compute_rgb_infra_image(frame->rgbInfra);
        }
        tComputeInfraImage = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }

    auto compute_cloud_from_depth() -> void{

        if(cFrame->calibration.empty() || frame->depth.empty() || !gSettings.cloud){
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

        frame->cloud.resize(cFrame->validVerticesCount, false);

        if(depthVertexCorrrespondance.size() < frame->depth.size()){
            depthVertexCorrrespondance.resize(frame->depth.size());
        }

        int currentValidId = 0;
        for(size_t id = 0; id < frame->depth.size(); ++id){
            if(frame->depth[id] != dc_invalid_depth_value){
                depthVertexCorrrespondance[id] = {static_cast<int>(id), currentValidId++};
            }else{
                depthVertexCorrrespondance[id] = {static_cast<int>(id), 0};
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

        auto useColors = !frame->rgbaDepthSizedColor.empty() && (gSettings.cloudColorMode == CloudColorMode::FromDepthSizedColorImage);

        static constexpr std::array<Pt3f, 5> depthGradient ={
            Pt3f{0.f,0.f,1.f},
            {0.f,1.f,1.f},
            {0.f,1.f,0.f},
            {1.f,1.f,0.f},
            {1.f,0.f,0.f},
        };

        const auto dRange = dc_depth_range(cFrame->mode)*1000.f;
        const auto diff = dRange(1) - dRange(0);

        auto tStart = Time::nanoseconds_since_epoch();
        std::for_each(std::execution::par_unseq, std::begin(depthVertexCorrrespondance), std::begin(depthVertexCorrrespondance) + frame->depth.size(), [&](const geo::Pt2<int> &id){

            auto idD = id.x();
            if(frame->depth[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = id.y();
            // frame->cloud.vertices[idV]= geo::Pt3f{
            //     static_cast<float>(-pointCloud[idD].x()),
            //     static_cast<float>(-pointCloud[idD].y()),
            //     static_cast<float>( pointCloud[idD].z())
            // }*0.001f;
            frame->cloud.vertices[idV]= geo::Pt3f{
                 static_cast<float>(pointCloud[idD].x()),
                 static_cast<float>(pointCloud[idD].y()),
                 static_cast<float>(pointCloud[idD].z())
             }*0.001f;


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
                auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;

                frame->cloud.colors[idV] = geo::Pt3f{
                   col.x(),col.y(),col.z()
                };
            }
        });

        tComputeColoredCloud = Time::difference_micro_s(tStart, Time::nanoseconds_since_epoch());
    }
};

DCFrameUncompressor::DCFrameUncompressor() : i(std::make_unique<Impl>()){}

DCFrameUncompressor::~DCFrameUncompressor(){
}

auto DCFrameUncompressor::uncompress(const DCFrameGenerationSettings &gSettings, DCCompressedFrame *cFrame, DCFrame &frame) -> bool{
    // i->do_work(cFrame,frame);
    i->do_work_task(gSettings,cFrame,frame);
    return true;
}


// auto DCFrameUncompressor::Impl::convert_to_cloud(
//     DCMode mode,
//     size_t validVerticesCount,
//     ColoredCloudData &cloud,
//     std::vector<Pt3<uint8_t>> &uncompressedColor,
//     std::vector<std::uint16_t> &uncompressedVertices
//     ) -> void{

//     const auto vvc = validVerticesCount;
//     update_id_array(vvc);

//     // resize cloud if necessary
//     cloud.resize(vvc);

//     if(mode != DCMode::Merged){

//         for_each(std::execution::par_unseq, std::begin(indicesValid1D), std::begin(indicesValid1D) + vvc, [&](size_t id){

//             cloud.vertices[id] = Pt3f{
//                 -(static_cast<float>(uncompressedVertices[        id]) - 4096),
//                 -(static_cast<float>(uncompressedVertices[vvc   + id]) - 4096),
//                 static_cast<float>(uncompressedVertices  [2*vvc + id])
//             }*0.001f;

//             cloud.colors[id] = Pt3f{
//                   static_cast<float>(uncompressedColor[id].x()),
//                   static_cast<float>(uncompressedColor[id].y()),
//                   static_cast<float>(uncompressedColor[id].z())
//             }/255.f;
//         });
//     }else{

//         for_each(std::execution::par_unseq, std::begin(indicesValid1D), std::begin(indicesValid1D) + vvc, [&](size_t id){

//             cloud.vertices[id] = Pt3f{
//                 static_cast<float>(uncompressedVertices[        id])  -32768.f,
//                 static_cast<float>(uncompressedVertices[vvc   + id])  -32768.f,
//                 static_cast<float>(uncompressedVertices [2*vvc + id]) -32768.f,
//             }*0.001f;

//             cloud.colors[id] = Pt3f{
//                   static_cast<float>(uncompressedColor[id].x()),
//                   static_cast<float>(uncompressedColor[id].y()),
//                   static_cast<float>(uncompressedColor[id].z())
//             }/255.f;
//         });
//     }
// }
