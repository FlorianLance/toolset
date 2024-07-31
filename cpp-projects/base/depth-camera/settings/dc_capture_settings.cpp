

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

#include "dc_capture_settings.hpp"

// local
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCCaptureSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    color               = read_value<bool>(json, unreadCount, "color"sv);
    depth               = read_value<bool>(json, unreadCount, "depth"sv);
    infra               = read_value<bool>(json, unreadCount, "infra"sv);
    bodyTracking        = read_value<bool>(json, unreadCount, "body_tracking"sv);
    btTemporalSmoothing = read_value<float>(json, unreadCount, "bt_temporal_smoothing"sv);
    audio               = read_value<bool>(json, unreadCount, "audio"sv);
    imu                 = read_value<bool>(json, unreadCount, "imu"sv);

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCCaptureDataSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCCaptureSettings::convert_to_json() const -> nlohmann::json{
    json json;
    add_value(json, "color"sv,                  color);
    add_value(json, "depth"sv,                  depth);
    add_value(json, "infra"sv,                  infra);
    add_value(json, "body_tracking"sv,          bodyTracking);
    add_value(json, "bt_temporal_smoothing"sv,  btTemporalSmoothing);
    add_value(json, "audio"sv,                  audio);
    add_value(json, "imu"sv,                    imu);
    return json;
}

