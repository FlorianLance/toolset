
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
#include "network_types.hpp"

namespace tool::net{

class UdpSender {

public:

    UdpSender();
    ~UdpSender();

    auto init_socket(std::string tagetName, std::string writingPort, Protocol protocol) -> bool;
    auto clean_socket() -> void;

    auto is_opened() const noexcept -> bool;
    auto update_size_packets(size_t newUdpPacketSize) -> void;
    auto simulate_failure(bool enabled, int percentage) -> void;

protected:

    auto send_mono_packet(Header &header) -> size_t;
    auto send_data(Header &header, std::span<const std::byte> dataToSend) -> size_t;

    auto generate_dataless_header(MessageType type) -> Header;
    auto generate_header(MessageType type) -> Header;

    size_t sizeUdpPacket = 9000;
    std::vector<std::byte> packetBuffer;
    std::vector<std::byte> bufferToSend;
    umap<MessageType, std::int32_t> currentIdMessages;

private:

    auto send_packet_data(std::span<const std::byte> packetData) -> size_t;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
