
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include "geometry/point.hpp"

namespace tool::img{

/**
 * @brief ColorHSV
 * h Hue:        [0, 360] 4 bytes
 * s Saturation: [0, 1] 4 bytes
 * v Value:      [0, 1] 4 bytes
 */
struct ColorHSV : geo::Point<float,3>{

    ColorHSV() = default;

    constexpr ColorHSV(const geo::Point<float,3> &c) noexcept{
        this->array = c.array;
    }
    constexpr ColorHSV(geo::Point<float,3> &&c) noexcept{
        this->array = std::move(c.array);
    }
    constexpr ColorHSV(float h, float s, float v) noexcept{
        this->array = {h,s,v};
    }

    [[nodiscard]] constexpr auto h()          noexcept -> float&  {return array[0];}
    [[nodiscard]] constexpr auto h() const    noexcept -> float   {return array[0];}

    [[nodiscard]] constexpr auto s()          noexcept -> float&  {return array[1];}
    [[nodiscard]] constexpr auto s() const    noexcept -> float   {return array[1];}

    [[nodiscard]] constexpr auto v()          noexcept -> float&  {return array[2];}
    [[nodiscard]] constexpr auto v() const    noexcept -> float   {return array[2];}

    [[nodiscard]] auto abs() const noexcept -> ColorHSV{
        return {std::abs(h()),std::abs(s()),std::abs(v())};
    }
};


}
