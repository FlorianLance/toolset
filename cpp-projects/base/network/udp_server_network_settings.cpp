

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
#include <numeric>
#include <format>

// local
#include "utility/logger.hpp"
#include "utility/string.hpp"

using namespace tool;
using namespace tool::network;


UdpServerNetworkSettings::UdpServerNetworkSettings(){
    sType   = io::SettingsType::Server_network;
    version = io::Version::v1_0;
}

auto UdpServerNetworkSettings::initialize() -> bool{

    // retrieve interfaces
    interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(interfaces.size() == 0){
        Logger::error("Cannot find any ipv4 interface. Abort initialization.\n");
        return false;
    }

    return true;
}

auto UdpServerNetworkSettings::init_from_text(std::string_view &text) -> void{

    io::BaseSettings::init_from_text(text);

    clientsInfo.clear();

    for(const auto &line : String::split_view(text, "\n"sv)){

        if(auto values = String::split_view(line, " "sv); values.size() >= 5){

            ReadSendNetworkInfos info;
            info.local          = values[0] == "local"sv;
            info.readingPort    = String::to_int(values[2]);

            size_t idReadingInterface = String::to_int(values[1]);
            if(idReadingInterface >= interfaces.size()){
                Logger::error(std::format("Invalid network config file format.\n"));
                return;
            }
            info.idReadingInterface = idReadingInterface;
            info.readingAdress      = interfaces[idReadingInterface].ipAddress;

            bool localhost   = (values[3] == "localhost") || (values[3] == "127.0.0.1");
            if(localhost){
                info.sendingAdress = interfaces[idReadingInterface].ipAddress;
            }else{
                info.sendingAdress = values[3];
            }
            info.sendingPort = String::to_int(values[4]);

            Logger::message(std::format("Network infos read: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}].\n",
                info.idReadingInterface, info.readingAdress, info.readingPort, info.sendingAdress, info.sendingPort));

            clientsInfo.push_back(std::move(info));
        }
    }
}

auto UdpServerNetworkSettings::write_to_text() const -> std::string{
    std::vector<std::string> lines;
    lines.reserve(clientsInfo.size() + 1);

    lines.push_back(BaseSettings::write_to_text());
    for(const auto &grabber : clientsInfo){
        lines.push_back(std::format("{} {} {} {} {}\n",
            grabber.local ? "local"sv : "remote"sv,
            grabber.idReadingInterface,
            grabber.readingPort,
            grabber.sendingAdress,
            grabber.sendingPort));
    }
    return String::join(lines);
}
