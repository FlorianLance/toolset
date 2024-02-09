
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

// local
#include "buffer.hpp"

namespace tool{

template<typename ElementType>
struct ImageSpan : public std::span<ElementType>{

    ImageSpan(size_t width, size_t height, const std::vector<ElementType> &values) : std::span<ElementType>(values), width(width), height(height){
    }

    size_t width = 0;
    size_t height = 0;
};

template<typename ElementType>
struct ImageBuffer : public Buffer<ElementType>{

    constexpr auto reset() noexcept -> void{
        width  = 0;
        height = 0;
        Buffer<ElementType>::clear();
    }

    constexpr auto resize_image(size_t width, size_t height) noexcept -> void{
        this->width  = width;
        this->height = height;
        Buffer<ElementType>::resize(width*height);
    }

    [[nodiscard]] constexpr auto is_encoded() const noexcept -> bool {
        return Buffer<ElementType>::size() != (width*height);
    }

    [[nodiscard]] constexpr auto is_valid_image() const noexcept -> bool {
        return !is_encoded() && (width != 0) && (height != 0);
    }

    size_t width = 0;
    size_t height = 0;
};

using BinaryImageBuffer = ImageBuffer<std::byte>;
}
