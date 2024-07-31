

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
    io::Settings::init_from_json(read_object(json, unreadCount, "base"sv));
    // config
    configS.init_from_json(read_object(json, unreadCount, "config"sv));
    // data
    dataS.init_from_json(read_object(json, unreadCount, "data"sv));

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDeviceSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv,              io::Settings::convert_to_json());
    // config
    add_value(json, "config"sv,            configS.convert_to_json());
    // data
    add_value(json, "data"sv,              dataS.convert_to_json());

    return json;
}

auto DCDeviceSettings::apply_remote_profile() -> void{

    // server
    dataS.server.apply_remote_profile();

    // client
    // # generation
    dataS.clientGeneration.calibration             = true;
    dataS.clientGeneration.depth                   = true;
    dataS.clientGeneration.depthSizedColorImage    = true;
    dataS.clientGeneration.cloud                   = true;
    dataS.clientGeneration.infra                   = false;
    dataS.clientGeneration.colorImage              = false;
    dataS.clientGeneration.depthImage              = false;
    dataS.clientGeneration.infraImage              = false;
    dataS.clientGeneration.bodyIdMapImage          = false;
    dataS.clientGeneration.bodyTracking            = false;
    dataS.clientGeneration.imu                     = false;
    dataS.clientGeneration.audio                   = false;
    dataS.clientGeneration.cloudGenMode            = CloudGenerationMode::FromDepth;
    dataS.clientGeneration.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
}

auto DCDeviceSettings::apply_local_profile() -> void{

    // server
    dataS.server.apply_local_profile();

    // client
    // # generation
    dataS.clientGeneration.calibration             = false;
    dataS.clientGeneration.depth                   = false;
    dataS.clientGeneration.depthSizedColorImage    = false;
    dataS.clientGeneration.cloud                   = false;
    dataS.clientGeneration.infra                   = false;
    dataS.clientGeneration.colorImage              = false;
    dataS.clientGeneration.depthImage              = false;
    dataS.clientGeneration.infraImage              = false;
    dataS.clientGeneration.bodyIdMapImage          = false;
    dataS.clientGeneration.bodyTracking            = false;
    dataS.clientGeneration.imu                     = false;
    dataS.clientGeneration.audio                   = false;
    dataS.clientGeneration.cloudGenMode            = CloudGenerationMode::FromDepth;
    dataS.clientGeneration.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
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
    
    Settings::init_from_data(data, offset, sizeData);
    configS.init_from_data(data, offset, sizeData);
    dataS.init_from_data(data, offset, sizeData);
    
    version = io::SettingsVersion::LastVersion;
}



