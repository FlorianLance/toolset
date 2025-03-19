

///*******************************************************************************
//** Toolset-base                                                               **
//** MIT License                                                                **
//** Copyright (c) [2018] [Florian Lance]                                       **
//**                                                                            **
//** Permission is hereby granted, free of charge, to any person obtaining a    **
//** copy of this software and associated documentation files (the "Software"), **
//** to deal in the Software without restriction, including without limitation  **
//** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
//** and/or sell copies of the Software, and to permit persons to whom the      **
//** Software is furnished to do so, subject to the following conditions:       **
//**                                                                            **
//** The above copyright notice and this permission notice shall be included in **
//** all copies or substantial portions of the Software.                        **
//**                                                                            **
//** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
//** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
//** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
//** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
//** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
//** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
//** DEALINGS IN THE SOFTWARE.                                                  **
//**                                                                            **
//********************************************************************************/

//#pragma once

//// local
//#include "geometry/point3.hpp"

//namespace tool::geo {


//template<typename acc>
//struct Line3{

//    Line3() = default;

//    constexpr Line3(const Pt3<acc> &s, const Pt3<acc> &e) noexcept: pts({s,e}){
//    }

//    constexpr Line3(std::initializer_list<Pt3<acc>> l) noexcept {
//        std::move(l.begin(), l.end(), pts.begin());
//    }

//    inline acc length() const noexcept{
//        return (end()-start()).norm();
//    }

//    constexpr acc square_length() const noexcept{
//        return (end()-start()).square_norm();
//    }

//    constexpr Pt3<acc> start() const noexcept{return pts[0];}
//    constexpr Pt3<acc> end() const noexcept{return pts[1];}

//    constexpr Pt3<acc>& start() noexcept{return pts[0];}
//    constexpr Pt3<acc>& end() noexcept{return pts[1];}

//    constexpr Vec3<acc> vec() const noexcept {return end()-start();}

//    std::array<Pt3<acc>,2> pts{};
//};


//template <typename acc>
//constexpr Pt3<acc> closest_point(const Line3<acc> &l, const Pt3<acc> &p) {

//    Vec3<acc> lVec = l.vec();
//    acc t = dot(p-l.start(),lVec) / dot(lVec, lVec); // TODO : check 0 div
//    t = std::max(acc{0}, std::min(t, acc{1}));
//    return l.start() + lVec*t;
//}
//}
