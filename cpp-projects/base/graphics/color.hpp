

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
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"

namespace tool{

/**
 * @brief ColorHSV
 * h Hue:        [0, 360] 4 bytes
 * s Saturation: [0, 1] 4 bytes
 * v Value:      [0, 1] 4 bytes
 */
struct ColorHSV{

    ColorHSV() = default;
    explicit constexpr ColorHSV(float h, float s, float v) : value({h,s,v}){}

    inline auto h() noexcept -> float& {return value.x();}
    constexpr auto h() const noexcept -> float {return value.x();}

    inline auto s() noexcept -> float& {return value.y();}
    constexpr auto s() const noexcept -> float {return value.y();}

    inline auto v() noexcept -> float& {return value.z();}
    constexpr auto v() const noexcept -> float {return value.z();}

    geo::Pt3f value = {0.f,0.f,0.f};
};

using ColorGray8  = std::uint8_t;
using ColorGray16 = std::uint16_t;

/**
 * @brief ColorRGB8
 * r Red:   [0, 255] 1 byte
 * g Green: [0, 255] 1 byte
 * b Blue:  [0, 255] 1 byte
 */
struct ColorRGB8{

    ColorRGB8() = default;
    ColorRGB8(const ColorRGB8& other) = default;
    ColorRGB8& operator=(const ColorRGB8& other) = default;
    ColorRGB8(ColorRGB8&& other) = default;
    ColorRGB8& operator=(ColorRGB8&& other) = default;

    constexpr explicit ColorRGB8(std::uint8_t r, std::uint8_t g, std::uint8_t b) : value({r,g,b}){}
    constexpr explicit ColorRGB8(const geo::Pt3<std::uint8_t> &rgb) : value(rgb){}

    inline auto r() noexcept -> std::uint8_t& {return value.x();}
    constexpr auto r() const noexcept -> std::uint8_t {return value.x();}

    inline auto g() noexcept -> std::uint8_t& {return value.y();}
    constexpr auto g() const noexcept -> std::uint8_t {return value.y();}

    inline auto b() noexcept -> std::uint8_t& {return value.z();}
    constexpr auto b() const noexcept -> std::uint8_t {return value.z();}

    inline auto rgb() noexcept -> geo::Pt3<std::uint8_t>& {return value;}
    constexpr auto rgb() const noexcept -> geo::Pt3<std::uint8_t> {return value;}

    geo::Pt3<std::uint8_t> value = {0,0,0};
};





/**
 * @brief ColorRGB8
 * r Red:   [0, 255] 1 byte
 * g Green: [0, 255] 1 byte
 * b Blue:  [0, 255] 1 byte
 */
struct ColorRGBA8{

    ColorRGBA8() = default;
    ColorRGBA8(const ColorRGBA8& other) = default;
    ColorRGBA8& operator=(const ColorRGBA8& other) = default;
    ColorRGBA8(ColorRGBA8&& other) = default;
    ColorRGBA8& operator=(ColorRGBA8&& other) = default;

    constexpr explicit ColorRGBA8(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : value({r,g,b, a}){}
    constexpr explicit ColorRGBA8(const geo::Pt4<std::uint8_t> &rgba) : value(rgba){}

    inline auto r() noexcept -> std::uint8_t& {return value.x();}
    constexpr auto r() const noexcept -> std::uint8_t {return value.x();}

    inline auto g() noexcept -> std::uint8_t& {return value.y();}
    constexpr auto g() const noexcept -> std::uint8_t {return value.y();}

    inline auto b() noexcept -> std::uint8_t& {return value.z();}
    constexpr auto b() const noexcept -> std::uint8_t {return value.z();}

    inline auto a() noexcept -> std::uint8_t& {return value.w();}
    constexpr auto a() const noexcept -> std::uint8_t {return value.w();}

    constexpr auto rgb() const noexcept -> geo::Pt3<std::uint8_t> {return value.xyz();}

    inline auto rgba() noexcept -> geo::Pt4<std::uint8_t>& {return value;}
    constexpr auto rgba() const noexcept -> geo::Pt4<std::uint8_t> {return value;}

    geo::Pt4<std::uint8_t> value = {0,0,0,0};
};


/**
 * @brief ColorRGB32
 * r Red:   [0, 1] 4 bytes
 * g Green: [0, 1] 4 bytes
 * b Blue:  [0, 1] 4 bytes
 */
struct ColorRGB32{

    ColorRGB32() = default;
    ColorRGB32(const ColorRGB32& other) = default;
    ColorRGB32& operator=(const ColorRGB32& other) = default;
    ColorRGB32(ColorRGB32&& other) = default;
    ColorRGB32& operator=(ColorRGB32&& other) = default;

    constexpr explicit ColorRGB32(float r, float g, float b) : value({r,g,b}){}
    constexpr explicit ColorRGB32(const geo::Pt3f &rgb) : value(rgb){}

    inline auto r() noexcept -> float& {return value.x();}
    constexpr auto r() const noexcept -> float {return value.x();}

    inline auto g() noexcept -> float& {return value.y();}
    constexpr auto g() const noexcept -> float {return value.y();}

    inline auto b() noexcept -> float& {return value.z();}
    constexpr auto b() const noexcept -> float {return value.z();}

    inline auto rgb() noexcept -> geo::Pt3f& {return value;}
    constexpr auto rgb() const noexcept -> geo::Pt3f {return value;}

    inline auto clamp(float min, float max) -> void{
        value = {
            std::clamp(value(0), min, max),
            std::clamp(value(1), min, max),
            std::clamp(value(2), min, max),
        };
    }

    geo::Pt3f value = {0.f,0.f,0.f};
};

/**
 * @brief ColorRGB32
 * r Red:           [0, 1] 4 bytes
 * g Green:         [0, 1] 4 bytes
 * b Blue:          [0, 1] 4 bytes
 * a Transparency:  [0, 1] 4 bytes
 */
struct ColorRGBA32{

    ColorRGBA32() = default;
    ColorRGBA32(const ColorRGBA32& other) = default;
    ColorRGBA32& operator=(const ColorRGBA32& other) = default;
    ColorRGBA32(ColorRGBA32&& other) = default;
    ColorRGBA32& operator=(ColorRGBA32&& other) = default;

    constexpr explicit ColorRGBA32(float r, float g, float b, float a) : value({r,g,b, a}){}
    constexpr explicit ColorRGBA32(const geo::Pt4f &rgba) : value(rgba){}

    inline auto r() noexcept -> float& {return value.x();}
    constexpr auto r() const noexcept -> float {return value.x();}

    inline auto g() noexcept -> float& {return value.y();}
    constexpr auto g() const noexcept -> float {return value.y();}

    inline auto b() noexcept -> float& {return value.z();}
    constexpr auto b() const noexcept -> float {return value.z();}

    inline auto a() noexcept -> float& {return value.w();}
    constexpr auto a() const noexcept -> float {return value.w();}

    constexpr auto rgb() const noexcept -> geo::Pt3f {return value.xyz();}

    inline auto rgba() noexcept -> geo::Pt4f& {return value;}
    constexpr auto rgba() const noexcept -> geo::Pt4f {return value;}

    geo::Pt4f value = {0.f,0.f,0.f, 0.f};
};

struct Convert{
    static constexpr auto to_rgba32(const ColorRGBA8 &rgba8) noexcept -> ColorRGBA32{
        return ColorRGBA32{geo::Pt4f(rgba8.rgba().conv<float>())/255.f};
    }

    static auto to_hsv(const ColorRGB32 &rgbf) noexcept -> ColorHSV;
    static auto to_hsv(const ColorRGBA8 &rgba) noexcept -> ColorHSV;
    static auto to_rgb32(const ColorHSV &hsv) noexcept -> ColorRGB32;
};

}
