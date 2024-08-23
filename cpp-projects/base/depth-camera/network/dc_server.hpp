
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
#include "thirdparty/sigslot/signal.hpp"
#include "network/settings/udp_connection_settings.hpp"
#include "depth-camera/settings/dc_server_settings.hpp"
#include "depth-camera/frame/dc_compressed_frame.hpp"

namespace tool::cam {

class DCServer{
public:

    DCServer();
    ~DCServer();

    auto initialize(const std::string &serverSettingsPath) -> bool;
    auto legacy_initialize(const std::string &legacyNetworkSettingsFilePath) -> bool;
    auto clean() -> void;
    auto update() -> void;

    // network
    auto ping_server() -> void;
    auto simulate_sending_failure(bool enabled, int percentage) -> void; // TODO

    // frames
    auto send_frame(std::shared_ptr<cam::DCCompressedFrame> frame) -> void;

    DCServerSettings settings;

    // signals
    // # connection
    // sigslot::signal<std::shared_ptr<net::UdpConnectionSettings>> receive_init_server_client_connection_signal;
    // # settings received
    sigslot::signal<std::shared_ptr<cam::DCDeviceSettings>> receive_device_settings_signal;
    sigslot::signal<std::shared_ptr<cam::DCColorSettings>> receive_color_settings_signal;
    sigslot::signal<std::shared_ptr<cam::DCFiltersSettings>> receive_filters_signal;
    sigslot::signal<cam::DCDelaySettings> receive_delay_settings_signal;
    // # command received
    sigslot::signal<> shutdown_signal;
    sigslot::signal<> quit_signal;
    sigslot::signal<> restart_signal;
    // # other
    sigslot::signal<size_t> timeout_messages_signal;

private:


    struct Impl;
    std::unique_ptr<Impl> i;
};

}
