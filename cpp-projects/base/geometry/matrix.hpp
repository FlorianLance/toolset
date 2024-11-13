

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
#include <algorithm>
#include <ostream>
#include <functional>
#include <array>
#include <numeric>
#include <span>

// local
#include "utility/math.hpp"

namespace tool::geo {

template<typename acc, int _rows, int _cols>
struct Matrix;

template<typename acc, int _rows>
using ColVec = Matrix<acc,_rows,1>;

template<typename acc, int _cols>
using RowVec = Matrix<acc,1,_cols>;

// Row major matrix
template<typename acc, int _rows, int _cols>
struct Matrix{

    static_assert((_rows != 0) && (_cols != 0), "Mat must have a number of rows and columns > 0" );

    Matrix() = default;

    constexpr Matrix(const std::array<acc, _rows*_cols> &array) noexcept {
        this->array = array;
    }
    constexpr Matrix(std::array<acc, _rows*_cols> &&array) noexcept{
        this->array = std::move(array);
    }

    [[nodiscard]] constexpr auto rows() const noexcept -> int{return _rows;}
    [[nodiscard]] constexpr auto cols() const noexcept -> int{return _cols;}
    [[nodiscard]] constexpr auto size() const noexcept -> int{return rows()*cols();}

    [[nodiscard]] constexpr auto operator[](size_t pos)             noexcept  ->       acc& {return array[pos];}
    [[nodiscard]] constexpr auto operator[](size_t pos)       const noexcept  -> const acc& {return array[pos];}

    [[nodiscard]] constexpr auto operator()(size_t pos)             noexcept  ->       acc& {return array[pos];}
    [[nodiscard]] constexpr auto operator()(size_t pos)       const noexcept  -> const acc& {return array[pos];}
    [[nodiscard]] constexpr auto operator()(int row, int col)                 ->       acc& {return array[row*_cols + col];}
    [[nodiscard]] constexpr auto operator()(int row, int col) const           -> const acc& {return array[row*_cols + col];}

    [[nodiscard]] constexpr auto at(size_t pos)                               ->       acc& {return array.at(pos);}
    [[nodiscard]] constexpr auto at(size_t pos)               const           -> const acc& {return array.at(pos);}
    [[nodiscard]] constexpr auto at(int row, int col)                         ->       acc& {return (*this)(row,col);}
    [[nodiscard]] constexpr auto at(int row, int col)         const           -> const acc& {return (*this)(row,col);}


    [[nodiscard]] constexpr auto row0() const noexcept -> RowVec<acc,_cols>{
        RowVec<acc,_cols> r;
        std::copy(std::begin(array), std::begin(array) + _cols, std::begin(r.array));
        return r;
    }
    [[nodiscard]] constexpr auto row1() const noexcept -> RowVec<acc,_cols>{
        static_assert(_rows > 1, "row0 only available for matrices with a row size > 1");
        RowVec<acc,_cols> r;
        std::copy(std::begin(array) + _cols, std::begin(array) + 2*_cols, std::begin(r.array));
        return r;
    }

    [[nodiscard]] constexpr auto row(int row) const noexcept -> RowVec<acc,_cols>{
        if(row >= _rows){
            return {};
        }
        RowVec<acc,_cols> r;
        const int start = row* _cols;
        std::copy(std::begin(array) + start, std::begin(array) + start + _cols, std::begin(r.array));
        return r;
    }

    [[nodiscard]] constexpr auto col(int col) const noexcept -> ColVec<acc,_rows>{
        if(col >= _cols){
            return {};
        }
        ColVec<acc,_rows> c;
        for(int ii = 0; ii < _rows; ++ii){
            c(ii) = at(ii, col);
        }
        return c;
    }

    [[nodiscard]] constexpr auto x()       noexcept ->       acc& {
        static_assert((_rows == 1 || _cols == 1), "X only available for row or column matrices.");
        return array[0];
    }
    [[nodiscard]] constexpr auto x() const noexcept -> const acc& {
        static_assert((_rows == 1 || _cols == 1), "X only available for row or column matrices.");
        return array[0];
    }

    [[nodiscard]] constexpr auto y()      noexcept  ->       acc& {
        static_assert((_rows == 1 || _cols == 1), "Y only available for row or column matrices.");
        static_assert(_rows*_cols > 1, "Y only available for row or column matrices of a dimension > 1.");
        return array[1];
    }
    [[nodiscard]] constexpr auto y() const noexcept -> const acc& {
        static_assert((_rows == 1 || _cols == 1), "Y only available for row or column matrices.");
        static_assert(_rows*_cols > 1, "Y only available for row or column matrices of a dimension > 1.");
        return array[1];
    }

    [[nodiscard]] constexpr auto z()       noexcept -> acc& {
        static_assert((_rows == 1 || _cols == 1), "Y only available for row or column matrices.");
        static_assert(_rows*_cols > 2, "Y only available for row or column matrices of a dimension > 2.");
        return array[2];
    }
    [[nodiscard]] constexpr auto z() const noexcept -> const acc& {
        static_assert((_rows == 1 || _cols == 1), "Z only available for row or column matrices.");
        static_assert(_rows*_cols > 2, "Z only available for row or column matrices of a dimension > 2.");
        return array[2];
    }

    [[nodiscard]] constexpr auto w()        noexcept -> acc& {
        static_assert((_rows == 1 || _cols == 1), "W only available for row or column matrices.");
        static_assert(_rows*_cols > 3, "W only available for row or column matrices of a dimension > 3.");
        return array[3];
    }
    [[nodiscard]] constexpr auto w() const noexcept -> const acc& {
        static_assert((_rows == 1 || _cols == 1), "W only available for row or column matrices.");
        static_assert(_rows*_cols > 3, "W only available for row or column matrices of a dimension > 3.");
        return array[3];
    }

    [[nodiscard]] constexpr auto xy() const noexcept -> RowVec<acc,2> {
        static_assert((_rows == 1 || _cols == 1), "XY only available for row or column matrices.");
        static_assert(_rows*_cols > 1, "XY only available for row or column matrices of a dimension >= 2.");
        return {array[0],array[1]};
    }

    [[nodiscard]] constexpr auto xyz() const noexcept -> RowVec<acc,3> {
        static_assert((_rows == 1 || _cols == 1), "XYZ only available for row or column matrices.");
        static_assert(_rows*_cols > 2, "XYZ only available for row or column matrices of a dimension >= 3.");
        return {{x(),y(),z()}};
    }

    static constexpr auto identity() noexcept -> Matrix<acc,_rows,_cols>{

        static_assert(_rows == _cols, "Identity only available for square matrices.");
        if constexpr(_rows == 1){
            return {{1}};
        } else if constexpr(_rows == 2){
            return {{1,0,
                     0,1}};
        } else if constexpr(_rows == 3){
            return {{1,0,0
                    ,0,1,0,
                    0,0,1}};
        } else if constexpr(_rows == 4){
            return {{1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1}};
        }else{
            Matrix m;
            for(int ii = 0; ii < _rows; ++ii){
                m(ii,ii) = acc{1};
            }
            return m;
        }
    }

    constexpr auto is_identity() const noexcept -> bool{
        for(int ii = 0; ii < _rows; ++ii){
            for(int jj = 0; jj < _cols; ++jj){
                if(ii == jj){
                    if(!almost_equal(at(ii,jj), acc{1})){
                        return false;
                    }
                }else{
                    if(!almost_equal(at(ii,jj), acc{0})){
                        return false;
                    }
                }
            }
        }
        return true;
    }

    template <typename acc2>
    constexpr auto conv() const noexcept -> Matrix<acc2, _rows, _cols>{
        Matrix<acc2, _rows, _cols> mc;
        for(int ii = 0; ii < _rows*_cols; ++ii){
            mc(ii) = static_cast<acc2>(at(ii));
        }
        return mc;
    }

    constexpr auto multiply_point(const RowVec<acc,_cols> &pt) const noexcept -> RowVec<acc,_cols>{

        if constexpr(_cols == 2){
            return {{
                pt.x() * at(0,0) + pt.y() * at(1,0),
                pt.x() * at(0,1) + pt.y() * at(1,1)
            }};
        } else if constexpr(_cols == 3){
            return {{
                pt.x() * at(0,0) + pt.y() * at(1,0) + pt.z() * at(2,0) + at(3,0),
                pt.x() * at(0,1) + pt.y() * at(1,1) + pt.z() * at(2,1) + at(3,1),
                pt.x() * at(0,2) + pt.y() * at(1,2) + pt.z() * at(2,2) + at(3,2)
            }};
        }else if constexpr(_cols == 4){
            return{{
                pt.x() * at(0,0) + pt.y() * at(1,0) + pt.z() * at(2,0) + pt.w() * at(3,0),
                pt.x() * at(0,1) + pt.y() * at(1,1) + pt.z() * at(2,1) + pt.w() * at(3,1),
                pt.x() * at(0,2) + pt.y() * at(1,2) + pt.z() * at(2,2) + pt.w() * at(3,2),
                pt.x() * at(0,3) + pt.y() * at(1,3) + pt.z() * at(2,3) + pt.w() * at(3,3)
            }};
        }else{
            static_assert("Not implemented.");
        }
    }

    constexpr auto multiply_vector(const RowVec<acc,_cols> &vec) const noexcept -> RowVec<acc,_cols>{
        if constexpr(_cols == 2){
            return {{
                dot(vec, RowVec<acc,_cols>{{at(0,0), at(1,0)}}),
                dot(vec, RowVec<acc,_cols>{{at(0,1), at(1,1)}})
            }};
        } else if constexpr(_cols == 3){
            return {{
                dot(vec, RowVec<acc,_cols>{{at(0,0), at(1,0), at(2,0)}}),
                dot(vec, RowVec<acc,_cols>{{at(0,1), at(1,1), at(2,1)}}),
                dot(vec, RowVec<acc,_cols>{{at(0,2), at(1,2), at(2,2)}})
            }};
        }else if constexpr(_cols == 4){
            return {{
                dot(vec, RowVec<acc,_cols>{{at(0,0), at(1,0), at(2,0), at(3,0)}}),
                dot(vec, RowVec<acc,_cols>{{at(0,1), at(1,1), at(2,1), at(3,1)}}),
                dot(vec, RowVec<acc,_cols>{{at(0,2), at(1,2), at(2,2), at(3,2)}}),
                dot(vec, RowVec<acc,_cols>{{at(0,3), at(1,3), at(2,3), at(3,3)}})
            }};
        }else{
            static_assert("Not implemented.");
        }
    }

    // operators
//    constexpr auto operator-(const Matrix &m) const noexcept -> Matrix{
//        return (*this) = (*this)*acc{-1};
//    }

    constexpr auto operator+=(const Matrix &r) noexcept -> Matrix{
        return (*this) = (*this)+r;
    }

    constexpr auto operator+=(acc v) noexcept -> Matrix{
        return (*this) = (*this)+v;
    }

    constexpr auto operator-=(const Matrix &r) noexcept -> Matrix{
        return (*this) = (*this)-r;
    }

    constexpr auto operator-=(acc v) noexcept -> Matrix{
        return (*this) = (*this)-v;
    }

    template <int _colsR>
    constexpr auto operator*=(const Matrix<acc,_cols,_colsR> &r) noexcept -> Matrix<acc,_rows,_colsR>{
        return (*this) = (*this)*r;
    }

    constexpr auto operator*=(acc v) noexcept -> Matrix{
        return (*this) = (*this)*v;
    }

    constexpr auto operator/=(acc v) noexcept -> Matrix{
        return (*this) = (*this)/v;
    }

    [[nodiscard]] auto span()       noexcept -> std::span<acc>      {return array;}
    [[nodiscard]] auto cspan() const noexcept -> std::span<const acc>{return array;}

    std::array<acc, _rows*_cols> array = {};
};

// functions
template <typename acc, int _rows, int _cols>
constexpr auto compare(const Matrix<acc, _rows, _cols> &l, const Matrix<acc, _rows, _cols> &r, int ulp = 3) noexcept -> bool{
    return std::equal(std::cbegin(l.array), std::cend(l.array), std::cbegin(r.array),
        [ulp](const acc &v1, const acc &v2) -> bool{
            return almost_equal<acc>(v1,v2,ulp);
        }
    );
}

template <typename acc, int _rows, int _cols>
constexpr auto add(const Matrix<acc,_rows,_cols> &l, const Matrix<acc,_rows,_cols> &r) noexcept -> Matrix<acc,_rows,_cols>{
    Matrix<acc,_rows,_cols> res;
    std::transform(l.array.cbegin(), l.array.cend(), r.array.cbegin(), std::begin(res.array), std::plus<acc>());
    return res;
}
template <typename acc, int _rows, int _cols>
constexpr auto add(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    Matrix<acc,_rows,_cols> res;
    std::transform(m.array.cbegin(), m.array.cend(), std::begin(res.array), std::bind(std::plus<acc>(), std::placeholders::_1, value));
    return res;
}
template <typename acc, int _rows, int _cols>
constexpr auto add(acc value, const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_rows,_cols>{
    Matrix<acc,_rows,_cols> res;
    std::transform(m.array.cbegin(), m.array.cend(), std::begin(res.array), std::bind(std::plus<acc>(), std::placeholders::_1, value));
    return res;
}

template <typename acc, int _rows, int _cols>
constexpr auto substract(const Matrix<acc,_rows,_cols> &l, const Matrix<acc,_rows,_cols> &r) noexcept -> Matrix<acc,_rows,_cols>{
    Matrix<acc,_rows,_cols> res;
    std::transform(l.array.cbegin(), l.array.cend(), r.array.cbegin(), std::begin(res.array), std::minus<acc>());
    return res;
}
template <typename acc, int _rows, int _cols>
constexpr auto substract(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    Matrix<acc,_rows,_cols> res;
    std::transform(m.array.cbegin(), m.array.cend(), std::begin(res.array), std::bind(std::minus<acc>(), std::placeholders::_1, value));
    return res;
}

template <typename acc, int _rowsL, int _colsL, int _rowsR, int _colsR>
constexpr auto multiply(const Matrix<acc,_rowsL,_colsL> &l, const Matrix<acc,_rowsR,_colsR> &r) noexcept -> Matrix<acc,_rowsL, _colsR>{

    static_assert(_colsL == _rowsR, "Invalid dimensions for multiplication.");

    Matrix<acc,_rowsL, _colsR> res;
    for(int ii = 0; ii < l.rows(); ++ii){
        for(int jj = 0; jj < r.cols(); ++jj){
            for(int kk = 0; kk < r.rows(); ++kk){
                res(ii,jj) += l(ii,kk) * r(kk,jj);
            }
        }
    }
    return res;
}
template <typename acc, int _rows, int _cols>
constexpr auto multiply(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    Matrix<acc,_rows,_cols> res;
    std::transform(m.array.cbegin(), m.array.cend(), std::begin(res.array), std::bind(std::multiplies<acc>(), std::placeholders::_1, value));
    return res;
}

template <typename acc, int _rows, int _cols>
constexpr auto divide(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    if(value > acc{0}){
        Matrix<acc,_rows,_cols> res;
        std::transform(m.array.cbegin(), m.array.cend(), std::begin(res.array), std::bind(std::divides<acc>(), std::placeholders::_1, value));
        return res;
    }
    return m;
}

template <typename acc, int _rows, int _cols>
constexpr auto transpose(const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_cols,_rows> {
    Matrix<acc,_cols,_rows> dest;
    for(int ii = 0; ii < _rows*_cols; ++ii){
        dest.array[ii] = m.array[_rows*(ii % _rows) + (ii / _rows)];
    }
    return dest;
}

template <typename acc, int _rows, int _cols>
constexpr auto cut(const Matrix<acc,_rows,_cols> &m, int row, int col) noexcept -> Matrix<acc,_rows-1,_cols-1>{
    Matrix<acc,_rows-1,_cols-1> res;
    int index = 0;
    for(int ii = 0; ii < _rows; ++ii){
        for(int jj = 0; jj < _cols; ++jj){
            if(ii != row && jj != col){
                res(index++) = m(_cols*ii+jj);
            }            
        }
    }
    return res;
}

//template <typename acc, int _rows, int _cols>
//constexpr auto cofactor(const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_rows,_cols>{
//    Matrix<acc,_rows,_cols> res;
//    for(int ii = 0; ii < _rows; ++ii){
//        for(int jj = 0; jj < _cols; ++jj){
//            int t = _cols * jj +ii;
//            res(t) = m(t)*static_cast<acc>(std::pow(acc{-1}, ii + jj));
//        }
//    }
//    return res;
//}

//template <typename acc, int _rows, int _cols>
//constexpr auto adjugate(const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_cols,_rows>{
//    return transpose(cofactor(m));
//}

//template <typename acc, int _rows, int _cols>
//constexpr auto minor(const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_rows,_cols>{

//    if constexpr (_rows == 2 && _cols == 2){
//        return {m(1,1), m(1,0), m(0,1), m(0,0)};
//    }else{
//        Matrix<acc,_rows,_cols> res;
//        for(int ii = 0; ii < _rows; ++ii){
//            for(int jj = 0; jj < _cols; ++jj){
//                res(ii,jj) =  determinant(cut(m,ii,jj));
//            }
//        }
//        return res;
//    }
//}

template <typename acc, int _rows, int _cols>
constexpr auto trace(const Matrix<acc,_rows,_cols> &m) noexcept -> acc{
    static_assert(_rows != _cols, "Trace can only be computed from a square matrix.");
    acc t{0};
    for(int ii = 0; ii < _rows; ++ii){
        t += m(ii,ii);
    }
    return t;
}

template <typename acc, int _rows, int _cols>
constexpr auto determinant(const Matrix<acc,_rows,_cols> &m) noexcept -> acc {
    static_assert("Not implemented.");
//    acc det{0};
//    auto cof = cofactor(m);
//    for(int ii = 0; ii < 4; ++ii){
//        det += m(ii) * cof(0,ii);
//    }
//    return det;
}

template <typename acc, int _rows, int _cols>
constexpr auto inverse(const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_rows,_cols>{
    static_assert("Not implemented.");
//    else{
//        return adjugate(m)*(acc{1}/det);
//    }
}




template <typename acc, int dim>
constexpr auto invert(const RowVec<acc,dim> &p) noexcept -> RowVec<acc,dim>{
    return multiply(p,acc{-1});
}

template <typename acc, int dim>
constexpr auto sum(const RowVec<acc,dim> &vec)  noexcept -> acc{
    return std::accumulate(vec.array.begin(), vec.array.end(), acc{0});
}

template <typename acc, int dim>
constexpr auto dot(const RowVec<acc, dim> &l, const RowVec<acc, dim> &r) noexcept -> acc {
    return std::inner_product(l.array.cbegin(), l.array.cend(), r.array.cbegin(), acc{0});
}
template <typename acc, int dim>
constexpr auto dot(const ColVec<acc, dim> &l, const ColVec<acc, dim> &r) noexcept -> acc {
    return std::inner_product(l.array.cbegin(), l.array.cend(), r.array.cbegin(), acc{0});
}

template <typename acc, int dim>
constexpr auto dot(std::span<const acc, dim> l, std::span<const acc, dim> r) noexcept -> acc {
    return std::inner_product(l.cbegin(), l.cend(), r.cbegin(), acc{0});
}
template <typename acc, int dim>
constexpr auto dot(const RowVec<acc, dim> &l, std::span<const acc, dim> r) noexcept -> acc {
    return std::inner_product(l.array.cbegin(), l.array.cend(), r.cbegin(), acc{0});
}
template <typename acc, int dim>
constexpr auto dot(std::span<const acc, dim> l, const RowVec<acc, dim> &r) noexcept -> acc {
    return std::inner_product(l.cbegin(), l.cend(), r.array.cbegin(), acc{0});
}

template <typename acc, int dim>
constexpr auto vec(const RowVec<acc, dim> &l, const RowVec<acc, dim> &r) noexcept -> RowVec<acc, dim>{
    return r-l;
}

template <typename acc, int dim>
constexpr auto square_norm(const RowVec<acc,dim> &p) noexcept -> acc{
    return dot(p,p);
}

template <typename acc, int dim>
inline auto norm(const RowVec<acc,dim> &p) noexcept -> acc{
    return sqrt(square_norm(p));
}

template <typename acc, int dim>
inline auto normalize(const RowVec<acc,dim> &vec) noexcept -> RowVec<acc,dim>{
    return divide(vec, norm(vec));
}

// operators
template <typename acc, int _rows, int _cols>
constexpr auto operator+(const Matrix<acc,_rows,_cols> &l, const Matrix<acc,_rows,_cols> &r) noexcept-> Matrix<acc,_rows,_cols>{
    return add(l,r);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator+(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    return add(m, value);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator-(const Matrix<acc,_rows,_cols> &l, const Matrix<acc,_rows,_cols> &r) noexcept-> Matrix<acc,_rows,_cols>{
    return substract(l,r);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator-(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    return substract(m,value);
}

template <typename acc, int _rowsL, int _colsL, int _rowsR, int _colsR>
constexpr auto operator*(const Matrix<acc,_rowsL,_colsL> &l, const Matrix<acc,_rowsR,_colsR> &r) noexcept -> Matrix<acc,_rowsL, _colsR>{
    return multiply(l,r);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator*(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    return multiply(m,value);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator*(acc value, const Matrix<acc,_rows,_cols> &m) noexcept -> Matrix<acc,_rows,_cols>{
    return multiply(m,value);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator/(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> Matrix<acc,_rows,_cols>{
    return divide(m,value);
}

template <typename acc, int _rows, int _cols>
constexpr auto operator==(const Matrix<acc,_rows,_cols> &lhs, const Matrix<acc, _rows, _cols> &rhs) noexcept -> bool { return compare(lhs,rhs); }
template <typename acc, int _rows, int _cols>
constexpr auto operator!=(const Matrix<acc,_rows,_cols> &lhs, const Matrix<acc, _rows, _cols> &rhs) noexcept -> bool { return !compare(lhs,rhs); }

template <typename acc, int _rows, int _cols>
constexpr auto operator<(const Matrix<acc,_rows,_cols> &lhs, const Matrix<acc, _rows, _cols> &rhs) noexcept -> bool{
    if(lhs != rhs){
        return std::lexicographical_compare(lhs.array.begin(), lhs.array.end(), rhs.array.begin(), rhs.array.end());
    }
    return false;
}
template <typename acc, int _rows, int _cols>
constexpr auto operator> (const Matrix<acc,_rows,_cols> &lhs, const Matrix<acc, _rows, _cols> &rhs) noexcept -> bool { return rhs < lhs;}

template <typename acc, int _rows, int _cols>
constexpr auto operator<=(const Matrix<acc,_rows,_cols> &lhs, const Matrix<acc, _rows, _cols> &rhs) noexcept -> bool { return !(lhs > rhs); }

template <typename acc, int _rows, int _cols>
constexpr auto operator>=(const Matrix<acc,_rows,_cols> &lhs, const Matrix<acc, _rows, _cols> &rhs) noexcept -> bool { return !(lhs < rhs);}

template <typename acc, int _rows, int _cols>
constexpr auto operator<(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> bool{
    for(const auto &mv : m.array){
        if(mv < value){
            continue;
        }
        return false;
    }
    return true;
}

template <typename acc, int _rows, int _cols>
constexpr auto operator>(const Matrix<acc,_rows,_cols> &m, acc value) noexcept -> bool{
    for(const auto &mv : m.array){
        if(mv > value){
            continue;
        }
        return false;
    }
    return true;
}

template <typename acc, int _rows, int _cols>
auto operator<<(std::ostream &flux, const Matrix<acc, _rows, _cols> &m) -> std::ostream&{
    for(int ii = 0; ii < _rows; ++ii){
        flux << "[ ";
        for(int jj = 0; jj < _cols; ++jj){
            flux << m(ii,jj) << " ";
        }
        flux << "]\n";
    }
    return flux;
}



}
