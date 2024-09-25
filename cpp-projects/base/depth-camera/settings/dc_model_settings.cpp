
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

#include "dc_model_settings.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

#include "utility/string.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;


auto DCModelSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // model
    read_and_update_array<float>(json, unreadCount, "transformation"sv, transformation.array);

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCModelSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCModelSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // model
    add_array<float>(json, "transformation"sv,    transformation.array);

    return json;
}

auto DCModelSettings::compute_full_transformation() const -> tool::geo::Mat4f{
    return geo::transform(scaling, rotation, translation) * transformation;
}

auto DCModelSettings::init_from_text(std::string_view &text) -> void  {
    
    io::Settings::init_from_text(text);

    size_t id = 0;
    for(const auto &line : String::split_view(text, "\n"sv)){
        if(line.length() == 0){
            return;
        }
        for(auto value : String::split_view(line, " "sv)){
            transformation.array[id++] = String::to_float(value);
            if(id == 16){
                return;
            }
        }
    }
}

