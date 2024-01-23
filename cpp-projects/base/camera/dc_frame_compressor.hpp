
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

// local
#include "dc_compressed_frame.hpp"

namespace tool::cam{

struct DCFrameCompressor{

    DCFrameCompressor();
    ~DCFrameCompressor();

    auto add_color(size_t width,  size_t height, int numChannels, std::uint8_t *data, int jpegQuality, DCCompressedFrame *cFrame) -> void;
    auto add_depth(size_t width,  size_t height, std::uint16_t *data, DCCompressedFrame *cFrame) -> void;
    auto add_infra(size_t width,  size_t height, std::uint16_t *data, DCCompressedFrame *cFrame) -> void;
    auto add_cloud(size_t colorWidth, size_t colorHeight, std::uint8_t *colorData, size_t depthSize, std::uint16_t *depthData, int jpegQuality, DCCompressedFrame *cFrame)  -> void;
    auto add_frame(DCFrame &frame, int jpegQuality, DCCompressedFrame *cFrame) -> void;
    auto compress_frame(DCFrame &frame, int jpegQuality) -> std::unique_ptr<DCCompressedFrame>;

private:
    struct Impl;
    std::unique_ptr<Impl> i;
};
}
