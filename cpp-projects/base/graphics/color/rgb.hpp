
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

    constexpr ColorRGB8(const geo::Point<std::uint8_t, 3> &p) noexcept{
        array = p.array;
    }
    constexpr ColorRGB8(geo::Point<std::uint8_t, 3> &&p) noexcept{
        array = std::move(p.array);
    }
    constexpr ColorRGB8(std::uint8_t r, std::uint8_t g = 0, std::uint8_t b = 0) noexcept{
        array = {r,g,b};
    }

    [[nodiscard]] constexpr auto r()          noexcept -> std::uint8_t&   {return array[0];}
    [[nodiscard]] constexpr auto r() const    noexcept -> std::uint8_t    {return array[0];}

    [[nodiscard]] constexpr auto g()          noexcept -> std::uint8_t&   {return array[1];}
    [[nodiscard]] constexpr auto g() const    noexcept -> std::uint8_t    {return array[1];}

    [[nodiscard]] constexpr auto b()          noexcept -> std::uint8_t&   {return array[2];}
    [[nodiscard]] constexpr auto b() const    noexcept -> std::uint8_t    {return array[2];}
};

/**
 * @brief ColorRGB8
 * r Red:   [0, 255] 1 byte
 * g Green: [0, 255] 1 byte
 * b Blue:  [0, 255] 1 byte
 */
struct ColorRGBA8 : geo::Point<std::uint8_t,4>{

    ColorRGBA8() = default;

    constexpr ColorRGBA8(const geo::Point<std::uint8_t,4> &c) noexcept{
        this->array = c.array;
    }
    constexpr ColorRGBA8(geo::Point<std::uint8_t,4> &&c) noexcept{
        this->array = std::move(c.array);
    }
    constexpr ColorRGBA8(std::uint8_t r, std::uint8_t g = 0, std::uint8_t b = 0, std::uint8_t a = 0) noexcept{
        this->array = {r,g,b,a};
    }

    [[nodiscard]] constexpr auto r()          noexcept -> std::uint8_t&   {return array[0];}
    [[nodiscard]] constexpr auto r() const    noexcept -> std::uint8_t    {return array[0];}

    [[nodiscard]] constexpr auto g()          noexcept -> std::uint8_t&   {return array[1];}
    [[nodiscard]] constexpr auto g() const    noexcept -> std::uint8_t    {return array[1];}

    [[nodiscard]] constexpr auto b()          noexcept -> std::uint8_t&   {return array[2];}
    [[nodiscard]] constexpr auto b() const    noexcept -> std::uint8_t    {return array[2];}

    [[nodiscard]] constexpr auto a()          noexcept -> std::uint8_t&   {return array[3];}
    [[nodiscard]] constexpr auto a() const    noexcept -> std::uint8_t    {return array[3];}

    [[nodiscard]] constexpr auto rgb() const noexcept -> ColorRGB8 {return geo::Point<std::uint8_t,4>::xyz();}
};

/**
 * @brief ColorRGB32
 * r Red:   [0, 1] 4 bytes
 * g Green: [0, 1] 4 bytes
 * b Blue:  [0, 1] 4 bytes
 */
struct ColorRGB32 : geo::Point<float,3>{

    ColorRGB32() = default;

    constexpr ColorRGB32(const geo::Point<float,3> &c) noexcept{
        array = c.array;
    }
    constexpr ColorRGB32(geo::Point<float,3> &&c) noexcept{
        array = std::move(c.array);
    }
    constexpr ColorRGB32(float r, float g = 0.f, float b = 0.f) noexcept{
        array = {r,g,b};
    }

    [[nodiscard]] constexpr auto r()          noexcept -> float&  {return array[0];}
    [[nodiscard]] constexpr auto r() const    noexcept -> float   {return array[0];}

    [[nodiscard]] constexpr auto g()          noexcept -> float&  {return array[1];}
    [[nodiscard]] constexpr auto g() const    noexcept -> float   {return array[1];}

    [[nodiscard]] constexpr auto b()          noexcept -> float&  {return array[2];}
    [[nodiscard]] constexpr auto b() const    noexcept -> float   {return array[2];}

    constexpr auto clamp(float min, float max) -> void{
        array[0] = std::clamp(r(), min, max);
        array[1] = std::clamp(g(), min, max);
        array[2] = std::clamp(b(), min, max);
    }

    [[nodiscard]] auto abs() const noexcept -> ColorRGB32{
        return {std::abs(r()),std::abs(g()),std::abs(b())};
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

    constexpr ColorRGBA32(const geo::Point<float,4> &c) noexcept{
        array = c.array;
    }
    constexpr ColorRGBA32(geo::Point<float,4> &&c) noexcept{
        array = std::move(c.array);
    }
    constexpr ColorRGBA32(float r, float g = 0.f, float b = 0.f, float a = 0.f) noexcept{
        array = {r,g,b,a};
    }

    [[nodiscard]] constexpr auto r()          noexcept -> float&  {return array[0];}
    [[nodiscard]] constexpr auto r() const    noexcept -> float   {return array[0];}

    [[nodiscard]] constexpr auto g()          noexcept -> float&  {return array[1];}
    [[nodiscard]] constexpr auto g() const    noexcept -> float   {return array[1];}

    [[nodiscard]] constexpr auto b()          noexcept -> float&  {return array[2];}
    [[nodiscard]] constexpr auto b() const    noexcept -> float   {return array[2];}

    [[nodiscard]] constexpr auto a()          noexcept -> float&  {return array[3];}
    [[nodiscard]] constexpr auto a() const    noexcept -> float   {return array[3];}

    [[nodiscard]] constexpr auto rgb() const noexcept -> ColorRGB32 {return geo::Point<float,4>::xyz();}
};


}
