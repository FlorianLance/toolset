

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

#include "dc_device_connection_settings.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"
#include "utility/string.hpp"
#include "data/json_utility.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCDeviceConnectionSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_object(json, unreadCount, "base"sv));
    // local
    connectionType = (read_value<std::string>(json, unreadCount, "connection_type"sv) == "Remote"sv) ? DCClientType::Remote : DCClientType::Local;
    if(connectionType == DCClientType::Remote){
        read_value(json, unreadCount, "id_reading_interface"sv, idReadingInterface);
        read_value<int>(json, unreadCount, "reading_port"sv, readingPort);
        read_value<std::string>(json, unreadCount, "sending_address"sv, sendingAddress);
        read_value<int>(json, unreadCount, "sending_port"sv, sendingPort);
        protocol = (read_value<std::string>(json, unreadCount, "protocol"sv) == "ipv6"sv) ? Protocol::ipv6 : Protocol::ipv4;
    }

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCDeviceConnectionSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDeviceConnectionSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // local
    add_value(json, "connection_type"sv, (connectionType == DCClientType::Remote) ? "Remote"sv : "Local"sv);
    if(connectionType == DCClientType::Remote){
        add_value(json, "id_reading_interface"sv,     idReadingInterface);
        add_value(json, "reading_port"sv,             readingPort);
        add_value(json, "sending_address"sv,          isLocalhost ? "localhost"sv : sendingAddress);
        add_value(json, "sending_port"sv,             sendingPort);
        add_value(json, "protocol"sv,                 (protocol == Protocol::ipv6) ? "ipv6"sv : "ipv4"sv);
    }

    return json;
}


auto DCDeprecatedClientConnectionSettings::init_from_json(const nlohmann::json &json) -> void{
    
    connectionsS.clear();

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_object(json, unreadCount, "base"sv));

    std::string arrayN = "devices";
    if(!json.contains(arrayN)){
        arrayN = "clients"; // old name
        if(!json.contains(arrayN)){
            Logger::error(std::format("[DCClientConnectionSettings::init_from_json] Invalid json file.\n"));
            return;
        }
    }

    if(!json[arrayN].is_array()){
        Logger::error(std::format("[DCClientConnectionSettings::init_from_json] Invalid json file.\n"));
        return;
    }

    for(const auto &devicesJson : json[arrayN]){
        
        auto isLocal = (read_value<std::string>(devicesJson, unreadCount, "type") == "local") ? true : false;
        if(devicesJson.contains("device"sv)){

            if(!isLocal){
                // local
                auto dcCRDS = std::make_unique<DCDeprecatedRemoteDeviceConnectionSettings>();
                dcCRDS->serverS.init_from_json(devicesJson["device"sv]);

                const auto &interfaces = (dcCRDS->serverS.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
                if(dcCRDS->serverS.idReadingInterface >= interfaces.size()){
                    Logger::error(std::format("[DCClientConnectionSettings::init_from_json] Invalid id reading interface.\n"));
                    return;
                }
                dcCRDS->serverS.readingAdress = interfaces[dcCRDS->serverS.idReadingInterface].ipAddress;

                if(dcCRDS->serverS.sendingAddress == "localhost"sv){
                    dcCRDS->serverS.isLocalhost = true;
                    dcCRDS->serverS.sendingAddress = interfaces[dcCRDS->serverS.idReadingInterface].ipAddress;
                }

                Logger::message(std::format("DC client remote device  settings read: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}] IPV:[{}].\n",
                        dcCRDS->serverS.idReadingInterface, dcCRDS->serverS.readingAdress, dcCRDS->serverS.readingPort,
                                            dcCRDS->serverS.sendingAddress, dcCRDS->serverS.sendingPort,
                        dcCRDS->serverS.protocol == Protocol::ipv6 ? "ipv6" : "ipv4"));
                
                connectionsS.push_back(std::move(dcCRDS));
            }else{
                // remote
                auto dcCLDS = std::make_unique<DCDeprecatedLocalDeviceConnectionSettings>();
                // ...
                
                connectionsS.push_back(std::move(dcCLDS));
            }
        }
    }

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[DCClientConnectionSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto DCDeprecatedClientConnectionSettings::convert_to_json() const -> nlohmann::json{

    json json;

    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // clients
    json::array_t arr;
    for(const auto &dev : connectionsS){

        nlohmann::json devJson;
        add_value(devJson, "type"sv, dev->isLocal ? "local"sv : "remote"sv);

        if(auto remote = dynamic_cast<DCDeprecatedRemoteDeviceConnectionSettings*>(dev.get())){
            add_value(devJson, "device"sv, remote->serverS.convert_to_json());
        }else if(auto local = dynamic_cast<DCDeprecatedLocalDeviceConnectionSettings*>(dev.get())){
            // ...
        }

        arr.push_back(devJson);
    }
    json["devices"sv] = arr;

    return json;
}


auto DCDeprecatedClientConnectionSettings::initialize(const std::string &filePath) -> bool{

    // retrieve interfaces
    ipv4Interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(ipv4Interfaces.size() == 0){
        Logger::warning("[DCClientConnectionSettings::reset_interfaces] Cannot find any ipv4 interface.\n"sv);
    }

    ipv6Interfaces = Interface::list_local_interfaces(Protocol::ipv6);
    if(ipv6Interfaces.size() == 0){
        Logger::warning("[DCClientConnectionSettings::reset_interfaces] Cannot find any ipv6 interface.\n"sv);
    }

    if(filePath.length() > 0){
        return load_from_file(this->filePath = filePath);
    }

    Logger::error("[DCClientConnectionSettings::initialize] Empty path.\n"sv);
    return false;
}


auto DCDeprecatedClientConnectionSettings::init_from_text(std::string_view &text) -> void{

    // read version
    io::Settings::init_from_text(text);

    // skip header
    text = String::advance_view_to_delim(text, "\n"sv);
    
    connectionsS.clear();

    for(const auto &line : String::split_view(text, "\n"sv)){

        if(auto values = String::split_view(line, " "sv); values.size() >= 5){
            
            if(values[0] != "local"sv){

                auto devS = std::make_unique<DCDeprecatedRemoteDeviceConnectionSettings>();
                devS->serverS.readingPort    = String::to_int(values[2]);

                // read protocol
                if(values.size() > 5){
                    if(values[5] == "ipv6"){
                        devS->serverS.protocol = Protocol::ipv6;
                    }else{
                        devS->serverS.protocol = Protocol::ipv4;
                    }
                }else{
                    devS->serverS.protocol = Protocol::ipv4;
                }

                const auto &interfaces = (devS->serverS.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
                size_t idReadingInterface = String::to_int(values[1]);
                if(idReadingInterface >= interfaces.size()){
                    Logger::error(std::format("[DCClientConnectionSettings::init_from_text] Invalid network config file format.\n"));
                    return;
                }
                devS->serverS.idReadingInterface = idReadingInterface;
                devS->serverS.readingAdress      = interfaces[idReadingInterface].ipAddress;

                bool localhost   = (values[3] == "localhost");
                if(localhost){
                    devS->serverS.sendingAddress = interfaces[idReadingInterface].ipAddress;
                    devS->serverS.isLocalhost = true;
                }else{
                    devS->serverS.sendingAddress = values[3];
                }
                devS->serverS.sendingPort = String::to_int(values[4]);

                Logger::message(std::format("DC client remote device  settings read: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}] IPV:[{}].\n",
                                            devS->serverS.idReadingInterface, devS->serverS.readingAdress, devS->serverS.readingPort, devS->serverS.sendingAddress, devS->serverS.sendingPort,
                    devS->serverS.protocol == Protocol::ipv6 ? "ipv6" : "ipv4"));
                
                connectionsS.push_back(std::move(devS));

            }else{
                auto devS = std::make_unique<DCDeprecatedLocalDeviceConnectionSettings>();
                
                connectionsS.push_back(std::move(devS));
            }

        }else{
            Logger::error("[DCClientConnectionSettings::init_from_text] Invalid line format.\n");
        }
    }
}



// auto UdpClientSettings::add_server(const net::ReadSendNetworkInfos &nServerInfo) -> void{

//     auto serverInfo = nServerInfo;
//     const auto &interfaces   = (serverInfo.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
//     if(serverInfo.idReadingInterface >= interfaces.size()){
//         Logger::error("UdpClientSettings::add_server: Invalid interface id.\n");
//         return;
//     }

//     serverInfo.local                = false;
//     serverInfo.idReadingInterface   = 0;
//     serverInfo.readingAdress        = interfaces[serverInfo.idReadingInterface].ipAddress;
//     serverInfo.readingPort          = 8888;
//     serverInfo.sendingAdress        = interfaces[serverInfo.idReadingInterface].ipAddress;
//     serverInfo.sendingPort          = 8889;
//     serversInfo.push_back(std::move(serverInfo));
// }

// auto UdpClientSettings::update_server(size_t idC, const net::ReadSendNetworkInfos &nServerInfo) -> void{

//     if(idC >= serversInfo.size()){
//         Logger::error("UdpClientSettings::update_server: Invalid server id.\n");
//         return;
//     }

//     serversInfo[idC] = nServerInfo;

//     auto &serverInfo = serversInfo[idC];
//     const auto &interfaces  = (serverInfo.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
//     if(serverInfo.idReadingInterface >= interfaces.size()){
//         Logger::error("UdpClientSettings::update_server: Invalid interface id.\n");
//         return;
//     }
//     serverInfo.readingAdress = interfaces[serverInfo.idReadingInterface].ipAddress;

//     if(serverInfo.sendingAdress == "localhost"){
//         serverInfo.sendingAdress = interfaces[serverInfo.idReadingInterface].ipAddress;
//         serverInfo.isLocalhost = true;
//     }
// }

