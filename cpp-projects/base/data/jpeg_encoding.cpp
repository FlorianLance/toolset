
/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "jpeg_encoding.hpp"

// turbojpg
#include <turbojpeg.h>

// local
#include "utility/logger.hpp"

using namespace tool::data;

struct JpegEncoder::Impl{
    tjhandle handle = nullptr;
    unsigned char *tjCompressedImage = nullptr;
    size_t currentSizeAllocatedToJPegCompressor = 0;
};

JpegEncoder::JpegEncoder() : i(std::make_unique<Impl>()){
    i->handle = tjInitCompress();
}

JpegEncoder::~JpegEncoder(){
    tjDestroy(i->handle);
}

auto JpegEncoder::encode(size_t width, size_t height, int format, ConstBinarySpan image, int quality) -> tool::BinarySpan {

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
    if(i->tjCompressedImage == nullptr){
        i->tjCompressedImage = tjAlloc(static_cast<int>(i->currentSizeAllocatedToJPegCompressor = imageSize));
    }else if(i->currentSizeAllocatedToJPegCompressor < imageSize){
        tjFree(i->tjCompressedImage);
        i->tjCompressedImage = tjAlloc(static_cast<int>(i->currentSizeAllocatedToJPegCompressor = imageSize));
    }

    quality = std::clamp(quality, 5, 100);

    long unsigned int jpegColorSize = 0;
    int ret = tjCompress2(
        i->handle,
        reinterpret_cast<const unsigned char *>(image.data()),
        static_cast<int>(width),
        0,  // pitch
        static_cast<int>(height),
        format,
        &i->tjCompressedImage, &jpegColorSize,
        TJSAMP_444,
        quality, TJFLAG_NOREALLOC | TJFLAG_FASTDCT
    );

    if(ret == -1){
        Logger::error(std::format("[JpegEncoder:encode] Error with code: {}\n", tjGetErrorStr2(i->handle)));
        return {};
    }

    return {
            reinterpret_cast<std::byte*>(i->tjCompressedImage),
        static_cast<size_t>(jpegColorSize)
    };
}

auto JpegEncoder::encode(size_t width, size_t height, std::span<ColorRGBA8> image, BinaryImageBuffer &encodedImage, int quality) -> bool{

    if(auto encodedData = encode(width, height, TJPF_RGBA, BinarySpan{reinterpret_cast<std::byte*>(image.data()), image.size_bytes()}, quality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width  = width;
        encodedImage.height = height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto JpegEncoder::encode(size_t width, size_t height, std::span<ColorRGB8> image, BinaryImageBuffer &encodedImage, int quality) -> bool{

    if(auto encodedData = encode(width, height, TJPF_RGB, BinarySpan{reinterpret_cast<std::byte*>(image.data()), image.size_bytes()}, quality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width  = width;
        encodedImage.height = height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto JpegEncoder::encode(size_t width, size_t height, std::span<ColorGray8> image, BinaryImageBuffer &encodedImage, int quality) -> bool{

    if(auto encodedData = encode(width, height, TJPF_GRAY, BinarySpan{reinterpret_cast<std::byte*>(image.data()), image.size_bytes()}, quality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width  = width;
        encodedImage.height = height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto JpegEncoder::encode(const ImageBuffer<ColorRGB8> &image, BinaryImageBuffer &encodedImage, int quality) -> bool{

    if(auto encodedData = encode(image.width, image.height, TJPF_RGB, image.byte_span(), quality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width = image.width;
        encodedImage.height = image.height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto JpegEncoder::encode(const ImageBuffer<ColorRGBA8> &image, BinaryImageBuffer &encodedImage, int quality) -> bool{

    if(auto encodedData = encode(image.width, image.height, TJPF_RGBA, image.byte_span(), quality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width = image.width;
        encodedImage.height = image.height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

auto JpegEncoder::encode(const ImageBuffer<ColorGray8> &image, BinaryImageBuffer &encodedImage, int quality) -> bool{

    if(auto encodedData = encode(image.width, image.height, TJPF_GRAY, image.byte_span(), quality); !encodedData.empty()){
        encodedImage.resize(encodedData.size());
        encodedImage.width = image.width;
        encodedImage.height = image.height;
        std::copy(encodedData.begin(), encodedData.end(), encodedImage.begin());
        return true;
    }
    return false;
}

struct JpegDecoder::Impl{
    tjhandle handle = nullptr;
};

JpegDecoder::JpegDecoder() : i(std::make_unique<Impl>()){
    i->handle = tjInitDecompress();
}

JpegDecoder::~JpegDecoder(){
    tjDestroy(i->handle);
}

auto JpegDecoder::decode(size_t width, size_t height, int tjpfFormat, ConstBinarySpan encodedImage, BinarySpan decodedImage) -> bool{

    if(decodedImage.size() < encodedImage.size()){
        Logger::error("[JpegDecoder::decode] Invalid inputs.\n");
    }

    // uncompress
    int ret = tjDecompress2(
        i->handle,
        reinterpret_cast<const unsigned char *>(encodedImage.data()),
        static_cast<unsigned long>(encodedImage.size()),
        reinterpret_cast<unsigned char *>(decodedImage.data()),
        static_cast<int>(width),
        0, // pitch
        static_cast<int>(height),
        static_cast<TJPF>(tjpfFormat),
        TJFLAG_FASTDCT
    );
    if(ret == -1){
        Logger::error(std::format("[JpegEncoder:decode] Error with code: {}\n", tjGetErrorStr2(i->handle)));
        return false;
    }
    return true;
}

auto JpegDecoder::decode(const BinaryImageBuffer &encodedImage, ImageBuffer<ColorRGBA8> &image) -> bool{
    image.resize_image(encodedImage.width, encodedImage.height);
    if(decode(encodedImage.width, encodedImage.height, TJPF_RGBA, encodedImage.byte_span(), image.byte_span())){
        return true;
    }
    image.reset();
    return false;
}

auto JpegDecoder::decode(const BinaryImageBuffer &encodedImage, ImageBuffer<ColorRGB8> &image) -> bool{
    image.resize_image(encodedImage.width, encodedImage.height);
    if(decode(encodedImage.width, encodedImage.height, TJPF_RGB, encodedImage.byte_span(), image.byte_span())){
        return true;
    }
    image.reset();
    return false;
}

auto JpegDecoder::decode(const BinaryImageBuffer &encodedImage, ImageBuffer<ColorGray8> &image) -> bool{
    image.resize_image(encodedImage.width, encodedImage.height);
    if(decode(encodedImage.width, encodedImage.height, TJPF_GRAY, encodedImage.byte_span(), image.byte_span())){
        return true;
    }
    image.reset();
    return false;
}

auto JpegDecoder::decode(size_t width, size_t height, ConstBinarySpan encodedImage, ImageBuffer<ColorRGBA8> &image) -> bool{
    image.resize_image(width, height);
    if(decode(width, height, TJPF_RGBA, encodedImage, image.byte_span())){
        return true;
    }
    image.reset();
    return false;
}

auto JpegDecoder::decode(size_t width, size_t height, ConstBinarySpan encodedImage, ImageBuffer<ColorRGB8> &image) -> bool{
    image.resize_image(width, height);
    if(decode(width, height, TJPF_RGB, encodedImage, image.byte_span())){
        return true;
    }
    image.reset();
    return false;
}
