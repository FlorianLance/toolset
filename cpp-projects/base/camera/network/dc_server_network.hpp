

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
#include "network/udp_server_network_settings.hpp"
#include "camera/settings/dc_device_settings.hpp"
#include "camera/settings/dc_color_settings.hpp"
#include "camera/settings/dc_filters_settings.hpp"
#include "camera/settings/dc_delay_settings.hpp"
#include "camera/dc_compressed_frame.hpp"
#include "thirdparty/sigslot/signal.hpp"

namespace tool::network {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

struct DCServerNetwork{

    DCServerNetwork();
    ~DCServerNetwork();
    auto initialize(const std::vector<network::ReadSendNetworkInfos> &clientsInfo) -> void;
    auto clean() -> void;
    // auto reset_device(size_t idD, const network::ReadSendNetworkInfos &clientInfo) -> void;

    auto init_connection(size_t idG) -> void;
    auto apply_command(size_t idG, Command command) -> void;
    auto update_device_settings(size_t idG, const camera::DCDeviceSettings &deviceS) -> void;
    auto update_color_settings(size_t idG, const camera::DCColorSettings &colorS) -> void;
    auto update_filters_settings(size_t idG, const camera::DCFiltersSettings &filtersS) -> void;
    auto update_delay_settings(size_t idG, const camera::DCDelaySettings &delayS) -> void;

    auto devices_nb() const noexcept -> size_t;
    auto device_connected(size_t idG) const noexcept -> bool;

    // signals
    SSS<size_t, std::int64_t> remote_synchro_signal;
    SSS<size_t, network::Feedback> remote_feedback_signal;
    SSS<size_t, std::shared_ptr<camera::DCFrame>> local_frame_signal;
    SSS<size_t, std::shared_ptr<camera::DCCompressedFrame>> remote_frame_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};

}
