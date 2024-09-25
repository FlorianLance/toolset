
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
struct ImageBuffer : public Buffer<ElementType>{

    constexpr auto reset() noexcept -> void{
        width  = 0;
        height = 0;
        Buffer<ElementType>::clear();
    }

    auto resize_image(size_t width, size_t height) -> void{
        this->width    = width;
        this->height   = height;
        Buffer<ElementType>::resize(width*height);
    }

    [[nodiscard]] constexpr auto is_valid_image() const noexcept -> bool {
        return (Buffer<ElementType>::size() == (width*height)) && (width != 0) && (height != 0);
    }    

    [[nodiscard]] constexpr auto get(size_t idX, size_t idY)       -> ElementType&       {return Buffer<ElementType>::values[idY*width + idX];}
    [[nodiscard]] constexpr auto get(size_t idX, size_t idY) const -> const ElementType& {return Buffer<ElementType>::values[idY*width + idX];}

    auto fill_neighbour(int idX, int idY, int kernelSize, const ElementType &elem) -> void{
        for(int ii = idX-kernelSize; ii < idX + kernelSize; ++ii){
            for(int jj = idY-kernelSize; jj < idY + kernelSize; ++jj)  {
                if((ii >= 0) && (ii < width) && (jj >= 0) && (jj < height)){
                    get(ii,jj) = elem;
                }
            }
        }
    }

    size_t width = 0;
    size_t height = 0;
};

struct BinaryImageBuffer : public Buffer<std::byte>{

    constexpr auto reset() noexcept -> void{
        width    = 0;
        height   = 0;
        Buffer::clear();
    }

    auto resize_image(size_t width, size_t height, std::uint8_t sizeType) -> void{
        this->width    = width;
        this->height   = height;
        Buffer::resize(width*height*sizeType);
    }

    [[nodiscard]] constexpr auto size_type() const noexcept -> size_t {
        return !empty() ? (width*height/size()) : 0;
    }

    size_t width = 0;
    size_t height = 0;
};
}

