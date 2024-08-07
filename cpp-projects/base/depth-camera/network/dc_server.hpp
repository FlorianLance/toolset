
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
#include "utility/ring_buffer.hpp"
#include "utility/safe_queue.hpp"
#include "network/udp_reader.hpp"
#include "network/udp_sender.hpp"
#include "network/settings/udp_server_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"
#include "depth-camera/frame/dc_compressed_frame.hpp"

namespace tool::net {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class DCServer{
public:

    DCServer();

    auto init_connections() -> void;
    auto start_reading_thread(net::UdpServerSettings *networkS) -> bool;
    auto init_sender(net::UdpServerSettings *networkS) -> void;
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
    // # connection
    SSS<UdpConnectionSettings> receive_init_server_client_connection_signal;
    // # settings received    
    SSS<cam::DCDelaySettings> receive_delay_settings_signal;
    SSS<std::shared_ptr<cam::DCDeviceSettings>> receive_device_settings_signal;
    SSS<std::shared_ptr<cam::DCColorSettings>> receive_color_settings_signal;
    SSS<std::shared_ptr<cam::DCFiltersSettings>> receive_filters_signal;
    // # command received
    SSS<> shutdown_signal;
    SSS<> quit_signal;
    SSS<> restart_signal;
    SSS<> disconnect_signal;
    // # other
    SSS<size_t> timeout_messages_signal;

    // auto dummy_device_trigger() -> void;

private:

    Protocol m_protocol = Protocol::unknow;


    std::unique_ptr<std::thread> sendMessagesT = nullptr;
    std::atomic_bool sendMessages = false;
    auto send_messages_loop() -> void;

    SafeQueue<std::variant<std::shared_ptr<cam::DCCompressedFrame>, Feedback>> messagesToSend;
    std::unordered_map<std::string, std::unique_ptr<UdpSender>> m_udpSenders;
    std::vector<std::byte> senderBuffer;

    UdpSender m_udpServerSender; // to be removed


    std::atomic<size_t> m_lastFrameIdSent = 0;
    std::atomic<std::int64_t> lastFrameSendingDurationMicrosS = 0;
    std::atomic<std::chrono::nanoseconds> m_lastFrameSentTS;

    // reception
    // # reader
    UdpReader m_udpServerReader;
    // # packets
    UdpMessageReception initServerClientConnectionReception;
    UdpMessageReception deviceSettingsReception;
    UdpMessageReception filtersSettingsReception;
    UdpMessageReception colorSettingsReception;
    // # messages
    SpinLock m_readerL;
    DoubleRingBuffer<std::byte> messagesBuffer;
    // processeed messages
    std::pair<EndPoint, std::optional<UdpConnectionSettings>> m_initServerClientConnectionMessage =
        std::make_pair<EndPoint, std::optional<UdpConnectionSettings>>({}, std::nullopt);
    std::pair<EndPoint, std::optional<cam::DCDelaySettings>> m_updateDelaySettingsMessage =
        std::make_pair<EndPoint, std::optional<cam::DCDelaySettings>>({},std::nullopt);
    std::pair<EndPoint, std::optional<Command>> m_commandMessage =
        std::make_pair<EndPoint, std::optional<Command>>({},std::nullopt);

    std::pair<EndPoint, std::shared_ptr<cam::DCDeviceSettings>> m_updateDeviceSettingsMessage =
        std::make_pair<EndPoint, std::shared_ptr<cam::DCDeviceSettings>>({},nullptr);
    std::pair<EndPoint, std::shared_ptr<cam::DCColorSettings>> m_updateColorSettingsMessage =
        std::make_pair<EndPoint, std::shared_ptr<cam::DCColorSettings>>({},nullptr);
    std::pair<EndPoint, std::shared_ptr<cam::DCFiltersSettings>> m_updateFiltersSettingsMessage =
        std::make_pair<EndPoint, std::shared_ptr<cam::DCFiltersSettings>>({},nullptr);
};

}
