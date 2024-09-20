
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
#include "network/settings/remote_server_settings.hpp"
#include "network/network_types.hpp"
#include "depth-camera/dc_enums.hpp"

namespace tool::cam {


struct DCDeviceConnectionSettings : io::Settings{

    DCDeviceConnectionSettings(){
        sType   = io::SettingsType::Dc_client_connection;
        version = io::SettingsVersion::LastVersion;
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;

    // all
    DCClientType connectionType = DCClientType::Remote;

    // remote
    size_t idReadingInterface = 0;
    bool anyReadingInterface = false;
    int readingPort = 8889;
    std::string sendingAddress = "localhost";
    int sendingPort = 8888;
    net::Protocol protocol = net::Protocol::ipv4;

    // # runtime
    std::string readingAddress;
    std::string processedSendingAddress;
    bool startReadingThread = true;
    // bool isLocalhost = false;
};



// DEPRECATED


struct DCDeprecatedDeviceConnectionSettings{
    bool isLocal = false;
    virtual ~DCDeprecatedDeviceConnectionSettings(){}
};
struct DCDeprecatedLocalDeviceConnectionSettings : public DCDeprecatedDeviceConnectionSettings{
    DCDeprecatedLocalDeviceConnectionSettings(){
        DCDeprecatedDeviceConnectionSettings::isLocal = true;
    }
};
struct DCDeprecatedRemoteDeviceConnectionSettings : public DCDeprecatedDeviceConnectionSettings{

    DCDeprecatedRemoteDeviceConnectionSettings(){
        DCDeprecatedDeviceConnectionSettings::isLocal = false;
    }
    net::RemoteServerSettings serverS;
};

struct DCDeprecatedClientConnectionSettings : io::Settings{

    std::vector<std::unique_ptr<DCDeprecatedDeviceConnectionSettings>> connectionsS;

    // runtime
    std::string filePath;
    std::vector<net::Interface> ipv4Interfaces = {};
    std::vector<net::Interface> ipv6Interfaces = {};

    DCDeprecatedClientConnectionSettings(){
        sType   = io::SettingsType::Dc_client_connection;
        version = io::SettingsVersion::LastVersion;
    }

    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;
    auto initialize(const std::string &filePath) -> bool;

private:

    // legacy
    auto init_from_text(std::string_view &text) -> void override;
};

}
