

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

#include "color.hpp"

using namespace tool;

auto Convert::to_hsv(const ColorRGB32 &rgbf) noexcept -> ColorHSV{

    ColorHSV hsv;

    float fCMax  = std::max(std::max(rgbf.r(), rgbf.g()), rgbf.b());
    float fCMin  = std::min(std::min(rgbf.r(), rgbf.g()), rgbf.b());
    float fDelta = fCMax - fCMin;

    if(fDelta > 0) {
        if(fCMax == rgbf.r()) {
            hsv.h() = 60.f * (fmod<float>(((rgbf.g() - rgbf.b()) / fDelta), 6.f));
        } else if(fCMax == rgbf.g()) {
            hsv.h() = 60.f * (((rgbf.b() - rgbf.r()) / fDelta) + 2.f);
        } else if(fCMax == rgbf.b()) {
            hsv.h() = 60.f * (((rgbf.r() - rgbf.g()) / fDelta) + 4.f);
        }

        if(fCMax > 0) {
            hsv.s() = fDelta / fCMax;
        } else {
            hsv.s() = 0;
        }

        hsv.v() = fCMax;

    } else {
        hsv.h() = 0;
        hsv.s() = 0;
        hsv.v() = fCMax;
    }

    if(hsv.h() < 0) {
        hsv.h() = 360 + hsv.h();
    }

    return hsv;
}

auto Convert::to_rgb32(const ColorHSV &hsv) noexcept -> ColorRGB32{

    float fC = hsv.v() * hsv.s(); // Chroma
    float fHPrime = fmod(hsv.h() / 60.f, 6.f);
    float fX = fC * (1.f - fabs(fmod(fHPrime, 2.f) - 1.f));
    float fM = hsv.v() - fC;

    ColorRGB32 rgb32;
    if(0 <= fHPrime && fHPrime < 1) {
        rgb32.r() = fC;
        rgb32.g() = fX;
        rgb32.b() = 0;
    } else if(1 <= fHPrime && fHPrime < 2) {
        rgb32.b() = fX;
        rgb32.g() = fC;
        rgb32.r() = 0;
    } else if(2 <= fHPrime && fHPrime < 3) {
        rgb32.b() = 0;
        rgb32.g() = fC;
        rgb32.r() = fX;
    } else if(3 <= fHPrime && fHPrime < 4) {
        rgb32.b() = 0;
        rgb32.g() = fX;
        rgb32.r() = fC;
    } else if(4 <= fHPrime && fHPrime < 5) {
        rgb32.b() = fX;
        rgb32.g() = 0;
        rgb32.r() = fC;
    } else if(5 <= fHPrime && fHPrime < 6) {
        rgb32.b() = fC;
        rgb32.g() = 0;
        rgb32.r() = fX;
    } else {
        rgb32.b() = 0;
        rgb32.g() = 0;
        rgb32.r() = 0;
    }

    rgb32.b() += fM;
    rgb32.g() += fM;
    rgb32.r() += fM;
    return rgb32;
}

auto Convert::to_hsv(const ColorRGBA8 &rgba) noexcept -> ColorHSV{
    return Convert::to_hsv(ColorRGB32(Convert::to_rgba32(rgba).rgb()));
}
