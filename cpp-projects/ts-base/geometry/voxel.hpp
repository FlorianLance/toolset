
/*******************************************************************************
** Toolset-ts-base                                                            **                      **
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
#include "geometry/point3.hpp"

namespace tool::geo{

struct Voxel {
public:
    geo::Pt3<std::int32_t> index;
    geo::Pt3f color;
};

struct UVoxel {
public:
    geo::Pt3<std::uint32_t> index;
    geo::Pt3f color;
};

struct CVoxel{

    std::uint64_t
        xIndex  : 14,
        yIndex  : 12,
        zIndex  : 14,
        red     : 8,
        green   : 8,
        blue    : 8;

    static constexpr std::uint16_t maxX = 16383; // mm
    static constexpr std::uint16_t maxY = 4095;  // mm
    static constexpr std::uint16_t maxZ = 16383; // mm
};

struct AvgColorVoxel {

    constexpr auto add(const tool::geo::Pt3<int> &index) noexcept -> void{
        if(numOfPoints == 0){
            voxel.index = index;
        }
    }

    constexpr auto add(const tool::geo::Pt3<int> &index, const tool::geo::Pt3f &color) noexcept -> void {
        if(numOfPoints > 0){
            if(index != voxel.index){
                voxel.color += color;
                numOfPoints++;
            }
        }else{
            voxel = {index, color};
            ++numOfPoints;
        }
    }

    constexpr auto add(const Voxel &v) noexcept -> void {
        if(numOfPoints > 0){
            if(v.index != voxel.index){
                voxel.color += v.color;
                numOfPoints++;
            }
        }else{
            voxel = v;
            ++numOfPoints;
        }
    }

    constexpr auto get_average_color() const noexcept -> tool::geo::Pt3f{
        if(numOfPoints > 0){
            return voxel.color / (1.f*numOfPoints);
        }else{
            return voxel.color;
        }
    }

    int numOfPoints = 0;
    Voxel voxel;
};
}
