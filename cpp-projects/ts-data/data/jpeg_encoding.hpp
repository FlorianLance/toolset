
/*******************************************************************************
** Toolset-ts-data                                                            **
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

#pragma once

// base
#include "image/rgb.hpp"
#include "utility/image_buffer.hpp"

namespace tool::data{

struct JpegEncoder{

    JpegEncoder();
    ~JpegEncoder();

    auto encode(size_t width, size_t height, int format, ConstBinarySpan image, int quality) -> BinarySpan;
    auto encode(size_t width, size_t height, std::span<img::ColorRGBA8> image, BinaryImageBuffer &encodedImage, int quality) -> bool;
    auto encode(size_t width, size_t height, std::span<img::ColorRGB8> image,  BinaryImageBuffer &encodedImage, int quality) -> bool;
    auto encode(size_t width, size_t height, std::span<img::ColorGray8> image, BinaryImageBuffer &encodedImage, int quality) -> bool;
    auto encode(const ImageBuffer<img::ColorRGBA8> &image, BinaryImageBuffer &encodedImage, int quality) -> bool;
    auto encode(const ImageBuffer<img::ColorRGB8> &image,  BinaryImageBuffer &encodedImage, int quality) -> bool;
    auto encode(const ImageBuffer<img::ColorGray8> &image, BinaryImageBuffer &encodedImage, int quality) -> bool;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};

struct JpegDecoder{

    JpegDecoder();
    ~JpegDecoder();

    auto decode(size_t width, size_t height, int tjpfFormat, ConstBinarySpan encodedImage, BinarySpan decodedImage) -> bool;
    auto decode(size_t width, size_t height, ConstBinarySpan encodedImage, ImageBuffer<img::ColorRGBA8> &image) -> bool;
    auto decode(size_t width, size_t height, ConstBinarySpan encodedImage, ImageBuffer<img::ColorRGB8> &image) -> bool;
    auto decode(const BinaryImageBuffer &encodedImage, ImageBuffer<img::ColorRGBA8> &image) -> bool;
    auto decode(const BinaryImageBuffer &encodedImage, ImageBuffer<img::ColorRGB8> &image) -> bool;
    auto decode(const BinaryImageBuffer &encodedImage, ImageBuffer<img::ColorGray8> &image) -> bool;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
