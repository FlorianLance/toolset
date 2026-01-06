
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
#pragma once

// std
#include <string>
#include <vector>
#include <span>

namespace tool{

struct str{

    // join
    static auto join(std::span<const std::string> parts) -> std::string;
    static auto join(std::span<const std::string> parts, const std::string &sep) -> std::string;

    // split
    static auto split_path_and_filename(const std::string &s) -> std::pair<std::string, std::string>;
    static auto split(const std::string &str, char delimiter = ' ') -> std::vector<std::string>;
    static auto split(const std::string &str, const std::string &delimiter = " ", bool trimEmpty = false) -> std::vector<std::string>;

    // trim
    static auto left_trim(std::string &str, char c) -> void;
    static auto right_trim(std::string &str, char c) -> void;
    static auto remove_from_left(std::string &str, char delim) -> void;
    static auto remove_from_right(std::string &str, char delim) -> void;
    static auto remove_after_right(std::string &str, char delim) -> void;

    // replace
    static auto replace_first(std::string& str, const std::string& from, const std::string& to) -> bool;
    static auto replace_all(std::string& str, const std::string& from, const std::string& to) -> void;
    static auto replace_all2(std::string& source, const std::string& from, const std::string& to) -> void;

    // view
    static auto advance_view_to_delim(std::string_view str, std::string_view delims) -> std::string_view;
    static auto split_view(std::string_view strv, std::string_view delims = " ") -> std::vector<std::string_view>;

    // convert
    static auto to_char(std::string_view str) -> char;
    static auto to_int(std::string_view str) -> int;
    static auto to_float(std::string_view str) -> float;

};
}
