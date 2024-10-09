
/*******************************************************************************
** Toolset-dc-grabber                                                  **
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

#include "dc_server_settings.hpp"

// local
#include "utility/logger.hpp"
#include "data/json_utility.hpp"

using namespace tool::net;
using namespace tool::cam;
using namespace std::chrono;
using namespace tool::data;
using json = nlohmann::json;

DCServerSettings::DCServerSettings(){

    sType   = io::SettingsType::Dc_server;
    version = io::SettingsVersion::LastVersion;

    // retrieve interfaces
    ipv4Interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(ipv4Interfaces.size() == 0){
        Logger::warning("[DCServerSettings::reset_interfaces] Cannot find any ipv4 interface.\n"sv);
    }
    ipv6Interfaces = Interface::list_local_interfaces(Protocol::ipv6);
    if(ipv6Interfaces.size() == 0){
        Logger::warning("[DCServerSettings::reset_interfaces] Cannot find any ipv6 interface.\n"sv);
    }
}

auto DCServerSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // local
    udpServerS.init_from_json(read_and_return_object(json, unreadCount, "udp_server"sv));
    deviceS.init_from_json(read_and_return_object(json, unreadCount, "device"sv));
    filtersS.init_from_json(read_and_return_object(json, unreadCount, "filters"sv));
    calibrationFiltersS.init_from_json(read_and_return_object(json, unreadCount, "calibration_filters"sv));
    colorS.init_from_json(read_and_return_object(json, unreadCount, "color"sv));
    modelS.init_from_json(read_and_return_object(json, unreadCount, "model"sv));
    miscS.init_from_json(read_and_return_object(json, unreadCount, "misc"sv));

    if(unreadCount != 0){
        Logger::warning(std::format("[DCServerSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }

    update_reading_interface();
}

auto DCServerSettings::convert_to_json() const -> nlohmann::json{
    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // local
    add_value(json, "udp_server"sv, udpServerS.convert_to_json());
    add_value(json, "device"sv, deviceS.convert_to_json());
    add_value(json, "filters"sv, filtersS.convert_to_json());
    add_value(json, "calibration_filters"sv, calibrationFiltersS.convert_to_json());
    add_value(json, "color"sv, colorS.convert_to_json());
    add_value(json, "model"sv, modelS.convert_to_json());
    add_value(json, "misc"sv, miscS.convert_to_json());

    return json;
}

auto DCServerSettings::update_reading_interface() -> void{
    const auto &interfaces = (udpServerS.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
    if(udpServerS.readingInterfaceId < interfaces.size()){
        if(!udpServerS.anyReadingInterface){
            udpReadingInterface = interfaces[udpServerS.readingInterfaceId];
        }else{
            udpReadingInterface = Interface(udpServerS.protocol, "any");
        }
    }else{
        Logger::error("[DCServerSettings::update_reading_interface] Invalid reading interface id, not enough interfaces.\n");
    }
}
