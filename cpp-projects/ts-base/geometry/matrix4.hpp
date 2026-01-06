
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
#include "utility/constants.hpp"
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"
#include "geometry/matrix3.hpp"

namespace tool::geo {


template<typename acc>
struct Matrix4;

template<typename acc>
using Mat4  = Matrix4<acc>;
using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

template<typename acc>
struct Matrix4 : Matrix<acc,4,4>{

    Matrix4() = default;

    constexpr Matrix4(const Matrix<acc,4,4> &m) noexcept{
        this->array = m.array;
    }
    constexpr Matrix4(Matrix<acc,4,4> &&m) noexcept{
        this->array = std::move(m.array);
    }

    constexpr explicit Matrix4(Matrix<acc,3,3> m) noexcept{
        this->array = {
            m(0),m(1),m(3),0,
            m(4),m(5),m(6),0,
            m(7),m(8),m(9),0,
            0,   0,   0,   1
        };
    }

    constexpr Matrix4(RowVec<acc,4> r1, RowVec<acc,4> r2, RowVec<acc,4> r3, RowVec<acc,4> r4) noexcept{
        this->array = {
            r1.x(), r1.y(), r1.z(), r1.w(),
            r2.x(), r2.y(), r2.z(), r2.w(),
            r3.x(), r3.y(), r3.z(), r3.w(),
            r4.x(), r4.y(), r4.z(), r4.w()
        };
    }
    constexpr Matrix4(ColVec<acc,4> c1, ColVec<acc,4> c2, ColVec<acc,4> c3, ColVec<acc,4> c4) noexcept{
        this->array = {
            c1.x(), c2.x(), c3.x(), c4.x(),
            c1.y(), c2.y(), c3.y(), c4.y(),
            c1.z(), c2.z(), c3.z(), c4.z(),
            c1.w(), c2.w(), c3.w(), c4.w()
        };
    }

    constexpr Matrix4(acc v00,          acc v01 = acc{0}, acc v02 = acc{0}, acc v03 = acc{0},
                      acc v10 = acc{0}, acc v11 = acc{0}, acc v12 = acc{0}, acc v13 = acc{0},
                      acc v20 = acc{0}, acc v21 = acc{0}, acc v22 = acc{0}, acc v23 = acc{0},
                      acc v30 = acc{0}, acc v31 = acc{0}, acc v32 = acc{0}, acc v33 = acc{0}) noexcept{
        this->array = {
            v00,v01,v02,v03,
            v10,v11,v12,v13,
            v20,v21,v22,v23,
            v30,v31,v32,v33
        };
    }
    constexpr Matrix4(acc *data, size_t size) noexcept{
        if(size <= this->array.size()){
            std::copy(data, data + size, this->array.begin());
        }
    }

    [[nodiscard]] constexpr auto row0() const noexcept -> RowVec<acc,4>{
        return {{(*this)[0],(*this)[1],(*this)[2],(*this)[3]}};
    }
    [[nodiscard]] constexpr auto row1() const noexcept -> RowVec<acc,4>{
        return {{(*this)[4],(*this)[5],(*this)[6],(*this)[7]}};
    }
    [[nodiscard]] constexpr auto row2() const noexcept -> RowVec<acc,4>{
        return {{(*this)[8],(*this)[9],(*this)[10],(*this)[11]}};
    }
    [[nodiscard]] constexpr auto row3() const noexcept -> RowVec<acc,4>{
        return {{(*this)[12],(*this)[13],(*this)[14],(*this)[15]}};
    }

    [[nodiscard]] constexpr auto col0() const noexcept -> ColVec<acc,4>{
        return {{(*this)[0],(*this)[4],(*this)[8],(*this)[12]}};
    }
    [[nodiscard]] constexpr auto col1() const noexcept -> ColVec<acc,4>{
        return {{(*this)[1],(*this)[5],(*this)[9],(*this)[13]}};
    }
    [[nodiscard]] constexpr auto col2() const noexcept -> ColVec<acc,4>{
        return {{(*this)[2],(*this)[6],(*this)[10],(*this)[14]}};
    }
    [[nodiscard]] constexpr auto col3() const noexcept -> ColVec<acc,4>{
        return {{(*this)[3],(*this)[7],(*this)[11],(*this)[15]}};
    }

    constexpr auto t(int id) const noexcept     -> acc{ return (*this)(id,3);}
    constexpr auto t(int id) noexcept           -> acc&{return (*this)(id,3);}
    constexpr auto s(int id) const noexcept     -> acc{ return (*this)(id,id);}
    constexpr auto s(int id) noexcept           -> acc&{return (*this)(id,id);}
    constexpr auto r(int id) const noexcept     -> acc{
        switch (id) {
            case 0:  return (*this)(0);
            case 1:  return (*this)(1);
            case 2:  return (*this)(2);
            case 3:  return (*this)(4);
            case 4:  return (*this)(5);
            case 5:  return (*this)(6);
            case 6:  return (*this)(8);
            case 7:  return (*this)(9);
            case 8:  return (*this)(10);
            default: return (*this)(0);
        }
    }
    constexpr auto r(int id) noexcept -> acc&{
        switch (id) {
            case 0:  return (*this)(0);
            case 1:  return (*this)(1);
            case 2:  return (*this)(2);
            case 3:  return (*this)(4);
            case 4:  return (*this)(5);
            case 5:  return (*this)(6);
            case 6:  return (*this)(8);
            case 7:  return (*this)(9);
            case 8:  return (*this)(10);
            default: return (*this)(0);
        }
    }
};


template <typename acc>
constexpr auto operator*(const Matrix<acc,4,4> &m, const ColVec<acc,3> &v) noexcept -> ColVec<acc,3>{
    return {{
        m(0,0) * v.x() + m(0,1) * v.y() + m(0,2) * v.z(),
        m(1,0) * v.x() + m(1,1) * v.y() + m(1,2) * v.z(),
        m(2,0) * v.x() + m(2,1) * v.y() + m(2,2) * v.z(),
    }};
}

template <typename acc>
constexpr auto operator*(const Matrix4<acc> &m, const ColVec<acc,4> &v) noexcept -> ColVec<acc,4>{
    // return {{
    //     m(0,0) * v.x() + m(0,1) * v.y() + m(0,2) * v.z() + m(0,3) * v.w(),
    //     m(1,0) * v.x() + m(1,1) * v.y() + m(1,2) * v.z() + m(1,3) * v.w(),
    //     m(2,0) * v.x() + m(2,1) * v.y() + m(2,2) * v.z() + m(2,3) * v.w(),
    //     m(3,0) * v.x() + m(3,1) * v.y() + m(3,2) * v.z() + m(3,3) * v.w(),
    // }};
    return {{
        dot(v, m.row0()),
        dot(v, m.row1()),
        dot(v, m.row2()),
        dot(v, m.row3())
    }};
}

template <typename acc>
constexpr auto operator*(const RowVec<acc,4> &v, const Matrix4<acc> &m) noexcept -> RowVec<acc,4>{
    // return {{
    //     m(0,0) * v.x() + m(1,0) * v.y() + m(2,0) * v.z() + m(3,0) * v.w(),
    //     m(0,1) * v.x() + m(1,1) * v.y() + m(2,1) * v.z() + m(3,1) * v.w(),
    //     m(0,2) * v.x() + m(1,2) * v.y() + m(2,2) * v.z() + m(3,2) * v.w(),
    //     m(0,3) * v.x() + m(1,3) * v.y() + m(2,3) * v.z() + m(3,3) * v.w(),
    // }};
    return {{
        dot(v, m.col0()),
        dot(v, m.col1()),
        dot(v, m.col2()),
        dot(v, m.col3())
    }};
}

template <typename acc>
constexpr auto operator*(const RowVec<acc,3> &v, const Matrix<acc,4,4> &m) noexcept -> RowVec<acc,3>{
    return {{
        m(0,0) * v.x() + m(1,0) * v.y() + m(2,0) * v.z(),
        m(0,1) * v.x() + m(1,1) * v.y() + m(2,1) * v.z(),
        m(0,2) * v.x() + m(1,2) * v.y() + m(2,2) * v.z(),
    }};
}

template <typename acc>
constexpr auto operator*(const Matrix<acc,4,4> &l, const Matrix<acc,4,4> &r) noexcept -> Matrix<acc,4,4>{

    const ColVec<acc,4> A0 = l.col(0);
    const ColVec<acc,4> A1 = l.col(1);
    const ColVec<acc,4> A2 = l.col(2);
    const ColVec<acc,4> A3 = l.col(3);

    const ColVec<acc,4> B0 = r.col(0);
    const ColVec<acc,4> B1 = r.col(1);
    const ColVec<acc,4> B2 = r.col(2);
    const ColVec<acc,4> B3 = r.col(3);

    return Mat4<acc>(
        A0 * B0(0) + A1 * B0(1) + A2 * B0(2) + A3 * B0(3),
        A0 * B1(0) + A1 * B1(1) + A2 * B1(2) + A3 * B1(3),
        A0 * B2(0) + A1 * B2(1) + A2 * B2(2) + A3 * B2(3),
        A0 * B3(0) + A1 * B3(1) + A2 * B3(2) + A3 * B3(3)
    );
}

template <typename acc>
constexpr auto trace(const Matrix<acc,4,4> &m) noexcept -> acc{
    return m(0) + m(5) + m(10) + m(15);
}

template <typename acc>
constexpr auto determinant(const Matrix<acc,4,4> &m) noexcept -> acc {

    const acc subFactor00 = m(2,2) * m(3,3) - m(2,3) * m(3,2);
    const acc subFactor01 = m(1,2) * m(3,3) - m(1,3) * m(3,2);
    const acc subFactor02 = m(1,2) * m(2,3) - m(1,3) * m(2,2);
    const acc subFactor03 = m(0,2) * m(3,3) - m(0,3) * m(3,2);
    const acc subFactor04 = m(0,2) * m(2,3) - m(0,3) * m(2,2);
    const acc subFactor05 = m(0,2) * m(1,3) - m(0,3) * m(1,2);

    auto detCof = RowVec<acc,4>{{
        + (m(1,1) * subFactor00 - m(2,1) * subFactor01 + m(3,1) * subFactor02),
        - (m(0,1) * subFactor00 - m(2,1) * subFactor03 + m(3,1) * subFactor04),
        + (m(0,1) * subFactor01 - m(1,1) * subFactor03 + m(3,1) * subFactor05),
        - (m(0,1) * subFactor02 - m(1,1) * subFactor04 + m(2,1) * subFactor05)
    }};
    return
        m(0,0) * detCof(0) + m(1,0) * detCof(1) +
        m(2,0) * detCof(2) + m(3,0) * detCof(3);
}


template <typename acc>
constexpr auto inverse(const Matrix<acc,4,4> &m) noexcept -> Matrix<acc,4,4>{

    const acc det  = determinant(m);
    if(almost_equal<acc>(det, acc{0})){
        return {};
    }

    acc c00 = m(2,2) * m(3,3) - m(2,3) * m(3,2);
    acc c02 = m(2,1) * m(3,3) - m(2,3) * m(3,1);
    acc c03 = m(2,1) * m(3,2) - m(2,2) * m(3,1);
    acc c04 = m(1,2) * m(3,3) - m(1,3) * m(3,2);
    acc c06 = m(1,1) * m(3,3) - m(1,3) * m(3,1);
    acc c07 = m(1,1) * m(3,2) - m(1,2) * m(3,1);
    acc c08 = m(1,2) * m(2,3) - m(1,3) * m(2,2);
    acc c10 = m(1,1) * m(2,3) - m(1,3) * m(2,1);
    acc c11 = m(1,1) * m(2,2) - m(1,2) * m(2,1);
    acc c12 = m(0,2) * m(3,3) - m(0,3) * m(3,2);
    acc c14 = m(0,1) * m(3,3) - m(0,3) * m(3,1);
    acc c15 = m(0,1) * m(3,2) - m(0,2) * m(3,1);
    acc c16 = m(0,2) * m(2,3) - m(0,3) * m(2,2);
    acc c18 = m(0,1) * m(2,3) - m(0,3) * m(2,1);
    acc c19 = m(0,1) * m(2,2) - m(0,2) * m(2,1);
    acc c20 = m(0,2) * m(1,3) - m(0,3) * m(1,2);
    acc c22 = m(0,1) * m(1,3) - m(0,3) * m(1,1);
    acc c23 = m(0,1) * m(1,2) - m(0,2) * m(1,1);

    Pt4<acc> f0{c00, c00, c02, c03};
    Pt4<acc> f1{c04, c04, c06, c07};
    Pt4<acc> f2{c08, c08, c10, c11};
    Pt4<acc> f3{c12, c12, c14, c15};
    Pt4<acc> f4{c16, c16, c18, c19};
    Pt4<acc> f5{c20, c20, c22, c23};

    Pt4<acc> v0{m(0,1), m(0,0), m(0,0), m(0,0)};
    Pt4<acc> v1{m(1,1), m(1,0), m(1,0), m(1,0)};
    Pt4<acc> v2{m(2,1), m(2,0), m(2,0), m(2,0)};
    Pt4<acc> v3{m(3,1), m(3,0), m(3,0), m(3,0)};

    Pt4<acc> i0 = v1 * f0 - v2 * f1 + v3 * f2;
    Pt4<acc> i1 = v0 * f0 - v2 * f3 + v3 * f4;
    Pt4<acc> i2 = v0 * f1 - v1 * f3 + v3 * f5;
    Pt4<acc> i3 = v0 * f2 - v1 * f4 + v2 * f5;

    Pt4<acc> sA{+1, -1, +1, -1};
    Pt4<acc> sB{-1, +1, -1, +1};


    return Mat4<acc>(
        transpose(Pt4<acc>(i0 * sA)),
        transpose(Pt4<acc>(i1 * sB)),
        transpose(Pt4<acc>(i2 * sA)),
        transpose(Pt4<acc>(i3 * sB))
    ) * acc{1}/ det;

    // RowVec<acc,4> f0{{c00, c00, c02, c03}};
    // RowVec<acc,4> f1{{c04, c04, c06, c07}};
    // RowVec<acc,4> f2{{c08, c08, c10, c11}};
    // RowVec<acc,4> f3{{c12, c12, c14, c15}};
    // RowVec<acc,4> f4{{c16, c16, c18, c19}};
    // RowVec<acc,4> f5{{c20, c20, c22, c23}};

    // RowVec<acc,4> v0{{m(0,1), m(0,0), m(0,0), m(0,0)}};
    // RowVec<acc,4> v1{{m(1,1), m(1,0), m(1,0), m(1,0)}};
    // RowVec<acc,4> v2{{m(2,1), m(2,0), m(2,0), m(2,0)}};
    // RowVec<acc,4> v3{{m(3,1), m(3,0), m(3,0), m(3,0)}};

    // RowVec<acc,4> i0 = v1 * f0 - v2 * f1 + v3 * f2;
    // RowVec<acc,4> i1 = v0 * f0 - v2 * f3 + v3 * f4;
    // RowVec<acc,4> i2 = v0 * f1 - v1 * f3 + v3 * f5;
    // RowVec<acc,4> i3 = v0 * f2 - v1 * f4 + v2 * f5;

    // RowVec<acc,4> sA{{+1, -1, +1, -1}};
    // RowVec<acc,4> sB{{-1, +1, -1, +1}};

    // return Mat4<acc>(
    //     transpose(i0 * sA),
    //     transpose(i1 * sB),
    //     transpose(i2 * sA),
    //     transpose(i3 * sB)
    // ) * acc{1}/ det;
}

template<typename acc>
constexpr auto translation_v3(const Matrix<acc,4,4> &m) noexcept -> RowVec<acc,3>{
    return {{m(3,0), m(3,1), m(3,2)}};
}
template<typename acc>
constexpr auto translation_v4(const Matrix<acc,4,4> &m) noexcept -> RowVec<acc,4>{
    return {{m(3,0), m(3,1), m(3,2), acc{1}}};
}
template<typename acc>
constexpr auto translation_m4x4(const RowVec<acc,3> &t) noexcept -> Matrix<acc,4,4>{
    return {{
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        t.x(), t.y(), t.z(), 1
    }};
}
template<typename acc>
constexpr auto translate(const Matrix<acc,4,4> &m, const RowVec<acc,3> &t) noexcept -> Matrix<acc,4,4> {
    return Mat4<acc>(
        m.row(0),
        m.row(1),
        m.row(2),
        m.row(0)*t.x() + m.row(1)*t.y() + m.row(2)*t.z() + m.row(3)
    );
}

/**
 * @param pitch: angle in degrees
 */
template<typename acc>
constexpr auto x_rotation_m4x4(acc pitch) noexcept -> Matrix<acc,3,4>{
    pitch = deg_2_rad<acc>(pitch);
    const acc cosA = cos(pitch);
    const acc sinA = sin(pitch);
    return {{
        1,  0,     0,    0,
        0,  cosA, -sinA, 0,
        0,  sinA,  cosA, 0,
        0,  0,     0,    1
    }};
}
/**
 * @param head: angle in degrees
 */
template<typename acc>
static constexpr auto y_rotation_m4x4(acc head) noexcept -> Matrix<acc,4,4>{
    head = deg_2_rad<acc>(head);
    const acc cosA = cos(head);
    const acc sinA = sin(head);
    return {{
        cosA,   0,  sinA, 0,
        0,      1,  0,    0,
        -sinA,  0,  cosA, 0,
        0,      0,  0,    1
    }};
}
/**
 * @param roll: angle in degrees
 */
template<typename acc>
constexpr auto z_rotation_m4x4(acc roll) noexcept -> Matrix<acc,4,4>{
    roll = deg_2_rad<acc>(roll);
    const acc cosA = cos(roll);
    const acc sinA = sin(roll);
    return {{
        cosA,   -sinA,  0, 0,
        sinA,   cosA,   0, 0,
        0,      0,      1, 0,
        0,      0,      0, 1
    }};
}
template<typename acc>
constexpr auto rotation_m3x3(const Matrix<acc,4,4> &m) noexcept -> Matrix<acc,3,3>{
    return {{
        m(0,0),m(0,1),m(0,2),
        m(1,0),m(1,1),m(1,2),
        m(2,0),m(2,1),m(2,2)
    }};
}
template<typename acc>
constexpr auto rotation_m4x4(const Matrix<acc,4,4> &m) noexcept -> Matrix<acc,4,4> {
    return {{
        m(0,0),m(0,1),m(0,2),0,
        m(1,0),m(1,1),m(1,2),0,
        m(2,0),m(2,1),m(2,2),0,
        0     ,0     ,0     ,1
    }};
}
template<typename acc>
constexpr auto rotation_m4x4(const RowVec<acc,3> &angles) noexcept -> Matrix<acc,4,4> {
    auto m = rotation_m3x3(angles);
    return {{
        m(0,0),m(0,1),m(0,2),0,
        m(1,0),m(1,1),m(1,2),0,
        m(2,0),m(2,1),m(2,2),0,
        0     ,0     ,0     ,1
    }};
}

template<typename acc>
constexpr auto rotate(const Matrix<acc,4,4> &m, const RowVec<acc,3> &angles) noexcept -> Matrix<acc,4,4> {
    return m*rotation_m4x4(angles*PI_180<acc>);
}
template<typename acc>
constexpr auto rotate(const Matrix<acc,4,4> &m, const RowVec<acc,3> &v, acc angle) noexcept -> Matrix<acc,4,4> {

    angle = deg_2_rad(angle);

    const acc a = angle;
    const acc c = cos(a);
    const acc s = sin(a);

    auto axis = normalize(v);
    auto temp = ((acc{1} - c) * axis);

    Mat3<acc> r = {
        temp(0) * axis(0) + c,
        temp(1) * axis(0) - axis(2) * s,
        temp(2) * axis(0) + axis(1) * s,
        temp(0) * axis(1) + axis(2) * s,
        c + temp(1) * axis(1),
        temp(2) * axis(1) - axis(0) * s,
        temp(0) * axis(2) - axis(1) * s,
        temp(1) * axis(2) + axis(0) * s,
        temp(2) * axis(2) + c
    };

    return Mat4<acc>(
        m.col(0) * r(0,0) +  m.col(1) * r(1,0) + m.col(2) * r(2,0),
        m.col(0) * r(0,1) +  m.col(1) * r(1,1) + m.col(2) * r(2,1),
        m.col(0) * r(0,2) +  m.col(1) * r(1,2) + m.col(2) * r(2,2),
        m.col(3)
    );
}

template<typename acc>
constexpr auto scale_m4x4(const RowVec<acc,3> &s) noexcept -> Matrix<acc,4,4> {
    return {{
        s.x(),  0,      0,      0,
        0,      s.y(),  0,      0,
        0,      0,      s.z(),  0,
        0,      0,      0,      1
    }};
}
template<typename acc>
constexpr auto scale(const Matrix<acc,4,4> &m, const RowVec<acc,3> &s) noexcept -> Matrix<acc,4,4> {
    return Mat4<acc>(
        m.row(0) * s.x(),
        m.row(1) * s.y(),
        m.row(2) * s.z(),
        m.row(3)
    );
}

template<typename acc>
constexpr auto perspective(acc fovY, acc aspectRatio, acc zNear, acc zFar) -> Matrix<acc,4,4> {
    // fovy = 2 atan(w/(2d))
    // w: width of the object perpendicular to the line of sight
    // d: distance of the object
    const auto c = acc{1}/std::tan(fovY/2);
    const auto diffZ = zFar-zNear;
    return {{
        c/aspectRatio, 0,    0,                     0,
        0,             c,    0,                     0,
        0,             0,    -(zFar+zNear)/diffZ,   -1,
        0,             0,    -(2*zFar*zNear)/diffZ, 0
    }};
}

template<typename acc>
constexpr auto transform(const RowVec<acc,3> &scale, const RowVec<acc,3> &rotation, const RowVec<acc,3> &translate) -> Matrix<acc,4,4> {
//    return translation_m4x4(translate)*rotation_m4x4(rotation*PI_180<acc>)*scale_m4x4(scale);
    return scale_m4x4(scale)*rotation_m4x4(rotation*PI_180<acc>)*translation_m4x4(translate);
}






template<typename acc>
constexpr auto axis_angle_m4x4(const Vec3<acc> &axis, acc angle) noexcept -> Matrix<acc,4,4>{
    return {axis_angle_m3x3(axis,angle)};
}



//template<typename acc>
//constexpr auto transform2(const RowVec<acc,3> &scale, const RowVec<acc,3> &rotation, const RowVec<acc,3> &translate) -> Matrix<acc,4,4> {
//    Mat4<acc> tr = rotation_m4x4(rotation*PI_180<acc>)*scale_m4x4(scale);
//    tr.t(0) = translate.x();
//    tr.t(1) = translate.y();
//    tr.t(2) = translate.z();
//    return tr;
////    return translation_m4x4(translate)*rotation_m4x4(rotation*PI_180<acc>)*scale_m4x4(scale);
//}

template<typename acc>
constexpr auto look_at(const Pt<acc,3> &eye, const Pt<acc,3> &at, const Pt<acc,3> &up) noexcept -> Matrix<acc,4,4> {

//    auto f = normalize(at - eye);
//    auto s = normalize(cross(f, up));
//    auto u = cross(s, f);
//    return{{
//        s.x(), s.y(), s.z(), -dot(s, eye),
//        u.x(), u.y(), u.z(), -dot(u, eye),
//       -f.x(),-f.y(),-f.z(), dot(f, eye),
//        0,     0,     0,     1
//    }};


    auto zaxis = normalize(at - eye);
    auto xaxis = normalize(cross(zaxis, up));
    auto yaxis = cross(xaxis, zaxis);
    zaxis = acc{-1}*zaxis;

    return {{
        xaxis.x(), yaxis.x(), zaxis.x(), 0,
        xaxis.y(), yaxis.y(), zaxis.y(), 0,
        xaxis.z(), yaxis.z(), zaxis.z(), 0,
        -dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1
    }};
}


template<typename acc>
constexpr auto orthographic(acc left, acc right, acc bottom, acc top, acc zNear, acc zFar) -> Matrix<acc,4,4> {
//        return Mat4<acc>{
//            acc{2}/(right-left),0,0,0,
//            0,acc{2}/(top-bottom),0,0,
//            0,0,acc{-2}/(zFar-zNear),0,
//            -((right+left)/(right-left)), -((top+bottom)/(top-bottom)), -((zFar+zNear)/(zFar-zNear)), 1
//        };
    return {{
        acc{2}/(right-left),0,0,-((right+left)/(right-left)),
        0,acc{2}/(top-bottom),0,-((top+bottom)/(top-bottom)),
        0,0,acc{-2}/(zFar-zNear),-((zFar+zNear)/(zFar-zNear)),
        0, 0, 0, 1
    }};
}

}


