
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

struct DCFrameUncompressor{

    enum ColorFormat : std::int8_t {
        RGB = 0,RGBA = 7,BGR = 1,BGRA = 8
    };

    DCFrameUncompressor();
    ~DCFrameUncompressor();

    auto uncompress_jpeg_data(size_t width, size_t height, ColorFormat format, size_t jpegSize, std::uint8_t *jpegData, std::uint8_t *data) -> bool;

    // uncompress to frame
    auto uncompress(DCCompressedFrame *cFrame, DCFrame &frame) -> bool;
    auto uncompress(DCCompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt3f *colors) -> bool;
    auto uncompress(DCCompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt4f *colors) -> bool;
    auto uncompress(DCCompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt3<std::uint8_t> *colors) -> bool;
    auto uncompress(DCCompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt4<std::uint8_t> *colors) -> bool;
    auto uncompress(DCCompressedFrame *cFrame, DCVertexMeshData *vertices) -> int;

private:
    struct Impl;
    std::unique_ptr<Impl> i;
};
}
