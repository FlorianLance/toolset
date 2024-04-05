

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
#include <string>
#include <chrono>

// local
#include "utility/ring_buffer.hpp"
#include "utility/unordered_map.hpp"
#include "network_enums.hpp"

namespace tool::net{

struct Host{
    [[maybe_unused]] static auto get_name() -> std::string;
};

struct Interface{
    Protocol protocol;
    std::string ipAddress;
    static auto list_local_interfaces(Protocol protocol) -> std::vector<Interface>;
};

struct ReadSendNetworkInfos{
    bool local = false;
    size_t idReadingInterface = 0;
    std::string readingAdress;
    int readingPort;
    std::string sendingAdress;
    int sendingPort;
    Protocol protocol = Protocol::unknow;
    bool startReadingThread = true;
};

struct UdpNetworkSendingSettings{
    UdpNetworkSendingSettings() = default;
    UdpNetworkSendingSettings(std::string ipAdressStr, uint16_t port, uint16_t maxSizeUdpPacket);
    std::array<char, 45> ipAdress;
    std::uint16_t port;
    std::uint16_t maxSizeUdpPacket;
};



struct Feedback{
    MessageType receivedMessageType;
    FeedbackType feedback;
};

struct Framerate{

    Framerate();
    auto add_frame() -> void;
    auto get_framerate() -> float;

private:
    static constexpr size_t nbMaxValues = 1000;
    SingleRingBuffer<std::chrono::nanoseconds> rTimes;
};

struct AverageSynch{
    AverageSynch();
    auto update_average_difference(std::int64_t timestampNS) -> void;
    std::int64_t averageDiffNs = 0;
private:
    static constexpr size_t nbMaxValues = 1000;    
    SingleRingBuffer<std::chrono::nanoseconds> diffNs;
};

struct Header{

    Header() = default;
    Header(std::span<std::int8_t> packet);

    std::int64_t currentPacketTimestampNs = 0;
    std::uint32_t totalSizeBytes = 0;
    std::uint32_t dataOffset = 0;
    std::int32_t idMessage = -1;
    std::uint16_t totalNumberPackets = 0;
    std::uint16_t currentPacketId = 0;
    std::uint16_t currentPacketSizeBytes = 0;
    std::uint32_t checkSum = 0;
    MessageType type = MessageType::undefined;

    constexpr auto total_headers_size_bytes() const noexcept -> size_t{
        return totalNumberPackets * sizeof(Header);
    }
    constexpr auto total_size_data_bytes() const noexcept -> size_t{
        return totalSizeBytes - total_headers_size_bytes();
    }
};


struct UdpReceptionInfo{

    std::int32_t idMessage = 0;
    std::chrono::nanoseconds firstPacketReceivedTS = {};
    std::chrono::nanoseconds firstPacketSentTS = {};
    std::span<std::int8_t> messageData = {};

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

struct MultiPacketsUdpReception{

    MultiPacketsUdpReception(){
        messageReceived.resize(100, 1);
    }

    auto update(const Header &header, std::span<const std::int8_t> data, DoubleRingBuffer<std::int8_t> &rBuffer) -> void;
    auto check_timeout_frames() -> size_t;
    auto message_fully_received(const Header &header) -> std::optional<UdpReceptionInfo>;
    auto get_percentage_success() -> int;

    umap<std::int32_t, UdpReceptionInfo> cFramesInfo;
    Buffer<std::int32_t> timeoutIdPacketsToRemove;
    std::chrono::nanoseconds lastFullMessageSentTS = {};

    SingleRingBuffer<std::int8_t> messageReceived;
};


struct UdpMonoPacketData{

    static auto copy_only_header_to_data(const Header &header, std::vector<std::int8_t> &outputData) -> void{

        if(outputData.size() < header.totalSizeBytes){
            outputData.resize(header.totalSizeBytes);
        }

        auto headerD = reinterpret_cast<const std::int8_t*>(&header);
        std::copy(headerD, headerD + sizeof(Header), outputData.begin());
    }

    template <typename T>
    static auto copy_to_data(const Header &header, const T* dataP, std::vector<int8_t> &data) -> void{

        auto messageData = reinterpret_cast<const std::int8_t *>(dataP);
        size_t dataSize = header.totalSizeBytes - sizeof(Header);
        if(data.size() < dataSize){
            data.resize(dataSize);
        }

        auto headerD = reinterpret_cast<const std::int8_t*>(&header);
        std::copy(headerD,      headerD     + sizeof(Header),   data.begin());
        std::copy(messageData,  messageData + dataSize,         data.begin() + sizeof(Header));
    }
};


struct UdpReceivedStatus{
    int percentageSuccess = 0;
    float framerate = 0.f;
};

}
