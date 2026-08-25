
/*******************************************************************************
** Toolset-ts-base                                                            **
** MIT License                                                                **
** Copyright (c) [2026] [Florian Lance]                                       **
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

#include "dc_mouse_ray_settings.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"
#include "utility/json_utility.hpp"

using namespace tool::cam;
using json = nlohmann::json;

auto DCMouseRaySettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    rayMode = static_cast<MouseRayMode>(read_and_return_value(json, unreadCount, "ray_mode"sv, static_cast<std::int8_t>(rayMode)));
    read_and_update_array(json, unreadCount, "ray_start"sv, rayStart.span());
    read_and_update_array(json, unreadCount, "ray_start"sv, rayStart.span());

    if(unreadCount != 0){
        tool::Log::warning(std::format("[DCMouseRaySettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCMouseRaySettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, Settings::convert_to_json());
    add_value(json, "ray_mode"sv,   static_cast<std::int8_t>(rayMode));
    add_array(json, "ray_start"sv,   rayStart.cspan());
    add_array(json, "ray_end"sv,    rayEnd.cspan());

    return json;
}





