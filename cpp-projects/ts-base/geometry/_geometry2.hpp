


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
//#include "geometry/matrix2.hpp"
//#include "geometry/line2.hpp"
//#include "geometry/circle.hpp"
//#include "geometry/rectangle.hpp"
//#include "geometry/interval.hpp"
//#include "utility/maths_utility.hpp"

namespace tool::geo {

//// aliases
//template <typename acc>
//constexpr bool point_on_line(const Pt2<acc> &p, const Line2<acc> &l) noexcept; // OK
//template <typename acc>
//constexpr bool point_in_circle(const Pt2<acc> &p, const Circle<acc> &c) noexcept; // OK
//template <typename acc>
//constexpr bool point_in_rectangle(const Pt2<acc> &p, const Rectangle2<acc> &r); // TODO : float accuracy  && constexpr
//template <typename acc>
//bool point_in_oriented_rectangle(const Pt2<acc> &p, const OrientedRectangle2<acc> &o); // TODO: constexpr
//template <typename acc>
//constexpr bool line_circle(const Line2<acc> &l, const Circle<acc> &c); // TODO: 0 div
//template <typename acc>
//bool line_rectangle(const Line2<acc> &l, const Rectangle2<acc> &r);
//template <typename acc>
//bool line_oriented_rectangle(const Line2<acc> &l, const OrientedRectangle2<acc> &o);
//template <typename acc>
//bool circle_circle(const Circle<acc> &c1, const Circle<acc> &c2);
//template <typename acc>
//bool circle_rectangle(const Circle<acc> &c, const Rectangle2<acc> &r);
//template <typename acc>
//bool circle_oriented_rectangle(const Circle<acc> &c, const OrientedRectangle2<acc> &o);
//template <typename acc>
//bool rectangle_rectangle(const Rectangle2<acc> &r1, const Rectangle2<acc> &r2);
//template <typename acc>
//Interval<acc> interval(const Rectangle2<acc> &r, const Vec2<acc> &axis);
//template <typename acc>
//bool overlap_on_axis(const Rectangle2<acc> &r1, const Rectangle2<acc> &r2, const Vec2<acc> &axis);


//template <typename acc>
//constexpr bool point_on_line(const Pt2<acc> &p, const Line2<acc> &l) noexcept{

//    // find the slope
//    const Vec2<acc> d = l.vec();
//    const acc M = d.y()/d.x();

//    // find the Y-intercept
//    const acc B{l.start().y() - l.start().x()*M};

//    // check the equation
//    return math::almost_equal<acc>(p.y(), M*p.x() + B);
//}

//template <typename acc>
//constexpr bool point_in_circle(const Pt2<acc> &p, const Circle<acc> &c) noexcept{
//    const auto length = Line2<acc>(p, c.position).square_length();
//    const auto squareRadius = c.radius*c.radius;
//    return (length < c.radius*c.radius || math::almost_equal(length, squareRadius) );
//}

//template <typename acc>
//constexpr bool point_in_rectangle(const Pt2<acc> &p, const Rectangle2<acc> &r){
//    const auto min = r.min();
//    const auto max = r.max();
//    return min.x() <= p.y()   &&
//           p.x()   <= max.x() &&
//           p.y()   <= max.y();
//}

//template <typename acc>
//bool point_in_oriented_rectangle(const Pt2<acc> &p, const OrientedRectangle2<acc> &o){
//    Vec2<acc> rotVector = p - o.position;
//    acc theta = -math::deg_2_rad<acc>(o.rotation);
//    Mat2<acc> zRotation2x2 = {cos(theta), sin(theta),
//                             -sin(theta), cos(theta)};
//    return point_in_rectangle(Pt2<acc>((zRotation2x2*rotVector) + o.halfExtends), Rectangle2<acc>(Pt2<acc>(), o.halfExtends * acc{2}));
//}


//template <typename acc>
//constexpr bool line_circle(const Line2<acc> &l, const Circle<acc> &c){

//    const Vec2<acc> ab = l.vec();
//    const acc t = dot(c.position - l.start, ab) / dot(ab,ab);  // TODO: check 0 div
//    if(t < acc{0} ||  t > acc{1} || alomst_equal(t,acc{1}) || almost_equal(t,acc{0})){
//        return false;
//    }

//    return Line2<acc>(c.position, l.start + ab * t).square_length() < (c.radius * c.radius);
//}


//template <typename acc>
//bool line_rectangle(const Line2<acc> &l, const Rectangle2<acc> &r){

//    if(point_in_rectangle(l.start, r) || point_in_rectangle(l.end, r)){
//        return true;
//    }

//    Vec2<acc> norm = normalized(vec(l.start,l.end));
//    norm.x() = (norm.x() != 0) ? (1 / norm.x()) : 0; // TODO
//    norm.y() = (norm.y() != 0) ? (1 / norm.y()) : 0; // TODO
//    Vec2<acc> minV = (r.min() - l.start) * norm;
//    Vec2<acc> maxV = (r.max() - l.start) * norm;

//    acc tMin = std::max(std::min(minV.x(), maxV.x()), std::min(minV.y(), maxV.y()));
//    acc tMax = std::min(std::max(minV.x(), maxV.x()), std::max(minV.y(), maxV.y()));

//    if(tMax < 0 || tMin > tMax){
//        return false;
//    }

//    acc t = (tMin < 0) ? tMax : tMin;
//    return t > 0 && (t * t < l.square_length());
//}


//template <typename acc>
//bool line_oriented_rectangle(const Line2<acc> &l, const OrientedRectangle2<acc> &o){
//    return true; // TODO
//}

//template <typename acc>
//bool circle_circle(const Circle<acc> &c1, const Circle<acc> &c2){
//    return true; // TODO
//}


//template <typename acc>
//bool circle_rectangle(const Circle<acc> &c, const Rectangle2<acc> &r){
//    return true; // TODO
//}


//template <typename acc>
//bool circle_oriented_rectangle(const Circle<acc> &c, const OrientedRectangle2<acc> &o){
//    return true; // TODO
//}

//template <typename acc>
//bool rectangle_rectangle(const Rectangle2<acc> &r1, const Rectangle2<acc> &r2){
//    return true; // TODO
//}

//template <typename acc>
//Interval<acc> interval(const Rectangle2<acc> &r, const Vec2<acc> &axis){
//    Interval<acc> res; // TODO
//    return res;
//}

//template <typename acc>
//bool overlap_on_axis(const Rectangle2<acc> &r1, const Rectangle2<acc> &r2, const Vec2<acc> &axis){
//    return true; // TODO
//}

// ### 2D collisions
// implement a shitload of functions
// rectangle_rectangle_SAT
// generic_SAT
// interval or axis
// overlap_on_axis rect or axis
// rectangle_oriented_rectangle
// oriented_rectangle_oriented_rectangle

// ### 2D Optimizations
// containing_circle
// containing_rectangle
// bounding_shape
// point_in_shape

// quad_tree.h
// QuadTreeData
// QuadTreeNode
// ...

// broad phase collisions


}
