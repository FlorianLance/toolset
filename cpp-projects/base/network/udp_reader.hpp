
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
    auto init_socket(std::string readingAdress, int readingPort, Protocol protocol, size_t maxPacketSize) -> bool;
    auto clean_socket() -> void;
    [[nodiscard]] auto is_connected() const noexcept -> bool;

    // reading thread
    auto start_threads() -> void;
    auto stop_threads() -> void;
    [[nodiscard]] auto are_threads_started() const noexcept -> bool;

    auto receive_data_from_external_thread() -> size_t;
    auto trigger_received_packets_from_external_thread() -> void;

    // signals
    // # from main thread
    sigslot::signal<bool> connection_state_signal;
    // # from reading thread if enabled
    sigslot::signal<> timeout_packet_signal;
    sigslot::signal<> invalid_packet_size_signal;
    sigslot::signal<> invalid_checksum_signal;
    // sigslot::signal<PData> packet_received_signal;
    sigslot::signal<std::span<PData>> packets_received_signal;

private :

    auto receive_data() -> size_t;
    auto trigger_received_packets() -> void;


    struct Impl;
    std::unique_ptr<Impl> i;
};
}
