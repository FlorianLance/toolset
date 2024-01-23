
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

    std::vector<size_t> indicesValid1D;
    std::vector<std::uint16_t> processedCloudData;
    std::vector<std::uint8_t> processedColorData;
    std::vector<std::int16_t> processedAudioData;


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

    auto jpegSize = width*height*dim;//cFrame->colorHeight*cFrame->colorWidth*4;
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
        dim == 4 ? TJPF_BGRA : TJPF_BGR,
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

auto DCFrameCompressor::add_cloud(
    size_t colorWidth, size_t colorHeight, uint8_t *colorData,
    size_t depthSize, uint16_t *depthData,
    int jpegQuality, DCCompressedFrame *cFrame) -> void{

    auto cData     = reinterpret_cast<const geo::Pt4<uint8_t>*>(colorData);

    // fill indices array
    const auto idV = cFrame->validVerticesCount;
    if(i->indicesValid1D.size() < depthSize){
        i->indicesValid1D.resize(depthSize);
        std::iota(std::begin(i->indicesValid1D), std::end(i->indicesValid1D), 0);
    }

    // fill valid id
    std::vector<size_t> validId;
    validId.reserve(idV);
    for_each(std::execution::unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + depthSize, [&](size_t id){
        if(depthData[id] != dc_invalid_depth_value){
            validId.push_back(id);
        }
    });

    // resize cloud data
    // # vertices
    size_t cloudVerticesBufferSize = idV*3;
    size_t rest = cloudVerticesBufferSize % 128;
    size_t paddeCloudVerticesBufferPaddedSize = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
    if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
        i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
    }
    std::fill(i->processedCloudData.begin(), i->processedCloudData.end(), 0);
    // # colors
    // color frame is always 128 padded
    cFrame->cloudColorWidth  = colorWidth;
    cFrame->cloudColorHeight = colorHeight;
    const size_t colorBufferSize = cFrame->cloudColorWidth*cFrame->cloudColorHeight*3;
    if(i->processedColorData.size() < colorBufferSize){
        i->processedColorData.resize(colorBufferSize);
    }
    std::fill(i->processedColorData.begin(), i->processedColorData.end(), 0);

    // fill data
    for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

        i->processedCloudData[id]         = static_cast<std::uint16_t>(static_cast<std::int32_t>(cData[validId[id]].x())+4096);
        i->processedCloudData[idV   + id] = static_cast<std::uint16_t>(static_cast<std::int32_t>(cData[validId[id]].y())+4096);
        i->processedCloudData[2*idV + id] = static_cast<std::uint16_t>(cData[validId[id]].z());

        i->processedColorData[id*3+0]     = cData[validId[id]].x();
        i->processedColorData[id*3+1]     = cData[validId[id]].y();
        i->processedColorData[id*3+2]     = cData[validId[id]].z();
    });

    // compress cloud vertices data
    i->compress_lossless_16_bits_128padded_data(paddeCloudVerticesBufferPaddedSize, i->processedCloudData.data(), cFrame->encodedCloudVerticesData);

    // compress cloud color data
    i->compress_jpeg_8_bits_data(cFrame->cloudColorWidth, cFrame->cloudColorHeight, 3, i->processedColorData.data(), cFrame->encodedCloudColorData, jpegQuality);
}


auto DCFrameCompressor::add_frame(DCFrame &frame, int jpegQuality, DCCompressedFrame *cFrame) -> void{

    // create compressed frame
    cFrame->mode            = frame.mode;
    cFrame->idCapture       = frame.idCapture;
    cFrame->afterCaptureTS  = frame.afterCaptureTS;

    // color
    if(!frame.imageColorData.empty()){
        add_color(frame.colorWidth, frame.colorHeight, 3, frame.imageColorData.front().array.data(), jpegQuality, cFrame);
    }

    // depth
    if(!frame.depthData.empty()){
        add_depth(frame.depthWidth, frame.depthHeight, frame.depthData.data(), cFrame);
    }

    // infra
    if(!frame.infraData.empty()){
        add_depth(frame.infraWidth, frame.infraHeight, frame.infraData.data(), cFrame);
    }

    // cloud
    if(!frame.cloud.empty()){

        const auto idV = frame.cloud.size();
        cFrame->validVerticesCount = idV;

        // fill indices array
        if(i->indicesValid1D.size() < idV){
            i->indicesValid1D.resize(idV);
            std::iota(std::begin(i->indicesValid1D), std::end(i->indicesValid1D), 0);
        }

        // resize cloud data
        // # vertices
        size_t cloudVerticesBufferSize = idV*3;
        size_t rest = cloudVerticesBufferSize % 128;
        size_t paddeCloudVerticesBufferPaddedSize = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
        if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
            i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
        }
        std::fill(i->processedCloudData.begin(), i->processedCloudData.begin() + paddeCloudVerticesBufferPaddedSize, 0);
        // # colors
        // we use the same padding than for the vertices
        if(i->processedColorData.size() < paddeCloudVerticesBufferPaddedSize){
            i->processedColorData.resize(paddeCloudVerticesBufferPaddedSize);
        }
        std::fill(i->processedColorData.begin(), i->processedColorData.begin() + paddeCloudVerticesBufferPaddedSize, 0);

        if(frame.mode != DCMode::Merged){

            // process cloud buffer
            for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

                i->processedCloudData[id]         = static_cast<std::uint16_t>((-frame.cloud.vertices[id].x()*1000.f) +4096.f);
                i->processedCloudData[idV   + id] = static_cast<std::uint16_t>((-frame.cloud.vertices[id].y()*1000.f) +4096.f);
                i->processedCloudData[2*idV + id] = static_cast<std::uint16_t>(frame.cloud.vertices[id].z()*1000.f);

                i->processedColorData[id*3+0]     = static_cast<std::uint8_t>(frame.cloud.colors[id].z()*255.f);
                i->processedColorData[id*3+1]     = static_cast<std::uint8_t>(frame.cloud.colors[id].y()*255.f);
                i->processedColorData[id*3+2]     = static_cast<std::uint8_t>(frame.cloud.colors[id].x()*255.f);
            });

        }else{

            // process cloud buffer
            for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

                i->processedCloudData[id]         = static_cast<std::uint16_t>(frame.cloud.vertices[id].x()*1000.f + 32768.f);
                i->processedCloudData[idV   + id] = static_cast<std::uint16_t>(frame.cloud.vertices[id].y()*1000.f + 32768.f);
                i->processedCloudData[2*idV + id] = static_cast<std::uint16_t>(frame.cloud.vertices[id].z()*1000.f + 32768.f);

                i->processedColorData[id*3+0]     = static_cast<std::uint8_t>(frame.cloud.colors[id].z()*255.f);
                i->processedColorData[id*3+1]     = static_cast<std::uint8_t>(frame.cloud.colors[id].y()*255.f);
                i->processedColorData[id*3+2]     = static_cast<std::uint8_t>(frame.cloud.colors[id].x()*255.f);
            });
        }


        // compress processed cloud data
        i->compress_lossless_16_bits_128padded_data(
            paddeCloudVerticesBufferPaddedSize,
            i->processedCloudData.data(),
            cFrame->encodedCloudVerticesData
        );

        // compress processed color data
        size_t cloudColorSize    = paddeCloudVerticesBufferPaddedSize/3;
        cFrame->cloudColorWidth  = cloudColorSize/128;
        cFrame->cloudColorHeight = 128;
        i->compress_jpeg_8_bits_data(cFrame->cloudColorWidth, cFrame->cloudColorHeight , 3, i->processedColorData.data(), cFrame->encodedCloudColorData, jpegQuality);
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


