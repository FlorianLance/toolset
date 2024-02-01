
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

// std
#include <vector>
#include <algorithm>
#include <span>

namespace tool {

template <typename ElementType>
struct Buffer{

    Buffer() = default;
    Buffer(const Buffer& other) = default;
    Buffer& operator=(const Buffer& other) = default;
    Buffer(Buffer&& other) = default;
    Buffer& operator=(Buffer&& other) = default;

    // memory
    [[nodiscard]] constexpr auto empty()    const noexcept -> size_t {return values.empty();}
    [[nodiscard]] constexpr auto size()     const noexcept -> size_t {return values.size();}
    [[nodiscard]] constexpr auto capacity() const noexcept -> size_t {return values.capacity();}
    constexpr auto resize(size_t size)                     -> void   {values.resize(size);}
    constexpr auto reserve(size_t capacity)                -> void   {values.reserve(capacity);}
    constexpr auto clear() noexcept                        -> void   {values.clear();}
    constexpr auto shrink_to_fit()                         -> void   {values.shrink_to_fit();}

    // get
    [[nodiscard]] constexpr auto get_data()                   noexcept -> ElementType*        {return values.data();}
    [[nodiscard]] constexpr auto get_data()             const noexcept -> const ElementType*  {return values.data();}
    [[nodiscard]] constexpr auto get_raw_data()               noexcept -> std::uint8_t*       {return reinterpret_cast<std::uint8_t*>(get_data());}
    [[nodiscard]] constexpr auto get_raw_data()         const noexcept -> const std::uint8_t* {return reinterpret_cast<const std::uint8_t*>(get_data());}
    [[nodiscard]] constexpr auto operator[](size_t id)        noexcept -> ElementType&        {return values[id];}
    [[nodiscard]] constexpr auto operator[](size_t id)  const noexcept -> const ElementType&  {return values[id];}
    [[nodiscard]] constexpr auto at(size_t id)                         -> ElementType&        {return values.at(id);}
    [[nodiscard]] constexpr auto at(size_t id)          const          -> const ElementType&  {return values.at(id);}
    [[nodiscard]] constexpr auto back()                       noexcept -> ElementType&        {return values.back();}
    [[nodiscard]] constexpr auto back()                 const noexcept -> const ElementType&  {return values.back();}
    [[nodiscard]] constexpr auto front()                      noexcept -> ElementType&        {return values.front();}
    [[nodiscard]] constexpr auto front()                const noexcept -> const ElementType&  {return values.front();}

    // iterators
    [[nodiscard]] constexpr auto begin()          noexcept{return values.begin();}
    [[nodiscard]] constexpr auto begin()    const noexcept{return values.cbegin();}
    [[nodiscard]] constexpr auto cbegin()   const noexcept{return values.cbegin();}
    [[nodiscard]] constexpr auto end()            noexcept{return values.end();}
    [[nodiscard]] constexpr auto end()      const noexcept{return values.cend();}
    [[nodiscard]] constexpr auto cend()     const noexcept{return values.cend();}
    [[nodiscard]] constexpr auto rbegin()         noexcept{return values.rbegin();}
    [[nodiscard]] constexpr auto rbegin()   const noexcept{return values.rcbegin();}
    [[nodiscard]] constexpr auto rcbegin()  const noexcept{return values.rcbegin();}
    [[nodiscard]] constexpr auto rend()           noexcept{return values.rend();}
    [[nodiscard]] constexpr auto rend()     const noexcept{return values.rcend();}
    [[nodiscard]] constexpr auto rcend()    const noexcept{return values.rcend();}

    // spans
    // spans
    [[nodiscard]] auto raw_span()           noexcept -> std::span<std::uint8_t>         {return std::span<std::uint8_t>{get_raw_data(),static_cast<size_t>(size()*sizeof(ElementType))};}
    [[nodiscard]] auto raw_span()   const   noexcept -> std::span<const std::uint8_t>   {return std::span<const std::uint8_t>{get_raw_data(),static_cast<size_t>(size()*sizeof(ElementType))};}
    [[nodiscard]] auto span()               noexcept -> std::span<ElementType>          {return std::span<ElementType>{get_data(), size()};}
    [[nodiscard]] auto span()       const   noexcept -> std::span<const ElementType>    {return std::span<const ElementType>{get_data(), size()};}

    // add
    constexpr auto push_back(const ElementType &v)              -> void{values.push_back(v);}
    constexpr auto push_back(ElementType &&nValue)              -> void{values.push_back(std::move(nValue));}
    template <typename ...ElementsType>
    constexpr auto push_back(const ElementsType&... nValues)    -> void{(push_back(std::forward(nValues)), ...);}
    template <typename ...ElementsType>
    constexpr auto push_back(ElementsType&&... nValue)          -> void{(push_back(std::move(nValue)), ...);}

    constexpr auto push_front(const ElementType &v) -> void{
        values.push_back(v);
        std::rotate(rbegin(), rbegin() + 1, rend());
    }
    constexpr auto push_front(ElementType &&nValue) -> void{
        values.push_back(std::move(nValue));
        std::rotate(rbegin(), rbegin() + 1, rend());
    }

    auto insert_at(size_t id, const ElementType &nValue) noexcept -> void{
        if(id <= values.size()){
            values.insert(values.begin() + id, nValue);
        }
    }
    auto insert_at(size_t id, ElementType &&nValue) noexcept -> void{
        if(id <= values.size()){
            values.insert(begin() + id, std::move(nValue));
        }
    }

    auto merge(const Buffer &valuesToMerge) noexcept -> void{

        if(valuesToMerge.empty()){
            return;
        }

        auto currentSize = size();
        resize(currentSize + valuesToMerge.size());
        std::copy(valuesToMerge.cbegin(), valuesToMerge.cend(), begin() + currentSize);
    }

    auto merge(Buffer &valuesToMerge) noexcept -> void{

        if(valuesToMerge.empty()){
            return;
        }

        auto currentSize = size();
        resize(currentSize + valuesToMerge.size());
        std::move(valuesToMerge.begin(), valuesToMerge.end(), begin() + currentSize);
    }

    auto fill(const ElementType &value) noexcept -> void{
        if(size() > 0){
            std::fill(begin(), end(), value);
        }
    }

    // subset
    [[nodiscard]] constexpr auto copy_subset(size_t idStart, size_t idEnd) const -> Buffer {
        Buffer subBuffer;
        copy_subset_to(idStart, idEnd, subBuffer);
        return subBuffer;
    }

    constexpr auto copy_subset_to(size_t idStart, size_t idEnd, Buffer &subBuffer, size_t subBufferStart = 0) const -> size_t {

        if(idStart > idEnd){
            return 0;
        }
        if(idEnd > values.size()){
            idEnd = values.size();
        }

        auto subSetSize = idEnd-idStart;
        if(subBuffer.size() < subSetSize + subBufferStart){
            subBuffer.resize(subSetSize + subBufferStart );
        }

        std::copy(cbegin() + idStart, cbegin() + idEnd, subBuffer.begin() + subBufferStart);
        return subSetSize;
    }

    [[nodiscard]] constexpr auto copy_last_values(size_t count) const -> Buffer {
        Buffer subBuffer;
        copy_last_values_to(count, subBuffer, 0);
        return subBuffer;
    }

    constexpr auto copy_last_values_to(size_t count, Buffer &subBuffer, size_t subBufferStart = 0) const -> size_t {
        if(count > size()){
            count = size();
        }
        auto start = size() - count;
        return copy_subset_to(start, size(), subBuffer, subBufferStart);
    }

    constexpr auto copy(Buffer &copiedBuffer) const -> size_t {
        return copy_subset_to(0, size(), copiedBuffer, 0);
    }

    // remove
    constexpr auto remove_first() noexcept -> void{
        if(size() > 0){
            remove_at(0);
        }
    }
    constexpr auto remove_last() noexcept -> void{
        if(size() > 0){
            remove_at(size()-1);
        }
    }

    constexpr auto remove_from_to(size_t idStart, size_t idEnd) -> void{
        if(idStart >= idEnd){
            return;
        }
        if(idEnd > values.size()){
            idEnd = values.size();
        }
        values.erase(begin() + idStart, begin() + idEnd);
    }

    constexpr auto remove_before(size_t id) -> void{
        if(id <= size()){
            values.erase(begin(), begin() + id);
        }
    }
    constexpr auto remove_after(size_t id) -> void{
        if(id < size()){
            resize(id+1);
        }
    }
    constexpr auto remove_at(size_t id) noexcept -> void{
        if(id < size()){
            values.erase(begin() + id, begin() + id + 1);
        }
    }

    constexpr auto keep_from_ids(const std::vector<size_t> &ids)  -> void{
        size_t cId = 0;
        for(const auto &id : ids){
            if(id != cId && id < size()){
                values[cId++] = std::move(values[id]);
            }
        }
        values.resize(cId);
    }

    constexpr auto remove_first_values_if_too_large(size_t maxSize, size_t newSize) -> void{
        if(size() > maxSize && newSize < maxSize){
            remove_from_to(0, size() - newSize);
        }
    }

    std::vector<ElementType> values;
};
}
