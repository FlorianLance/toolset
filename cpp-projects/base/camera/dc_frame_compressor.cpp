
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

// turbopfor
#include "TurboPFor/vp4.h"

// local
// # utility
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::cam;

struct DCFrameCompressor::Impl{

    tjhandle jpegCompressor = nullptr;
    unsigned char *tjCompressedImage = nullptr;
    size_t currentSizeAllocatedToJPegCompressor = 0;

    std::vector<size_t> indices1D;
    std::vector<geo::Pt2<size_t>> indicesValid1D;
    std::vector<std::uint16_t> processedCloudData;
    // std::vector<std::int16_t> processedAudioData;

    auto compress_jpeg_8_bits_data(size_t width, size_t height, size_t dim, std::uint8_t *data, std::vector<std::uint8_t> &encodedData, int jpegQuality) -> bool;
    auto compress_lossless_16_bits_128padded_data(size_t size, std::uint16_t *data, std::vector<std::uint8_t> &encodedData) -> void;
};

DCFrameCompressor::DCFrameCompressor() : i(std::make_unique<Impl>()){
    i->jpegCompressor = tjInitCompress();
}

DCFrameCompressor::~DCFrameCompressor(){
    if(i->tjCompressedImage != nullptr){
        tjFree(i->tjCompressedImage);
    }
    tjDestroy(i->jpegCompressor);

}

auto DCFrameCompressor::Impl::compress_jpeg_8_bits_data(size_t width, size_t height, size_t dim, std::uint8_t *data, std::vector<std::uint8_t> &encodedData, int jpegQuality) -> bool{

    if(dim < 3 || dim > 4){
        Logger::error(std::format("[DCFrameCompressor:compress_jpeg_8_bits_data] Invalid dimension {}.\n", dim));
        return false;
    }

    auto jpegSize = width*height*dim;
    if(tjCompressedImage == nullptr){
        tjCompressedImage = tjAlloc(static_cast<int>(currentSizeAllocatedToJPegCompressor = jpegSize));
    }else if(currentSizeAllocatedToJPegCompressor < jpegSize){
        tjFree(tjCompressedImage);
        tjCompressedImage = tjAlloc(static_cast<int>(currentSizeAllocatedToJPegCompressor = jpegSize));
    }

    long unsigned int jpegColorSize = 0;
    int ret = tjCompress2(
        jpegCompressor,
        data,
        static_cast<int>(width),
        0,
        static_cast<int>(height),
        dim == 4 ? TJPF_RGBA : TJPF_RGB,
        &tjCompressedImage, &jpegColorSize, TJSAMP_444, jpegQuality, TJFLAG_NOREALLOC | TJFLAG_FASTDCT
    );

    if(ret == -1){
        Logger::error(std::format("[DCFrameCompressor:compress_jpeg_8_bits_data] tjCompress2 error with code: {}\n", tjGetErrorStr2(jpegCompressor)));
        return false;
    }

    encodedData.resize(jpegColorSize);
    std::copy(tjCompressedImage, tjCompressedImage + jpegColorSize, encodedData.begin());

    return true;
}

auto DCFrameCompressor::Impl::compress_lossless_16_bits_128padded_data(size_t size, std::uint16_t *data, std::vector<std::uint8_t> &encodedData)  -> void{

    if(size % 128 != 0){
        Logger::error("[DCFrameCompressor:compress_lossless_16_bits_128padded_data] Data is not 128 padded.\n");
        return;
    }

    // resize encoding buffer
    encodedData.resize(size*2);

    // encode
    size_t encodedBytesNb = p4nzenc128v16(
        data,
        size,
        encodedData.data()
    );
    encodedData.resize(encodedBytesNb);
}

auto DCFrameCompressor::add_color(size_t width, size_t height, int numChannels, uint8_t *data, int jpegQuality, DCCompressedFrame *cFrame) -> void{
    cFrame->colorWidth  = width;
    cFrame->colorHeight = height;
    i->compress_jpeg_8_bits_data(
        cFrame->colorWidth, cFrame->colorHeight, numChannels,
        data, cFrame->encodedColorData, jpegQuality
    );
}

auto DCFrameCompressor::add_depth_sized_color(size_t width, size_t height, int numChannels, uint8_t *data, int jpegQuality, DCCompressedFrame *cFrame) -> void{
    cFrame->depthSizedColorWidth  = width;
    cFrame->depthSizedColorHeight = height;
    i->compress_jpeg_8_bits_data(
        cFrame->depthSizedColorWidth, cFrame->depthSizedColorHeight, numChannels,
        data, cFrame->encodedDepthSizedColorData, jpegQuality
    );
}

auto DCFrameCompressor::add_depth(size_t width, size_t height, uint16_t *data, DCCompressedFrame *cFrame) -> void{
    cFrame->depthWidth  = width;
    cFrame->depthHeight = height;
    i->compress_lossless_16_bits_128padded_data(
        cFrame->depthWidth*cFrame->depthHeight,
        data, cFrame->encodedDepthData
    );
}

auto DCFrameCompressor::add_infra(size_t width, size_t height, uint16_t *data, DCCompressedFrame *cFrame) -> void{
    cFrame->infraWidth  = width;
    cFrame->infraHeight = height;
    i->compress_lossless_16_bits_128padded_data(
        cFrame->infraWidth*cFrame->infraHeight,
        data, cFrame->encodedInfraData
    );
}


auto DCFrameCompressor::add_cloud(std::span<std::uint16_t> depthData, std::span<ColorRGBA8> depthSizedColorData, std::span<geo::Pt3<std::int16_t>> depthCloudData, std::vector<std::uint8_t> &encodedCloudData)  -> void{

    if(depthSizedColorData.empty() || depthCloudData.empty()){
        tool::Logger::error("[DCFrameCompressor::add_cloud] Invalid cloud.\n");
        return;
    }

    if(i->indicesValid1D.size() < depthData.size()){
        i->indicesValid1D.resize(depthData.size());
    }

    size_t idV = 0;
    for(size_t id = 0; id < depthData.size(); ++id){
        if(depthData[id] != dc_invalid_depth_value){
            i->indicesValid1D[idV] = {idV,id};
            ++idV;
        }
    }

    // resize cloud data
    // data mapping: XXYYZZRGB0
    size_t cloudVerticesBufferSize              = idV*5;
    size_t rest                                 = cloudVerticesBufferSize % 128;
    size_t paddeCloudVerticesBufferPaddedSize   = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
    if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
        i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
    }
    std::fill(i->processedCloudData.begin(), i->processedCloudData.end(), 0);

    auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(i->processedCloudData.data());

    // fill data
    std::for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](const geo::Pt2<size_t> id){
        i->processedCloudData[id.x()]         = static_cast<std::uint16_t>(static_cast<std::int32_t>(depthCloudData[id.y()].x())+4096); // X
        i->processedCloudData[idV   + id.x()] = static_cast<std::uint16_t>(static_cast<std::int32_t>(depthCloudData[id.y()].y())+4096); // Y
        i->processedCloudData[2*idV + id.x()] = static_cast<std::uint16_t>(depthCloudData[id.y()].z());                                 // Z
        processedCloudData8[6*idV + id.x()]    = depthSizedColorData[id.y()].r();
        processedCloudData8[7*idV + id.x()]    = depthSizedColorData[id.y()].g();
        processedCloudData8[8*idV + id.x()]    = depthSizedColorData[id.y()].b();
    });

    // compress cloud vertices data
    i->compress_lossless_16_bits_128padded_data(paddeCloudVerticesBufferPaddedSize, i->processedCloudData.data(), encodedCloudData);
}

auto DCFrameCompressor::add_cloud(DCMode mode, const geo::ColoredCloudData &cloud, std::vector<uint8_t> &encodedCloudData) -> void{

    if(!cloud.has_colors() || !cloud.has_vertices()){
        tool::Logger::error("[DCFrameCompressor::add_cloud] Invalid cloud.\n");
        return;
    }

    size_t validVerticesCount = cloud.size();
    if(i->indices1D.size() < validVerticesCount){
        i->indices1D.resize(validVerticesCount);
    }

    // resize cloud data
    // data mapping: XXYYZZRGB0
    size_t cloudVerticesBufferSize = validVerticesCount*5;
    size_t rest = cloudVerticesBufferSize % 128;
    size_t paddeCloudVerticesBufferPaddedSize = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
    if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
        i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
    }
    std::fill(i->processedCloudData.begin(), i->processedCloudData.begin() + paddeCloudVerticesBufferPaddedSize, 0);    

    auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(i->processedCloudData.data());

    // process cloud buffer
    if(mode != DCMode::Merged){
        std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + validVerticesCount, [&](size_t id){
            i->processedCloudData[id]                             = static_cast<std::uint16_t>((-cloud.vertices[id].x()*1000.f) +4096.f);
            i->processedCloudData[validVerticesCount   + id]      = static_cast<std::uint16_t>((-cloud.vertices[id].y()*1000.f) +4096.f);
            i->processedCloudData[2*validVerticesCount + id]      = static_cast<std::uint16_t>(cloud.vertices[id].z()*1000.f);            
            processedCloudData8[6*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].x()*255.f);
            processedCloudData8[7*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].y()*255.f);
            processedCloudData8[8*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].z()*255.f);
        });

    }else{
        std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + cloud.size(), [&](size_t id){
            i->processedCloudData[id]                             = static_cast<std::uint16_t>(cloud.vertices[id].x()*1000.f + 32768.f);
            i->processedCloudData[validVerticesCount   + id]      = static_cast<std::uint16_t>(cloud.vertices[id].y()*1000.f + 32768.f);
            i->processedCloudData[2*validVerticesCount + id]      = static_cast<std::uint16_t>(cloud.vertices[id].z()*1000.f + 32768.f);
            processedCloudData8[6*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].x()*255.f);
            processedCloudData8[7*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].y()*255.f);
            processedCloudData8[8*validVerticesCount + id]        = static_cast<std::uint8_t>(cloud.colors[id].z()*255.f);
        });
    }

    // compress processed cloud data
    i->compress_lossless_16_bits_128padded_data(
        paddeCloudVerticesBufferPaddedSize,
        i->processedCloudData.data(),
        encodedCloudData
    );
}

auto DCFrameCompressor::add_frame(DCFrame &frame, int jpegQuality, DCCompressedFrame *cFrame) -> void{

    // create compressed frame
    cFrame->mode            = frame.mode;
    cFrame->idCapture       = frame.idCapture;
    cFrame->afterCaptureTS  = frame.afterCaptureTS;
    cFrame->receivedTS      = frame.receivedTS;

    // color image
    if(!frame.imageColorData.empty()){        
        add_color(
            frame.colorWidth, frame.colorHeight, 3,
            reinterpret_cast<std::uint8_t*>(frame.imageColorData.data()), jpegQuality, cFrame
        );
    }

    // depth-sized color image
    if(!frame.depthSizedImageColorData.empty()){
        add_depth_sized_color(
            frame.depthWidth, frame.depthWidth, 3,
            reinterpret_cast<std::uint8_t*>(frame.depthSizedImageColorData.data()), jpegQuality, cFrame
        );
    }

    // depth data
    if(!frame.depthData.empty()){
        add_depth(frame.depthWidth, frame.depthHeight, frame.depthData.data(), cFrame);
    }

    // infra data
    if(!frame.infraData.empty()){
        add_depth(frame.infraWidth, frame.infraHeight, frame.infraData.data(), cFrame);
    }

    // cloud
    if(!frame.cloud.empty()){
        cFrame->validVerticesCount = frame.cloud.size();
        add_cloud(frame.mode, frame.cloud, cFrame->encodedCloudVerticesData);
    }

    // imu
    if(frame.imuSample.has_value()){
        cFrame->imuSample = frame.imuSample;
    }

    // audio
    cFrame->audioFrames = frame.audioFrames;

    // bodies
    // ...
}

auto DCFrameCompressor::compress_frame(DCFrame &frame, int jpegQuality) -> std::unique_ptr<DCCompressedFrame>{
    auto cFrame = std::make_unique<DCCompressedFrame>();
    add_frame(frame, jpegQuality, cFrame.get());
    return cFrame;
}


