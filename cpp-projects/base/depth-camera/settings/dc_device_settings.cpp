

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

#include "dc_device_settings.hpp"

// std
#include <format>

// utility
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool;
using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;
using namespace tool::cam;

auto DCDeviceSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Logger::message("2\n");
    io::BaseSettings::init_from_json(read_object(json, unreadCount, "base"sv));
    // config
    Logger::message("3\n");
    configS.init_from_json(read_object(json, unreadCount, "config"sv));
    // data
    Logger::message("4\n");
    dataS.init_from_json(read_object(json, unreadCount, "data"sv));

    Logger::message("5\n");
    if(unreadCount != 0){
        tool::Logger::warning(std::format("[{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDeviceSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv,              io::BaseSettings::convert_to_json());
    // config
    add_value(json, "config"sv,            configS.convert_to_json());
    // data
    add_value(json, "data"sv,              dataS.convert_to_json());

    return json;
}

auto DCDeviceSettings::apply_remote_grabber_profile() -> void{
    // client
    // # capture
    dataS.client.capture.color                      = true;
    dataS.client.capture.depth                      = true;
    dataS.client.capture.infra                      = false;
    dataS.client.capture.audio                      = false;
    dataS.client.capture.imu                        = false;
    dataS.client.capture.bodyTracking               = false;
    // # generation
    dataS.client.generation.calibration             = false;
    dataS.client.generation.depth                   = false;
    dataS.client.generation.depthSizedColorImage    = false;
    dataS.client.generation.cloud                   = false;
    dataS.client.generation.depthImage              = false;
    dataS.client.generation.infra                   = false;
    dataS.client.generation.infraImage              = false;
    dataS.client.generation.colorImage              = false;
    dataS.client.generation.bodyIdMapImage          = false;
    dataS.client.generation.bodyTracking            = false;
    dataS.client.generation.imu                     = false;
    dataS.client.generation.audio                   = false;
    dataS.client.generation.cloudGenMode            = CloudGenerationMode::FromDepth;
    dataS.client.generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
    // # compression
    dataS.client.compression.jpegCompressionRate    = 80;
    dataS.client.compression.calibration            = true;
    dataS.client.compression.depth                  = true;
    dataS.client.compression.depthSizedColor        = true;
    dataS.client.compression.color                  = false;
    dataS.client.compression.infra                  = false;
    dataS.client.compression.bodyIdMap              = false;
    dataS.client.compression.cloud                  = false;
    dataS.client.compression.bodyTracking           = false;
    dataS.client.compression.audio                  = false;
    dataS.client.compression.imu                    = false;
    dataS.client.compression.cloudColorMode         = CloudColorMode::FromDepthSizedColorImage;
    // server
    // # generation
    dataS.server.generation.calibration             = true;
    dataS.server.generation.depth                   = true;
    dataS.server.generation.depthSizedColorImage    = true;
    dataS.server.generation.cloud                   = true;
    dataS.server.generation.infra                   = false;
    dataS.server.generation.colorImage              = false;
    dataS.server.generation.depthImage              = false;
    dataS.server.generation.infraImage              = false;
    dataS.server.generation.bodyIdMapImage          = false;
    dataS.server.generation.bodyTracking            = false;
    dataS.server.generation.imu                     = false;
    dataS.server.generation.audio                   = false;
    dataS.server.generation.cloudGenMode            = CloudGenerationMode::FromDepth;
    dataS.server.generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
}

auto DCDeviceSettings::apply_only_manager_profile() -> void{
    // client
    // # capture
    dataS.client.capture.color                      = true;
    dataS.client.capture.depth                      = true;
    dataS.client.capture.infra                      = false;
    dataS.client.capture.audio                      = false;
    dataS.client.capture.imu                        = false;
    dataS.client.capture.bodyTracking               = false;
    // # generation
    dataS.client.generation.calibration             = true;
    dataS.client.generation.depth                   = true;
    dataS.client.generation.depthSizedColorImage    = true;
    dataS.client.generation.cloud                   = true;
    dataS.client.generation.depthImage              = true;
    dataS.client.generation.infra                   = false;
    dataS.client.generation.infraImage              = false;
    dataS.client.generation.colorImage              = false;
    dataS.client.generation.bodyIdMapImage          = false;
    dataS.client.generation.bodyTracking            = false;
    dataS.client.generation.imu                     = false;
    dataS.client.generation.audio                   = false;
    dataS.client.generation.cloudGenMode            = CloudGenerationMode::FromDepth;
    dataS.client.generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
    // # compression
    dataS.client.compression.jpegCompressionRate    = 80;
    dataS.client.compression.calibration            = false;
    dataS.client.compression.depth                  = false;
    dataS.client.compression.depthSizedColor        = false;
    dataS.client.compression.color                  = false;
    dataS.client.compression.infra                  = false;
    dataS.client.compression.bodyIdMap              = false;
    dataS.client.compression.cloud                  = false;
    dataS.client.compression.bodyTracking           = false;
    dataS.client.compression.audio                  = false;
    dataS.client.compression.imu                    = false;
    dataS.client.compression.cloudColorMode         = CloudColorMode::FromDepthSizedColorImage;
    // server
    // # generation
    dataS.server.generation.calibration             = false;
    dataS.server.generation.depth                   = false;
    dataS.server.generation.depthSizedColorImage    = false;
    dataS.server.generation.cloud                   = false;
    dataS.server.generation.infra                   = false;
    dataS.server.generation.colorImage              = false;
    dataS.server.generation.depthImage              = false;
    dataS.server.generation.infraImage              = false;
    dataS.server.generation.bodyIdMapImage          = false;
    dataS.server.generation.bodyTracking            = false;
    dataS.server.generation.imu                     = false;
    dataS.server.generation.audio                   = false;
    dataS.server.generation.cloudGenMode            = CloudGenerationMode::FromDepth;
    dataS.server.generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
}

auto DCDeviceSettings::update_with_device_id(size_t order, size_t id) -> void{
    configS.idDevice = static_cast<std::uint32_t>(id);
    configS.synchronizeColorAndDepth = true;
    configS.delayBetweenColorAndDepthUsec = 0;
    if(order == 0){
        configS.synchMode = DCSynchronisationMode::Main;
        configS.subordinateDelayUsec = 0;
    }else{
        configS.synchMode = DCSynchronisationMode::Subordinate;
        configS.subordinateDelayUsec = static_cast<int>(order*160);
    }
}

auto DCDeviceSettings::default_init_for_grabber() -> DCDeviceSettings{
    DCDeviceSettings device;
    device.configS  = DCConfigSettings::default_init_for_grabber();
    return device;
}

auto DCDeviceSettings::default_init_for_manager() -> DCDeviceSettings{
    DCDeviceSettings device;
    device.configS  = DCConfigSettings::default_init_for_manager();
    return device;
}

auto DCDeviceSettings::init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void {

    BaseSettings::init_from_data(data, offset, sizeData);
    configS.init_from_data(data, offset, sizeData);
    dataS.init_from_data(data, offset, sizeData);
    
    version = io::SettingsVersion::LastVersion;
}



