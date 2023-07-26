
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
#include <algorithm>

namespace tool{

// read
template<typename ReadValueType>
static auto read(ReadValueType &v, const std::int8_t *data) -> void {
    std::copy(
        data,
        data + sizeof(ReadValueType),
        reinterpret_cast<std::int8_t*>(&v)
    );
}

template<typename ReadValueType>
static auto read(ReadValueType &v, const std::int8_t *data, size_t &offset) -> void {
    std::copy(
        data + offset,
        data + offset + sizeof(ReadValueType),
        reinterpret_cast<std::int8_t*>(&v)
    );
    offset += sizeof(ReadValueType);
}

template<typename ReadValueType>
static auto read(ReadValueType &v, const std::int8_t *data, size_t &offset, size_t sizeData) -> void {

    if((offset + sizeof(ReadValueType) >= sizeData) && (sizeData != 0)){
        return;
    }

    std::copy(
        data + offset,
        data + offset + sizeof(ReadValueType),
        reinterpret_cast<std::int8_t*>(&v)
    );
    offset += sizeof(ReadValueType);
}

template<typename ReadValueType>
static auto read(ReadValueType &v, const std::int8_t *data, ReadValueType min, ReadValueType max) -> void {
    read(v, data);
    v = std::clamp<ReadValueType>(v, min, max);
}

template<typename ReadValueType>
static auto read(ReadValueType &v, const std::int8_t *data, size_t &offset, ReadValueType min, ReadValueType max) -> void {
    read(v, data, offset);
    v = std::clamp<ReadValueType>(v, min, max);
}

template<typename ReadValueType>
static auto read(ReadValueType &v, const std::int8_t *data, size_t &offset, ReadValueType min, ReadValueType max, size_t sizeData) -> void {
    read(v, data, offset, sizeData);
    v = std::clamp<ReadValueType>(v, min, max);
}

template<typename ReadArrayValueType>
static auto read_array(ReadArrayValueType *a, const std::int8_t *data, size_t sizeArray) -> void {
    auto nbBytes = sizeArray * sizeof(ReadArrayValueType);
    std::copy(
        data,
        data + nbBytes,
        reinterpret_cast<std::int8_t*>(a)
    );
}

template<typename ReadArrayValueType>
static auto read_array(ReadArrayValueType *a, const std::int8_t *data, size_t sizeArray, size_t &offset) -> void {
    auto nbBytes = sizeArray * sizeof(ReadArrayValueType);
    std::copy(
        data + offset,
        data + offset + nbBytes,
        reinterpret_cast<std::int8_t*>(a)
    );
    offset += nbBytes;
}

template<typename ReadArrayValueType>
static auto read_array(ReadArrayValueType *a, const std::int8_t *data, size_t sizeArray, size_t &offset, size_t sizeData) -> void {

    auto nbBytes = sizeArray * sizeof(ReadArrayValueType);
    if((offset + nbBytes >= sizeData) && (sizeData != 0)){
        return;
    }

    std::copy(
        data + offset,
        data + offset + nbBytes,
        reinterpret_cast<std::int8_t*>(a)
    );
    offset += nbBytes;
}


// write
template<typename WriteValueType>
static auto write(const WriteValueType &v, std::int8_t *data) -> void {
    std::copy(
        reinterpret_cast<const std::int8_t*>(&v),
        reinterpret_cast<const std::int8_t*>(&v) + sizeof(WriteValueType),
        data
    );
}

template<typename WriteValueType>
static auto write(const WriteValueType &v, std::int8_t *data, size_t &offset) -> void {
    std::copy(
        reinterpret_cast<const std::int8_t*>(&v),
        reinterpret_cast<const std::int8_t*>(&v) + sizeof(WriteValueType),
        data + offset
    );
    offset += sizeof(WriteValueType);
}

template<typename WriteArrayValueType>
static auto write_array(const WriteArrayValueType *a, std::int8_t *data, size_t sizeArray) -> void {
    auto nbBytes = sizeArray * sizeof(WriteArrayValueType);
    std::copy(
        reinterpret_cast<const std::int8_t*>(a),
        reinterpret_cast<const std::int8_t*>(a) + nbBytes,
        data
    );
}

template<typename WriteArrayValueType>
static auto write_array(const WriteArrayValueType *a, std::int8_t *data, size_t sizeArray, size_t &offset) -> void {
    auto nbBytes = sizeArray * sizeof(WriteArrayValueType);
    std::copy(
        reinterpret_cast<const std::int8_t*>(a),
        reinterpret_cast<const std::int8_t*>(a) + nbBytes,
        data+ offset
    );
    offset += nbBytes;
}

}
