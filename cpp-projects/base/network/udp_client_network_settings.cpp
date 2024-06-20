

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

#include "udp_client_network_settings.hpp"

// local
#include "utility/logger.hpp"
#include "data/json_utility.hpp"
#include "utility/string.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::data;
using json = nlohmann::json;

auto UdpClientNetworkSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::BaseSettings::init_from_json(read_object(json, unreadCount, "base"sv));
    // network
    udpReadingInterfaceId   = read_value<size_t>(json, unreadCount, "id_interface");
    udpReadingPort          = read_value<int>(json, unreadCount, "reading_port");
    protocol                = (read_value<std::string>(json, unreadCount, "protocol") == "ipv6") ? Protocol::ipv6 : Protocol::ipv4;

    const auto &interfaces = (protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
    udpReadingInterface = interfaces[udpReadingInterfaceId];

    if(unreadCount != 0){
        tool::Logger::warning(std::format("[UdpClientNetworkSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }
}

auto UdpClientNetworkSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, io::BaseSettings::convert_to_json());
    // network
    add_value(json, "id_interface"sv,   udpReadingInterfaceId);
    add_value(json, "reading_port"sv,   udpReadingPort);
    add_value(json, "protocol"sv,       (protocol == Protocol::ipv6) ? "ipv6" : "ipv4");

    return json;
}

auto UdpClientNetworkSettings::initialize() -> bool{

    // retrieve interfaces
    ipv4Interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(ipv4Interfaces.size() == 0){
        Logger::warning("Cannot find any ipv4 interface.\n");
    }

    ipv6Interfaces = Interface::list_local_interfaces(Protocol::ipv6);
    if(ipv6Interfaces.size() == 0){
        Logger::warning("Cannot find any ipv6 interface.\n");
    }

    if(ipv4Interfaces.empty() && ipv6Interfaces.empty()){
        Logger::error("Cannot find any ipv4/ipv6 interface. Abort initialization.\n");
        return false;        
    }

    return true;
}

auto UdpClientNetworkSettings::init_sending_settings(const UdpNetworkSendingSettings &sendingSettings) -> void {
    udpSendingAdress = std::string(sendingSettings.ipAdress.begin(), sendingSettings.ipAdress.end());
    String::remove_after_right(udpSendingAdress, ' ');
    udpSendingPort  = sendingSettings.port;
    m_connectedToManager = true;
}


auto UdpClientNetworkSettings::init_from_text(std::string_view &text) -> void{

    Logger::message("UdpClientNetworkSettings::init_from_text\n");

    // read version
    io::BaseSettings::init_from_text(text);

    // skip header
    auto line = String::advance_view_to_delim(text, "\n"sv);

    Logger::message(line);

    if(auto values = String::split_view(line, " "sv); values.size() >= 2){

        // read protocol
        if(values.size() > 2){
            if(values[2] == "ipv6"){
                protocol = Protocol::ipv6;
            }else{
                protocol = Protocol::ipv4;
            }
        }else{
            protocol = Protocol::ipv4;
        }
        udpReadingInterfaceId = String::to_int(values[0]);
        udpReadingPort        = String::to_int(values[1]);

        const auto &interfaces = (protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
        udpReadingInterface = interfaces[udpReadingInterfaceId];

    }else{
        Logger::error("DCClientNetworkSettings::init_from_file: Invalid file format.\n");
        return;
    }
}




