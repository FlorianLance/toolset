

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

#include "dc_frame_compression_settings.hpp"

// local
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCFrameCompressionSettings::init_from_json(const nlohmann::json &json) -> void{
    size_t unreadCount = 0;
    jpegCompressionRate = read_value<std::uint8_t>(json, unreadCount, "jpeg_compression_rate"sv);
    addCalibration         = read_value<bool>(json, unreadCount, "calibration"sv);
    addDepth               = read_value<bool>(json, unreadCount, "depth"sv);
    addDepthSizedColor     = read_value<bool>(json, unreadCount, "dept_sized_color"sv);
    addColor               = read_value<bool>(json, unreadCount, "color"sv);
    addInfra               = read_value<bool>(json, unreadCount, "infra"sv);
    addBodyIdMap           = read_value<bool>(json, unreadCount, "body_id_map"sv);
    addCloud               = read_value<bool>(json, unreadCount, "cloud"sv);
    addBodyTracking        = read_value<bool>(json, unreadCount, "body_tracking"sv);
    addAudio               = read_value<bool>(json, unreadCount, "audio"sv);
    addImu                 = read_value<bool>(json, unreadCount, "imu"sv);
    cloudColorMode         = static_cast<CloudColorMode>(read_value<int>(json, unreadCount, "cloud_color_mode"sv));

    depthCompressionMode            = static_cast<DCCompressionMode>(read_value<int>(json, unreadCount, "depth_compression_mode"sv));
    depthSizedColorCompressionMode  = static_cast<DCCompressionMode>(read_value<int>(json, unreadCount, "depth_sized_color_compression_mode"sv));
    colorCompressionMode            = static_cast<DCCompressionMode>(read_value<int>(json, unreadCount, "color_compression_mode"sv));
    infraCompressionMode            = static_cast<DCCompressionMode>(read_value<int>(json, unreadCount, "infra_compression_mode"sv));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFrameCompressionSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCFrameCompressionSettings::convert_to_json() const -> nlohmann::json{
    json json;
    add_value(json, "jpeg_compression_rate"sv,  jpegCompressionRate);
    add_value(json, "calibration"sv,            addCalibration);
    add_value(json, "depth"sv,                  addDepth);
    add_value(json, "dept_sized_color"sv,       addDepthSizedColor);
    add_value(json, "color"sv,                  addColor);
    add_value(json, "infra"sv,                  addInfra);
    add_value(json, "body_id_map"sv,            addBodyIdMap);
    add_value(json, "cloud"sv,                  addCloud);
    add_value(json, "body_tracking"sv,          addBodyTracking);
    add_value(json, "audio"sv,                  addAudio);
    add_value(json, "imu"sv,                    addImu);
    add_value(json, "cloud_color_mode"sv,       static_cast<int>(cloudColorMode));

    add_value(json, "depth_compression_mode"sv,             static_cast<int>(depthCompressionMode));
    add_value(json, "depth_sized_color_compression_mode"sv, static_cast<int>(depthSizedColorCompressionMode));
    add_value(json, "color_compression_mode"sv,             static_cast<int>(colorCompressionMode));
    add_value(json, "infra_compression_mode"sv,             static_cast<int>(infraCompressionMode));

    return json;
}
