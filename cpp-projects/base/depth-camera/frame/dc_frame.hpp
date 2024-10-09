
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

#pragma once

// std
#include <variant>

// local
#include "utility/image_buffer.hpp"
#include "geometry/color_cloud.hpp"
#include "graphics/color/rgb.hpp"

#include "camera/frame.hpp"
#include "depth-camera/dc_types.hpp"
#include "utility/unordered_map.hpp"

namespace tool::cam{

using ImageBufferV = std::variant<
    ImageBuffer<ColorRGBA8>,
    ImageBuffer<ColorRGB8>,
    ImageBuffer<ColorGray8>,
    ImageBuffer<ColorGray16>>;

struct DCFrame2 : Frame{

    DCMode mode;
    size_t validVerticesCount = 0;
    umap<DCInfoType, std::int32_t> infosB;
    umap<DCBufferType, BinaryBuffer> datasB;
    umap<DCImageBufferType, ImageBufferV> imagesB;
};

struct DCFrame : Frame{

    // info
    DCMode mode;

    // images
    ImageBuffer<ColorRGBA8> rgbaColor;          // color-sized
    ImageBuffer<ColorRGBA8> rgbaDepthSizedColor;// depth-sized
    ImageBuffer<ColorRGB8>  rgbDepth;           // depth-sized
    ImageBuffer<ColorRGB8>  rgbInfra;           // depth-sized
    ImageBuffer<ColorGray8> grayBodiesIdMap;    // depth-sized
    ImageBuffer<std::uint16_t> depth;           // depth-sized
    ImageBuffer<std::uint16_t> infra;           // depth-sized

    geo::ColorCloud cloud;                // valid-vertices sized

    // binary encoding
    BinaryBuffer calibration;
    BinaryBuffer imu;

    // audio
    // TODO: add nbChannels variables and use a simple vector
    std::vector<std::array<float, 7>> audioFrames;

    // bodyTracking
    Buffer<DCBody> bodyTracking;

    // utilities
    auto compute_rgb_depth_image(ImageBuffer<ColorRGB8> &rgbDepth) -> void;
    auto compute_rgb_infra_image(ImageBuffer<ColorRGB8> &rgbInfra) -> void;
};

}
