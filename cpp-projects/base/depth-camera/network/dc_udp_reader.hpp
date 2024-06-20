

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

#include "network/udp_reader.hpp"
#include "network/network_types.hpp"

#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/dc_compressed_frame.hpp"

namespace tool::net {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class DCClientUdpReader : public UdpReader{
public:

    // signals
    SSS<Header, UdpNetworkSendingSettings> init_network_infos_signal;
    SSS<Header, cam::DCDelaySettings> update_delay_signal;
    SSS<Header, Command> command_signal;

    SSS<Header, std::shared_ptr<cam::DCDeviceSettings>> update_device_settings_signal;
    SSS<Header, std::shared_ptr<cam::DCColorSettings>> update_color_settings_signal;
    SSS<Header, std::shared_ptr<cam::DCFiltersSettings>> update_filters_signal;

protected:

    auto process_packet(std::span<const std::byte> packet) -> void override;

private:

    UdpMessageReception deviceReception;
    UdpMessageReception filtersReception;
    UdpMessageReception colorReception;
};


class DCServerUdpReader : public UdpReader{
public:

    // signals
    SSS<std::int64_t> synchro_signal;
    SSS<Header, Feedback> feedback_signal;
    SSS<Header, std::shared_ptr<cam::DCCompressedFrame>> compressed_frame_signal;

    SSS<UdpReceivedStatus> status_signal;

protected:

    auto process_packet(std::span<const std::byte> packet) -> void override;

private:

    AverageSynch synchro;
    Framerate framerate;
    AverageBandwidth bandwidth;
    AverageLatency latency;
    UdpMessageReception cFramesReception;
};


}
