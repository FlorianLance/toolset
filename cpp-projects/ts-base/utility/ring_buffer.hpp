
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include "utility/buffer.hpp"

namespace tool{

template <typename ElementType>
struct SingleRingBuffer{

    SingleRingBuffer() = default;
    SingleRingBuffer(size_t count, ElementType initValue){
        resize(count, initValue);
    }

    constexpr auto resize(size_t count, ElementType initValue) -> void{
        if(count == 0){
            count = 1;
        }
        buffers.resize(count);
        buffers.fill(initValue);
    }

    auto set_current(ElementType value) noexcept -> void{
        buffers[currentId] = value;
    }

    auto current() const noexcept  -> ElementType{
        return buffers[currentId];
    }

    auto current() noexcept  -> ElementType&{
        return buffers[currentId];
    }

    auto current_id() const noexcept -> size_t{
        return currentId;
    }

    auto increment() noexcept -> void{
        currentId = (currentId + 1) % buffers.size();
    }

    auto span() noexcept -> std::span<ElementType>{
        return buffers.span();
    }

    auto span() const noexcept -> std::span<const ElementType>{
        return buffers.span();
    }

private:
    size_t currentId = 0;
    Buffer<ElementType> buffers;
};


template <typename ElementType>
struct DoubleRingBuffer{

    constexpr auto resize(size_t count, size_t size = 0) -> void{
        buffers.resize(count);
        if(size != 0){
            for(auto &buffer : buffers){
                buffer.resize(size);
            }
        }
        currentId = 0;
    }

    constexpr auto resize_current(size_t size) -> void{
        if(currentId < buffers.size()){
            buffers[currentId].resize(size);
        }
    }

    constexpr auto current_size() const noexcept -> size_t{
        if(currentId < buffers.size()){
            return buffers[currentId].size();
        }
        return 0;
    }

    auto current_span(size_t size) noexcept -> std::span<ElementType>{
        if(currentId < buffers.size()){
            if(size > buffers[currentId].size()){
                size = buffers[currentId].size();
            }
            return std::span<ElementType>(buffers[currentId].get_data(), size);;
        }
        return {};
    }

    auto increment() noexcept -> void{
        currentId = (currentId + 1) % buffers.size();
    }

private:
    size_t currentId = 0;
    Buffer<Buffer<ElementType>> buffers;
};

}




