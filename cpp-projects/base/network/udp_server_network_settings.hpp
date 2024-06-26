
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

#pragma once

// local
#include "io/settings.hpp"
#include "network/network_types.hpp"

namespace tool::net {

struct UdpServerNetworkSettings : io::BaseSettings{

    std::string filePath;
    std::vector<net::Interface> ipv4Interfaces = {};
    std::vector<net::Interface> ipv6Interfaces = {};
    std::vector<net::ReadSendNetworkInfos> clientsInfo;

    UdpServerNetworkSettings(){
        sType   = io::SettingsType::Server_network;
        version = io::SettingsVersion::LastVersion;
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    /**
     * @brief Populate ipv4 and ipv6 network interfaces
     */
    auto reset_interfaces() -> void;
    auto initialize(const std::string &filePath) -> bool;
    auto default_initialize() -> void;
    auto add_client(const net::ReadSendNetworkInfos &clientInfo) -> void;
    auto update_client(size_t idC, const net::ReadSendNetworkInfos &nClientInfo) -> void;

private:

    // legacy
    auto init_from_text(std::string_view &text) -> void override;
};
}
