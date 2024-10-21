

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "dc_client_settings.hpp"

// local
#include "data/json_utility.hpp"

using namespace tool::net;
using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

DCClientSettings::DCClientSettings(){

    sType   = io::SettingsType::Dc_client;
    version = io::SettingsVersion::LastVersion;

    // retrieve interfaces
    ipv4Interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(ipv4Interfaces.size() == 0){
        Log::warning("[DCClientSettings::reset_interfaces] Cannot find any ipv4 interface.\n"sv);
    }
    ipv6Interfaces = Interface::list_local_interfaces(Protocol::ipv6);
    if(ipv6Interfaces.size() == 0){
        Log::warning("[DCClientSettings::reset_interfaces] Cannot find any ipv6 interface.\n"sv);
    }
}

auto DCClientSettings::init_from_json(const nlohmann::json &json) -> void{

    devicesS.clear();

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    read_and_update_value(json, unreadCount, "_client_id"sv, clientId);
    // local
    if(!json.contains("client_devices"sv)){
        Log::error(std::format("[DCClientSettings::init_from_json] Invalid json file.\n"));
        return;
    }

    for(const auto &clientDeviceJson : json["client_devices"sv]){
        DCClientDeviceSettings clientDevice;
        clientDevice.init_from_json(read_and_return_object(clientDeviceJson, unreadCount, "client_device"sv));
        clientDevice.set_id(devicesS.size());
        update_connection_settings(clientDevice.connectionS);
        devicesS.push_back(std::move(clientDevice));
    }

    if(unreadCount != 0){
        tool::Log::warning(std::format("[DCClientSettings::init_from_json] [{}] values have not been initialized from json data.\n"sv, unreadCount));
    }
}

auto DCClientSettings::convert_to_json() const -> nlohmann::json{

    json json;

    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // local
    add_value(json, "_client_id"sv, clientId);
    json::array_t arr;
    for(const auto &clientDeviceS : devicesS){
        nlohmann::json clientDeviceJson;
        add_value(clientDeviceJson, "client_device"sv, clientDeviceS.convert_to_json());
        arr.push_back(clientDeviceJson);
    }
    json["client_devices"sv] = arr;

    return json;
}

auto DCClientSettings::add_device(DCClientType connectionType) -> void{
    DCClientDeviceSettings clientDeviceS;
    clientDeviceS.connectionS.connectionType = connectionType;
    clientDeviceS.connectionS.startReadingThread = true;        
    // ...
    clientDeviceS.set_id(devicesS.size());

    if(connectionType == DCClientType::Local){
        clientDeviceS.deviceS.dataS.apply_local_profile();
    }else if(connectionType == DCClientType::Remote){
        clientDeviceS.deviceS.dataS.apply_remote_profile();
    }

    update_connection_settings(clientDeviceS.connectionS);
    devicesS.push_back(std::move(clientDeviceS));
}

auto DCClientSettings::remove_last_device() -> void{
    devicesS.remove_last();
}

auto DCClientSettings::update_connection_settings(DCDeviceConnectionSettings &connectionS) -> void{

    if(connectionS.connectionType == DCClientType::Remote){

        // retrieve interfaces from current protocol
        const auto &interfaces = (connectionS.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
        if(connectionS.idReadingInterface >= interfaces.size()){
            Log::error(std::format("[DCClientSettings::update_connection_settings] Invalid id reading interface.\n"sv));
            return;
        }

        // retrieve reading address
        if(!connectionS.anyReadingInterface){
            connectionS.readingAddress = interfaces[connectionS.idReadingInterface].ipAddress;
        }else{
            connectionS.readingAddress = "any";
        }

        // retrieve localhost corresponding address
        if(connectionS.sendingAddress == "localhost"sv){
            // connectionS.isLocalhost = true;
            connectionS.processedSendingAddress = interfaces[connectionS.idReadingInterface].ipAddress;
        }else{
            connectionS.processedSendingAddress = connectionS.sendingAddress;
        }
    }

}
