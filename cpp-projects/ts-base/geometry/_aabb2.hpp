
// /*******************************************************************************
// ** Toolset-base                                                               **
// ** MIT License                                                                **
// ** Copyright (c) [2018] [Florian Lance]                                       **
// **                                                                            **
// ** Permission is hereby granted, free of charge, to any person obtaining a    **
// ** copy of this software and associated documentation files (the "Software"), **
// ** to deal in the Software without restriction, including without limitation  **
// ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
// ** and/or sell copies of the Software, and to permit persons to whom the      **
// ** Software is furnished to do so, subject to the following conditions:       **
// **                                                                            **
// ** The above copyright notice and this permission notice shall be included in **
// ** all copies or substantial portions of the Software.                        **
// **                                                                            **
// ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
// ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
// ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
// ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
// ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
// ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
// ** DEALINGS IN THE SOFTWARE.                                                  **
// **                                                                            **
// ********************************************************************************/

// #pragma once

// // local
// #include "point2.hpp"
// // #include "utility/maths_utility.hpp"

// namespace tool::geo {

// template<typename acc>
// struct AABB2{


//     AABB2() = default;

//     constexpr AABB2(Pt2<acc> o, Vec2<acc> s) noexcept : origin(o), size(s){
//     }

//     constexpr Pt2<acc> min() const noexcept{
//         const Vec2<acc> p1{origin + size};
//         const Vec2<acc> p2{origin - size};
//         return Pt2<acc>{std::min(p1.x(), p2.x()), std::min(p1.y(), p2.y())};
//     }

//     constexpr Vec2<acc> max() const noexcept{
//         const Vec2<acc> p1{origin + size};
//         const Vec2<acc> p2{origin - size};
//         return Vec2<acc>(std::max(p1.x(), p2.x()), std::max(p1.y(), p2.y()));
//     }

//     constexpr std::array<AABB2<acc>,4> subdivide() const noexcept{
//         const Vec2<acc> midSize = size*acc{0.5};
//         return {{AABB2<acc>{origin - midSize,                              midSize},
//                  AABB2<acc>{origin + Vec2<acc>{-midSize.x(), midSize.y()}, midSize},
//                  AABB2<acc>{origin + midSize,                              midSize},
//                  AABB2<acc>{origin + Vec2<acc>{midSize.x(), -midSize.y()}, midSize}}};
//     }

//     Pt2<acc> origin = {0,0};
//     Vec2<acc> size  = {1,1};
// };


// // template <typename acc>
// // constexpr bool point_in_aabb(const Pt2<acc> &p, const AABB2<acc> &aabb) noexcept{

// //     const Pt2<acc> pMin = aabb.min();
// //     const Pt2<acc> pMax = aabb.max();
// //     const bool xMinE = math::almost_equal<acc>(p.x(),pMin.x());
// //     const bool yMinE = math::almost_equal<acc>(p.y(),pMin.y());

// //     if((p.x() > pMin.x() || xMinE) && (p.y() > pMin.y() || yMinE) ){

// //         const bool xMaxE = math::almost_equal<acc>(p.x(),pMax.x());
// //         const bool yMaxE = math::almost_equal<acc>(p.y(),pMax.y());
// //         if((p.x() < pMax.x() || xMaxE) && (p.y() < pMax.y() || yMaxE)){
// //             return true;
// //         }
// //     }

// //     return false;
// // }

// }
