
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
#include <fstream>
#include <ostream>
#include <vector>

namespace tool{

// read
template<typename ReadValueType>
auto read(std::ifstream &file) -> ReadValueType{
    ReadValueType value;
    file.read(reinterpret_cast<char*>(&value), sizeof(ReadValueType));
    return value;
}
template<typename ReadValueType>
[[deprecated("Use read(ReadValueType *value, std::ifstream &file) instead.")]]
auto read(std::ifstream &file, ReadValueType *value) -> void{
    file.read(reinterpret_cast<char*>(value), sizeof(ReadValueType));
}
template<typename ReadValueType>
auto read(ReadValueType *value, std::ifstream &file) -> void{
    file.read(reinterpret_cast<char*>(value), sizeof(ReadValueType));
}
template<typename ReadValueType>
auto read(ReadValueType &value, std::ifstream &file) -> void{
    file.read(reinterpret_cast<char*>(&value), sizeof(ReadValueType));
}

template<typename ReadArrayValueType>
[[deprecated("Use read_array(ReadArrayValueType *value, std::ifstream &file, size_t size) instead.")]]
auto read_array(std::ifstream &file, ReadArrayValueType *value, size_t size) -> void{
    if(size > 0){
        file.read(reinterpret_cast<char*>(value), sizeof(ReadArrayValueType)*size);
    }
}
template<typename ReadArrayValueType>
auto read_array(ReadArrayValueType *value, std::ifstream &file, size_t size) -> void{
    if(size > 0){
        file.read(reinterpret_cast<char*>(value), sizeof(ReadArrayValueType)*size);
    }
}
template<typename ReadArrayValueType>
auto read_array(std::ifstream &file, size_t size) -> std::vector<ReadArrayValueType>{
    std::vector<ReadArrayValueType> values(size);
    file.read(reinterpret_cast<char*>(values.data()), sizeof(ReadArrayValueType)*size);
    return values;
}

// write
template<typename WriteValueType>
[[deprecated("Use write(const WriteValueType &value, std::ofstream &file) instead.")]]
auto write(std::ofstream &file, WriteValueType value) -> void{
    file.write(reinterpret_cast<char*>(&value), sizeof(WriteValueType));
}
template<typename WriteValueType>
[[deprecated("Use write(const WriteValueType *value, std::ofstream &file) instead.")]]
auto write(std::ofstream &file, WriteValueType *value) -> void{
    file.write(reinterpret_cast<char*>(value), sizeof(WriteValueType));
}
template<typename WriteValueType>
auto write(const WriteValueType &value, std::ofstream &file) -> void{
    file.write(reinterpret_cast<const char*>(&value), sizeof(WriteValueType));
}
template<typename WriteValueType>
auto write(const WriteValueType *value, std::ofstream &file) -> void{
    file.write(reinterpret_cast<const char*>(value), sizeof(WriteValueType));
}

template<typename WriteArrayValueType>
[[deprecated("Use write_array(const WriteArrayValueType *value, std::ofstream &file, size_t size) instead.")]]
auto write_array(std::ofstream &file, WriteArrayValueType *value, size_t size) -> void{
    if(size > 0 ){
        file.write(reinterpret_cast<char*>(value), sizeof(WriteArrayValueType)*size);
    }
}
template<typename WriteArrayValueType>
auto write_array(const WriteArrayValueType *value, std::ofstream &file, size_t size) -> void{
    if(size > 0 ){
        file.write(reinterpret_cast<const char*>(value), sizeof(WriteArrayValueType)*size);
    }
}

}
