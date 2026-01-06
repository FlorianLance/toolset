

/*******************************************************************************
** Toolset-ts-base                                                            **
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

#include "io_file.hpp"

// std
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif


using namespace tool;

auto File::read_content(const std::string &filePath) -> std::optional<std::string>{

    // open file
    std::ifstream file(filePath);
    if(!file.is_open()){
        return std::nullopt;
    }

    // copy content
    std::string str;
    file.seekg(0, std::ios::end);
    str.reserve(static_cast<size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return {str};
}

auto File::write_text_content(const std::string &filePath, std::string_view text) -> bool{

    std::ofstream file;
    file.open(filePath, std::ios::out);
    if(!file.is_open()){
        return false;
    }

    file.write(text.data(), text.size());
    return true;
}

auto File::write_binary_content(const std::string &filePath, std::span<const std::int8_t> data) -> bool{

    std::ofstream file;
    file.open(filePath, std::ios::out | std::ios::binary);
    if(!file.is_open()){
        return false;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

auto File::write_binary_content(const std::string &filePath, std::span<const uint8_t> data) -> bool{
    return write_binary_content(filePath, std::span<const std::int8_t>{reinterpret_cast<const std::int8_t*>(data.data()), data.size()});
}

auto File::execute_file(std::string_view filePath) -> bool{

#ifdef _WIN32
    std::wstring temp = std::wstring(filePath.begin(), filePath.end());
    LPCWSTR lpFile = temp.c_str();
    ShellExecute(0, 0, lpFile, 0, 0 , SW_SHOW );
#elif
    static_assert(false,"Not implemented.");
#endif

    return true;
}



