

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
#include "geometry/point2.hpp"
#include "geometry/point3.hpp"

namespace tool::geo {

struct TriIds{

    [[nodiscard]] constexpr auto id1() const noexcept -> std::uint32_t {return ids[0];}
    [[nodiscard]] constexpr auto id2() const noexcept -> std::uint32_t {return ids[1];}
    [[nodiscard]] constexpr auto id3() const noexcept -> std::uint32_t {return ids[2];}

    [[nodiscard]] constexpr auto id1() noexcept -> std::uint32_t&{return ids[0];}
    [[nodiscard]] constexpr auto id2() noexcept -> std::uint32_t&{return ids[1];}
    [[nodiscard]] constexpr auto id3() noexcept -> std::uint32_t&{return ids[2];}

    std::array<std::uint32_t,3> ids;
};

template<typename acc>
struct Triangle2{

    [[nodiscard]] constexpr auto a() const noexcept -> Pt2<acc> {return v[0];}
    [[nodiscard]] constexpr auto b() const noexcept -> Pt2<acc> {return v[1];}
    [[nodiscard]] constexpr auto c() const noexcept -> Pt2<acc> {return v[2];}

    [[nodiscard]] constexpr auto a() noexcept -> Pt2<acc>&{return v[0];}
    [[nodiscard]] constexpr auto b() noexcept -> Pt2<acc>&{return v[1];}
    [[nodiscard]] constexpr auto c() noexcept -> Pt2<acc>&{return v[2];}

    std::array<Pt2<acc>,3> v;
};

template<typename acc>
struct Triangle3{

    [[nodiscard]] constexpr auto a() const noexcept -> Pt3<acc> {return v[0];}
    [[nodiscard]] constexpr auto b() const noexcept -> Pt3<acc> {return v[1];}
    [[nodiscard]] constexpr auto c() const noexcept -> Pt3<acc> {return v[2];}

    [[nodiscard]] constexpr auto a() noexcept -> Pt3<acc>&{return v[0];}
    [[nodiscard]] constexpr auto b() noexcept -> Pt3<acc>&{return v[1];}
    [[nodiscard]] constexpr auto c() noexcept -> Pt3<acc>&{return v[2];}

    std::array<Pt3<acc>,3> v;
};


//template <typename acc>
//constexpr auto barycentric(const Pt3<acc> &p, const Triangle3<acc> &t) -> Pt3<acc>{

//    const Vec3<acc> ab = t.b() - t.a();
//    const Vec3<acc> bc = t.c() - t.b();
//    const Vec3<acc> ca = t.a() - t.c();

//    Vec3<acc> v = ab - project(ab,vec(t.c(), t.b()));
//    const acc a = 1 - (dot(v, vec(t.a(),p)) / dot(v,ab));

//    v = bc - project(bc, vec(t.a(),t.c()));
//    const acc b = 1 - (dot(v,vec(t.b(),p)) / dot(v, bc));

//    v = ca - project(ca, ab);
//    const acc c = 1 - (dot(v, vec(t.c(),p)) / dot(v, ca));

//    return Pt3<acc>(a,b,c);
//}



//template<typename acc>
//constexpr auto normal(const Triangle3<acc> &t) -> Normal3<acc>{
//    return ((a() + b() + c())/acc{3});
//}

//template<typename acc>
//constexpr auto interval(const Triangle3<acc> &t, const Vec3<acc> &axis) -> Interval<acc>{

//    Interval<acc> res;
//    res.min() = dot(axis, t.a());
//    res.max() = res.min();

//    for(int ii = 1; ii < t.v.size(); ++ii){
//        acc value = dot(axis, t.v[ii]);
//        res.min() = std::min(res.min(), value);
//        res.max() = std::min(res.max(), value);
//    }

//    return res;
//}

//template <typename acc>
//constexpr auto barycentric(const Pt3<acc> &p, const Triangle3<acc> &t) -> Pt3<acc>{

//    const Vec3<acc> ab = t.b() - t.a();
//    const Vec3<acc> bc = t.c() - t.b();
//    const Vec3<acc> ca = t.a() - t.c();

//    Vec3<acc> v = ab - project(ab,vec(t.c(), t.b()));
//    const acc a = 1 - (dot(v, vec(t.a(),p)) / dot(v,ab));

//    v = bc - project(bc, vec(t.a(),t.c()));
//    const acc b = 1 - (dot(v,vec(t.b(),p)) / dot(v, bc));

//    v = ca - project(ca, ab);
//    const acc c = 1 - (dot(v, vec(t.c(),p)) / dot(v, ca));

//    return Pt3<acc>(a,b,c);
//}

//template <typename acc>
//constexpr auto generate_point(const Pt2<acc> &factors, const Triangle3<acc> &t) noexcept -> Pt3<acc>{
//    Vec3<acc> ab = vec(t.a(),t.b());
//    Vec3<acc> ac = vec(t.a(),t.c());
//    return t.a() + ab*factors[0] + ac*factors[1];
//}

//template <typename acc>
//constexpr auto barycenter_to_point(const Pt3<acc> &barycenter, const Triangle3<acc> &t) noexcept -> Pt3<acc>{

//    // TODO check x + y + z == 1
//    return Pt3<acc>{
//        barycenter.x()*t.a().x() + barycenter.y()*t.b().x() + barycenter.z()*t.c().x(),
//        barycenter.x()*t.a().y() + barycenter.y()*t.b().y() + barycenter.z()*t.c().y(),
//        barycenter.x()*t.a().z() + barycenter.y()*t.b().z() + barycenter.z()*t.c().z()
//    };
//}
}
