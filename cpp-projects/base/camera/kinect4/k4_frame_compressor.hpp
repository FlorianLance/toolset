
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

// kinect
#include "k4a/k4a.hpp"

// local
#include "k4_compressed_frame.hpp"
#include "k4_frame.hpp"
#include "k4_data_settings.hpp"

namespace tool::camera{

struct K4FrameCompressor{

    K4FrameCompressor();
    ~K4FrameCompressor();

    auto set_settings(const K4DataSettings &ds) -> void;

    auto compress(
        K4Mode mode,
        std::optional<k4a::image> colorImage, int jpegQuality,
        std::optional<k4a::image> depthImage, size_t validDepthValues,
        std::optional<k4a::image> infraImage,
        std::optional<k4a::image> cloud,
        std::optional<k4a::calibration> calibration,
        float *audioData, size_t audioSize,
        K4ImuSample *imuSample
    ) -> std::unique_ptr<K4CompressedFrame>;

    auto compress(K4Frame &frame, int jpegQuality) -> std::unique_ptr<K4CompressedFrame>;

    auto test() -> void;

private:

    auto compress_jpeg_8_bits_data(size_t width, size_t height, size_t dim, std::uint8_t *data, std::vector<std::uint8_t> &encodedData, int jpegQuality) -> bool;
    auto compress_lossless_16_bits_128padded_data(size_t size, std::uint16_t *data, std::vector<std::uint8_t> &encodedData) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}









