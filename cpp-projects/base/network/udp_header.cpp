
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

#include "udp_header.hpp"

// local
#include "utility/time.hpp"
#include "utility/logger.hpp"

using namespace std::chrono;
using namespace tool::net;


auto Header::generate_mono_packet(MessageType type, size_t messageNbBytes) -> Header{
    Header header;
    header.type = type;
    header.totalSizeBytes = static_cast<std::uint32_t>(sizeof(Header) + messageNbBytes);
    header.totalNumberPackets = 1;
    header.currentPacketId = 0;
    header.currentPacketSizeBytes = header.totalSizeBytes;
    header.currentPacketTime = Time::nanoseconds_since_epoch().count();
    header.dataOffset = 0;
    return header;
}

auto UdpMultiPacketsMessage::copy_packet_to_data(const Header &header, size_t nbBytes, std::int8_t *packetData, std::vector<int8_t> &data) -> bool{

    if(header.idMessage != idLastMultiPacketsMessageReceived){
        // new frame
        receivingFrame = true;
        idLastMultiPacketsMessageReceived = header.idMessage;
        totalBytesReceived   = 0;
        nbPacketsReceived    = 0;
        firstPacketTimestamp = Time::nanoseconds_since_epoch().count();
    }

    if(!receivingFrame){
        return false;
    }

    size_t totalPacketSizeBytes = header.totalNumberPackets * sizeof(Header);
    size_t totalDataSizeBytes   = header.totalSizeBytes - totalPacketSizeBytes;

    // resize data
    if(data.size() < totalDataSizeBytes){
        data.resize(totalDataSizeBytes);
    }

    // copy packet
    std::copy(
        packetData,
        packetData + header.currentPacketSizeBytes - sizeof (Header),
        data.begin() + header.dataOffset
    );

    totalBytesReceived += nbBytes;
    nbPacketsReceived++;

    // check integrity
    // # nb bytes received
    if(header.currentPacketSizeBytes != nbBytes){
        // drop packet
        Logger::error(std::format("[UdpMultiPacketsMessage::copy_packet_to_data] Invalid packet size {}/{}\n", header.currentPacketSizeBytes,nbBytes));
        receivingFrame = false;
        return false;
    }

    // # packet timeout
    auto currentTime = Time::nanoseconds_since_epoch();
    auto ts = duration_cast<milliseconds>(currentTime - nanoseconds(firstPacketTimestamp)).count();
    if(ts > timeoutMs){
        // drop packet
        Logger::error(std::format("[UdpMultiPacketsMessage::copy_packet_to_data] Timeout packet {}ms\n", ts));
        receivingFrame = false;
        return false;
    }

    return true;
}

auto UdpMultiPacketsMessage::all_received(const Header &header) -> bool{
    if(nbPacketsReceived == header.totalNumberPackets && totalBytesReceived == header.totalSizeBytes){
        receivingFrame = false;
        return true;
    }
    return false;
}

