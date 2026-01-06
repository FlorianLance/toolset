

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

#include "string.hpp"

// std
#include <numeric>
#include <sstream>
#include <charconv>
#include <algorithm>

auto tool::str::join(std::span<const std::string> parts) -> std::string{
    return std::accumulate(parts.begin(), parts.end(), std::string{});
}

auto tool::str::join(std::span<const std::string> parts, const std::string &delim) -> std::string{

    if (parts.empty()){
        return std::string();
    }

    return std::accumulate(
        next(begin(parts)), // there is at least 1 element, so OK.
        end(parts),
        parts[0], // the initial value
        [&delim](auto result, const auto &value) {
            return result + delim + value;
        }
    );
}

auto tool::str::left_trim(std::string &str, char c) -> void{
    str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), [c](int i) -> int { return i == c; }));
}

auto tool::str::right_trim(std::string &str, char c) -> void{
    str.erase((std::find_if_not(str.rbegin(), str.rend(), [c](int i) -> int { return i == c; })).base(), str.end());
}

auto tool::str::remove_from_left(std::string &str, char delim) -> void{
    auto pos = str.find_last_of(delim);
    if(pos ==  std::string::npos){
        return;
    }
    str.erase(0, pos+1);
}

auto tool::str::remove_from_right(std::string &str, char delim) -> void{
    auto pos = str.find_last_of(delim);
    if(pos ==  std::string::npos){
        return;
    }
    str.erase(pos);
}

auto tool::str::remove_after_right(std::string &str, char delim) -> void{
    auto pos = str.find_first_of(delim);
    if(pos ==  std::string::npos){
        return;
    }
    str.erase(pos);
}

auto tool::str::split_path_and_filename(const std::string &str) -> std::pair<std::string, std::string>{

    auto pos1 = str.find_last_of('/');
    auto pos2 = str.find_last_of('\\');
    if(pos1 ==  std::string::npos){
        if(pos2 == std::string::npos){
            return {".", str};
        }else{
            return std::make_pair(str.substr(0, pos2), str.substr(pos2+1));
        }
    }

    return std::make_pair(str.substr(0, pos1), str.substr(pos1+1));
}

auto tool::str::split(const std::string &str, char delimiter) -> std::vector<std::string>{
    std::vector<std::string> elems;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)){
        elems.push_back(item);
    }
    return elems;
}


auto tool::str::split(const std::string& str, const std::string &delimiter, bool trimEmpty) -> std::vector<std::string>{

    std::string::size_type pos, lastPos = 0, length = str.length(), dLength = delimiter.length();
    std::vector<std::string> tokens;

    while(lastPos < length + 1){

        pos = str.find(delimiter, lastPos);
        if(pos == std::string::npos){
            pos = length;
        }

        if(pos != lastPos || !trimEmpty){
            tokens.push_back(std::string(str.data() + lastPos, pos-lastPos));
        }

        lastPos = pos + dLength;
    }
    return tokens;
}


auto tool::str::replace_first(std::string &str, const std::string &from, const std::string &to) -> bool{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos){
        return false;
    }
    str.replace(start_pos, from.length(), to);
    return true;
}

auto tool::str::replace_all(std::string &str, const std::string &from, const std::string &to) -> void{
    if(from.empty()){
        return;
    }
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

auto tool::str::replace_all2(std::string &source, const std::string &from, const std::string &to) -> void{

    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos))){
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString.append(source, lastPos, source.length() - lastPos);
    source.swap(newString);
}

auto tool::str::advance_view_to_delim(std::string_view str, std::string_view delims) -> std::string_view{
    if(auto elem = str.find_first_of(delims); elem != std::string_view::npos){
        return str.substr(elem+1);
    }
    return str;
}

auto tool::str::split_view(std::string_view strv, std::string_view delims) -> std::vector<std::string_view>{

    std::vector<std::string_view> output;
    size_t first = 0;
    while (first < strv.size()){

        const auto second = strv.find_first_of(delims, first);
        if (first != second){
            output.emplace_back(strv.substr(first, second-first));
        }
        if (second == std::string_view::npos){
            break;
        }
        first = second + 1;
    }
    return output;
}

auto tool::str::to_char(std::string_view str) -> char{
    char value;
    std::from_chars(str.data(), str.data() + str.size(), value);
    return value;
}

auto tool::str::to_int(std::string_view str) -> int{
    int value;
    std::from_chars(str.data(), str.data() + str.size(), value);
    return value;
}

auto tool::str::to_float(std::string_view str) -> float{
    float value;
    std::from_chars(str.data(), str.data() + str.size(), value);
    return value;
}

