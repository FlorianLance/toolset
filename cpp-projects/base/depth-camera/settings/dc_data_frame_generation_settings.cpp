

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

#include "dc_data_frame_generation_settings.hpp"

// local
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCDataFrameGenerationSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;

    read_and_update_value(json, unreadCount, "jpeg_compression_rate"sv,             depthSizedColorJPEGCQ);
    read_and_update_value(json, unreadCount, "original_size_color_jpeg_quality"sv,  originalSizeColorJPEGCQ);

    read_and_update_value(json, unreadCount, "depth"sv,                             addDepth);
    read_and_update_value(json, unreadCount, "dept_sized_color"sv,                  addDepthSizedColor);
    read_and_update_value(json, unreadCount, "color"sv,                             addOriginalSizeColor);
    read_and_update_value(json, unreadCount, "infra"sv,                             addInfra);
    read_and_update_value(json, unreadCount, "body_id_map"sv,                       addBodyIdMap);
    read_and_update_value(json, unreadCount, "cloud"sv,                             addCloud);
    read_and_update_value(json, unreadCount, "body_tracking"sv,                     addBodyTracking);
    read_and_update_value(json, unreadCount, "audio"sv,                             addAudio);
    read_and_update_value(json, unreadCount, "imu"sv,                               addImu);

    cloudColorMode      = static_cast<CloudColorMode>(   read_and_return_value(json, unreadCount, "cloud_color_mode"sv,                     static_cast<int>(cloudColorMode)));

    depthCM             = static_cast<DCCompressionMode>(read_and_return_value(json, unreadCount, "depth_compression_mode"sv,               static_cast<int>(depthCM)));
    depthSizedColorCM   = static_cast<DCCompressionMode>(read_and_return_value(json, unreadCount, "depth_sized_color_compression_mode"sv,   static_cast<int>(depthSizedColorCM)));
    originalSizeColorCM = static_cast<DCCompressionMode>(read_and_return_value(json, unreadCount, "color_compression_mode"sv,               static_cast<int>(originalSizeColorCM)));
    infraCM             = static_cast<DCCompressionMode>(read_and_return_value(json, unreadCount, "infra_compression_mode"sv,               static_cast<int>(infraCM)));
    bodiesIdMapCM       = static_cast<DCCompressionMode>(read_and_return_value(json, unreadCount, "bodies_id_map_compression_mode"sv,       static_cast<int>(bodiesIdMapCM)));
    cloudCM             = static_cast<DCCompressionMode>(read_and_return_value(json, unreadCount, "cloud_compression_mode"sv,               static_cast<int>(cloudCM)));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFrameCompressionSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDataFrameGenerationSettings::convert_to_json() const -> nlohmann::json{
    json json;
    add_value(json, "jpeg_compression_rate"sv,              depthSizedColorJPEGCQ);
    add_value(json, "original_size_color_jpeg_quality"sv,   originalSizeColorJPEGCQ);

    add_value(json, "depth"sv,                              addDepth);
    add_value(json, "dept_sized_color"sv,                   addDepthSizedColor);
    add_value(json, "color"sv,                              addOriginalSizeColor);
    add_value(json, "infra"sv,                              addInfra);
    add_value(json, "body_id_map"sv,                        addBodyIdMap);
    add_value(json, "cloud"sv,                              addCloud);
    add_value(json, "body_tracking"sv,                      addBodyTracking);
    add_value(json, "audio"sv,                              addAudio);
    add_value(json, "imu"sv,                                addImu);
    add_value(json, "cloud_color_mode"sv,                   static_cast<int>(cloudColorMode));
    add_value(json, "depth_compression_mode"sv,             static_cast<int>(depthCM));
    add_value(json, "depth_sized_color_compression_mode"sv, static_cast<int>(depthSizedColorCM));
    add_value(json, "color_compression_mode"sv,             static_cast<int>(originalSizeColorCM));
    add_value(json, "infra_compression_mode"sv,             static_cast<int>(infraCM));
    add_value(json, "bodies_id_map_compression_mode"sv,     static_cast<int>(bodiesIdMapCM));
    add_value(json, "cloud_compression_mode"sv,             static_cast<int>(cloudCM));
    return json;
}
