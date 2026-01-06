

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

namespace tool::geo {

template<typename acc>
struct AABB3{

    Pt3<acc> min;
    Pt3<acc> max;
//    AABB3() = default;

//    constexpr AABB3(const Pt3<acc> &o, const Vec3<acc> &s) noexcept : origin(o), size(s){
//    }

//    constexpr auto min() const noexcept -> Vec3<acc>{
//        Vec3<acc> p1 = origin + size;
//        Vec3<acc> p2 = origin - size;
//        return Vec3<acc>(std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y()),std::min(p1.z(), p2.z()));
//    }

//    constexpr auto max() const noexcept-> Vec3<acc>{
//        Vec3<acc> p1 = origin + size;
//        Vec3<acc> p2 = origin - size;
//        return Vec3<acc>(std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()),std::max(p1.z(), p2.z()));
//    }

//    Pt3<acc> origin = {0,0,0};
//    Vec3<acc> size  = {1,1,1};

};


//template<typename acc>
//constexpr AABB3<acc> aabb_from_points(const Vec3<acc> &min, const Vec3<acc> &max) noexcept{
//    return AABB3<acc>((min + max) * acc{0.5}, (max - min) * acc{0.5});
//}

//template <typename acc>
//constexpr bool point_in_aabb(const Pt3<acc> &p, const AABB3<acc> &aabb) noexcept{

//    const Pt3<acc> pMin = aabb.min();
//    const Pt3<acc> pMax = aabb.max();
//    const bool xMinE = tool::almost_equal<acc>(p.x(),pMin.x());
//    const bool yMinE = tool::almost_equal<acc>(p.y(),pMin.y());
//    const bool zMinE = tool::almost_equal<acc>(p.z(),pMin.z());

//    if((p.x() > pMin.x() || xMinE) && (p.y() > pMin.y() || yMinE) && (p.z() > pMin.z() || zMinE)){

//        const bool xMaxE = tool::almost_equal<acc>(p.x(),pMax.x());
//        const bool yMaxE = tool::almost_equal<acc>(p.y(),pMax.y());
//        const bool zMaxE = tool::almost_equal<acc>(p.z(),pMax.z());

//        if((p.x() < pMax.x() || xMaxE) && (p.y() < pMax.y() || yMaxE) && (p.z() < pMax.z() || zMaxE)){
//            return true;
//        }
//    }

//    return false;
//}

//template <typename acc>
//constexpr Pt3<acc> closest_point(const AABB3<acc> &aabb, const Pt3<acc> &p) noexcept{

//    Pt3<acc> res = p;
//    const Pt3<acc> pMin = aabb.min();
//    const Pt3<acc> pMax = aabb.max();

//    res.x() = (res.x() < pMin.x()) ? pMin.x() : res.x();
//    res.y() = (res.y() < pMin.y()) ? pMin.y() : res.y();
//    res.z() = (res.z() < pMin.z()) ? pMin.z() : res.z();

//    res.x() = (res.x() > pMax.x()) ? pMax.x() : res.x();
//    res.y() = (res.y() > pMax.y()) ? pMax.y() : res.y();
//    res.z() = (res.z() > pMax.z()) ? pMax.z() : res.z();

//    return res;
//}

//template<typename acc>
//constexpr Interval<acc> interval(const AABB3<acc> &aabb, const Vec3<acc> &axis) noexcept{

//    const Pt3<acc> i = aabb.min();
//    const Pt3<acc> a = aabb.max();

//    const std::array<Vec3<acc>,8> vertices = {
//        Pt3<acc>(i.x(), a.y(), a.z()),
//        Pt3<acc>(i.x(), a.y(), i.z()),
//        Pt3<acc>(i.x(), i.y(), a.z()),
//        Pt3<acc>(i.x(), i.y(), i.z()),
//        Pt3<acc>(a.x(), a.y(), a.z()),
//        Pt3<acc>(a.x(), a.y(), i.z()),
//        Pt3<acc>(a.x(), i.y(), a.z()),
//        Pt3<acc>(a.x(), i.y(), i.z())
//    };

//    const acc dotV = dot(axis, vertices[0]);
//    Interval<acc> res{dotV,dotV};
//    for(const auto &pt : vertices){
//        const acc projection = dot(axis,pt);
//        res.min() = (projection < res.min()) ? projection : res.min();
//        res.max() = (projection > res.max()) ? projection : res.max();
//    }

//    return res;
//}

}


