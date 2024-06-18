
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

#include "dc_data_settings.hpp"

// std
#include <format>

// local
#include "utility/io_data.hpp"
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCDataSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::BaseSettings::init_from_json(read_object(json, unreadCount, "base"sv));
    // client
    client.capture.init_from_json(read_object(json, unreadCount, "client_capture"sv));
    client.compression.init_from_json(read_object(json, unreadCount, "client_compression"sv));
    client.generation.init_from_json(read_object(json, unreadCount, "client_generation"sv));
    // server
    server.generation.init_from_json(read_object(json, unreadCount, "server_generation"sv));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFiltersSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDataSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv,               io::BaseSettings::convert_to_json());
    // client
    add_value(json, "client_capture"sv,     client.capture.convert_to_json());
    add_value(json, "client_compression"sv, client.compression.convert_to_json());
    add_value(json, "client_generation"sv,  client.generation.convert_to_json());
    // server
    add_value(json, "server_generation"sv,  server.generation.convert_to_json());

    return json;
}


auto DCDataSettings::init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void{

    BaseSettings::init_from_data(data, offset, sizeData);
    read(client, data, offset, sizeData);
    read(server, data, offset, sizeData);

    version = io::SettingsVersion::LastVersion;
}

auto DCCaptureDataSettings::init_from_json(const nlohmann::json &json) -> void{

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

auto DCCaptureDataSettings::convert_to_json() const -> nlohmann::json{
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

auto DCFrameCompressionSettings::init_from_json(const nlohmann::json &json) -> void{
    size_t unreadCount = 0;
    jpegCompressionRate = read_value<std::uint8_t>(json, unreadCount, "jpeg_compression_rate"sv);
    calibration         = read_value<bool>(json, unreadCount, "calibration"sv);
    depth               = read_value<bool>(json, unreadCount, "depth"sv);
    depthSizedColor     = read_value<bool>(json, unreadCount, "dept_sized_color"sv);
    color               = read_value<bool>(json, unreadCount, "color"sv);
    infra               = read_value<bool>(json, unreadCount, "infra"sv);
    bodyIdMap           = read_value<bool>(json, unreadCount, "body_id_map"sv);
    cloud               = read_value<bool>(json, unreadCount, "cloud"sv);
    bodyTracking        = read_value<bool>(json, unreadCount, "body_tracking"sv);
    audio               = read_value<bool>(json, unreadCount, "audio"sv);
    imu                 = read_value<bool>(json, unreadCount, "imu"sv);
    cloudColorMode      = static_cast<CloudColorMode>(read_value<int>(json, unreadCount, "cloud_color_mode"sv));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFrameCompressionSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCFrameCompressionSettings::convert_to_json() const -> nlohmann::json{
    json json;
    add_value(json, "jpeg_compression_rate"sv,  jpegCompressionRate);
    add_value(json, "calibration"sv,            calibration);
    add_value(json, "depth"sv,                  depth);
    add_value(json, "dept_sized_color"sv,       depthSizedColor);
    add_value(json, "color"sv,                  color);
    add_value(json, "infra"sv,                  infra);
    add_value(json, "body_id_map"sv,            bodyIdMap);
    add_value(json, "cloud"sv,                  cloud);
    add_value(json, "body_tracking"sv,          bodyTracking);
    add_value(json, "audio"sv,                  audio);
    add_value(json, "imu"sv,                    imu);
    add_value(json, "cloud_color_mode"sv,       static_cast<int>(cloudColorMode));
    return json;
}

auto DCFrameGenerationSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    calibration             = read_value<bool>(json, unreadCount, "calibration"sv);
    depth                   = read_value<bool>(json, unreadCount, "depth"sv);
    depthSizedColorImage    = read_value<bool>(json, unreadCount, "depth_sized_color_image"sv);
    cloud                   = read_value<bool>(json, unreadCount, "cloud"sv);
    infra                   = read_value<bool>(json, unreadCount, "infra"sv);
    colorImage              = read_value<bool>(json, unreadCount, "color_image"sv);
    depthImage              = read_value<bool>(json, unreadCount, "depth_image"sv);
    infraImage              = read_value<bool>(json, unreadCount, "infra_image"sv);
    bodyIdMapImage          = read_value<bool>(json, unreadCount, "body_id_map_image"sv);
    bodyTracking            = read_value<bool>(json, unreadCount, "body_tracking"sv);
    imu                     = read_value<bool>(json, unreadCount, "imu"sv);
    audio                   = read_value<bool>(json, unreadCount, "audio"sv);
    cloudGenMode            = static_cast<CloudGenerationMode>(read_value<int>(json, unreadCount, "cloud_gen_mode"sv));
    cloudColorMode          = static_cast<CloudColorMode>(read_value<int>(json, unreadCount, "cloud_color_mode"sv));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCFrameGenerationSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCFrameGenerationSettings::convert_to_json() const -> nlohmann::json{
    json json;
    add_value(json, "calibration"sv,                calibration);
    add_value(json, "depth"sv,                      depth);
    add_value(json, "depth_sized_color_image"sv,    depthSizedColorImage);
    add_value(json, "cloud"sv,                      cloud);
    add_value(json, "infra"sv,                      infra);
    add_value(json, "color_image"sv,                colorImage);
    add_value(json, "depth_image"sv,                depthImage);
    add_value(json, "infra_image"sv,                infraImage);
    add_value(json, "body_id_map_image"sv,          bodyIdMapImage);
    add_value(json, "body_tracking"sv,              bodyTracking);
    add_value(json, "imu"sv,                        imu);
    add_value(json, "audio"sv,                      audio);
    add_value(json, "cloud_gen_mode"sv,             static_cast<int>(cloudGenMode));
    add_value(json, "cloud_color_mode"sv,           static_cast<int>(cloudColorMode));
    return json;
}
