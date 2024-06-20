

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

#include "udp_server_network_settings.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"
#include "utility/string.hpp"
#include "data/json_utility.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::data;
using json = nlohmann::json;


auto UdpServerNetworkSettings::init_from_json(const nlohmann::json &json) -> void{

    clientsInfo.clear();

    size_t unreadCount = 0;
    // base
    io::BaseSettings::init_from_json(read_object(json, unreadCount, "base"sv));

    if(json.contains("clients"sv)){

        if(!json["clients"sv].is_array()){
            Logger::error(std::format("UdpServerNetworkSettings::init_from_json: Invalid json file.\n"));
            return;
        }

        for(const auto &clientJson : json["clients"sv]){

            ReadSendNetworkInfos info;
            info.local               = (read_value<std::string>(clientJson, unreadCount, "type") == "local") ? true : false;
            info.idReadingInterface  = read_value<size_t>(clientJson, unreadCount, "id_reading_interface");
            info.readingPort         = read_value<int>(clientJson, unreadCount, "reading_port");
            info.sendingAdress       = read_value<std::string>(clientJson, unreadCount, "sending_address");
            info.sendingPort         = read_value<int>(clientJson, unreadCount, "sending_port");
            info.protocol            = (read_value<std::string>(clientJson, unreadCount, "protocol") == "ipv6") ? Protocol::ipv6 : Protocol::ipv4;

            const auto &interfaces = (info.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
            if(info.idReadingInterface >= interfaces.size()){
                Logger::error(std::format("UdpServerNetworkSettings::init_from_json: Invalid id reading interface.\n"));
                return;
            }
            info.readingAdress = interfaces[info.idReadingInterface].ipAddress;

            if(info.sendingAdress == "localhost"sv){
                info.isLocalhost = true;
                info.sendingAdress = interfaces[info.idReadingInterface].ipAddress;
            }

            Logger::message(std::format("Network infos read: M:[{}] RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}] IPV:[{}].\n",
                info.local ? "local"sv : "remote"sv, info.idReadingInterface, info.readingAdress, info.readingPort, info.sendingAdress, info.sendingPort,
                info.protocol == Protocol::ipv6 ? "ipv6" : "ipv4"));

            clientsInfo.push_back(std::move(info));
        }
    }

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[UdpServerNetworkSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}


auto UdpServerNetworkSettings::convert_to_json() const -> nlohmann::json{

    json json;

    // base
    add_value(json, "base"sv, io::BaseSettings::convert_to_json());
    // clients
    json::array_t arr;
    for(const auto &info : clientsInfo){
        nlohmann::json clientJson;
        add_value(clientJson, "type"sv,                     info.local ? "local"sv : "remote"sv);
        add_value(clientJson, "id_reading_interface"sv,     info.idReadingInterface);
        add_value(clientJson, "reading_port"sv,             info.readingPort);
        add_value(clientJson, "sending_address"sv,          info.isLocalhost ? "localhost"sv : info.sendingAdress);
        add_value(clientJson, "sending_port"sv,             info.sendingPort);
        add_value(clientJson, "protocol"sv,                 (info.protocol == Protocol::ipv6) ? "ipv6"sv : "ipv4"sv);
        arr.push_back(clientJson);
    }
    json["clients"sv] = arr;

    return json;
}



auto UdpServerNetworkSettings::init_from_text(std::string_view &text) -> void{

    // read version
    io::BaseSettings::init_from_text(text);

    // skip header
    text = String::advance_view_to_delim(text, "\n"sv);

    clientsInfo.clear();

    for(const auto &line : String::split_view(text, "\n"sv)){

        if(auto values = String::split_view(line, " "sv); values.size() >= 5){

            ReadSendNetworkInfos info;
            info.local          = values[0] == "local"sv;
            info.readingPort    = String::to_int(values[2]);

            // read protocol
            if(values.size() > 5){
                if(values[5] == "ipv6"){
                    info.protocol = Protocol::ipv6;
                }else{
                    info.protocol = Protocol::ipv4;
                }
            }else{
                info.protocol = Protocol::ipv4;
            }

            const auto &interfaces = (info.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
            size_t idReadingInterface = String::to_int(values[1]);
            if(idReadingInterface >= interfaces.size()){
                Logger::error(std::format("UdpServerNetworkSettings::init_from_text: Invalid network config file format.\n"));
                return;
            }
            info.idReadingInterface = idReadingInterface;
            info.readingAdress      = interfaces[idReadingInterface].ipAddress;

            bool localhost   = (values[3] == "localhost");
            if(localhost){
                info.sendingAdress = interfaces[idReadingInterface].ipAddress;
                info.isLocalhost = true;
            }else{
                info.sendingAdress = values[3];
            }
            info.sendingPort = String::to_int(values[4]);

            Logger::message(std::format("Network infos read: M:[{}] RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}] IPV:[{}].\n",
                                        info.local ? "local"sv : "remote"sv, info.idReadingInterface, info.readingAdress, info.readingPort, info.sendingAdress, info.sendingPort,
                                        info.protocol == Protocol::ipv6 ? "ipv6" : "ipv4"));

            clientsInfo.push_back(std::move(info));
        }else{
            Logger::error("UdpServerNetworkSettings::init_from_text: Invalid line format.\n");
        }
    }
}


auto UdpServerNetworkSettings::reset_interfaces() -> void{

    // retrieve interfaces
    ipv4Interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(ipv4Interfaces.size() == 0){
        Logger::warning("UdpServerNetworkSettings::reset_interfaces: Cannot find any ipv4 interface.\n");
    }

    ipv6Interfaces = Interface::list_local_interfaces(Protocol::ipv6);
    if(ipv6Interfaces.size() == 0){
        Logger::warning("UdpServerNetworkSettings::reset_interfaces: Cannot find any ipv6 interface.\n");
    }
}

auto UdpServerNetworkSettings::initialize(const std::string &filePath) -> bool{

    reset_interfaces();

    if(filePath.length() > 0){
        return load_from_file(this->filePath = filePath);
    }

    Logger::error("UdpServerNetworkSettings::initialize: Empty path.\n");
    return false;
}

auto UdpServerNetworkSettings::default_initialize() -> void{
    reset_interfaces();
    clientsInfo.resize(1);
}

auto UdpServerNetworkSettings::add_client(const net::ReadSendNetworkInfos &nClientInfo) -> void{

    auto clientInfo = nClientInfo;
    const auto &interfaces   = (clientInfo.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
    if(clientInfo.idReadingInterface >= interfaces.size()){
        Logger::error("UdpServerNetworkSettings::add_client: Invalid interface id.\n");
        return;
    }

    clientInfo.local                = false;
    clientInfo.idReadingInterface   = 0;
    clientInfo.readingAdress        = interfaces[clientInfo.idReadingInterface].ipAddress;
    clientInfo.readingPort          = 8888;
    clientInfo.sendingAdress        = interfaces[clientInfo.idReadingInterface].ipAddress;
    clientInfo.sendingPort          = 8889;
    clientsInfo.push_back(std::move(clientInfo));
}

auto UdpServerNetworkSettings::update_client(size_t idC, const net::ReadSendNetworkInfos &nClientInfo) -> void{

    if(idC >= clientsInfo.size()){
        Logger::error("UdpServerNetworkSettings::update_client: Invalid client id.\n");
        return;
    }

    clientsInfo[idC] = nClientInfo;

    auto &clientInfo        = clientsInfo[idC];
    const auto &interfaces   = (clientInfo.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
    if(clientInfo.idReadingInterface >= interfaces.size()){
        Logger::error("UdpServerNetworkSettings::update_client: Invalid interface id.\n");
        return;
    }
    clientInfo.readingAdress = interfaces[clientInfo.idReadingInterface].ipAddress;

    if(clientInfo.sendingAdress == "localhost"){
        clientInfo.sendingAdress = interfaces[clientInfo.idReadingInterface].ipAddress;
        clientInfo.isLocalhost = true;
    }
}
