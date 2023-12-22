
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
#include "dc_server_device.hpp"
#include "network/udp_header.hpp"
#include "camera/dc_compressed_frame.hpp"
#include "thirdparty/sigslot/signal.hpp"

namespace tool::network {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class DCServerRemoteDevice : public DCServerDevice{
public:

    DCServerRemoteDevice();
    ~DCServerRemoteDevice() override;

    auto initialize(const ReadSendNetworkInfos &infos) -> bool override;
    auto init_remote_connection() -> void;
    auto clean() -> void override;

    auto apply_command(Command command) -> void override;
    auto update_device_settings(const camera::DCDeviceSettings &deviceS) -> void override;
    auto update_color_settings(const camera::DCColorSettings &colorS) -> void override;
    auto update_filters_settings(const camera::DCFiltersSettings &filtersS) -> void override;
    auto update_delay_settings(const camera::DCDelaySettings &delayS) -> void override;

    constexpr auto type() const noexcept -> DCServerType override {return DCServerType::remote;}
    auto device_connected() const noexcept -> bool override;

    // signals
    SSS<std::int64_t> remote_synchro_signal;
    SSS<network::Feedback> remote_feedback_signal;
    SSS<std::shared_ptr<camera::DCCompressedFrame>> remote_frame_signal;

private:

    auto receive_feedback(Header h, UdpMonoPacketMessage<Feedback> message) -> void;
    auto receive_compressed_frame(Header h, std::shared_ptr<camera::DCCompressedFrame> compressedFrame) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};

}
