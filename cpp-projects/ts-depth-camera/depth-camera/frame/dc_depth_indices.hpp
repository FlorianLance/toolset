
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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
#include <vector>

// geo
#include "geometry/point3.hpp"

namespace tool::cam{

struct DCDepthIndices{

    auto initialize(bool hasDepth, size_t depthWidth, size_t depthHeight) -> void;

    // indices
    std::vector<size_t> depths1D;
    std::vector<size_t> depths1DNoBorders;
    std::vector<std::array<std::int32_t,2>> neighbours2HDepth1D;    // 0 0 0
                                                                    // X P X
                                                                    // 0 0 0

    std::vector<std::array<std::int32_t,2>> neighbours2VDepth1D;    // 0 X 0
                                                                    // 0 P 0
                                                                    // 0 X 0

    std::vector<std::array<std::int32_t,4>> neighbours4Depth1D;     // 0 X 0
                                                                    // X P X
                                                                    // 0 X 0

    std::vector<std::array<std::int32_t,8>> neighbours8Depth1D;     // X X X
                                                                    // X P X
                                                                    // X X X
    // # 3D
    std::vector<geo::Pt3<size_t>> depths3D;
    // # Correspondance
    std::vector<std::tuple<size_t, std::int32_t>> depthVertexCorrrespondance;
    // std::vector<std::tuple<size_t, std::int16_t>> depthsSortedCorrespondanceNoBorders;

    // iterators
    std::vector<size_t>::iterator depths1DStart;
    std::vector<size_t>::iterator depths1DEnd;
    std::vector<size_t>::iterator depths1DNoBordersStart;
    std::vector<size_t>::iterator depths1DNoBordersEnd;
    std::vector<std::array<std::int32_t,2>>::iterator neighbours2HDepth1DStart;
    std::vector<std::array<std::int32_t,2>>::iterator neighbours2HDepth1DEnd;
    std::vector<std::array<std::int32_t,2>>::iterator neighbours2VDepth1DStart;
    std::vector<std::array<std::int32_t,2>>::iterator neighbours2VDepth1DEnd;
    std::vector<std::array<std::int32_t,4>>::iterator neighbours4Depth1DStart;
    std::vector<std::array<std::int32_t,4>>::iterator neighbours4Depth1DEnd;
    std::vector<std::array<std::int32_t,8>>::iterator neighbours8Depth1DStart;
    std::vector<std::array<std::int32_t,8>>::iterator neighbours8Depth1DEnd;
    std::vector<geo::Pt3<size_t>>::iterator depths3DStart;
    std::vector<geo::Pt3<size_t>>::iterator depths3DEnd;
    std::vector<std::tuple<size_t, std::int32_t>>::iterator depthVertexCorrrespondanceStart;
    std::vector<std::tuple<size_t, std::int32_t>>::iterator depthVertexCorrrespondanceEnd;

};
}
