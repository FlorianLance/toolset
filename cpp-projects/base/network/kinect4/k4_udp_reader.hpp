

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
#include "k4_network.hpp"
#include "camera/kinect4/k4_compressed_frame.hpp"
#include "camera/kinect4/k4_filters.hpp"

namespace tool::network {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class K4ClientUdpReader : public UdpReader{
public:

    // signals
    SSS<Header, std::shared_ptr<K4NetworkSendingSettings>> init_network_infos_signal;
    SSS<Header, std::shared_ptr<K4UdpDeviceSettings>> update_device_settings_signal;
    SSS<Header, std::shared_ptr<K4UdpColorSettings>> update_color_settings_signal;
    SSS<Header, std::shared_ptr<camera::K4Filters>> update_filters_signal;
    SSS<Header, K4UdpDelay> update_delay_signal;
    SSS<Header, K4Command> command_signal;

protected:

    auto process_packet(std::vector<char> *packet, size_t nbBytes) -> void override;

private:
    UdpMultiPacketsMessage filtersMessage;
};

struct Synchro{

    Synchro(){
        diffNs.resize(nbMaxValues);
        std::fill(std::begin(diffNs), std::end(diffNs), std::chrono::nanoseconds(0));
    }

    size_t currentId = 0;
    std::vector<std::chrono::nanoseconds> diffNs;
    std::int64_t averageDiffNs = 0;
    static constexpr size_t nbMaxValues = 1000;
};

class K4ServerUdpReader : public UdpReader{
public:

    // signals
    SSS<std::int64_t> synchro_signal;
    SSS<Header, K4UdpFeedback> feedback_signal;
    SSS<Header, std::shared_ptr<camera::K4CompressedFrame>> compressed_frame_signal;

protected:

    auto process_packet(std::vector<char> *packet, size_t nbBytes) -> void override;

private:

    UdpMultiPacketsMessage compressedFrameMessage;
    Synchro synchro;
};


}
