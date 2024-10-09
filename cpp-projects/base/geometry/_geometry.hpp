



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
#include "point.hpp"

namespace tool::geo {

//template <typename acc, std::size_t dim>
//Vector<acc,dim> project(const Vector<acc,dim> len, const Vector<acc,dim> dir){
//    return len.project(dir);
//}

//template <typename acc, std::size_t dim>
//Vector<acc,dim> perpendicular(const Vector<acc,dim> len, const Vector<acc,dim> dir){
//    return len.perpendicular(dir);
//}


//inline Vec<acc,dim> project(const Vector<acc,dim> &dir) const {
//    return dir * (dot(*this, dir) / dir.square_norm());
//}

//inline Vec<acc,dim> perpendicular(const Vector<acc,dim> &dir) const {
//    return (*this) - project(dir);
//}

//inline Vec<acc,dim> reflection(const Vector<acc,dim> &normal) const{
//    return (*this) - normal * (dot(*this,normal) * acc{2});
//}


//template <typename acc, std::size_t dim>
//acc angle(const Pt<acc, dim> &l, const Pt<acc, dim> &r){
//    acc m = sqrt(l.square_norm()*r.square_norm());
//    return std::acos(dot(l,r)/m);
//}

//template <typename acc, std::size_t dim>
//bool is_perpendicular(const vecN<acc, dim> &l, const vecN<acc, dim> &r, int ulp = 3) noexcept{
//    return almost_equal(dot(l,r), acc{0}, ulp);
//}


}
