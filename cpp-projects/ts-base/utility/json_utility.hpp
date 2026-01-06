
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
#include "utility/logger.hpp"

// json
#include <nlohmann/json.hpp>

namespace tool{

    using namespace std::string_view_literals;

    template <typename T>
    static auto read_and_update_value(const nlohmann::json &j, size_t &unreadCount, std::string_view key, T &value) -> void{
        if(j.contains(key)){
            value = j[key].template get<T>();
            return;
        }
        Log::warning(std::format("Cannot read value with key [{}] from json data.\n"sv, key));
        ++unreadCount;
    }

    template <typename T>
    static auto read_and_return_value(const nlohmann::json &j, size_t &unreadCount, std::string_view key, T defaultValue = {}) -> T{

        if(j.contains(key)){
            return j[key].template get<T>();
        }
        Log::warning(std::format("Cannot read value with key [{}] from json data.\n"sv, key));
        ++unreadCount;
        return defaultValue;
    }


    [[maybe_unused]] static auto read_and_return_object(const nlohmann::json &j, size_t &unreadCount, std::string_view key) -> nlohmann::json{
        if(j.contains(key)){
            return j[key];            
        }
        Log::warning(std::format("Cannot read object with key [{}] from json data.\n"sv, key));
        ++unreadCount;
        return {};
    }

    template <typename T>
    static auto read_and_update_array(const nlohmann::json &j, size_t &unreadCount, std::string_view key, std::span<T> values) -> void{
        if(j.contains(key)){
            if(j[key].is_array()){
            nlohmann::json::array_t array = j[key];
                if(values.size() >= array.size()){
                    std::move(array.begin(), array.end(), values.begin());
                    return;
                }else{
                    Log::warning(std::format("Cannot read array with key [{}] from json data, invalid size.\n"sv, key));
                }
            }else{
                Log::warning(std::format("Cannot read array with key [{}] from json data, value found is not an array.\n"sv, key));
            }
        }
        ++unreadCount;
    }

    template <typename T>
    static auto add_array(nlohmann::json &j, std::string_view key, std::span<const T> data) -> void{
        if(!j.contains(key)){
            j[key] = data;
        }else{
            Log::warning(std::format("Value with key [{}] already added in json data.\n"sv, key));
        }
    }

    template <typename T>
    static auto add_value(nlohmann::json &j, std::string_view key, const T& value) -> void{
        if(!j.contains(key)){
            j[key] = value;
        }else{
            Log::warning(std::format("Value with key [{}] already added in json data.\n"sv, key));
        }
    }


    // template <>
    // [[maybe_unused]] auto read_value(const nlohmann::json &j, size_t &unreadCount, std::string_view key) -> geo::Pt3f{
    //     if(j.contains(key)){
    //         nlohmann::json d = j[key];
    //         if(d.contains("x"sv) && d.contains("y"sv) && d.contains("z"sv)){
    //             return {
    //                 d["x"sv].template get<float>(),
    //                 d["y"sv].template get<float>(),
    //                 d["z"sv].template get<float>()
    //             };
    //         }
    //     }
    //     Log::warning(std::format("Cannot read Pt3f with key [{}] from json data.\n"sv, key));
    //     ++unreadCount;
    //     return {};
    // }

    // template <>
    // [[maybe_unused]] auto read_value(const nlohmann::json &j, size_t &unreadCount, std::string_view key) -> ColorRGB32{
    //     if(j.contains(key)){
    //         nlohmann::json d = j[key];
    //         if(d.contains("r"sv) && d.contains("g"sv) && d.contains("b"sv)){
    //             return {
    //                 d["r"sv].template get<float>(),
    //                 d["g"sv].template get<float>(),
    //                 d["b"sv].template get<float>()
    //             };
    //         }
    //     }
    //     Log::warning(std::format("Cannot read ColorRGB32 with key [{}] from json data.\n"sv, key));
    //     ++unreadCount;
    //     return {};
    // }



    // template<>
    // [[maybe_unused]] auto add_value<geo::Pt3f>(nlohmann::json &j, std::string_view key, const geo::Pt3f& value) -> void{
    //     add_value(j, key, nlohmann::json{{"x", value.x()}, {"y", value.y()}, {"z", value.z()}});
    // }

    // template<>
    // [[maybe_unused]] auto add_value<ColorRGB32>(nlohmann::json &j, std::string_view key, const ColorRGB32& value) -> void{
    //     add_value(j, key, nlohmann::json{{"r", value.r()}, {"g", value.g()}, {"b", value.b()}});
    // }


}
