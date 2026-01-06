

/*******************************************************************************
** Toolset-ts-base                                                            **
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


#include "decimal.hpp"

// std
#include <cmath>

using namespace tool;

auto Decimal::to_int() const -> int{
    if(is_int()){
        return std::get<int>(v);
    }else if(is_float()){
        return static_cast<int>(std::get<float>(v));
    }
    return static_cast<int>(std::get<double>(v));
}

auto Decimal::to_float() const -> float{

    if(is_int()){
        return static_cast<float>(std::get<int>(v));
    }else if(is_float()){
        return std::get<float>(v);
    }
    return static_cast<float>(std::get<double>(v));
}

auto Decimal::to_double() const -> double{

    if(is_int()){
        return static_cast<double>(std::get<int>(v));
    }else if(is_float()){
        return static_cast<double>(std::get<float>(v));
    }
    return std::get<double>(v);
}

auto tool::add(Decimal d1, Decimal d2) -> Decimal{

    if(d1.is_int()){

        if(d2.is_int()){
            return Decimal(d1.to_int() + d2.to_int());
        }else if(d2.is_float()){
            return Decimal(d1.to_float() + d2.to_float());
        }
        return Decimal(d1.to_double() + d2.to_double());
    }else if(d1.is_float()){

        if(d2.is_double()){
            return Decimal(d1.to_double() + d2.to_double());
        }
        return Decimal(d1.to_float() + d2.to_float());
    }
    return Decimal(d1.to_double() + d2.to_double());
}

auto tool::sub(Decimal d1, Decimal d2) -> Decimal{

    if(d1.is_int()){

        if(d2.is_int()){
            return Decimal(d1.to_int() - d2.to_int());
        }else if(d2.is_float()){
            return Decimal(d1.to_float() - d2.to_float());
        }
        return Decimal(d1.to_double() - d2.to_double());
    }else if(d1.is_float()){

        if(d2.is_double()){
            return Decimal(d1.to_double() - d2.to_double());
        }
        return Decimal(d1.to_float() - d2.to_float());
    }
    return Decimal(d1.to_double() - d2.to_double());
}

auto tool::mul(Decimal d1, Decimal d2) -> Decimal{

    if(d1.is_int()){

        if(d2.is_int()){
            return Decimal(d1.to_int() * d2.to_int());
        }else if(d2.is_float()){
            return Decimal(d1.to_float() * d2.to_float());
        }
        return Decimal(d1.to_double() * d2.to_double());
    }else if(d1.is_float()){

        if(d2.is_double()){
            return Decimal(d1.to_double() * d2.to_double());
        }
        return Decimal(d1.to_float() * d2.to_float());
    }
    return Decimal(d1.to_double() * d2.to_double());
}

auto tool::div(Decimal d1, Decimal d2) -> Decimal{

    if(d1.is_int()){

        if(d2.is_int()){
            return Decimal(d1.to_int() / d2.to_int());
        }else if(d2.is_float()){
            return Decimal(d1.to_float() / d2.to_float());
        }
        return Decimal(d1.to_double() / d2.to_double());
    }else if(d1.is_float()){

        if(d2.is_double()){
            return Decimal(d1.to_double() / d2.to_double());
        }
        return Decimal(d1.to_float() / d2.to_float());
    }
    return Decimal(d1.to_double() / d2.to_double());
}

auto tool::mod(Decimal d1, Decimal d2) -> Decimal{

    if(d1.is_int()){

        if(d2.is_int()){
            return Decimal(d1.to_int() % d2.to_int());
        }else if(d2.is_float()){
            return Decimal(std::fmodf(d1.to_float(), d2.to_float()));
        }
        return Decimal(std::fmod(d1.to_double(), d2.to_double()));
    }else if(d1.is_float()){

        if(d2.is_double()){
            return Decimal(std::fmod(d1.to_double(), d2.to_double()));
        }
        return Decimal(std::fmodf(d1.to_float(), d2.to_float()));
    }
    return Decimal(std::fmod(d1.to_double(),d2.to_double()));
}

auto tool::cos(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::cos(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::cosf(d.to_float()));
    }
    return Decimal(std::cos(d.to_double()));
}

auto tool::sin(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::sin(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::sinf(d.to_float()));
    }
    return Decimal(std::sin(d.to_double()));
}

auto tool::tan(Decimal d) -> Decimal{

    if(d.is_int()){
        return Decimal(std::tan(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::tanf(d.to_float()));
    }
    return Decimal(std::tan(d.to_double()));
}

auto tool::acos(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::acos(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::acosf(d.to_float()));
    }
    return Decimal(std::acos(d.to_double()));
}

auto tool::asin(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::asin(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::asinf(d.to_float()));
    }
    return Decimal(std::asin(d.to_double()));
}

auto tool::atan(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::atan(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::atanf(d.to_float()));
    }
    return Decimal(std::atan(d.to_double()));
}

auto tool::cosh(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::cosh(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::coshf(d.to_float()));
    }
    return Decimal(std::cosh(d.to_double()));
}

auto tool::sinh(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::sinh(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::sinhf(d.to_float()));
    }
    return Decimal(std::sinh(d.to_double()));
}

auto tool::tanh(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::tanh(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::tanhf(d.to_float()));
    }
    return Decimal(std::tanh(d.to_double()));
}

auto tool::acosh(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::acosh(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::acoshf(d.to_float()));
    }
    return Decimal(std::acosh(d.to_double()));
}

auto tool::asinh(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::asinh(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::asinhf(d.to_float()));
    }
    return Decimal(std::asinh(d.to_double()));
}

auto tool::atanh(Decimal d) -> Decimal{
    if(d.is_int()){
        return Decimal(std::atanh(d.to_int()));
    }else if(d.is_float()){
        return Decimal(std::atanhf(d.to_float()));
    }
    return Decimal(std::atanh(d.to_double()));
}

auto tool::no_equal(Decimal d1, Decimal d2) -> bool{
    return !almost_equal<double>(d1.to_double(),d2.to_double());
}

auto tool::equal(Decimal d1, Decimal d2) -> bool{
    return almost_equal<double>(d1.to_double(),d2.to_double());
}

auto tool::superior_or_equal(Decimal d1, Decimal d2) -> bool{

    if(almost_equal<double>(d1.to_double(),d2.to_double())){
        return true;
    }
    return superior(d1,d2);
}

auto tool::inferior(Decimal d1, Decimal d2) -> bool{
    return d1.to_double() < d2.to_double();
}

auto tool::inferior_or_equal(Decimal d1, Decimal d2) -> bool{

    if(almost_equal<double>(d1.to_double(),d2.to_double())){
        return true;
    }
    return inferior(d1,d2);
}

auto tool::superior(Decimal d1, Decimal d2) -> bool{
    return d1.to_double() > d2.to_double();
}
