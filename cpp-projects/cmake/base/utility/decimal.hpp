
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
#include <cmath>
#include <variant>
#include <vector>

// local
#include "math.hpp"

namespace tool {

struct Decimal;
using DecimalList   = std::vector<Decimal>;
using DoubleList    = std::vector<double>;
using RealList      = DoubleList;

struct Decimal{

    using DecimalV      = std::variant<int,float,double>;

    Decimal() : v(0){}
    explicit Decimal(bool value)    : v(value ? 1 : 0){}
    explicit Decimal(int value)     : v(value){}
    explicit Decimal(float value)   : v(value){}
    explicit Decimal(double value)  : v(value){}

    constexpr auto is_int() const -> int {return std::holds_alternative<int>(v);}
    constexpr auto is_float() const -> float{return std::holds_alternative<float>(v);}
    constexpr auto is_double() const -> double {return std::holds_alternative<double>(v);}
    constexpr auto is_null() const -> bool{return almost_equal(to_double(), 0.0);}

    auto to_int() const -> int;
    auto to_float() const -> float;
    auto to_double() const -> double;

    DecimalV v;
};

auto add(Decimal d1, Decimal d2) -> Decimal;
auto sub(Decimal d1, Decimal d2) -> Decimal;
auto mul(Decimal d1, Decimal d2) -> Decimal;
auto div(Decimal d1, Decimal d2) -> Decimal;
auto mod(Decimal d1, Decimal d2) -> Decimal;
auto cos(Decimal d) -> Decimal;
auto sin(Decimal d) -> Decimal;
auto tan(Decimal d) -> Decimal;
auto acos(Decimal d) -> Decimal;
auto asin(Decimal d) -> Decimal;
auto atan(Decimal d) -> Decimal;
auto cosh(Decimal d) -> Decimal;
auto sinh(Decimal d) -> Decimal;
auto tanh(Decimal d) -> Decimal;
auto acosh(Decimal d) -> Decimal;
auto asinh(Decimal d) -> Decimal;
auto atanh(Decimal d) -> Decimal;
auto inferior(Decimal d1, Decimal d2) -> bool;
auto superior(Decimal d1, Decimal d2) -> bool;
auto inferior_or_equal(Decimal d1, Decimal d2) -> bool;
auto superior_or_equal(Decimal d1, Decimal d2) -> bool;
auto equal(Decimal d1, Decimal d2) -> bool;
auto no_equal(Decimal d1, Decimal d2) -> bool;

}
