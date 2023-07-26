
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
#include "geometry/matrix.hpp"

namespace tool::geo {

template<typename acc>
struct Matrix2;

template<typename acc>
using Mat2  = Matrix2<acc>;
using Mat2f = Mat2<float>;
using Mat2d = Mat2<double>;

template<typename acc>
struct Matrix2 : Matrix<acc,2,2>{

    Matrix2() = default;
    Matrix2(const Matrix2& other) = default;
    Matrix2& operator=(const Matrix2& other) = default;
    Matrix2(Matrix2&& other) = default;
    Matrix2& operator=(Matrix2&& other) = default;

    constexpr Matrix2(const Matrix<acc,2,2> &m) noexcept{
        this->array = m.array;
    }
    constexpr Matrix2(Matrix<acc,2,2> &&m) noexcept{
        this->array = std::move(m.array);
    }
    constexpr Matrix2(RowVec<acc,2> r1, RowVec<acc,2> r2) noexcept{
        this->array = {
            r1.x(), r1.y(),
            r2.x(), r2.y()
        };
    }
    constexpr Matrix2(ColVec<acc,2> c1, ColVec<acc,2> c2) noexcept{
        this->array = {
            c1.x(), c2.x(),
            c1.y(), c2.y()
        };
    }
    constexpr Matrix2(acc v00, acc v01 = acc{0}, acc v10 = acc{0}, acc v11 = acc{0}) noexcept{
        this->array = {
            v00,v01,
            v10,v11
        };
    }
};

template <typename acc>
constexpr auto operator*(const Matrix<acc,2,2> &m, const ColVec<acc,2> &v) noexcept -> ColVec<acc,2>{
    return {{
        m(0,0) * v.x() + m(0,1) * v.y(),
        m(1,0) * v.x() + m(1,1) * v.y()
    }};
}

template <typename acc>
constexpr auto operator*(const RowVec<acc,2> &v, const Matrix<acc,2,2> &m) noexcept -> RowVec<acc,2>{
    return {{
        m(0,0) * v.x() + m(1,0) * v.y(),
        m(0,1) * v.x() + m(1,1) * v.y()
    }};
}

template <typename acc>
constexpr auto operator*(const Matrix<acc,2,2> &l, const Matrix<acc,2,2> &r) noexcept -> Matrix<acc,2,2>{
    return{{
        l(0,0) * r(0,0) + l(0,1) * r(1,0),
        l(0,0) * r(0,1) + l(0,1) * r(1,1),
        l(1,0) * r(0,0) + l(1,1) * r(1,0),
        l(1,0) * r(0,1) + l(1,1) * r(1,1)
    }};
}

template <typename acc>
constexpr auto trace(const Matrix<acc,2,2> &m) noexcept -> acc{
    return m(0) + m(3);
}

template <typename acc>
constexpr auto determinant(const Matrix<acc,2,2> &m) noexcept -> acc {
    return m(0,0)*m(1,1)-m(0,1)*m(1,0);
}

template <typename acc>
constexpr auto inverse(const Matrix<acc,2,2> &m) noexcept -> Matrix<acc,2,2>{

    const acc det  = determinant(m);
    if(almost_equal<acc>(det, acc{0})){
        return {};
    }

    const acc invDet = acc{1}/ det;
    return {{
        + m(1,1) * invDet,
        - m(0,1) * invDet,
        - m(1,0) * invDet,
        + m(0,0) * invDet
    }};
}
}
