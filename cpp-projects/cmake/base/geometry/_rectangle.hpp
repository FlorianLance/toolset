
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
//#include "geometry/point2.hpp"

//namespace tool::geo {

//template<typename acc>
//struct Rectangle2{

//    Rectangle2(const Pt2<acc> &o, const Vec2<acc> &s) : origin(o), size(s) {
//    }

//    Vec2<acc> min() const{
//        const Vec2<acc> p2 = origin + size;
//        return Vec2<acc>(std::min(origin.x(),p2.x()), std::min(origin.y(),p2.y()));
//    }

//    Vec2<acc> max() const{
//        const Vec2<acc> p2 = origin + size;
//        return Vec2<acc>(std::max(origin.x(),p2.x()), std::max(origin.y(),p2.y()));
//    }

//    static Rectangle2<acc> from_min_max(const Pt2<acc> &min, const Pt2<acc> &max) {
//        return Rectangle2<acc>(min, max-min);
//    }

//    Pt2<acc> origin;
//    Vec2<acc> size = {1,1};
//};

//template<typename acc>
//struct OrientedRectangle2{

//    OrientedRectangle2(const Pt2<acc> &p, const Vec2<acc> &ext, acc rot = 0) : position(p), halfExtends(ext), rotation(rot){
//    }

//    Pt2<acc> position;
//    Vec2<acc> halfExtends = {1,1};
//    acc rotation = 0;
//};
//}
