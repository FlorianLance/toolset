
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
#include <span>

// local
#include "dc_compressed_frame.hpp"
#include "dc_frame.hpp"
#include "graphics/color.hpp"

namespace tool::cam{

struct DCFrameCompressor{

    DCFrameCompressor();
    ~DCFrameCompressor();

    auto encode_frame(DCFrame &frame, int jpegQuality, DCCompressedFrame *cFrame) -> void;
    auto encode_frame(DCFrame &frame, int jpegQuality) -> std::unique_ptr<DCCompressedFrame>;

    auto encode_to_jpeg(size_t width, size_t height, std::span<ColorRGBA8> image, BinaryImageBuffer &encodedImage, int jpegQuality) -> bool;
    auto encode_to_jpeg(size_t width, size_t height, std::span<ColorRGB8> image,  BinaryImageBuffer &encodedImage, int jpegQuality) -> bool;
    auto encode_to_jpeg(size_t width, size_t height, std::span<ColorGray8> image, BinaryImageBuffer &encodedImage, int jpegQuality) -> bool;
    auto encode_to_jpeg(const ImageBuffer<ColorRGBA8> &image, BinaryImageBuffer &encodedImage, int jpegQuality) -> bool;
    auto encode_to_jpeg(const ImageBuffer<ColorRGB8> &image,  BinaryImageBuffer &encodedImage, int jpegQuality) -> bool;
    auto encode_to_jpeg(const ImageBuffer<ColorGray8> &image, BinaryImageBuffer &encodedImage, int jpegQuality) -> bool;

    auto encode_to_lossless_16_bits(size_t width, size_t height, std::span<std::uint16_t> data, BinaryImageBuffer &encodedData) -> bool;
    auto encode_to_lossless_16_bits(ImageBuffer<std::uint16_t> &data, BinaryImageBuffer &encodedData) -> bool;

    auto encode_colored_cloud_to_lossless_16_bits(
        std::span<std::uint16_t> depthData, std::span<ColorRGBA8> depthSizedColorData,
        std::span<geo::Pt3<std::int16_t>> depthCloudData, BinaryBuffer &encodedColoredCloudData) -> bool;
    auto encode_colored_cloud_to_lossless_16_bits(
        geo::ColoredCloudData &cloud,
        BinaryBuffer &encodedColoredCloudData) -> bool;

private:
    struct Impl;
    std::unique_ptr<Impl> i;
};
}
