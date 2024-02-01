
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

    // jpeg compression
    tjhandle jpegCompressor = nullptr;
    unsigned char *tjCompressedImage = nullptr;
    size_t currentSizeAllocatedToJPegCompressor = 0;

    std::vector<size_t> indices1D;
    std::vector<geo::Pt2<size_t>> indicesValid1D;
    std::vector<std::uint16_t> processedCloudData;
    // std::vector<std::int16_t> processedAudioData;

    auto encode_to_jpeg(size_t width, size_t height, TJPF format, const std::uint8_t *sourceData, int jpegQuality) -> std::span<std::uint8_t>;
    auto compress_lossless_16_bits_128padded_data(size_t sourceSize, std::uint16_t *sourceData, std::vector<std::uint8_t> &encodedData) -> bool;
};

auto DCFrameCompressor::Impl::encode_to_jpeg(size_t width, size_t height, TJPF format, const uint8_t *sourceData, int jpegQuality) -> std::span<uint8_t>{

    size_t dim = 0;
    switch (format) {
    case TJPF_RGB:
        dim = 3;
        break;
    case TJPF_RGBA:
        dim = 4;
        break;
    case TJPF_BGR:
        dim = 3;
        break;
    case TJPF_BGRA:
        dim = 4;
        break;
    default:
        return {};
    }

    size_t imageSize = width * height * dim;
    if(tjCompressedImage == nullptr){
        tjCompressedImage = tjAlloc(static_cast<int>(currentSizeAllocatedToJPegCompressor = imageSize));
    }else if(currentSizeAllocatedToJPegCompressor < imageSize){
        tjFree(tjCompressedImage);
        tjCompressedImage = tjAlloc(static_cast<int>(currentSizeAllocatedToJPegCompressor = imageSize));
    }

    long unsigned int jpegColorSize = 0;
    int ret = tjCompress2(
        jpegCompressor,
        sourceData,
        static_cast<int>(width),
        0,  // pitch
        static_cast<int>(height),
        format,
        &tjCompressedImage, &jpegColorSize,
        TJSAMP_444,
        jpegQuality, TJFLAG_NOREALLOC | TJFLAG_FASTDCT
        );

    if(ret == -1){
        Logger::error(std::format("[DCFrameCompressor:encode_to_jpeg] tjCompress2 error with code: {}\n", tjGetErrorStr2(jpegCompressor)));
        return {};
    }

    return {
        reinterpret_cast<std::uint8_t*>(tjCompressedImage),
        static_cast<size_t>(jpegColorSize)
    };
}


auto DCFrameCompressor::Impl::compress_lossless_16_bits_128padded_data(size_t sourceSize, std::uint16_t *sourceData, std::vector<std::uint8_t> &encodedData)  -> bool{

    if(sourceSize % 128 != 0){
        Logger::error("[DCFrameCompressor:compress_lossless_16_bits_128padded_data] Data is not 128 padded.\n");
        return false;
    }

    // resize encoding buffer
    encodedData.resize(sourceSize*2);

    // encode
    size_t encodedBytesNb = p4nzenc128v16(
        sourceData,
        sourceSize,
        encodedData.data()
    );
    encodedData.resize(encodedBytesNb);

    if(encodedBytesNb == 0){
        Logger::error("[DCFrameCompressor:compress_lossless_16_bits_128padded_data] p4nzenc128v16 failure.\n");
        return false;
    }

    return true;
}




DCFrameCompressor::DCFrameCompressor() : i(std::make_unique<Impl>()){
    i->jpegCompressor = tjInitCompress();
}

DCFrameCompressor::~DCFrameCompressor(){
    if(i->tjCompressedImage != nullptr){
        tjFree(i->tjCompressedImage);
    }
    tjDestroy(i->jpegCompressor);
}

auto DCFrameCompressor::encode_to_jpeg(size_t width, size_t height, std::span<ColorRGBA8> image, ImageBuffer<uint8_t> &encodedImage, int jpegQuality) -> bool{

    if(auto encodedData = i->encode_to_jpeg(width, height, TJPF_RGBA, reinterpret_cast<std::uint8_t*>(image.data()), jpegQuality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width  = width;
        encodedImage.height = height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto DCFrameCompressor::encode_to_jpeg(size_t width, size_t height, std::span<ColorRGB8> image, ImageBuffer<uint8_t> &encodedImage, int jpegQuality) -> bool{

    if(auto encodedData = i->encode_to_jpeg(width, height, TJPF_RGB, reinterpret_cast<std::uint8_t*>(image.data()), jpegQuality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width  = width;
        encodedImage.height = height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto DCFrameCompressor::encode_to_jpeg(const ImageBuffer<ColorRGB8> &image, ImageBuffer<uint8_t> &encodedImage, int jpegQuality) -> bool{

    if(auto encodedData = i->encode_to_jpeg(image.width, image.height, TJPF_RGB, image.get_raw_data(), jpegQuality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width = image.width;
        encodedImage.height = image.height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto DCFrameCompressor::encode_to_jpeg(const ImageBuffer<ColorRGBA8> &image, ImageBuffer<uint8_t> &encodedImage, int jpegQuality) -> bool{

    if(auto encodedData = i->encode_to_jpeg(image.width, image.height, TJPF_RGBA, image.get_raw_data(), jpegQuality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width = image.width;
        encodedImage.height = image.height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto DCFrameCompressor::encode_to_lossless_16_bits(size_t width, size_t height, std::span<uint16_t> data, ImageBuffer<uint8_t> &encodedData) -> bool{

    if(i->compress_lossless_16_bits_128padded_data(
            data.size(), data.data(), encodedData.buffer.values
    )){
        encodedData.width  = width;
        encodedData.height = height;
        return true;
    }
    return false;
}

auto DCFrameCompressor::encode_to_lossless_16_bits(ImageBuffer<uint16_t> &data, ImageBuffer<uint8_t> &encodedData) -> bool{

    if(i->compress_lossless_16_bits_128padded_data(
            data.size(), data.get_data(), encodedData.buffer.values
        )){
        encodedData.width  = data.width;
        encodedData.height = data.height;
        return true;
    }
    return false;
}

auto DCFrameCompressor::encode_colored_cloud_to_lossless_16_bits(std::span<uint16_t> depthData, std::span<ColorRGBA8> depthSizedColorData, std::span<geo::Pt3<int16_t> > depthCloudData, Buffer<std::uint8_t> &encodedColoredCloudData) -> bool{

    if(depthData.empty() || depthSizedColorData.empty() || depthCloudData.empty()){
        tool::Logger::error("[DCFrameCompressor::encode_colored_cloud_to_lossless_16_bits] Invalid inputs.\n");
        return false;
    }

    // update id
    if(i->indicesValid1D.size() < depthData.size()){
        i->indicesValid1D.resize(depthData.size());
    }
    size_t validVerticesCount = 0;
    for(size_t id = 0; id < depthData.size(); ++id){
        if(depthData[id] != dc_invalid_depth_value){
            i->indicesValid1D[validVerticesCount] = {validVerticesCount,id};
            ++validVerticesCount;
        }
    }

    // resize cloud data
    // data mapping: XXYYZZRGB0
    size_t cloudVerticesBufferSize              = validVerticesCount*5;
    size_t rest                                 = cloudVerticesBufferSize % 128;
    size_t paddeCloudVerticesBufferPaddedSize   = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
    if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
        i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
    }
    std::fill(i->processedCloudData.begin(), i->processedCloudData.end(), 0);

    auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(i->processedCloudData.data());

    // fill data
    std::for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + validVerticesCount, [&](const geo::Pt2<size_t> id){
        i->processedCloudData[id.x()]         = static_cast<std::uint16_t>(static_cast<std::int32_t>(depthCloudData[id.y()].x())+4096); // X
        i->processedCloudData[validVerticesCount   + id.x()] = static_cast<std::uint16_t>(static_cast<std::int32_t>(depthCloudData[id.y()].y())+4096); // Y
        i->processedCloudData[2*validVerticesCount + id.x()] = static_cast<std::uint16_t>(depthCloudData[id.y()].z());                                 // Z
        processedCloudData8[6*validVerticesCount + id.x()]    = depthSizedColorData[id.y()].r();
        processedCloudData8[7*validVerticesCount + id.x()]    = depthSizedColorData[id.y()].g();
        processedCloudData8[8*validVerticesCount + id.x()]    = depthSizedColorData[id.y()].b();
    });

    // compress cloud vertices data
    return i->compress_lossless_16_bits_128padded_data(
        paddeCloudVerticesBufferPaddedSize,
        i->processedCloudData.data(),
        encodedColoredCloudData.values
    );
}

auto DCFrameCompressor::encode_colored_cloud_to_lossless_16_bits(geo::ColoredCloudData &cloud, Buffer<std::uint8_t> &encodedColoredCloudData) -> bool{

    if(cloud.empty()){
        tool::Logger::error("[DCFrameCompressor::encode_colored_cloud_to_lossless_16_bits] Invalid inputs.\n");
        return false;
    }

    // update id
    size_t sizeCloud = cloud.size();
    if(i->indices1D.size() < sizeCloud){
        i->indices1D.resize(sizeCloud);
    }
    std::iota(std::begin(i->indices1D), std::begin(i->indices1D) + sizeCloud, 0);

    // resize cloud data
    // data mapping: XXYYZZRGB0
    size_t cloudVerticesBufferSize              = sizeCloud*5;
    size_t rest                                 = cloudVerticesBufferSize % 128;
    size_t paddeCloudVerticesBufferPaddedSize   = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
    if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
        i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
    }
    std::fill(i->processedCloudData.begin(), i->processedCloudData.end(), 0);

    auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(i->processedCloudData.data());


    // fill data
    std::for_each(std::execution::par_unseq, std::begin(i->indices1D), std::begin(i->indices1D) + sizeCloud, [&](size_t id){

        i->processedCloudData[id]               = static_cast<std::uint16_t>(static_cast<std::int32_t>(-cloud.vertices[id].x()*1000.f)+4096); // X
        i->processedCloudData[sizeCloud   + id] = static_cast<std::uint16_t>(static_cast<std::int32_t>(-cloud.vertices[id].y()*1000.f)+4096); // Y
        i->processedCloudData[2*sizeCloud + id] = static_cast<std::uint16_t>(cloud.vertices[id].z()*1000.f);                                  // Z
        processedCloudData8[6*sizeCloud + id]   = cloud.colors[id].x()*255.f;
        processedCloudData8[7*sizeCloud + id]   = cloud.colors[id].y()*255.f;
        processedCloudData8[8*sizeCloud + id]   = cloud.colors[id].z()*255.f;
    });

    // compress cloud vertices data
    return i->compress_lossless_16_bits_128padded_data(
        paddeCloudVerticesBufferPaddedSize,
        i->processedCloudData.data(),
        encodedColoredCloudData.values
    );
}




// auto DCFrameCompressor::Impl::compress_jpeg_8_bits_data(size_t width, size_t height, size_t dim, std::uint8_t *data, std::vector<std::uint8_t> &encodedData, int jpegQuality) -> bool{

//     if(dim < 3 || dim > 4){
//         Logger::error(std::format("[DCFrameCompressor:compress_jpeg_8_bits_data] Invalid dimension {}.\n", dim));
//         return false;
//     }

//     auto jpegSize = width*height*dim;
//     if(tjCompressedImage == nullptr){
//         tjCompressedImage = tjAlloc(static_cast<int>(currentSizeAllocatedToJPegCompressor = jpegSize));
//     }else if(currentSizeAllocatedToJPegCompressor < jpegSize){
//         tjFree(tjCompressedImage);
//         tjCompressedImage = tjAlloc(static_cast<int>(currentSizeAllocatedToJPegCompressor = jpegSize));
//     }

//     long unsigned int jpegColorSize = 0;
//     int ret = tjCompress2(
//         jpegCompressor,
//         data,
//         static_cast<int>(width),
//         0,
//         static_cast<int>(height),
//         dim == 4 ? TJPF_RGBA : TJPF_RGB,
//         &tjCompressedImage, &jpegColorSize, TJSAMP_444, jpegQuality, TJFLAG_NOREALLOC | TJFLAG_FASTDCT
//     );

//     if(ret == -1){
//         Logger::error(std::format("[DCFrameCompressor:compress_jpeg_8_bits_data] tjCompress2 error with code: {}\n", tjGetErrorStr2(jpegCompressor)));
//         return false;
//     }

//     encodedData.resize(jpegColorSize);
//     std::copy(tjCompressedImage, tjCompressedImage + jpegColorSize, encodedData.begin());

//     return true;
// }



// auto DCFrameCompressor::add_cloud(std::span<std::uint16_t> depthData, std::span<ColorRGBA8> depthSizedColorData, std::span<geo::Pt3<std::int16_t>> depthCloudData, std::vector<std::uint8_t> &encodedCloudData)  -> void{

//     if(depthSizedColorData.empty() || depthCloudData.empty()){
//         tool::Logger::error("[DCFrameCompressor::add_cloud] Invalid cloud.\n");
//         return;
//     }

//     if(i->indicesValid1D.size() < depthData.size()){
//         i->indicesValid1D.resize(depthData.size());
//     }

//     size_t idV = 0;
//     for(size_t id = 0; id < depthData.size(); ++id){
//         if(depthData[id] != dc_invalid_depth_value){
//             i->indicesValid1D[idV] = {idV,id};
//             ++idV;
//         }
//     }

//     // resize cloud data
//     // data mapping: XXYYZZRGB0
//     size_t cloudVerticesBufferSize              = idV*5;
//     size_t rest                                 = cloudVerticesBufferSize % 128;
//     size_t paddeCloudVerticesBufferPaddedSize   = rest == 0 ? cloudVerticesBufferSize : (cloudVerticesBufferSize + 128 - rest);
//     if(i->processedCloudData.size() < paddeCloudVerticesBufferPaddedSize){
//         i->processedCloudData.resize(paddeCloudVerticesBufferPaddedSize);
//     }
//     std::fill(i->processedCloudData.begin(), i->processedCloudData.end(), 0);

//     auto processedCloudData8 = reinterpret_cast<std::uint8_t*>(i->processedCloudData.data());

//     // fill data
//     std::for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](const geo::Pt2<size_t> id){
//         i->processedCloudData[id.x()]         = static_cast<std::uint16_t>(static_cast<std::int32_t>(depthCloudData[id.y()].x())+4096); // X
//         i->processedCloudData[idV   + id.x()] = static_cast<std::uint16_t>(static_cast<std::int32_t>(depthCloudData[id.y()].y())+4096); // Y
//         i->processedCloudData[2*idV + id.x()] = static_cast<std::uint16_t>(depthCloudData[id.y()].z());                                 // Z
//         processedCloudData8[6*idV + id.x()]    = depthSizedColorData[id.y()].r();
//         processedCloudData8[7*idV + id.x()]    = depthSizedColorData[id.y()].g();
//         processedCloudData8[8*idV + id.x()]    = depthSizedColorData[id.y()].b();
//     });

//     // compress cloud vertices data
//     i->compress_lossless_16_bits_128padded_data(paddeCloudVerticesBufferPaddedSize, i->processedCloudData.data(), encodedCloudData);
// }

auto DCFrameCompressor::add_cloud(DCMode mode, const geo::ColoredCloudData &cloud, std::vector<uint8_t> &encodedCloudData) -> void{

    if(!cloud.has_colors() || !cloud.has_vertices()){
        tool::Logger::error("[DCFrameCompressor::add_cloud] Invalid cloud.\n");
        return;
    }

    size_t validVerticesCount = cloud.size();
    if(i->indices1D.size() < validVerticesCount){
        i->indices1D.resize(validVerticesCount);
    }
    std::iota(std::begin(i->indices1D), std::begin(i->indices1D) + validVerticesCount, 0);

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
    cFrame->mode               = frame.mode;
    cFrame->idCapture          = frame.idCapture;
    cFrame->afterCaptureTS     = frame.afterCaptureTS;
    cFrame->receivedTS         = frame.receivedTS;
    cFrame->validVerticesCount = frame.cloud.size();

    // color image
    if(!frame.colorImage.empty()){
        encode_to_jpeg(
            frame.colorImage.width, frame.colorImage.height, frame.colorImage, cFrame->encodedColorImage, jpegQuality
        );
    }

    // depth-sized color image
    if(!frame.depthSizedColorImage.empty()){
        encode_to_jpeg(
            frame.depthSizedColorImage.width, frame.depthSizedColorImage.height, frame.depthSizedColorImage, cFrame->encodedDepthSizedColorImage, jpegQuality
        );
    }

    // depth data
    if(!frame.depthData.empty()){
        encode_to_lossless_16_bits(
            frame.depthData, cFrame->encodedDepthData
        );
    }

    // infra data
    if(!frame.infraData.empty()){
        encode_to_lossless_16_bits(
            frame.infraData, cFrame->encodedInfraData
        );
    }

    // cloud
    if(!frame.cloud.empty()){
        encode_colored_cloud_to_lossless_16_bits(frame.cloud, cFrame->encodedColoredCloudData);
    }

    // imu
    // if(frame.imuSample.has_value()){
    //     cFrame->imuSample = frame.imuSample;
    // }

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


