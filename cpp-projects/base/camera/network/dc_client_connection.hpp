
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
#include <queue>
#include <variant>

// local
#include "utility/thread.hpp"
#include "dc_udp_reader.hpp"
#include "dc_udp_sender.hpp"
#include "network/udp_client_network_settings.hpp"


template<class T>
class Queue{

//    std::condition_variable consumer_, producer_;
    std::mutex mutex_;
    using unique_lock = std::unique_lock<std::mutex>;
    std::queue<T> queue_;

public:
    template<class U>
    auto push_back(U&& item) -> void {

        std::unique_lock lock(mutex_);
//        while(MaxQueueSize == queue_.size()){
//            pop_front();
////            producer_.wait(lock);
//        }
        queue_.push(std::forward<U>(item));
        if(queue_.size() > 20){
            queue_.pop();
        }
//        consumer_.notify_one();
    }

    auto pop_front() -> std::optional<T> {
        std::unique_lock lock(mutex_);
//        if(!mutex_.try_lock()){
//            return std::nullopt;
//        }
        if(queue_.empty()){
            return std::nullopt;
        }
//        while(queue_.empty()){
//            consumer_.wait(lock);
//        }
//        auto full = MaxQueueSize == queue_.size();
        auto item = queue_.front();
        queue_.pop();
//        if(full){
//            producer_.notify_all();
//        }
        return item;
    }

    auto clean() -> void{
        std::unique_lock lock(mutex_);
        queue_ = {};
    }
};

namespace tool::network {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class DCClientConnection{
public:

    auto init_connections() -> void;
    auto start_reading(network::UdpClientNetworkSettings *networkS) -> bool;
    auto init_sender(network::UdpClientNetworkSettings *networkS) -> bool;
    auto ping_server() -> void;
    auto disconnect_sender() -> void;
    auto update() -> void;
    auto clean() -> void;

    auto send_frame(std::shared_ptr<camera::DCCompressedFrame> frame) -> void;
    auto send_feedback(Feedback feedback) -> void;

    auto last_frame_id_sent() const -> size_t;
    auto last_frame_sent_timestamp_nanosecond() const -> std::chrono::nanoseconds {return m_lastFrameSentTS;}
    auto last_frame_sending_duration_micros_s() const -> std::int64_t{return lastFrameSendingDurationMicrosS;}

    // signals
    static inline SSS<std::shared_ptr<network::UdpNetworkSendingSettings>> receive_init_network_sending_settings_signal;
    static inline SSS<std::shared_ptr<UdpMonoPacketMessage<camera::DCDeviceSettings>>> receive_device_settings_signal;
    static inline SSS<std::shared_ptr<UdpMonoPacketMessage<camera::DCColorSettings>>> receive_color_settings_signal;
    static inline SSS<std::shared_ptr<camera::DCFiltersSettings>> receive_filters_signal;    
    static inline SSS<UdpMonoPacketMessage<camera::DCDelaySettings>> receive_delay_signal;

    static inline SSS<> shutdown_signal;
    static inline SSS<> quit_signal;
    static inline SSS<> restart_signal;
    static inline SSS<> disconnect_signal;
    // static inline SSS<> update_device_list_signal;

    auto dummy_device_trigger() -> void;

private:

    std::unique_ptr<std::thread> sendMessagesT = nullptr;
    std::atomic_bool sendMessages = false;

    Queue<std::variant<std::shared_ptr<camera::DCCompressedFrame>, Feedback>> messagesToSend;
    auto send_messages_loop() -> void;

    // reader
    DCClientUdpReader m_udpReaderG;
    // sender
    DCClientUdpSender m_udpSenderG;

    // lockers
    SpinLock m_readerL;

    // messages
    std::pair<Header, std::shared_ptr<UdpNetworkSendingSettings>> m_initNetworkInfosMessage =
        std::make_pair<Header, std::shared_ptr<UdpNetworkSendingSettings>>({},nullptr);
    std::pair<Header, std::shared_ptr<UdpMonoPacketMessage<camera::DCDeviceSettings>>> m_updateDeviceSettingsMessage =
        std::make_pair<Header, std::shared_ptr<UdpMonoPacketMessage<camera::DCDeviceSettings>>>({},nullptr);
    std::pair<Header, std::shared_ptr<UdpMonoPacketMessage<camera::DCColorSettings>>> m_updateColorSettingsMessage =
        std::make_pair<Header, std::shared_ptr<UdpMonoPacketMessage<camera::DCColorSettings>>>({},nullptr);
    std::pair<Header, std::optional<Command>> m_commandMessage =
        std::make_pair<Header, std::optional<Command>>({},std::nullopt);
    std::pair<Header, std::shared_ptr<camera::DCFiltersSettings>> m_updateFiltersMessage =
        std::make_pair<Header, std::shared_ptr<camera::DCFiltersSettings>>({},nullptr);
    std::pair<Header, std::optional<UdpMonoPacketMessage<camera::DCDelaySettings>>> m_updateDelayMessage =
        std::make_pair<Header, std::optional<UdpMonoPacketMessage<camera::DCDelaySettings>>>({},std::nullopt);

//    std::int64_t m_diffTimestampFromManager = 0;
    std::atomic<size_t> m_lastFrameIdSent = 0;
    std::atomic<std::int64_t> lastFrameSendingDurationMicrosS = 0;
    std::atomic<std::chrono::nanoseconds> m_lastFrameSentTS;
};

}
