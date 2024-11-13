

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

#pragma once

// std
#include <memory>

// local
#include "utility/image_buffer.hpp"

namespace tool::data {

struct FastPForEncoder{

    FastPForEncoder();
    ~FastPForEncoder();

    auto encode(ConstBinarySpan buffer, BinaryBuffer &encodedBuffer) -> bool;
    auto encode64(ConstBinarySpan buffer, BinaryBuffer &encodedBuffer) -> bool;
    auto encode64(std::span<const std::uint64_t> buffer, Buffer<std::uint32_t> &encodedBuffer) -> bool;

    auto encode(size_t width, size_t height, std::span<std::uint16_t> image, BinaryImageBuffer &encodedImage) -> bool;   
    auto encode(const ImageBuffer<std::uint16_t> &image, BinaryImageBuffer &encodedImage) -> bool;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};

struct FastPForDecoder{

    FastPForDecoder();
    ~FastPForDecoder();

    auto decode(ConstBinarySpan encodedBuffer, BinarySpan buffer) -> bool;    
    auto decode64(ConstBinarySpan encodedBuffer, BinarySpan buffer) -> bool;
    auto decode(const BinaryImageBuffer &encodedImage, ImageBuffer<std::uint16_t> &image) -> bool;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};

}

