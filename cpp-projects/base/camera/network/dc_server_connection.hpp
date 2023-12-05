
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
#include <optional>
#include <deque>

// local
#include "dc_udp_sender.hpp"
#include "dc_udp_reader.hpp"

namespace tool::network {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class DCServerConnection{
public:

    ~DCServerConnection();
    auto initialize(size_t id, const ReadSendNetworkInfos &infos) -> bool;
    auto clean() -> void;

    auto init_connection_with_grabber() -> void;
    auto disconnect_grabber() -> void;
    auto quit_grabber() -> void;
    auto shutdown_grabber_computer() -> void;
    auto restart_grabber_computer() -> void;
    auto update_device_list() -> void;
    inline auto grabber_connected_to_server() const noexcept -> bool{return m_grabberConnectedToServer;}
    inline auto do_not_use_global_signals() -> void{m_useGlobalsSignals = false;}

    // signals
    // # global
    static inline SSS<size_t, std::int64_t> synchro_signal;
    static inline SSS<size_t, network::Feedback> feedback_signal;
    static inline SSS<size_t, std::shared_ptr<camera::DCCompressedFrame>> compressed_frame_signal;
    // # local
    SSS<std::int64_t> grabber_synchro_signal;
    SSS<network::Feedback> grabber_feedback_signal;
    SSS<std::shared_ptr<camera::DCCompressedFrame>> grabber_compressed_frame_signal;

    DCServerUdpSender udpSender;
    DCServerUdpReader udpReader;

private:

    auto receive_feedback(Header h, UdpMonoPacketMessage<Feedback> message) -> void;
    auto receive_compressed_frame(Header h, std::shared_ptr<camera::DCCompressedFrame> compressedFrame) -> void;

    std::int64_t m_initTs;
    std::atomic<size_t> m_totalReceivedBytes = 0;
    std::atomic_bool m_grabberConnectedToServer = false;
    std::atomic_bool m_useGlobalsSignals = true;
    size_t m_id = 0;
    ReadSendNetworkInfos m_infos;

    static constexpr std::uint16_t maxSizeUpdMessage = 9000;
};
}
