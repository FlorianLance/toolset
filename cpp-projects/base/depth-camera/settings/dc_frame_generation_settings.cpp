

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

#include "dc_frame_generation_settings.hpp"


// local
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCFrameGenerationSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
        read_and_update_value(json, unreadCount, "cloud"sv,                     cloud);
    read_and_update_value(json, unreadCount, "depth_sized_color_image"sv,   depthSizedColorImage);
    read_and_update_value(json, unreadCount, "color_image"sv,               originalSizeColorImage);
    read_and_update_value(json, unreadCount, "depth_image"sv,               depthImage);
    read_and_update_value(json, unreadCount, "infra_image"sv,               infraImage);
    // read_and_update_value(json, unreadCount, "body_id_map_image"sv,         bodyIdMapImage);
    // read_and_update_value(json, unreadCount, "body_tracking"sv,             bodyTracking);
    // read_and_update_value(json, unreadCount, "imu"sv,                       imu);
    // read_and_update_value(json, unreadCount, "audio"sv,                     audio);
    // cloudGenMode    = static_cast<CloudGenerationMode>(read_and_return_value(json, unreadCount, "cloud_gen_mode"sv,     static_cast<int>(cloudGenMode)));
    cloudColorMode  = static_cast<CloudColorMode>(read_and_return_value(json, unreadCount,      "cloud_color_mode"sv,   static_cast<int>(cloudColorMode)));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFrameGenerationSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCFrameGenerationSettings::convert_to_json() const -> nlohmann::json{
    json json;    
    add_value(json, "cloud"sv,                      cloud);
    add_value(json, "depth_sized_color_image"sv,    depthSizedColorImage);
    add_value(json, "color_image"sv,                originalSizeColorImage);
    add_value(json, "depth_image"sv,                depthImage);
    add_value(json, "infra_image"sv,                infraImage);
    // add_value(json, "body_id_map_image"sv,          bodyIdMapImage);
    // add_value(json, "body_tracking"sv,              bodyTracking);
    // add_value(json, "imu"sv,                        imu);
    // add_value(json, "audio"sv,                      audio);
    // add_value(json, "cloud_gen_mode"sv,             static_cast<int>(cloudGenMode));
    add_value(json, "cloud_color_mode"sv,           static_cast<int>(cloudColorMode));
    return json;
}
