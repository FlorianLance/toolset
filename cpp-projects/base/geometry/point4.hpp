
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
#include "geometry/point.hpp"

namespace tool::geo {

template<typename acc>
struct Point4;

template<typename acc>
using Pt4 = Point4<acc>;
using Pt4f = Pt4<float>;
using Pt4d = Pt4<double>;

template<typename acc>
using Vec4  = Pt4<acc>;
using Vec4f = Pt4<float>;
using Vec4d = Pt4<double>;

template<typename acc>
struct Point4 : Point<acc,4>{

    Point4() = default;
    Point4(const Point4& other) = default;
    Point4& operator=(const Point4& other) = default;
    Point4(Point4&& other) = default;
    Point4& operator=(Point4&& other) = default;

    constexpr Point4(const Point<acc,4> &p) noexcept{
        this->array = p.array;
    }
    constexpr Point4(Point<acc,4> &&p) noexcept{
        this->array = std::move(p.array);
    }

    constexpr Point4(acc x, acc y = acc{0}, acc z = acc{0}, acc w = acc{0}) noexcept{
        this->array = {x,y,z,w};
    }
};

template <typename acc>
constexpr auto operator*(const RowVec<acc,4> &l, const RowVec<acc,4> &r) noexcept -> RowVec<acc,4>{
    return {{l.x()*r.x(),l.y()*r.y(),l.z()*r.z(),l.w()*r.w()}};
}

template <typename acc>
constexpr auto operator*(const ColVec<acc,4> &l, const ColVec<acc,4> &r) noexcept -> ColVec<acc,4>{
    return {{l.x()*r.x(),l.y()*r.y(),l.z()*r.z(),l.w()*r.w()}};
}

template <typename acc>
constexpr auto to_pt4(const Point<acc,3> &pt, acc w) noexcept -> Pt4<acc>{
    return {pt.x(),pt.y(),pt.z(),w};
}


}
