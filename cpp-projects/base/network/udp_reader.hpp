
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
#include "utility/ring_buffer.hpp"
#include "thirdparty/sigslot/signal.hpp"
#include "network_enums.hpp"

namespace tool::net{

class UdpReader {

public:

    UdpReader();
    ~UdpReader();

    // slots
    auto init_connection(std::string readingAdress, int readingPort, Protocol protocol) -> bool;
    auto clean_connection() -> void;
    auto is_connected() const noexcept -> bool;

    // reading thread
    auto start_reading_thread() -> void;
    auto stop_reading_thread() -> void;
    auto is_reading_thread_started() const noexcept -> bool;

    // packets
    auto read_packet() -> size_t;

    // signals
    sigslot::signal<bool> connection_state_signal;
    sigslot::signal<> timeout_packet_signal;
    sigslot::signal<size_t> timeout_messages_signal;

protected:

    virtual auto process_packet(std::span<const std::byte> packet) -> void;
    
    DoubleRingBuffer<std::byte> messagesBuffer;

private :

    auto read_data_thread() -> void;

    static constexpr size_t packetSize = 9000;
    static constexpr size_t receiveBufferSize = packetSize * 50;
    static constexpr size_t timeoutMs = 500;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
