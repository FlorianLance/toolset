
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
#include "network_types.hpp"

namespace tool::net{

class UdpReader {

public:

    UdpReader();
    ~UdpReader();

    // socket
    auto init_socket(std::string readingAdress, int readingPort, Protocol protocol) -> bool;
    auto clean_socket() -> void;
    [[nodiscard]] auto is_connected() const noexcept -> bool;

    auto read_data() -> size_t;

    // reading thread
    auto start_reading_thread() -> void;
    auto stop_reading_thread() -> void;
    [[nodiscard]] auto is_reading_thread_started() const noexcept -> bool;

    // signals
    // # from main thread
    sigslot::signal<bool> connection_state_signal;
    // # from reading thread if enabled
    sigslot::signal<> timeout_packet_signal;
    sigslot::signal<> invalid_packet_size_signal;
    sigslot::signal<> invalid_checksum_signal;
    sigslot::signal<EndPoint, Header, std::span<const std::byte>> packed_received_signal;

private :

    auto read_data_thread() -> void;
    auto read_packet() -> size_t;

    static constexpr size_t packetSize = 9000;
    static constexpr size_t receiveBufferSize = packetSize * 50;
    static constexpr size_t timeoutMs = 500;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
