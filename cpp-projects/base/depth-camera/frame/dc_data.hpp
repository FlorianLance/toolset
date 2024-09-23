
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
#include <span>

// local
#include "utility/image_buffer.hpp"
#include "graphics/color/rgb.hpp"
#include "depth-camera/dc_types.hpp"

namespace tool::cam{

struct DCData{

    auto reset(const DCModeInfos &mInfos) -> void;
    auto reset_spans() -> void;

    // device data
    // # global
    BinarySpan binaryCalibration;
    // # per frame
    BinarySpan rawColor;
    std::span<ColorRGBA8> color;
    std::span<ColorRGBA8> depthSizedColor;    
    std::span<std::uint16_t> depth;
    std::span<std::uint16_t> infra;
    std::span<ColorGray8> bodiesIdMap;
    std::span<geo::Pt3<std::int16_t>> depthCloud;    
    BinarySpan binaryIMU;
    std::pair<size_t, std::span<float>> audioChannels;    
    std::span<DCBody> bodies;

    // processed data
    ImageBuffer<ColorRGBA8> convertedColorData;
    size_t validDepthValues = 0;
    size_t meanBiggestZoneId = 0;
    std::vector<std::int8_t> filteringMask;
    std::vector<int> depthMask;
    std::vector<int> zonesId;
    std::vector<std::int16_t> depthFiltering;
};
}
