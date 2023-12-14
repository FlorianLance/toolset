

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

// std
#include <fstream>

// local
#include "utility/logger.hpp"
#include "utility/string.hpp"

using namespace tool;
using namespace tool::network;


UdpClientNetworkSettings::UdpClientNetworkSettings(){
    sType   = io::SettingsType::Client_network;
    version = io::Version::v1_0;
}

auto UdpClientNetworkSettings::initialize() -> bool{

    interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(interfaces.size() == 0){
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

    io::BaseSettings::init_from_text(text);

    auto lines = String::split_view(text, "\n"sv);
    if(lines.size() < 2){
        Logger::error("DCClientNetworkSettings::init_from_file: Invalid file format.\n");
        return;
    }

    // udp_id_reading_interface
    auto split = String::split_view(lines[0], " "sv);
    if(split.size() > 1){
        udpReadingInterfaceId = String::to_int(split[1]);
    }else{
        Logger::error("DCClientNetworkSettings::init_from_file: Invalid file format.\n");
        return;
    }

    // udp_reading_port
    split = String::split_view(lines[1], " "sv);
    if(split.size() > 1){
        udpReadingPort = String::to_int(split[1]);
    }else{
        Logger::error("DCClientNetworkSettings::init_from_file: Invalid file format.\n");
        return;
    }

    udpReadingInterface = interfaces[udpReadingInterfaceId];
}

auto UdpClientNetworkSettings::write_to_text() const -> std::string{
    return std::format(
        "{}udp_id_reading_interface {}\nudp_reading_port {}\n",
        BaseSettings::write_to_text(),
        udpReadingInterfaceId,
        udpReadingPort
    );
}


