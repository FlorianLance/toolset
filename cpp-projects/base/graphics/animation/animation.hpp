
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
#include <vector>

// local
#include "geometry/quaternion.hpp"

namespace tool::graphics {

struct AnimationKeys{

    std::vector<double> positionTimes;
    std::vector<geo::Pt3f> positionKeys;

    std::vector<double> rotationTimes;
    std::vector<geo::Quatf> rotationKeys;

    std::vector<double> scalingTimes;
    std::vector<geo::Pt3f> scalingKeys;

    auto find_position(float time) const noexcept -> size_t;
    auto find_rotation(float time) const noexcept -> size_t;
    auto find_scaling(float time) const noexcept -> size_t;

    auto calculate_interpolated_scaling(float time) const noexcept -> geo::Pt3f;
    auto calculate_interpolated_position(float time) const noexcept -> geo::Pt3f;
    auto calculate_interpolated_rotation( float time) -> geo::Quatf;
};

struct Animation{
    std::string name;
    double duration;
    double ticksPerSecond;
};



}
