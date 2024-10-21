

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

#include "fastpfor_encoding.hpp"

// fastfor
#include "fastpfor/fastpfor.h"
#include "fastpfor/compositecodec.h"
#include "fastpfor/variablebyte.h"

// local
#include "utility/logger.hpp"

using namespace tool::data;
using namespace  FastPForLib;

struct FastPForEncoder::Impl{
    CompositeCodec<FastPFor<8>, VariableByte> codec;
};

FastPForEncoder::FastPForEncoder() : i(std::make_unique<Impl>()){
}

FastPForEncoder::~FastPForEncoder(){}

auto FastPForEncoder::encode(ConstBinarySpan buffer, BinaryBuffer &encodedBuffer) -> bool {

    size_t inputSizeBits    = buffer.size();
    size_t inputSizeInt32   = inputSizeBits / 4;
    size_t outputSizeInt32  = inputSizeInt32 + 1024;
    encodedBuffer.resize(outputSizeInt32 * 4);

    try{
        i->codec.encodeArray(
            reinterpret_cast<const std::uint32_t*>(buffer.data()),
            inputSizeInt32,
            reinterpret_cast<std::uint32_t*>(encodedBuffer.get_data()),
            outputSizeInt32
        );
        encodedBuffer.resize(outputSizeInt32 * 4);

    }catch(const std::logic_error &e){
        Log::error(std::format("[FastPForEncoding::encode] Logic error: {}\n", e.what()));
        return false;
    }catch(std::exception e){
        Log::error(std::format("[FastPForEncoding::encode] Error: {}\n", e.what()));
        return false;
    }

    return true;
}

auto FastPForEncoder::encode(size_t width, size_t height, std::span<uint16_t> image, BinaryImageBuffer &encodedImage) -> bool{

    if(encode(BinarySpan{reinterpret_cast<std::byte*>(image.data()), image.size_bytes()}, encodedImage)){
        encodedImage.width  = width;
        encodedImage.height = height;
        return true;
    }
    encodedImage.reset();
    return false;
}

auto FastPForEncoder::encode(const ImageBuffer<uint16_t> &image, BinaryImageBuffer &encodedImage) -> bool{
    if(encode(image.byte_span(), encodedImage)){
        encodedImage.width  = image.width;
        encodedImage.height = image.height;
        return true;
    }
    encodedImage.reset();
    return false;
}

struct FastPForDecoder::Impl{
    CompositeCodec<FastPFor<8>, VariableByte> codec;
};

FastPForDecoder::FastPForDecoder() : i(std::make_unique<Impl>()){
}

FastPForDecoder::~FastPForDecoder(){}

auto FastPForDecoder::decode(ConstBinarySpan encodedBuffer, BinarySpan buffer) -> bool{

    try{
        size_t decodedSize = buffer.size()/4;
        i->codec.decodeArray(
            reinterpret_cast<const std::uint32_t*>(encodedBuffer.data()),
            encodedBuffer.size()/4,
            reinterpret_cast<std::uint32_t*>(buffer.data()),
            decodedSize
        );
    }catch(const std::logic_error &e){
        Log::error(std::format("[FastPForEncoding::decode] Logic error: {}\n", e.what()));
        return false;
    }catch(std::exception e){
        Log::error(std::format("[FastPForEncoding::decode] Error: {}\n", e.what()));
        return false;
    }
    return true;
}

auto FastPForDecoder::decode(const BinaryImageBuffer &encodedImage, ImageBuffer<uint16_t> &image) -> bool{
    image.width  = encodedImage.width;
    image.height = encodedImage.height;
    image.resize(encodedImage.width*encodedImage.height);
    if(decode(encodedImage.byte_span(), image.byte_span())){
        return true;
    }
    image.reset();
    return false;
}
