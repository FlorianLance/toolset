

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
#include "hsv.hpp"
#include "rgb.hpp"

namespace tool{

struct Convert{

    static constexpr auto to_rgba32(const ColorRGBA8 &rgba8) noexcept -> ColorRGBA32{
        return ColorRGBA32{rgba8.conv<float>()/255.f};
    }
    static constexpr auto to_rgba32(const ColorRGB8 &rgb8) noexcept -> ColorRGBA32{
        return ColorRGBA32{rgb8.r()/255.f,rgb8.g()/255.f,rgb8.b()/255.f};
    }

    static auto to_hsv(const ColorRGB32 &rgbf) noexcept -> ColorHSV;
    static auto to_hsv(const ColorRGB8 &rgb) noexcept -> ColorHSV;
    static auto to_hsv(const ColorRGBA8 &rgba) noexcept -> ColorHSV;
    static auto to_rgb32(const ColorHSV &hsv) noexcept -> ColorRGB32;
};

}
