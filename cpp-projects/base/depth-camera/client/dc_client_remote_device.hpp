
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
#include "depth-camera/frame/dc_compressed_frame.hpp"
#include "dc_client_device.hpp"

namespace tool::cam {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class DCClientRemoteDevice : public DCClientDevice{
public:

    DCClientRemoteDevice();
    ~DCClientRemoteDevice() override;
    
    auto initialize(DCDeviceConnectionSettings *deviceS) -> bool override;
    auto init_remote_connection() -> void;
    auto clean() -> void override;

    auto apply_command(net::Command command) -> void override;
    auto update_device_settings(const cam::DCDeviceSettings &deviceS) -> void override;
    auto update_color_settings(const cam::DCColorSettings &colorS) -> void override;
    auto update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void override;
    auto update_delay_settings(const cam::DCDelaySettings &delayS) -> void override;

    constexpr auto type() const noexcept -> DCClientType override {return DCClientType::remote;}
    auto device_connected() const noexcept -> bool override;

    // when no reading thread started
    auto read_data_from_network() -> size_t;

    // signals
    SSS<std::int64_t> remote_synchro_signal;
    SSS<net::Feedback> remote_feedback_signal;
    SSS<std::shared_ptr<cam::DCCompressedFrame>> remote_frame_signal;
    SSS<net::UdpNetworkStatus> remote_network_status_signal;
    // # other
    SSS<size_t> timeout_messages_signal;

private:

    auto process_received_packet(net::EndPoint endpoint, net::Header header, std::span<const std::byte> dataToProcess) -> void;
    auto receive_feedback(net::Header h, net::Feedback message) -> void;
    auto receive_compressed_frame(net::Header h, std::shared_ptr<cam::DCCompressedFrame> compressedFrame) -> void;
    auto receive_network_status(net::UdpNetworkStatus status) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};

}