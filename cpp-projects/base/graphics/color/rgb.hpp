
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
#include "geometry/point.hpp"

namespace tool{

using ColorGray8  = std::uint8_t;
using ColorGray16 = std::uint16_t;

/**
 * @brief ColorRGB8
 * r Red:   [0, 255] 1 byte
 * g Green: [0, 255] 1 byte
 * b Blue:  [0, 255] 1 byte
 */
struct ColorRGB8 : geo::Point<std::uint8_t,3>{

    ColorRGB8() = default;
    ColorRGB8(const ColorRGB8& other) = default;
    ColorRGB8& operator=(const ColorRGB8& other) = default;
    ColorRGB8(ColorRGB8&& other) = default;
    ColorRGB8& operator=(ColorRGB8&& other) = default;

    constexpr ColorRGB8(const geo::Point<std::uint8_t,3> &p) noexcept{
        this->array = p.array;
    }
    constexpr ColorRGB8(geo::Point<std::uint8_t,3> &&p) noexcept{
        this->array = std::move(p.array);
    }
    constexpr ColorRGB8(std::uint8_t r, std::uint8_t g = 0, std::uint8_t b = 0) noexcept{
        this->array = {r,g,b};
    }

    inline auto r() noexcept -> std::uint8_t& {return x();}
    constexpr auto r() const noexcept -> std::uint8_t {return x();}

    inline auto g() noexcept -> std::uint8_t& {return y();}
    constexpr auto g() const noexcept -> std::uint8_t {return y();}

    inline auto b() noexcept -> std::uint8_t& {return z();}
    constexpr auto b() const noexcept -> std::uint8_t {return z();}
};

/**
 * @brief ColorRGB8
 * r Red:   [0, 255] 1 byte
 * g Green: [0, 255] 1 byte
 * b Blue:  [0, 255] 1 byte
 */
struct ColorRGBA8 : geo::Point<std::uint8_t,4>{

    ColorRGBA8() = default;
    ColorRGBA8(const ColorRGBA8& other) = default;
    ColorRGBA8& operator=(const ColorRGBA8& other) = default;
    ColorRGBA8(ColorRGBA8&& other) = default;
    ColorRGBA8& operator=(ColorRGBA8&& other) = default;

    constexpr ColorRGBA8(const geo::Point<std::uint8_t,4> &c) noexcept{
        this->array = c.array;
    }
    constexpr ColorRGBA8(geo::Point<std::uint8_t,4> &&c) noexcept{
        this->array = std::move(c.array);
    }
    constexpr ColorRGBA8(std::uint8_t r, std::uint8_t g = 0, std::uint8_t b = 0, std::uint8_t a = 0) noexcept{
        this->array = {r,g,b,a};
    }

    inline auto r() noexcept -> std::uint8_t& {return x();}
    constexpr auto r() const noexcept -> std::uint8_t {return x();}

    inline auto g() noexcept -> std::uint8_t& {return y();}
    constexpr auto g() const noexcept -> std::uint8_t {return y();}

    inline auto b() noexcept -> std::uint8_t& {return z();}
    constexpr auto b() const noexcept -> std::uint8_t {return z();}

    inline auto a() noexcept -> std::uint8_t& {return w();}
    constexpr auto a() const noexcept -> std::uint8_t {return w();}

    constexpr auto rgb() const noexcept -> ColorRGB8 {return xyz();}
};

/**
 * @brief ColorRGB32
 * r Red:   [0, 1] 4 bytes
 * g Green: [0, 1] 4 bytes
 * b Blue:  [0, 1] 4 bytes
 */
struct ColorRGB32 : geo::Point<float,3>{

    ColorRGB32() = default;
    ColorRGB32(const ColorRGB32& other) = default;
    ColorRGB32& operator=(const ColorRGB32& other) = default;
    ColorRGB32(ColorRGB32&& other) = default;
    ColorRGB32& operator=(ColorRGB32&& other) = default;

    constexpr ColorRGB32(const geo::Point<float,3> &c) noexcept{
        this->array = c.array;
    }
    constexpr ColorRGB32(geo::Point<float,3> &&c) noexcept{
        this->array = std::move(c.array);
    }
    constexpr ColorRGB32(float r, float g = 0.f, float b = 0.f) noexcept{
        this->array = {r,g,b};
    }

    inline auto r() noexcept -> float& {return x();}
    constexpr auto r() const noexcept -> float {return x();}

    inline auto g() noexcept -> float& {return y();}
    constexpr auto g() const noexcept -> float {return y();}

    inline auto b() noexcept -> float& {return z();}
    constexpr auto b() const noexcept -> float {return z();}

    inline auto clamp(float min, float max) -> void{
        for(auto &v : array){
            v = std::clamp(v, min, max);
        }
    }
};

/**
 * @brief ColorRGB32
 * r Red:           [0, 1] 4 bytes
 * g Green:         [0, 1] 4 bytes
 * b Blue:          [0, 1] 4 bytes
 * a Transparency:  [0, 1] 4 bytes
 */
struct ColorRGBA32 : geo::Point<float,4>{

    ColorRGBA32() = default;
    ColorRGBA32(const ColorRGBA32& other) = default;
    ColorRGBA32& operator=(const ColorRGBA32& other) = default;
    ColorRGBA32(ColorRGBA32&& other) = default;
    ColorRGBA32& operator=(ColorRGBA32&& other) = default;

    constexpr ColorRGBA32(const geo::Point<float,4> &c) noexcept{
        this->array = c.array;
    }
    constexpr ColorRGBA32(geo::Point<float,4> &&c) noexcept{
        this->array = std::move(c.array);
    }
    constexpr ColorRGBA32(float r, float g = 0.f, float b = 0.f, float a = 0.f) noexcept{
        this->array = {r,g,b,a};
    }

    inline auto r() noexcept -> float& {return x();}
    constexpr auto r() const noexcept -> float {return x();}

    inline auto g() noexcept -> float& {return y();}
    constexpr auto g() const noexcept -> float {return y();}

    inline auto b() noexcept -> float& {return z();}
    constexpr auto b() const noexcept -> float {return z();}

    inline auto a() noexcept -> float& {return w();}
    constexpr auto a() const noexcept -> float {return w();}

    constexpr auto rgb() const noexcept -> ColorRGB32 {return xyz();}
};


}
