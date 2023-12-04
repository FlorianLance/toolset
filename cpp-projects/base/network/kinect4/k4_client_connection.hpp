
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
#include "k4_udp_reader.hpp"
#include "dc_udp_sender.hpp"
#include "k4_client_network_settings.hpp"


template<class T>
class Queue{

//    std::condition_variable consumer_, producer_;
    std::mutex mutex_;
    using unique_lock = std::unique_lock<std::mutex>;

    std::queue<T> queue_;

public:
    template<class U>
    void push_back(U&& item) {

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

    std::optional<T> pop_front() {
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

    void clean(){
        std::unique_lock lock(mutex_);
        queue_ = {};
    }
};

namespace tool::network {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;


class K4ClientConnection{
public:

    auto init_connections() -> void;
    auto start_reading(network::K4ClientNetworkSettings *networkS) -> bool;
    auto init_sender(network::K4ClientNetworkSettings *networkS) -> bool;
    auto ping_server() -> void;
    auto disconnect_sender() -> void;
    auto update() -> void;
    auto clean() -> void;

    auto send_frame(std::shared_ptr<camera::DCCompressedFrame> frame) -> void;
    auto send_feedback(K4Feedback feedback) -> void;

    auto last_frame_id_sent() const -> size_t;
    auto last_frame_sent_timestamp_nanosecond() const -> std::chrono::nanoseconds {return m_lastFrameSentTS;}
    auto last_frame_sending_duration_micros_s() const -> std::int64_t{return lastFrameSendingDurationMicrosS;}

    // signals
    static inline SSS<std::shared_ptr<network::K4NetworkSendingSettings>> receive_init_network_sending_settings_signal;
    static inline SSS<std::shared_ptr<network::K4UdpDeviceSettings>> receive_device_settings_signal;
    static inline SSS<std::shared_ptr<network::DCUdpColorSettings>>receive_color_settings_signal;    
    static inline SSS<std::shared_ptr<camera::DCFilters>> receive_filters_signal;    
    static inline SSS<network::DCUdpDelay> receive_delay_signal;

    static inline SSS<> shutdown_signal;
    static inline SSS<> quit_signal;
    static inline SSS<> restart_signal;
    static inline SSS<> disconnect_signal;
    static inline SSS<> update_device_list_signal;

    auto dummy_device_trigger() -> void;

private:

    std::unique_ptr<std::thread> sendMessagesT = nullptr;
    std::atomic_bool sendMessages = false;

    Queue<std::variant<std::shared_ptr<camera::DCCompressedFrame>, K4Feedback>> messagesToSend;
    auto send_messages_loop() -> void;

    // reader
    DCClientUdpReader m_udpReaderG;
    // sender
    DCClientUdpSender m_udpSenderG;

    // lockers
    SpinLock m_readerL;

    // messages
    std::pair<Header, std::shared_ptr<K4NetworkSendingSettings>> m_initNetworkInfosMessage      = std::make_pair<Header, std::shared_ptr<K4NetworkSendingSettings>>({},nullptr);
    std::pair<Header, std::shared_ptr<K4UdpDeviceSettings>> m_updateDeviceSettingsMessage       = std::make_pair<Header, std::shared_ptr<K4UdpDeviceSettings>>({},nullptr);
    std::pair<Header, std::shared_ptr<DCUdpColorSettings>> m_updateColorSettingsMessage         = std::make_pair<Header, std::shared_ptr<DCUdpColorSettings>>({},nullptr);
    std::pair<Header, std::optional<K4Command>> m_commandMessage                                = std::make_pair<Header, std::optional<K4Command>>({},std::nullopt);
    std::pair<Header, std::shared_ptr<camera::DCFilters>> m_updateFiltersMessage                = std::make_pair<Header, std::shared_ptr<camera::DCFilters>>({},nullptr);    
    std::pair<Header, std::optional<DCUdpDelay>> m_updateDelayMessage                           = std::make_pair<Header, std::optional<DCUdpDelay>>({},std::nullopt);

//    std::int64_t m_diffTimestampFromManager = 0;
    std::atomic<size_t> m_lastFrameIdSent = 0;
    std::atomic<std::int64_t> lastFrameSendingDurationMicrosS = 0;
    std::atomic<std::chrono::nanoseconds> m_lastFrameSentTS;
};

}
