


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

// boost
#include <boost/asio.hpp>
#include <boost/asio/ip/host_name.hpp>

// local
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


UdpNetworkSendingSettings::UdpNetworkSendingSettings(std::string ipAdressStr, uint16_t port, uint16_t maxSizeUdpPacket) : port(port), maxSizeUdpPacket(maxSizeUdpPacket) {

    std::fill(ipAdress.begin(), ipAdress.end(), ' ');
    if(ipAdressStr.size() <= 45){
        std::copy(std::begin(ipAdressStr), std::end(ipAdressStr), ipAdress.begin());
    }
}

Framerate::Framerate(){
    rTimes.resize(nbMaxValues, std::chrono::nanoseconds(0));
}

auto Framerate::add_frame() -> void{
    rTimes.set_current(Time::nanoseconds_since_epoch());
    rTimes.increment();
}

auto Framerate::get_framerate() -> float{
    size_t count = 0;

    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    for(const auto time : rTimes.span()){
        if(Time::difference_ms(time, currentTimestampNS).count() < 5000){
            ++count;
        }
    }

    return count / 5.f;
}

AverageSynch::AverageSynch(){
    diffNs.resize(nbMaxValues, std::chrono::nanoseconds(0));
}

auto AverageSynch::update_average_difference(int64_t timestampNS) -> void{

    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    diffNs.set_current(currentTimestampNS - nanoseconds(timestampNS));
    diffNs.increment();

    auto span = diffNs.span();
    averageDiffNs = static_cast<std::int64_t>(
        1.0 * std::accumulate(std::begin(span), std::end(span), nanoseconds(0)).count() / nbMaxValues
    );
}

Header::Header(std::span<int8_t> packet){
    if(packet.size_bytes() == sizeof(Header)){
        std::copy(std::begin(packet), std::end(packet), reinterpret_cast<std::int8_t*>(this));
    }else{
        Logger::error(std::format("[Header::Header] Invalid packet size, size: [{}], expected: [{}] \n", packet.size_bytes(),  sizeof(Header)));
    }
}

auto MultiPacketsUdpReception::update(const Header &header, std::span<const int8_t> dataToProcess, DoubleRingBuffer<int8_t> &rBuffer) -> void{

    if(!cFramesInfo.contains(header.idMessage)){

        // first packet of a new frame to be received
        UdpReceptionInfo info;
        info.idMessage              = header.idMessage;
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

        cFramesInfo[header.idMessage] = std::move(info);
    }

    UdpReceptionInfo &info = cFramesInfo[header.idMessage];

    if(header.currentPacketId == 0){
        // packet n°1 of the frame received
        info.firstPacketSentTS = std::chrono::nanoseconds(header.currentPacketTimestampNs);
    }

    // copy data
    std::copy(dataToProcess.data(), dataToProcess.data() + header.currentPacketSizeBytes - sizeof(Header), info.messageData.data() + header.dataOffset);
    info.totalBytesReceived += header.currentPacketSizeBytes;
    info.nbPacketsReceived++;
}

auto MultiPacketsUdpReception::check_timeout_frames() -> size_t{
    // sanity remover
    auto ts = Time::nanoseconds_since_epoch();
    timeoutIdPacketsToRemove.clear();
    for(const auto &info : cFramesInfo){
        if(Time::difference_ms(info.second.firstPacketReceivedTS, ts).count() > 1000){
            timeoutIdPacketsToRemove.push_back(info.first);
        }
    }
    for(const auto &idM : timeoutIdPacketsToRemove){
        {
            const auto &info = cFramesInfo[idM];
            Logger::warning(std::format("[MultiPacketsUdpReception::check] Frame [{}] dropped for timeout, packets received: [{}], expected: [{}].\n", idM, info.nbPacketsReceived, info.totalNumberOfPacket));
        }
        cFramesInfo.erase(idM);
        messageReceived.set_current(0);
        messageReceived.increment();
    }
    return timeoutIdPacketsToRemove.size();
}

auto MultiPacketsUdpReception::message_fully_received(const Header &header) -> std::optional<UdpReceptionInfo>{

    const UdpReceptionInfo &info = cFramesInfo[header.idMessage];

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
                Logger::error(std::format("[MultiPacketsUdpReception::message_fully_received] Message [{}] dropped, a newer message was sent before.\n", info.idMessage));
            }
        }else{
            messageReceived.set_current(0);
            messageReceived.increment();
            Logger::error(std::format("[MultiPacketsUdpReception::message_fully_received] All packets for message [{}] received, but with invalid size: [{}], expected: [{}].\n", info.idMessage, info.totalBytesReceived, info.totalSizeBytes));
        }
        cFramesInfo.erase(header.idMessage);
    }
    return oInfo;
}

auto MultiPacketsUdpReception::get_percentage_success() -> int{
    int percentage = 0;
    auto span = messageReceived.span();
    percentage = static_cast<int>(
        std::accumulate(std::begin(span), std::end(span), 0)
    );
    return percentage;
}


AverageLatency::AverageLatency(){
    latencies.resize(nbMaxValues, 0);
}

auto AverageLatency::update_average_latency(int64_t latency) -> void{

    latencies.set_current(latency);
    latencies.increment();

    auto span = latencies.span();
    averageLatency = static_cast<std::int64_t>(
        1.0 * std::accumulate(std::begin(span), std::end(span), 0) / nbMaxValues
    );
}
