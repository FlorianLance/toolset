
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
    // data::JpegEncoder bodiesIdEncoder;
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
    if(frame.volumesB.contains(DCVolumeBufferType::ColoredCloud)){
        dFrame->validVerticesCount = std::get<geo::ColorCloud>(frame.volumesB[DCVolumeBufferType::ColoredCloud]).size();
    }else{
        dFrame->validVerticesCount = 0;
    }

    // data buffers
    // # calibration
    if(auto iBuffer = frame.data_buffer(DCDataBufferType::Calibration)){
        dFrame->insert_data_buffer(DCDataBufferType::Calibration, DCCompressionMode::None, iBuffer->byte_span());
    }    
    // # imu
    if(dfgS.addImu){
        if(auto iBuffer = frame.data_buffer(DCDataBufferType::IMU)){
            dFrame->insert_data_buffer(DCDataBufferType::IMU, DCCompressionMode::None, iBuffer->byte_span());
        }
    }

    // image buffers
    // # depth
    if(dfgS.addDepth){

        if(auto iBuffer = frame.image_buffer<std::uint16_t>(DCImageBufferType::Depth16)){

            if(dfgS.depthCM == DCCompressionMode::FastPFor){

                if(auto eBuffer = dFrame->insert_image_buffer(DCImageBufferType::Depth16, DCCompressionMode::FastPFor)){
                    i->depthEncoder.encode(*iBuffer, *eBuffer);
                }

            }else if(dfgS.depthCM == DCCompressionMode::None){                
                dFrame->insert_image_buffer(DCImageBufferType::Depth16, DCCompressionMode::None, iBuffer->width, iBuffer->height, 2, iBuffer->byte_span());
            }
        }
    }

    // # infrared
    if(dfgS.addInfra){

        if(auto iBuffer = frame.image_buffer<std::uint16_t>(DCImageBufferType::Infrared16)){

            if(dfgS.infraCM == DCCompressionMode::FastPFor){

                if(auto eBuffer = dFrame->insert_image_buffer(DCImageBufferType::Infrared16, DCCompressionMode::FastPFor)){
                    i->infraEncoder.encode(*iBuffer, *eBuffer);
                }

            }else if(dfgS.infraCM == DCCompressionMode::None){
                dFrame->insert_image_buffer(DCImageBufferType::Infrared16, DCCompressionMode::None, iBuffer->width, iBuffer->height, 2, iBuffer->byte_span());
            }
        }
    }

    // # depth-sized color
    if(dfgS.addDepthSizedColor){

        if(auto iBuffer = frame.image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8)){

            if(dfgS.depthSizedColorCM == DCCompressionMode::JPEG){

                if(auto eBuffer = dFrame->insert_image_buffer(DCImageBufferType::DepthSizedColorRGBA8, DCCompressionMode::JPEG)){
                    i->depthSizedColorEncoder.encode(*iBuffer, *eBuffer, dfgS.depthSizedColorJPEGCQ);
                }

            }else if(dfgS.depthSizedColorCM == DCCompressionMode::None){                
                dFrame->insert_image_buffer(DCImageBufferType::DepthSizedColorRGBA8, DCCompressionMode::None, iBuffer->width, iBuffer->height, 4, iBuffer->byte_span());
            }
        }
    }

    // # original size color
    if(dfgS.addOriginalSizeColor){

        if(auto iBuffer = frame.image_buffer<ColorRGBA8>(DCImageBufferType::OriginalColorRGBA8)){

            if(dfgS.originalSizeColorCM == DCCompressionMode::JPEG){

                if(auto eBuffer = dFrame->insert_image_buffer(DCImageBufferType::OriginalColorRGBA8, DCCompressionMode::JPEG)){
                    i->colorEncoder.encode(*iBuffer, *eBuffer, dfgS.depthSizedColorJPEGCQ);
                }

            }else if(dfgS.originalSizeColorCM == DCCompressionMode::None){
                dFrame->insert_image_buffer(DCImageBufferType::OriginalColorRGBA8, DCCompressionMode::None, iBuffer->width, iBuffer->height, 4, iBuffer->byte_span());
            }
        }
    }

    // # bodies id map
    if(dfgS.addBodiesId){
        if(auto iBuffer = frame.image_buffer<ColorGray8>(DCImageBufferType::BodiesIdMap8)){
            dFrame->insert_image_buffer(DCImageBufferType::BodiesIdMap8, DCCompressionMode::None, iBuffer->width, iBuffer->height, 1, iBuffer->byte_span());
        }
    }
}





// auto DCFrameCompressor::add_cloud(DCMode mode, const geo::ColoredCloudData &cloud, std::vector<uint8_t> &encodedCloudData) -> void{

//     if(!cloud.has_colors() || !cloud.has_vertices()){
//         tool::Log::error("[DCFrameCompressor::add_cloud] Invalid cloud.\n");
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

