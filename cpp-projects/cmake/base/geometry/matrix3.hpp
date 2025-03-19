
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

// std
#include <numeric>

// local
#include "utility/constants.hpp"
#include "geometry/matrix.hpp"

namespace tool::geo {

template<typename acc>
struct Matrix3;

template<typename acc>
using Mat3  = Matrix3<acc>;
using Mat3f = Mat3<float>;
using Mat3d = Mat3<double>;

template<typename acc>
struct Matrix3 : Matrix<acc,3,3>{

    Matrix3() = default;

    constexpr Matrix3(const Matrix<acc,3,3> &m) noexcept{
        this->array = m.array;
    }
    constexpr Matrix3(Matrix<acc,3,3> &&m) noexcept{
        this->array = std::move(m.array);
    }
    constexpr Matrix3(RowVec<acc,3> r1, RowVec<acc,3> r2, RowVec<acc,3> r3) noexcept{
        this->array = {
            r1.x(), r1.y(), r1.z(),
            r2.x(), r2.y(), r2.z(),
            r3.x(), r3.y(), r3.z()
        };
    }
    constexpr Matrix3(ColVec<acc,3> c1, ColVec<acc,3> c2, ColVec<acc,3> c3) noexcept{
        this->array = {
            c1.x(), c2.x(), c3.x(),
            c1.y(), c2.y(), c3.y(),
            c1.z(), c2.z(), c3.z()
        };
    }

    constexpr Matrix3(acc v00,          acc v01 = acc{0}, acc v02 = acc{0},
                      acc v10 = acc{0}, acc v11 = acc{0}, acc v12 = acc{0},
                      acc v20 = acc{0}, acc v21 = acc{0}, acc v22 = acc{0}) noexcept{
        this->array = {
            v00,v01,v02,
            v10,v11,v12,
            v20,v21,v22
        };
    }

    [[nodiscard]] constexpr auto row0() const noexcept -> RowVec<acc,3>{
        return {{(*this)[0],(*this)[1],(*this)[2]}};
    }
    [[nodiscard]] constexpr auto row1() const noexcept -> RowVec<acc,3>{
        return {{(*this)[4],(*this)[5],(*this)[6]}};
    }
    [[nodiscard]] constexpr auto row2() const noexcept -> RowVec<acc,3>{
        return {{(*this)[8],(*this)[9],(*this)[10]}};
    }

    [[nodiscard]] constexpr auto col0() const noexcept -> ColVec<acc,3>{
        return {{(*this)[0],(*this)[3],(*this)[6]}};
    }
    [[nodiscard]] constexpr auto col1() const noexcept -> ColVec<acc,3>{
        return {{(*this)[1],(*this)[4],(*this)[7]}};
    }
    [[nodiscard]] constexpr auto col2() const noexcept -> ColVec<acc,3>{
        return {{(*this)[2],(*this)[5],(*this)[8]}};
    }
};

template <typename acc>
constexpr auto operator*(const Matrix<acc,3,3> &m, const ColVec<acc,3> &v) noexcept -> ColVec<acc,3>{
    return {{
        m(0,0) * v.x() + m(0,1) * v.y() + m(0,2) * v.z(),
        m(1,0) * v.x() + m(1,1) * v.y() + m(1,2) * v.z(),
        m(2,0) * v.x() + m(2,1) * v.y() + m(2,2) * v.z()
    }};
}

template <typename acc>
constexpr auto operator*(const RowVec<acc,3> &v, const Matrix<acc,3,3> &m) noexcept -> RowVec<acc,3>{
    return {{
        m(0,0) * v.x() + m(1,0) * v.y() + m(2,0) * v.z(),
        m(0,1) * v.x() + m(1,1) * v.y() + m(2,1) * v.z(),
        m(0,2) * v.x() + m(1,2) * v.y() + m(2,2) * v.z()
    }};
}

template <typename acc>
constexpr auto operator*(const Matrix<acc,3,3> &l, const Matrix<acc,3,3> &r) noexcept -> Matrix<acc,3,3>{

    const acc A00 = l(0,0);
    const acc A01 = l(0,1);
    const acc A02 = l(0,2);
    const acc A10 = l(1,0);
    const acc A11 = l(1,1);
    const acc A12 = l(1,2);
    const acc A20 = l(2,0);
    const acc A21 = l(2,1);
    const acc A22 = l(2,2);

    const acc B00 = r(0,0);
    const acc B01 = r(0,1);
    const acc B02 = r(0,2);
    const acc B10 = r(1,0);
    const acc B11 = r(1,1);
    const acc B12 = r(1,2);
    const acc B20 = r(2,0);
    const acc B21 = r(2,1);
    const acc B22 = r(2,2);

    return {{
        A00 * B00 + A01 * B10 + A02 * B20,
        A00 * B01 + A01 * B11 + A02 * B21,
        A00 * B02 + A01 * B12 + A02 * B22,
        A10 * B00 + A11 * B10 + A12 * B20,
        A10 * B01 + A11 * B11 + A12 * B21,
        A10 * B02 + A11 * B12 + A12 * B22,
        A20 * B00 + A21 * B10 + A22 * B20,
        A20 * B01 + A21 * B11 + A22 * B21,
        A20 * B02 + A21 * B12 + A22 * B22
    }};
}

template <typename acc>
constexpr auto trace(const Matrix<acc,3,3> &m) noexcept -> acc{
    return m(0) + m(4) + m(8);
}

template <typename acc>
constexpr auto determinant(const Matrix<acc,3,3> &m) noexcept -> acc {
    return
        + m(0,0) * (m(1,1) * m(2,2) - m(1,2) * m(2,1))
        - m(0,1) * (m(1,0) * m(2,2) - m(1,2) * m(2,0))
        + m(0,2) * (m(1,0) * m(2,1) - m(1,1) * m(2,0));
}

template <typename acc>
constexpr auto inverse(const Matrix<acc,3,3> &m) noexcept -> Matrix<acc,3,3>{

    const acc det  = determinant(m);
    if(almost_equal<acc>(det, acc{0})){
        return {};
    }

    const acc invDet = acc{1}/ det;
    return{{
        + (m(1,1) * m(2,2) - m(1,2) * m(2,1)) * invDet,
        - (m(0,1) * m(2,2) - m(0,2) * m(2,1)) * invDet,
        + (m(0,1) * m(1,2) - m(0,2) * m(1,1)) * invDet,
        - (m(1,0) * m(2,2) - m(1,2) * m(2,0)) * invDet,
        + (m(0,0) * m(2,2) - m(0,2) * m(2,0)) * invDet,
        - (m(0,0) * m(1,2) - m(0,2) * m(1,0)) * invDet,
        + (m(1,0) * m(2,1) - m(1,1) * m(2,0)) * invDet,
        - (m(0,0) * m(2,1) - m(0,1) * m(2,0)) * invDet,
        + (m(0,0) * m(1,1) - m(0,1) * m(1,0)) * invDet
    }};
}

/**
 * @param pitch: angle in degrees
 */
template<typename acc>
constexpr auto x_rotation_m3x3(acc pitch) noexcept -> Matrix<acc,3,3>{
    pitch = deg_2_rad<acc>(pitch);
    const acc cosA = cos(pitch);
    const acc sinA = sin(pitch);
    return {{
        1,  0,     0,
        0,  cosA, -sinA,
        0,  sinA,  cosA
    }};
}

/**
 * @param head: angle in degrees
 */
template<typename acc>
static constexpr auto y_rotation_m3x3(acc head) noexcept -> Matrix<acc,3,3>{
    head = deg_2_rad<acc>(head);
    const acc cosA = cos(head);
    const acc sinA = sin(head);
    return {{
        cosA,   0,  sinA,
        0,      1,  0,
        -sinA,  0,  cosA
    }};
}

/**
 * @param roll: angle in degrees
 */
template<typename acc>
constexpr auto z_rotation_m3x3(acc roll) noexcept -> Matrix<acc,3,3>{
    roll = deg_2_rad<acc>(roll);
    const acc cosA = cos(roll);
    const acc sinA = sin(roll);
    return {{
        cosA,   -sinA,  0,
        sinA,   cosA,   0,
        0,      0,      1
    }};
}

/**
 * equivalent to: return y_rotation_matrix(angles.y())*x_rotation_matrix(angles.x())*z_rotation_matrix(angles.z());
 */
template<typename acc>
constexpr auto rotation_m3x3(const RowVec<acc,3> &angles) noexcept -> Matrix<acc,3,3>{

    const auto cosH = std::cos(angles.y());
    const auto cosP = std::cos(angles.x());
    const auto cosR = std::cos(angles.z());
    const auto sinH = std::sin(angles.y());
    const auto sinP = std::sin(angles.x());
    const auto sinR = std::sin(angles.z());

    return {{
        cosR*cosH-sinR*sinP*sinH, -sinR*cosP, cosR*sinH+sinR*sinP*cosH,
        sinR*cosH+cosR*sinP*sinH, cosR*cosP,  sinR*sinH-cosR*sinP*cosH,
        -cosP*sinH,               sinP,       cosP*cosH
    }};
}

template<typename acc>
auto axis_angle_m3x3(const RowVec<acc,3> &axis, acc angle) noexcept -> Matrix<acc,3,3>{

    angle = deg_2_rad<acc>(angle);
    auto u = normalize(axis);
    const auto cA = cos(angle);
    const auto sA = sin(angle);
    const auto ux = u.x();
    const auto uy = u.y();
    const auto uz = u.z();
    const auto ux2 = ux*ux;
    const auto uy2 = uy*uy;
    const auto uz2 = uz*uz;

    return {{
        cA + ux2*(1-cA), ux*uy*(1-cA) - uz*sA, ux*uz*(1-cA) + uy*sA,
        uy*ux*(1-cA) + uz*sA,   cA+uy2*(1-cA), uy*uz*(1-cA)-ux*sA,
        uz*ux*(1-cA)-uy*sA, uz*uy*(1-cA) +ux*sA,    cA +uz2*(1-cA)
    }};
}

/**
 * @return head/pitch/roll angles in degrees
 */
template<typename acc>
constexpr auto to_hpr_angles(const Matrix<acc,3,3> &m) noexcept -> RowVec<acc,3>{ // head/pitch/roll

    const auto p = std::asin(m(1,2));
    if(almost_equal<acc>(std::abs(p), PI_2<acc>)){ // grimbal lock
        return{
            0,
            d180_PI<acc>*p,
            d180_PI<acc>*(std::atan2(m(0,1),m(0,0)))
        };
    }

    return {{
        d180_PI<acc>*std::atan2(-m(0,2),m(2,2)),
        d180_PI<acc>*p,
        d180_PI<acc>*std::atan2(-m(1,0),m(1,1))
    }};
}


}
