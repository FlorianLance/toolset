
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
struct Point3;

template<typename acc>
using Pt3 = Point3<acc>;
using Pt3f = Pt3<float>;
using Pt3d = Pt3<double>;

template<typename acc>
using Vec3  = Pt3<acc>;
using Vec3f = Pt3<float>;
using Vec3d = Pt3<double>;

using Col3 = Pt3<std::uint8_t>;
using Col3f= Pt3f;

template<typename acc>
using Normal3 = Vec3<acc>;

template<typename acc>
struct Point3 : Point<acc,3>{

    Point3() = default;
    Point3(const Point3& other) = default;
    Point3& operator=(const Point3& other) = default;
    Point3(Point3&& other) = default;
    Point3& operator=(Point3&& other) = default;

    constexpr Point3(const Point<acc,3> &p) noexcept{
        this->array = p.array;
    }
    constexpr Point3(Point<acc,3> &&p) noexcept{
        this->array = std::move(p.array);
    }
    constexpr Point3(acc x, acc y = acc{0}, acc z = acc{0}) noexcept{
        this->array = {x,y,z};
    }
};

template <typename acc>
constexpr auto operator*(const RowVec<acc,3> &l, const RowVec<acc,3> &r) noexcept -> RowVec<acc,3>{
    return {{l.x()*r.x(),l.y()*r.y(),l.z()*r.z()}};
}

template <typename acc>
constexpr auto operator*(const ColVec<acc,3> &l, const ColVec<acc,3> &r) noexcept -> ColVec<acc,3>{
    return {{l.x()*r.x(),l.y()*r.y(),l.z()*r.z()}};
}

template <typename acc>
constexpr auto cross(const RowVec<acc,3> &l, const RowVec<acc,3> &r) noexcept -> RowVec<acc,3>{
    static_assert(std::numeric_limits<acc>::is_iec559, "'cross' accepts only floating-point inputs");
    return {{
        l.y() * r.z() - l.z() * r.y(),
        l.z() * r.x() - l.x() * r.z(),
        l.x() * r.y() - l.y() * r.x()
    }};
}

};


