
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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
#include "geometry/point4.hpp"

namespace tool::cam{

//    struct DCVoxelData{
//        std::int64_t idX : 13, idY : 13, idZ : 14, r : 8, g : 8, b : 8;
//    };

    struct DCPackedVoxel{
        static auto pack(const geo::Pt3<std::int16_t> &pos, const geo::Pt4<std::uint8_t> &col) noexcept -> std::tuple<std::uint32_t,std::uint32_t>;
        static auto unpack(std::uint32_t p1, std::uint32_t p2, geo::Pt3<std::int16_t> &pos, geo::Pt3<std::uint8_t> &col) noexcept -> void;
        static auto pack64(const geo::Pt3<std::int16_t> &pos, const geo::Pt4<std::uint8_t> &col) -> std::uint64_t;
        static auto unpack64(std::uint64_t p, geo::Pt3<std::int16_t> &pos, geo::Pt4<std::uint8_t> &col) -> void;
        static auto pack_xy(std::int16_t x, std::int16_t y) -> geo::Pt4<std::uint8_t>;
    };
}









