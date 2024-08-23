
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

class UdpSender {

public:

    UdpSender();
    ~UdpSender();

    auto init_socket(std::string tagetName, std::string writingPort, Protocol protocol) -> bool;
    auto set_sender_id(size_t idClient) -> void;
    auto clean_socket() -> void;

    [[nodiscard]] auto is_connected() const noexcept -> bool;

    auto update_size_packets(size_t newUdpPacketSize) -> void;
    auto simulate_failure(bool enabled, int percentage) -> void;

    // headers
    auto generate_dataless_header(MessageTypeId type) -> Header;
    auto generate_header(MessageTypeId type) -> Header;
    // data
    auto generate_data_packets(MessageTypeId messageType, std::span<const std::byte> dataToSend, std::vector<std::vector<std::byte>> &dataPackets) -> void;
    // send
    auto send_message(MessageTypeId messageType, std::span<const std::byte> data = {}) -> size_t;
    auto send_packet_data(std::span<const std::byte> packetData) -> size_t;
    auto send_data(Header &header, std::span<const std::byte> dataToSend) -> size_t;

    // signals
    sigslot::signal<bool> connection_state_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
