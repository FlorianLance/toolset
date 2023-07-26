
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
struct Point2;

template<typename acc>
using Pt2 = Point2<acc>;
using Pt2f = Pt2<float>;
using Pt2d = Pt2<double>;

template<typename acc>
using Vec2  = Pt2<acc>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

template<typename acc>
struct Point2 : Point<acc,2>{

    Point2() = default;
    Point2(const Point2& other) = default;
    Point2& operator=(const Point2& other) = default;
    Point2(Point2&& other) = default;
    Point2& operator=(Point2&& other) = default;

    constexpr Point2(const Point<acc,2> &p) noexcept{
        this->array = p.array;
    }
    constexpr Point2(Point<acc,2> &&p) noexcept{
        this->array = std::move(p.array);
    }
    constexpr Point2(acc x, acc y = acc{0}) noexcept{
        this->array = {x,y};
    }
};

template <typename acc>
constexpr auto operator*(const RowVec<acc,2> &l, const RowVec<acc,2> &r) noexcept -> RowVec<acc,2>{
    return {{l.x()*r.x(),l.y()*r.y()}};
}

template <typename acc>
constexpr auto operator*(const ColVec<acc,2> &l, const ColVec<acc,2> &r) noexcept -> ColVec<acc,2>{
    return {{l.x()*r.x(),l.y()*r.y()}};
}

}
