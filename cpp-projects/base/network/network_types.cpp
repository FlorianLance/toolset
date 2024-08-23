
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

#include "network_types.hpp"

// std
#include <numeric>

// local
#include "boost_asio.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"

using namespace boost::asio;
using namespace tool::net;
using namespace std::chrono;

auto Host::get_name() -> std::string{
    return boost::asio::ip::host_name();
}

auto Interface::list_local_interfaces(Protocol protocol) -> std::vector<Interface>{

    using namespace boost::asio;
    using namespace boost::asio::ip;

    io_service ioService;
    udp::resolver resolver(ioService);
    udp::resolver::query query(protocol ==  Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4(), host_name(), "");

    std::vector<Interface> interfaces;
    try{
        udp::resolver::iterator it = resolver.resolve(query);

        while(it!=udp::resolver::iterator()){
            address addr = (it++)->endpoint().address();            
            interfaces.emplace_back(Interface{protocol,addr.to_string()});
        }
    }catch (const boost::system::system_error &error){
        Logger::error(std::format("list_local_interfaces: Cannot list interfaces, error message: {}\n", error.what()));
    }

    ioService.stop();
    return interfaces;
}


// UdpNetworkSendingSettings::UdpNetworkSendingSettings(std::string_view ipAdressStr, uint16_t port, uint16_t maxSizeUdpPacket) : port(port), maxSizeUdpPacket(maxSizeUdpPacket) {

//     std::fill(ipAdress.begin(), ipAdress.end(), ' ');
//     if(ipAdressStr.size() <= 45){
//         std::copy(std::begin(ipAdressStr), std::end(ipAdressStr), ipAdress.begin());
//     }else{
//         Logger::error(std::format("UdpNetworkSendingSettings::UdpNetworkSendingSettings invalid ip adresss: [{}]\n", ipAdressStr));
//     }
// }


Header::Header(std::span<const std::byte> packet){
    if(packet.size_bytes() == sizeof(Header)){
        std::copy(std::begin(packet), std::end(packet), reinterpret_cast<std::byte*>(this));
    }else{
        Logger::error(std::format("[Header::Header] Invalid packet size, size: [{}], expected: [{}] \n", packet.size_bytes(),  sizeof(Header)));
    }
}

auto UdpMessageReception::update(const Header &header, std::span<const std::byte> dataToProcess, DoubleRingBuffer<std::byte> &rBuffer) -> void{
    
    if(!infos.contains(header.messageId)){

        // first packet of a new frame to be received
        UdpReceptionInfo info;
        info.idMessage              = header.messageId;
        info.firstPacketReceivedTS  = Time::nanoseconds_since_epoch();
        info.totalNumberOfPacket    = header.totalNumberPackets;
        info.totalSizeBytes         = header.totalSizeBytes;
        info.totalHeaderSizeBytes   = info.totalNumberOfPacket * sizeof(Header);
        info.totalDataSizeBytes     = info.totalSizeBytes - info.totalHeaderSizeBytes;

        if(rBuffer.current_size() < info.totalDataSizeBytes){
            rBuffer.resize_current(info.totalDataSizeBytes);
        }
        info.messageData = rBuffer.current_span(info.totalDataSizeBytes);
        rBuffer.increment();
        
        infos[header.messageId] = std::move(info);
    }
    
    UdpReceptionInfo &info = infos[header.messageId];

    if(header.currentPacketId == 0){
        // packet n°1 of the frame received
        info.firstPacketSentTS = std::chrono::nanoseconds(header.currentPacketTimestampNs);
    }

    // copy data
    std::copy(dataToProcess.data(), dataToProcess.data() + header.currentPacketSizeBytes - sizeof(Header), info.messageData.data() + header.dataOffset);
    info.totalBytesReceived += header.currentPacketSizeBytes;
    info.nbPacketsReceived++;
}

auto UdpMessageReception::check_message_timeout() -> size_t{
    // sanity remover
    auto ts = Time::nanoseconds_since_epoch();
    timeoutIdPacketsToRemove.clear();
    for(const auto &info : infos){
        if(Time::difference_ms(info.second.firstPacketReceivedTS, ts).count() > 1000){
            timeoutIdPacketsToRemove.push_back(info.first);
        }
    }
    for(const auto &idM : timeoutIdPacketsToRemove){
        {
            const auto &info = infos[idM];
            Logger::warning(std::format("[MultiPacketsUdpReception::check_message_timeout] Message [{}] dropped for timeout, packets received: [{}], expected: [{}].\n"sv, idM, info.nbPacketsReceived, info.totalNumberOfPacket));
        }
        infos.erase(idM);
        messageReceived.set_current(0);
        messageReceived.increment();
    }
    return timeoutIdPacketsToRemove.size();
}

auto UdpMessageReception::message_fully_received(const Header &header) -> std::optional<UdpReceptionInfo>{
    
    const UdpReceptionInfo &info = infos[header.messageId];

    std::optional<UdpReceptionInfo> oInfo = {};
    if(info.all_packets_received()){
        if(info.all_bytes_received()){
            if(lastFullMessageSentTS < info.firstPacketSentTS){
                oInfo = info;
                lastFullMessageSentTS = info.firstPacketSentTS;
                messageReceived.set_current(1);
                messageReceived.increment();
            }else{
                messageReceived.set_current(0);
                messageReceived.increment();
                Logger::error(std::format("[MultiPacketsUdpReception::message_fully_received] Message [{}] dropped, a newer message was sent before.\n"sv, info.idMessage));
            }
        }else{
            messageReceived.set_current(0);
            messageReceived.increment();
            Logger::error(std::format("[MultiPacketsUdpReception::message_fully_received] All packets for message [{}] received, but with invalid size: [{}], expected: [{}].\n"sv, info.idMessage, info.totalBytesReceived, info.totalSizeBytes));
        }
        infos.erase(header.messageId);
    }
    return oInfo;
}

auto UdpMessageReception::get_percentage_success() -> int{
    int percentage = 0;
    auto span = messageReceived.span();
    percentage = static_cast<int>(
        std::accumulate(std::begin(span), std::end(span), 0)
    );
    return percentage;
}


