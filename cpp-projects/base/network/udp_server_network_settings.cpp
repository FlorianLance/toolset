

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

using namespace tool;
using namespace tool::net;


UdpServerNetworkSettings::UdpServerNetworkSettings(){
    sType   = io::SettingsType::Server_network;
    version = io::Version::v1_0;
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
        return init_from_file(this->filePath = filePath);
    }

    Logger::error("UdpServerNetworkSettings::initialize: Empty path.\n");
    return false;
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
    }
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

            bool localhost   = (values[3] == "localhost");;
            if(localhost){
                info.sendingAdress = interfaces[idReadingInterface].ipAddress;
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

auto UdpServerNetworkSettings::write_to_text() const -> std::string{
    std::vector<std::string> lines;
    lines.reserve(clientsInfo.size() + 1);

    // write version
    lines.push_back(BaseSettings::write_to_text());

    // write header
    lines.push_back("local/remote | id_interface | reading_port | sending_address | sending_port | ipv4/ipv6");

    for(const auto &clientInfo : clientsInfo){
        lines.push_back(std::format("{} {} {} {} {} {}\n",
            clientInfo.local ? "local"sv : "remote"sv,
            clientInfo.idReadingInterface,
            clientInfo.readingPort,
            clientInfo.sendingAdress,
            clientInfo.sendingPort,
            clientInfo.protocol == Protocol::ipv6 ? "ipv6" : "ipv4"));
    }
    return String::join(lines);
}
