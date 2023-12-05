
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

// std
#include <chrono>

// base
#include "network/network_types.hpp"
#include "files/text_settings.hpp"

namespace tool::network {

struct UdpClientNetworkSettings : files::TextSettings{

    auto initialize() -> bool;
    auto init_sending_settings(const UdpNetworkSendingSettings &sendingSettings) -> void;

    auto is_connected_to_manager() const noexcept -> bool {return m_connectedToManager;}
    auto disconnect_from_manager() -> void {m_connectedToManager = false;}

    // data
    size_t udpReadingInterfaceId = 0;
    int udpReadingPort = 8888;

    // local
    std::vector<network::Interface> interfaces = {};
    network::Interface udpReadingInterface;
    int udpSendingPort = 8889;
    std::string udpSendingAdress;
    size_t lastFrameIdSent = 0;
    std::chrono::nanoseconds lastFrameSentTS;

private:

    auto init_from_text(const std::string &text) -> void override;
    auto convert_to_text() const -> std::string override;
    auto type() const noexcept  -> std::int32_t override {return static_cast<std::int32_t>(tool::files::FileSettingsType::Network);};

    // local
    bool m_connectedToManager = false;
};
}
