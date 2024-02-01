
/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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
#include <span>

// local
#include "buffer_vector.hpp"

namespace tool{

template<typename ElementType>
struct ImageBuffer{

    size_t width = 0;
    size_t height = 0;
    Buffer<ElementType> buffer;

    constexpr auto reset() noexcept -> void{
        width  = 0;
        height = 0;
        buffer.clear();
    }
    constexpr auto resize(size_t size) -> void{buffer.resize(size);}

    [[nodiscard]] constexpr auto empty()        const   noexcept -> bool                  {return buffer.empty();}
    [[nodiscard]] constexpr auto size()         const   noexcept -> size_t                {return buffer.size();}
    [[nodiscard]] constexpr auto get_data()             noexcept -> ElementType*          {return buffer.get_data();}
    [[nodiscard]] constexpr auto get_data()     const   noexcept -> const ElementType*    {return buffer.get_data();}
    [[nodiscard]] constexpr auto get_raw_data()         noexcept -> std::uint8_t*         {return buffer.get_raw_data();}
    [[nodiscard]] constexpr auto get_raw_data() const   noexcept -> const std::uint8_t*   {return buffer.get_raw_data();}

    // iterators
    [[nodiscard]] constexpr auto begin()                noexcept {return buffer.begin();}
    [[nodiscard]] constexpr auto begin()        const   noexcept {return buffer.cbegin();}
    [[nodiscard]] constexpr auto cbegin()       const   noexcept {return buffer.cbegin();}
    [[nodiscard]] constexpr auto end()                  noexcept {return buffer.end();}
    [[nodiscard]] constexpr auto end()          const   noexcept {return buffer.cend();}
    [[nodiscard]] constexpr auto cend()         const   noexcept {return buffer.cend();}
    [[nodiscard]] constexpr auto rbegin()               noexcept {return buffer.rbegin();}
    [[nodiscard]] constexpr auto rbegin()       const   noexcept {return buffer.rcbegin();}
    [[nodiscard]] constexpr auto rcbegin()      const   noexcept {return buffer.rcbegin();}
    [[nodiscard]] constexpr auto rend()                 noexcept {return buffer.rend();}
    [[nodiscard]] constexpr auto rend()         const   noexcept {return buffer.rcend();}
    [[nodiscard]] constexpr auto rcend()        const   noexcept {return buffer.rcend();}

    // spans
    [[nodiscard]] auto raw_span()                       noexcept -> std::span<std::uint8_t>         { return buffer.raw_span();}
    [[nodiscard]] auto raw_span()               const   noexcept -> std::span<const std::uint8_t>   { return buffer.raw_span();}
    [[nodiscard]] auto span()                           noexcept -> std::span<ElementType>          { return buffer.span();}
    [[nodiscard]] auto span()                   const   noexcept -> std::span<const ElementType>    { return buffer.span();}
};
}
