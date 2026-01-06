
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include <string>
#include <chrono>

// base
#include "utility/ring_buffer.hpp"
#include "utility/unordered_map.hpp"

// local
#include "network_enums.hpp"

namespace tool::net{

struct Host{
    [[maybe_unused]] static auto get_name() -> std::string;
};

struct Interface{
    Protocol protocol;
    std::string ipAddress;
};

struct EndPointId{

    EndPointId() = default;
    EndPointId(const std::string &address, std::uint32_t senderId) : address(address), senderId(senderId){
        id = std::format("{}_{}", address, senderId);
    }

    std::string address;
    std::uint32_t senderId = 0;
    std::string id;
};
using MessageTypeId = std::int8_t;

struct Feedback{
    MessageTypeId receivedMessageType = 0;
    FeedbackType type = FeedbackType::undefined;
};

struct Header{

    Header() = default;
    Header(std::span<const std::byte> packet);

    std::uint32_t senderId = 0;
    std::int32_t messageId = -1;
    std::uint32_t totalSizeBytes = 0;
    std::uint32_t dataOffset = 0;
    std::int64_t creationTimestampNs = 0;
    std::uint32_t checkSum = 0;
    std::uint16_t totalNumberPackets = 0;
    std::uint16_t currentPacketId = 0;
    std::uint16_t currentPacketSizeBytes = 0;
    std::int64_t receptionTimestampNs = 0;
    MessageTypeId type = 0;

    constexpr auto total_headers_size_bytes() const noexcept -> size_t{
        return totalNumberPackets * sizeof(Header);
    }
    constexpr auto total_size_data_bytes() const noexcept -> size_t{
        return totalSizeBytes - total_headers_size_bytes();
    }
};

struct PData{
    EndPointId endPoint;
    Header header;
    std::span<const std::byte> data;
};


struct UdpReceptionInfo{

    std::int32_t idMessage = 0;
    std::chrono::nanoseconds firstPacketEmissionTS = {};
    std::chrono::nanoseconds firstPacketReceptionTS = {};
    std::span<std::byte> messageData = {};

    size_t totalBytesReceived   = 0;
    size_t nbPacketsReceived    = 0;

    size_t totalNumberOfPacket  = 0;
    size_t totalHeaderSizeBytes = 0;
    size_t totalDataSizeBytes   = 0;
    size_t totalSizeBytes       = 0;

    [[nodiscard]] constexpr auto all_packets_received() const noexcept -> bool{
        return (nbPacketsReceived == totalNumberOfPacket);
    }

    [[nodiscard]] constexpr auto all_bytes_received() const noexcept -> bool{
        return (totalSizeBytes == totalBytesReceived);
    }
};

struct UdpMessageReception{

    UdpMessageReception(){
        messageReceived.resize(100, 1);
    }

    auto update(const Header &header, std::span<const std::byte> data, DoubleRingBuffer<std::byte> &rBuffer) -> void;
    auto check_message_timeout() -> size_t;
    auto message_fully_received(const Header &header) -> std::optional<UdpReceptionInfo>;
    auto get_percentage_success() -> int;

    umap<std::int32_t, UdpReceptionInfo> infos;
    Buffer<std::int32_t> timeoutIdPacketsToRemove;
    std::chrono::nanoseconds lastFullMessageSentTS = {};

    SingleRingBuffer<std::uint8_t> messageReceived;
};


}
