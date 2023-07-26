

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

#include "k4_frame_compressor.hpp"

// std
#include <execution>

// turbojpg
#include <turbojpeg.h>

// turbopfor
#include "TurboPFor/vp4.h"

// local
// # utility
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::camera;

struct K4FrameCompressor::Impl{
    tjhandle jpegCompressor = nullptr;
    unsigned char *tjCompressedImage = nullptr;
    size_t currentSizeAllocatedToJPegCompressor = 0;

    std::vector<size_t> indicesValid1D;
    std::vector<std::uint16_t> processedCloudData;
    std::vector<std::uint8_t> processedColorData;
    std::vector<std::int16_t> processedAudioData;

    bool addColor  = false;
    bool addDepth  = false;
    bool addInfra  = false;
    bool addCloud  = false;
    bool addIMU    = false;
    bool addAudio  = false;
    bool addBodies = false;
};

K4FrameCompressor::K4FrameCompressor() : i(std::make_unique<Impl>()){
    i->jpegCompressor = tjInitCompress();
}
K4FrameCompressor::~K4FrameCompressor(){
    if(i->tjCompressedImage != nullptr){
        tjFree(i->tjCompressedImage);
    }
    tjDestroy(i->jpegCompressor);
}

auto K4FrameCompressor::set_settings(const K4DataSettings &ds) -> void{
    i->addColor  = ds.sendColor;
    i->addDepth  = ds.sendDepth;
    i->addInfra  = ds.sendInfra;
    i->addCloud  = ds.sendCloud;
    i->addIMU    = ds.sendIMU;
    i->addAudio  = ds.sendAudio;
    i->addBodies = ds.sendBodies;
}

auto K4FrameCompressor::compress_jpeg_8_bits_data(size_t width, size_t height, size_t dim, std::uint8_t *data, std::vector<std::uint8_t> &encodedData, int jpegQuality) -> bool{

    if(dim < 3 || dim > 4){
        Logger::error(std::format("Invalid dimension {}.\n", dim));
        return false;
    }

    auto jpegSize = width*height*dim;//cFrame->colorHeight*cFrame->colorWidth*4;
    if(i->tjCompressedImage == nullptr){
        i->tjCompressedImage = tjAlloc(static_cast<int>(i->currentSizeAllocatedToJPegCompressor = jpegSize));
    }else if(i->currentSizeAllocatedToJPegCompressor < jpegSize){
        tjFree(i->tjCompressedImage);
        i->tjCompressedImage = tjAlloc(static_cast<int>(i->currentSizeAllocatedToJPegCompressor = jpegSize));
    }

    long unsigned int jpegColorSize = 0;
    int ret = tjCompress2(
        i->jpegCompressor,
        data,
        static_cast<int>(width),
        0,
        static_cast<int>(height),
        dim == 4 ? TJPF_BGRA : TJPF_BGR,
        &i->tjCompressedImage, &jpegColorSize, TJSAMP_444, jpegQuality, TJFLAG_NOREALLOC | TJFLAG_FASTDCT
    );

    if(ret == -1){
        Logger::error(std::format("[K4FrameCompressor:compress_color_image] tjCompress2 error with code: {}\n", tjGetErrorStr2(i->jpegCompressor)));
        return false;
    }

    encodedData.resize(jpegColorSize);
    std::copy(i->tjCompressedImage, i->tjCompressedImage + jpegColorSize, encodedData.begin());

    return true;
}

auto K4FrameCompressor::compress_lossless_16_bits_128padded_data(size_t size, std::uint16_t *data, std::vector<std::uint8_t> &encodedData)  -> void{

    if(size % 128 != 0){
        Logger::error("[K4FrameCompressor:compress_lossless_16_bits_128padded_data] Data is not 128 padded.\n");
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


auto K4FrameCompressor::compress(
        K4Mode mode,
        std::optional<k4a::image> colorImage, int jpegQuality,
        std::optional<k4a::image> depthImage, size_t validDepthValues,
        std::optional<k4a::image> infraImage,
        std::optional<k4a::image> cloud, std::optional<k4a::calibration> calibration,
        float *audioData, size_t audioSize, K4ImuSample *imuSample) -> std::unique_ptr<K4CompressedFrame>{

    // create compressed frame
    auto cFrame = std::make_unique<K4CompressedFrame>();
    cFrame->mode = mode;

    // color
    if(colorImage.has_value() && i->addColor){
        cFrame->colorWidth  = colorImage->get_width_pixels();
        cFrame->colorHeight = colorImage->get_height_pixels();
        compress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, 4, colorImage.value().get_buffer(), cFrame->encodedColorData, jpegQuality);
    }
    // depth    
    if(depthImage.has_value() && i->addDepth){
        cFrame->depthWidth  = depthImage->get_width_pixels();
        cFrame->depthHeight = depthImage->get_height_pixels();
        compress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight,reinterpret_cast<std::uint16_t*>(depthImage.value().get_buffer()), cFrame->encodedDepthData);
    }

    // infrared
    if(infraImage.has_value() && i->addInfra){
        cFrame->infraWidth  = infraImage->get_width_pixels();
        cFrame->infraHeight = infraImage->get_height_pixels();
        compress_lossless_16_bits_128padded_data(cFrame->infraWidth*cFrame->infraHeight, reinterpret_cast<std::uint16_t*>(infraImage.value().get_buffer()), cFrame->encodedInfraData);
    }

    // cloud
    cFrame->validVerticesCount = validDepthValues;
    if(colorImage.has_value() && depthImage.has_value() && cloud.has_value() && i->addCloud){        

        auto colorData = reinterpret_cast<const geo::Pt4<uint8_t>*>(colorImage.value().get_buffer());
        auto depthData = reinterpret_cast<const uint16_t*>(depthImage.value().get_buffer());
        auto cloudData = reinterpret_cast<geo::Pt3<int16_t>*>(cloud.value().get_buffer());

        // fill indices array
        const auto idV = cFrame->validVerticesCount;
        const size_t depthSize = depthImage.value().get_width_pixels()*depthImage.value().get_height_pixels();
        if(i->indicesValid1D.size() < depthSize){
            i->indicesValid1D.resize(depthSize);
            std::iota(std::begin(i->indicesValid1D), std::end(i->indicesValid1D), 0);
        }

        // fill valid id
        std::vector<size_t> validId;
        validId.reserve(idV);
        for_each(std::execution::unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + depthSize, [&](size_t id){
            if(depthData[id] != k4_invalid_depth_value){
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
        cFrame->cloudColorWidth  = colorImage.value().get_width_pixels();
        cFrame->cloudColorHeight = colorImage.value().get_height_pixels();
        const size_t colorBufferSize = cFrame->cloudColorWidth*cFrame->cloudColorHeight*3;
        if(i->processedColorData.size() < colorBufferSize){
            i->processedColorData.resize(colorBufferSize);
        }
        std::fill(i->processedColorData.begin(), i->processedColorData.end(), 0);

        // fill data
        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            i->processedCloudData[id]         = static_cast<std::uint16_t>(static_cast<std::int32_t>(cloudData[validId[id]].x())+4096);
            i->processedCloudData[idV   + id] = static_cast<std::uint16_t>(static_cast<std::int32_t>(cloudData[validId[id]].y())+4096);
            i->processedCloudData[2*idV + id] = static_cast<std::uint16_t>(cloudData[validId[id]].z());

            i->processedColorData[id*3+0]     = colorData[validId[id]].x();
            i->processedColorData[id*3+1]     = colorData[validId[id]].y();
            i->processedColorData[id*3+2]     = colorData[validId[id]].z();
        });

        // compress cloud vertices data
        compress_lossless_16_bits_128padded_data(paddeCloudVerticesBufferPaddedSize, i->processedCloudData.data(), cFrame->encodedCloudVerticesData);

        // compress cloud color data
        compress_jpeg_8_bits_data(cFrame->cloudColorWidth, cFrame->cloudColorHeight, 3, i->processedColorData.data(), cFrame->encodedCloudColorData, jpegQuality);
    }
    if(calibration.has_value()){
        cFrame->calibration = calibration.value();
    }

    // audio
    if(audioData != nullptr && audioSize > 0 && i->addAudio){
        cFrame->audioFrames.resize(audioSize);
        std::copy(audioData, audioData + 7*audioSize, reinterpret_cast<float*>(cFrame->audioFrames.data()));
    }

    // imu    
    if(imuSample != nullptr && i->addIMU){
        cFrame->imuSample = *imuSample;
    }

    // bodies
    if(i->addBodies){
        // ...
    }

    return cFrame;
}


auto K4FrameCompressor::compress(K4Frame &frame, int jpegQuality) -> std::unique_ptr<K4CompressedFrame>{

    // create compressed frame
    auto cFrame = std::make_unique<K4CompressedFrame>();
    cFrame->mode = frame.mode;
    cFrame->idCapture = frame.idCapture;
    cFrame->afterCaptureTS = frame.afterCaptureTS;

    // color
    if(!frame.imageColorData.empty()){
        cFrame->colorWidth  = frame.colorWidth;
        cFrame->colorHeight = frame.colorHeight;
        compress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, 3, frame.imageColorData.front().array.data(), cFrame->encodedColorData, jpegQuality);        
    }

    // depth
    if(!frame.depthData.empty()){
        cFrame->depthWidth  = frame.depthWidth;
        cFrame->depthHeight = frame.depthHeight;
        compress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, frame.depthData.data(), cFrame->encodedDepthData);
    }

    // infra
    if(!frame.infraData.empty()){
        cFrame->infraWidth  = frame.infraWidth;
        cFrame->infraHeight = frame.infraHeight;
        compress_lossless_16_bits_128padded_data(cFrame->infraWidth*cFrame->infraHeight, frame.infraData.data(), cFrame->encodedInfraData);
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

        if(frame.mode != K4Mode::Merged){

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
        compress_lossless_16_bits_128padded_data(
            paddeCloudVerticesBufferPaddedSize,
            i->processedCloudData.data(),
            cFrame->encodedCloudVerticesData
        );

        // compress processed color data
        size_t cloudColorSize    = paddeCloudVerticesBufferPaddedSize/3;
        cFrame->cloudColorWidth  = cloudColorSize/128;
        cFrame->cloudColorHeight = 128;
        compress_jpeg_8_bits_data(cFrame->cloudColorWidth, cFrame->cloudColorHeight , 3, i->processedColorData.data(), cFrame->encodedCloudColorData, jpegQuality);
    }

    // imu
    if(frame.imuSample.has_value()){
        cFrame->imuSample = frame.imuSample;
    }

    // audio
    cFrame->audioFrames = frame.audioFrames;

    // bodies
    // ...

    return cFrame;
}

#include <iostream>

auto K4FrameCompressor::test() -> void{

    std::cout << "ENCODING:\n";

    size_t inputSize = 128 + 35;
    std::vector<std::uint16_t> input;
    input.resize(inputSize);
    std::iota(input.begin(), input.end(), 12000);


    for(const auto &v : input){
        std::cout << v << " ";
    }
    std::cout << "\n";


    std::cout << "input size " << input.size() << "\n";

    size_t rest = input.size() % 128;
    size_t paddedSize = rest == 0 ? input.size() : input.size() + 128-rest;
    input.resize(paddedSize);

    std::cout << "padded size " << input.size() << "\n";


    std::vector<std::uint8_t> encoded;
    encoded.resize(paddedSize*2);

    std::cout << "base encoded size " << encoded.size() << "\n";

    size_t encodedBytesNb = p4nzenc128v16(
        reinterpret_cast<std::uint16_t*>(input.data()),
        paddedSize,
        encoded.data()
    );

    encoded.resize(encodedBytesNb);
    std::cout << "encoded size " << encoded.size() << "\n";

    std::cout << "DECODING:\n";

    std::vector<std::uint16_t> decoded;
    decoded.resize(paddedSize);

    size_t decodedBytesNb = p4nzdec128v16(
        encoded.data(),
        paddedSize,
        decoded.data()
    );
    decoded.resize(inputSize);

    std::cout << "decoded bytes " << decodedBytesNb << "\n";

    for(const auto &v : decoded){
        std::cout << v << " ";
    }
    std::cout << "\n";

}



// audio compressed
// ...

//    if(audioSize > 0){

// resize processed data
//        const size_t paddedAudioSizeDiff = 128-((audioSize)%128);
//        const size_t processsedAudioDataSize = idV*3 + paddedDiff;
//        if(i->processedCloudData.size() < processsedCloudDataSize){
//            i->processedCloudData.resize(processsedCloudDataSize);
//        }
//        std::fill(i->processedCloudData.begin(), i->processedCloudData.end(), 0);

//        cFrame->encodedAudioData;

    // process audio buffer
    // ...

    // compress audio buffer
    // ...


    // -32768
    // 32767

    // -1.000 +1.0000
    // * 32767
    // -32767 +32767

    //    encodedBytesNb = p
//    }
