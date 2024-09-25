
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

#include "dc_data_frame_generator.hpp"

// std
#include <format>
#include <execution>

// turbojpg
#include <turbojpeg.h>

// local
#include "utility/logger.hpp"
#include "data/jpeg_encoding.hpp"
#include "data/fastpfor_encoding.hpp"

using namespace tool;
using namespace tool::cam;

struct DCDataFrameGenerator::Impl{
    data::JpegEncoder colorEncoder;
    data::JpegEncoder depthSizedColorEncoder;
    data::JpegEncoder bodiesIdEncoder;
    data::FastPForEncoder depthEncoder;
    data::FastPForEncoder infraEncoder;
};

DCDataFrameGenerator::DCDataFrameGenerator() : i(std::make_unique<Impl>()){
}

DCDataFrameGenerator::~DCDataFrameGenerator(){
}

auto DCDataFrameGenerator::generate(const DCDataFrameGenerationSettings &dfgS, DCFrame &frame) -> std::unique_ptr<DCDataFrame>{
    auto dFrame = std::make_unique<DCDataFrame>();
    generate(dfgS, frame, dFrame.get());
    return dFrame;
}

auto DCDataFrameGenerator::generate(const DCDataFrameGenerationSettings &dfgS, DCFrame &frame, DCDataFrame *dFrame) -> void{

    // frame
    dFrame->idCapture          = frame.idCapture;
    dFrame->afterCaptureTS     = frame.afterCaptureTS;
    dFrame->receivedTS         = frame.receivedTS;

    // info
    dFrame->mode               = frame.mode;
    dFrame->validVerticesCount = frame.cloud.size();

    // calibration
    if(!frame.calibration.empty()){
        dFrame->datasB.insert({DCDataBufferType::Calibration, {DCCompressionMode::None, frame.calibration}});
    }

    // depth
    if(dfgS.addDepth && !frame.depth.empty()){
        if(dfgS.depthCM == DCCompressionMode::FastPFor){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::Depth16, {DCCompressionMode::FastPFor, {}}});
            i->depthEncoder.encode(frame.depth, std::get<1>(dInfo.first->second));
        }else if(dfgS.depthCM == DCCompressionMode::None){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::Depth16, {DCCompressionMode::None, {}}});
            auto &image = std::get<1>(dInfo.first->second);
            image.resize_image(frame.depth.width, frame.depth.height, 2);
            std::copy(frame.depth.begin(), frame.depth.end(), reinterpret_cast<std::uint16_t*>(image.get_data()));
        }
    }

    // infrared
    if(dfgS.addInfra && !frame.infra.empty()){
        if(dfgS.infraCM == DCCompressionMode::FastPFor){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::Infrared16, {DCCompressionMode::FastPFor, {}}});
            i->infraEncoder.encode(frame.infra, std::get<1>(dInfo.first->second));
        }else if(dfgS.infraCM == DCCompressionMode::None){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::Infrared16, {DCCompressionMode::None, {}}});
            auto &image = std::get<1>(dInfo.first->second);
            image.resize_image(frame.infra.width, frame.infra.height, 2);
            std::copy(frame.infra.begin(), frame.infra.end(), reinterpret_cast<std::uint16_t*>(image.get_data()));
        }
    }

    // depth-sized color
    if(dfgS.addDepthSizedColor && !frame.rgbaDepthSizedColor.empty()){
        if(dfgS.depthSizedColorCM == DCCompressionMode::JPEG){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::DepthSizedColorRGBA8, {DCCompressionMode::JPEG, {}}});
            i->depthSizedColorEncoder.encode(frame.rgbaDepthSizedColor, std::get<1>(dInfo.first->second), dfgS.depthSizedColorJPEGCQ);
        }else if(dfgS.depthSizedColorCM == DCCompressionMode::None){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::DepthSizedColorRGBA8, {DCCompressionMode::None, {}}});
            auto &image = std::get<1>(dInfo.first->second);
            image.resize_image(frame.rgbaDepthSizedColor.width, frame.rgbaDepthSizedColor.height, 4);
            std::copy(frame.rgbaDepthSizedColor.begin(), frame.rgbaDepthSizedColor.end(), reinterpret_cast<ColorRGBA8*>(image.get_data()));
        }
    }


    // original color
    if(dfgS.addOriginalSizeColor && !frame.rgbaColor.empty()){
        if(dfgS.originalSizeColorCM == DCCompressionMode::JPEG){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::OriginalColorRGBA8, {DCCompressionMode::JPEG, {}}});
            i->colorEncoder.encode(frame.rgbaColor, std::get<1>(dInfo.first->second), dfgS.depthSizedColorJPEGCQ);
        }else if(dfgS.originalSizeColorCM == DCCompressionMode::None){
            auto dInfo = dFrame->imagesB.insert({DCDataImageBufferType::OriginalColorRGBA8, {DCCompressionMode::None, {}}});
            auto &image = std::get<1>(dInfo.first->second);
            image.resize_image(frame.rgbaColor.width, frame.rgbaColor.height, 4);
            std::copy(frame.rgbaColor.begin(), frame.rgbaColor.end(), reinterpret_cast<ColorRGBA8*>(image.get_data()));
        }
    }

    // // gray bodies id
    // if(dfgS.addBodyIdMap && !frame.grayBodiesIdMap.empty()){
    //     // i->bodiesIdEncoder.encode(frame.grayBodiesIdMap, dFrame->jpegG8BodiesIdMap, dsS.jpegCompressionRate);
    // }

    // // imu
    // if(dfgS.addImu && !frame.imu.empty()){
    //     dFrame->datasB.insert({DCDataBufferType::IMU, {DCCompressionMode::None, frame.imu}});
    // }

    // // audio
    // if(dfgS.addAudio && !frame.audioFrames.empty()){
    //     // dFrame->audioFrames = frame.audioFrames;
    // }
}





// auto DCFrameCompressor::add_cloud(DCMode mode, const geo::ColoredCloudData &cloud, std::vector<uint8_t> &encodedCloudData) -> void{

//     if(!cloud.has_colors() || !cloud.has_vertices()){
//         tool::Logger::error("[DCFrameCompressor::add_cloud] Invalid cloud.\n");
//         return;
//     }

//     size_t validVerticesCount = cloud.size();
//     if(i->indices1D.size() < validVerticesCount){
//         i->indices1D.resize(validVerticesCount);
//     }
//     std::iota(std::begin(i->indices1D), std::begin(i->indices1D) + validVerticesCount, 0);

//     // resize cloud data
//     // data mapping: XXYYZZRGB0
//     size_t cloudVerticesBufferSize = validVerticesCount*5;
//     size_t rest = cloudVerticesBufferSize % 128;
//     size_t paddeCloudVerticesBufferPaddedSize = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
//     if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
//         i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
//     }
//     std::fill(i->processedCloudData.begin(), i->processedCloudData.begin() + paddeCloudVerticesBufferPaddedSize, 0);

//     auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(i->processedCloudData.data());

//     // process cloud buffer
//     if(mode != DCMode::Merged){
//         std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + validVerticesCount, [&](size_t id){
//             i->processedCloudData[id]                             = static_cast<std::uint16_t>((-cloud.vertices[id].x()*1000.f) +4096.f);
//             i->processedCloudData[validVerticesCount   + id]      = static_cast<std::uint16_t>((-cloud.vertices[id].y()*1000.f) +4096.f);
//             i->processedCloudData[2*validVerticesCount + id]      = static_cast<std::uint16_t>(cloud.vertices[id].z()*1000.f);
//             processedCloudData8[6*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].x()*255.f);
//             processedCloudData8[7*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].y()*255.f);
//             processedCloudData8[8*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].z()*255.f);
//         });

//     }else{
//         std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + cloud.size(), [&](size_t id){
//             i->processedCloudData[id]                             = static_cast<std::uint16_t>(cloud.vertices[id].x()*1000.f + 32768.f);
//             i->processedCloudData[validVerticesCount   + id]      = static_cast<std::uint16_t>(cloud.vertices[id].y()*1000.f + 32768.f);
//             i->processedCloudData[2*validVerticesCount + id]      = static_cast<std::uint16_t>(cloud.vertices[id].z()*1000.f + 32768.f);
//             processedCloudData8[6*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].x()*255.f);
//             processedCloudData8[7*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].y()*255.f);
//             processedCloudData8[8*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].z()*255.f);
//         });
//     }

//     // compress processed cloud data
//     i->compress_lossless_16_bits_128padded_data(
//         paddeCloudVerticesBufferPaddedSize,
//         i->processedCloudData.data(),
//         encodedCloudData
//         );
// }

