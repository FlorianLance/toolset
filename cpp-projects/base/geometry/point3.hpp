
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

template<typename acc>
using Normal3 = Vec3<acc>;

template<typename acc>
struct Point3 : Point<acc,3>{

    Point3() = default;

    constexpr Point3(const Point<acc,3> &p) noexcept{
        this->array = p.array;
    }
    constexpr Point3(Point<acc,3> &&p) noexcept{
        this->array = std::move(p.array);
    }
    constexpr Point3(acc x, acc y = acc{0}, acc z = acc{0}) noexcept{
        this->array = {x,y,z};
    }

    [[nodiscard]] auto abs() const noexcept -> Point3{
        return {std::abs(Point<acc,3>::x()),std::abs(Point<acc,3>::y()),std::abs(Point<acc,3>::z())};
    }

    constexpr auto clamp(acc min, acc max) -> void{
        (*this)[0] = std::clamp(Point<acc,3>::x(), min, max);
        (*this)[1] = std::clamp(Point<acc,3>::y(), min, max);
        (*this)[2] = std::clamp(Point<acc,3>::z(), min, max);
    }

};

template <typename acc>
constexpr auto operator*(const RowVec<acc,3> &l, const RowVec<acc,3> &r) noexcept -> RowVec<acc,3>{
    return {{l[0]*r[0],l[1]*r[1],l[2]*r[2]}};
}

template <typename acc>
constexpr auto operator*(const ColVec<acc,3> &l, const ColVec<acc,3> &r) noexcept -> ColVec<acc,3>{
    return {{l[0]*r[0],l[1]*r[1],l[2]*r[2]}};
}

template <typename acc>
constexpr auto operator*(const Point3<acc> &l, const Point3<acc> &r) noexcept -> acc {
    return {{l[0]*r[0],l[1]*r[1],l[2]*r[2]}};
}

template <typename acc>
constexpr auto dot(const Point3<acc> &l, const Point3<acc> &r) noexcept -> acc {
    return l[0]*r[0] + l[1]*r[1] + l[2]*r[2];
}

template <typename acc>
constexpr auto square_norm(const Point3<acc> &p) noexcept -> acc {
    return dot(p,p);
}

template <typename acc>
constexpr auto norm(const Point3<acc> &p) noexcept -> acc {
    return sqrt(dot(p,p));
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


