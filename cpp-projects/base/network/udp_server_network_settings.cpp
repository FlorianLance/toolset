

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
#include <filesystem>
#include <fstream>
#include <numeric>

// local
#include "utility/logger.hpp"
#include "utility/string.hpp"
#include "utility/format.hpp"

using namespace tool;
using namespace tool::network;

namespace fs = std::filesystem;

auto UdpServerNetworkSettings::initialize() -> bool{

    // retrieve interfaces
    interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(interfaces.size() == 0){
        Logger::error("Cannot find any ipv4 interface. Abort initialization.\n");
        return false;
    }

    return true;
}

auto UdpServerNetworkSettings::init_from_text(const std::string &text) -> void{

    clientsInfo.clear();

    for(const auto &line : String::split(text, '\n')){

        auto values = String::split(line, ' ');
        size_t idReadingInterface = std::stoi(values[0]);

        if(idReadingInterface >= interfaces.size()){
            Logger::error(fmt("Invalid network config file format.\n"));
            return;
        }

        ReadSendNetworkInfos info;
        info.idReadingInterface = idReadingInterface;
        info.readingAdress = interfaces[idReadingInterface].ipAddress;
        info.readingPort   = std::stoi(values[1]);

        bool localhost   = (values[2] == "localhost") || (values[2] == "127.0.0.1");
        if(localhost){
            info.sendingAdress = interfaces[idReadingInterface].ipAddress;
        }else{
            info.sendingAdress = values[2];
        }
        info.sendingPort = std::stoi(values[3]);

        Logger::message(fmt("Network infos read: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}].\n",
            info.idReadingInterface, info.readingAdress, info.readingPort, info.sendingAdress, info.sendingPort));

        clientsInfo.push_back(std::move(info));
    }
}

auto UdpServerNetworkSettings::convert_to_text() const -> std::string{
    std::vector<std::string> lines;
    for(const auto &grabber : clientsInfo){
        lines.push_back(fmt("{} {} {} {}\n",
            grabber.idReadingInterface,grabber.readingPort,
            grabber.sendingAdress,grabber.sendingPort));
    }
    return std::accumulate(lines.begin(), lines.end(), std::string{});
}
