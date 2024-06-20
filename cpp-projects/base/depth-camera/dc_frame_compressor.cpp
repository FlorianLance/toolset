
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

#include "dc_frame_compressor.hpp"

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

struct DCFrameCompressor::Impl{

    std::vector<size_t> indices1D;
    BinaryBuffer processedCloud;

    // std::vector<std::int16_t> processedAudioData;

    data::JpegEncoder colorEncoder;
    data::JpegEncoder depthSizedColorEncoder;
    data::JpegEncoder bodiesIdEncoder;

    data::FastPForEncoder depthEncoder;
    data::FastPForEncoder infraEncoder;
    data::FastPForEncoder cloudEncoder;
};

DCFrameCompressor::DCFrameCompressor() : i(std::make_unique<Impl>()){
}

DCFrameCompressor::~DCFrameCompressor(){
}

auto DCFrameCompressor::compress(DCFrame &frame, int jpegQuality, DCCompressedFrame *cFrame) -> void{

    // frame
    cFrame->idCapture          = frame.idCapture;
    cFrame->afterCaptureTS     = frame.afterCaptureTS;
    cFrame->receivedTS         = frame.receivedTS;

    // info
    cFrame->mode               = frame.mode;
    cFrame->validVerticesCount = frame.cloud.size();

    cFrame->calibration        = frame.calibration;

    // rgba color
    if(!frame.rgbaColor.empty()){
        i->colorEncoder.encode(frame.rgbaColor, cFrame->jpegRGBA8Color, jpegQuality);
    }

    // rgba depth-sized color
    if(!frame.rgbaDepthSizedColor.empty()){
        i->depthSizedColorEncoder.encode(frame.rgbaDepthSizedColor, cFrame->jpegRGBA8DepthSizedColor, jpegQuality);
    }

    // gray bodies id
    if(!frame.depth.empty()){
        i->bodiesIdEncoder.encode(frame.grayBodiesIdMap, cFrame->jpegG8BodiesIdMap, jpegQuality);
    }

    // depth
    if(!frame.depth.empty()){
        i->depthEncoder.encode(frame.depth, cFrame->fpfDepth);
    }

    // infra
    if(!frame.infra.empty()){
        i->infraEncoder.encode(frame.infra, cFrame->fpfInfra);
    }

    // cloud
    if(!frame.cloud.empty()){

        // update id
        size_t sizeCloud = frame.cloud.size();
        if(i->indices1D.size() < sizeCloud){
            i->indices1D.resize(sizeCloud);
        }
        std::iota(std::begin(i->indices1D), std::begin(i->indices1D) + sizeCloud, 0);

        // resize cloud data
        // data mapping: XXYYZZRGB0
        i->processedCloud.resize(sizeCloud *10);

        auto s8 = std::span<std::uint8_t>(
            reinterpret_cast<std::uint8_t*>(i->processedCloud.get_data()),
            i->processedCloud.size()/4
        );
        auto s16 = std::span<std::uint16_t>(
            reinterpret_cast<std::uint16_t*>(i->processedCloud.get_data()),
            i->processedCloud.size()/2
        );

        if(frame.mode != DCMode::Merged){
            std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + sizeCloud, [&](size_t id){
                s16[id]               = static_cast<std::uint16_t>(-frame.cloud.vertices[id].x()*1000.f + 4096); // X
                s16[sizeCloud   + id] = static_cast<std::uint16_t>(-frame.cloud.vertices[id].y()*1000.f + 4096); // Y
                s16[2*sizeCloud + id] = static_cast<std::uint16_t>(frame.cloud.vertices[id].z()*1000.f);         // Z
                s8[6*sizeCloud + id]  = static_cast<std::uint8_t>(frame.cloud.colors[id].x()*255.f);
                s8[7*sizeCloud + id]  = static_cast<std::uint8_t>(frame.cloud.colors[id].y()*255.f);
                s8[8*sizeCloud + id]  = static_cast<std::uint8_t>(frame.cloud.colors[id].z()*255.f);
            });
        }else{
            std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + sizeCloud, [&](size_t id){
                s16[id]               = static_cast<std::uint16_t>(frame.cloud.vertices[id].x()*1000.f + 32768); // X
                s16[sizeCloud   + id] = static_cast<std::uint16_t>(frame.cloud.vertices[id].y()*1000.f + 32768); // Y
                s16[2*sizeCloud + id] = static_cast<std::uint16_t>(frame.cloud.vertices[id].z()*1000.f + 32768); // Z
                s8[6*sizeCloud + id]  = static_cast<std::uint8_t>(frame.cloud.colors[id].x()*255.f);
                s8[7*sizeCloud + id]  = static_cast<std::uint8_t>(frame.cloud.colors[id].y()*255.f);
                s8[8*sizeCloud + id]  = static_cast<std::uint8_t>(frame.cloud.colors[id].z()*255.f);
            });
        }

        i->cloudEncoder.encode(
            i->processedCloud,
            cFrame->fpfColoredCloud
        );
    }

    // imu
    cFrame->imu = frame.imu;

    // audio
    cFrame->audioFrames = frame.audioFrames;

    // bodies
    // ...
}

auto DCFrameCompressor::compress(DCFrame &frame, int jpegQuality) -> std::unique_ptr<DCCompressedFrame>{
    auto cFrame = std::make_unique<DCCompressedFrame>();
    compress(frame, jpegQuality, cFrame.get());
    return cFrame;
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

