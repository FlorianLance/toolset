
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
#include <variant>

// local
#include "utility/thread.hpp"
#include "dc_udp_reader.hpp"
#include "dc_udp_sender.hpp"
#include "network/udp_client_network_settings.hpp"
#include "utility/safe_queue.hpp"

namespace tool::net {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class DCClientConnection{
public:

    auto init_connections() -> void;
    auto start_reading_thread(net::UdpClientNetworkSettings *networkS) -> bool;
    auto init_sender(net::UdpClientNetworkSettings *networkS) -> void;
    auto ping_server() -> void;
    auto disconnect_sender() -> void;
    auto update() -> void;
    auto clean() -> void;

    auto send_frame(std::shared_ptr<cam::DCCompressedFrame> frame) -> void;
    auto send_feedback(Feedback feedback) -> void;

    auto last_frame_id_sent() const -> size_t;
    auto last_frame_sent_timestamp_nanosecond() const -> std::chrono::nanoseconds {return m_lastFrameSentTS;}
    auto last_frame_sending_duration_micros_s() const -> std::int64_t{return lastFrameSendingDurationMicrosS;}

    auto simulate_sending_failure(bool enabled, int percentage) -> void;

    // signals
    static inline SSS<UdpNetworkSendingSettings> receive_init_network_sending_settings_signal;
    static inline SSS<cam::DCDelaySettings> receive_delay_signal;

    static inline SSS<std::shared_ptr<cam::DCDeviceSettings>> receive_device_settings_signal;
    static inline SSS<std::shared_ptr<cam::DCColorSettings>> receive_color_settings_signal;
    static inline SSS<std::shared_ptr<cam::DCFiltersSettings>> receive_filters_signal;    

    static inline SSS<> shutdown_signal;
    static inline SSS<> quit_signal;
    static inline SSS<> restart_signal;
    static inline SSS<> disconnect_signal;

    auto dummy_device_trigger() -> void;

private:

    std::unique_ptr<std::thread> sendMessagesT = nullptr;
    std::atomic_bool sendMessages = false;

    SafeQueue<std::variant<std::shared_ptr<cam::DCCompressedFrame>, Feedback>> messagesToSend;
    auto send_messages_loop() -> void;

    // reader
    DCClientUdpReader m_udpReaderG;
    // sender
    DCClientUdpSender m_udpSenderG;

    // lockers
    SpinLock m_readerL;

    // messages
    std::pair<Header, std::optional<UdpNetworkSendingSettings>> m_initNetworkInfosMessage =
        std::make_pair<Header, std::optional<UdpNetworkSendingSettings>>({}, std::nullopt);
    std::pair<Header, std::optional<cam::DCDelaySettings>> m_updateDelayMessage =
        std::make_pair<Header, std::optional<cam::DCDelaySettings>>({},std::nullopt);
    std::pair<Header, std::optional<Command>> m_commandMessage =
        std::make_pair<Header, std::optional<Command>>({},std::nullopt);

    std::pair<Header, std::shared_ptr<cam::DCDeviceSettings>> m_updateDeviceSettingsMessage =
        std::make_pair<Header, std::shared_ptr<cam::DCDeviceSettings>>({},nullptr);
    std::pair<Header, std::shared_ptr<cam::DCColorSettings>> m_updateColorSettingsMessage =
        std::make_pair<Header, std::shared_ptr<cam::DCColorSettings>>({},nullptr);
    std::pair<Header, std::shared_ptr<cam::DCFiltersSettings>> m_updateFiltersMessage =
        std::make_pair<Header, std::shared_ptr<cam::DCFiltersSettings>>({},nullptr);

    std::atomic<size_t> m_lastFrameIdSent = 0;
    std::atomic<std::int64_t> lastFrameSendingDurationMicrosS = 0;
    std::atomic<std::chrono::nanoseconds> m_lastFrameSentTS;
};

}
