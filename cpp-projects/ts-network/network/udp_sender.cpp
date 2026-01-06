
/*******************************************************************************
** Toolset-ts-network                                                         **
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

#include "udp_sender.hpp"

// std
#include <format>
#include <execution>

// boost
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/ip/host_name.hpp>
#include <boost/asio/io_service.hpp>

// base
#include "utility/logger.hpp"
#include "utility/time.hpp"
#include "network/checksum.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

using namespace boost::asio;

using namespace tool;
using namespace tool::net;

struct UdpSender::Impl{

    // udp connection
    io_service ioService;
    std::unique_ptr<ip::udp::socket> socket = nullptr;
    boost::asio::ip::basic_resolver_results<ip::udp> endpoint;
    std::atomic_bool connected = false;

    // packet data
    size_t sizeUdpPacket = 9000;
    std::vector<std::byte> packetBuffer;
    umap<MessageTypeId, std::int32_t> currentIdMessages;

    // failure simulation
    bool simulateFailure = false;
    int percentageFailture = 0;

    // id
    size_t senderId = 0;

    std::vector<std::vector<std::byte>> frameDataPackets;
};

UdpSender::UdpSender(): i(std::make_unique<Impl>()){

}

UdpSender::~UdpSender(){

    if(is_connected()){
        clean_socket();
    }
}

auto UdpSender::is_connected() const noexcept -> bool{
    return i->connected;
}

auto UdpSender::init_socket(std::string targetName, std::string port, Protocol protocol, size_t maxPacketSize) -> bool{

    update_size_packets(i->sizeUdpPacket = maxPacketSize);

    // reset socket if necessary
    if(is_connected()){
        clean_socket();
    }

    try{
        // open socket
        i->socket = std::make_unique<ip::udp::socket>(i->ioService);
        i->socket->open(protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4());
        i->socket->set_option(detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{ 5 });
        i->socket->set_option(ip::udp::socket::reuse_address(true));
        i->socket->set_option(ip::udp::socket::send_buffer_size(9000*100));
        i->socket->set_option(ip::udp::socket::receive_buffer_size(9000*5));

        // init resolver
        ip::udp::resolver resolver(i->ioService);
        if(ip::host_name() == targetName){
            i->endpoint = resolver.resolve(ip::udp::resolver::query(
                protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4(),
                targetName = "localhost", port, ip::udp::resolver::canonical_name)
            );
        }else{
            i->endpoint = resolver.resolve(ip::udp::resolver::query(
                protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4(),
                targetName,
                port
            ));
        }
        
        Log::message(std::format("ENDPOINT {} {}\n", targetName, port));

    }catch (const boost::system::system_error& error){
        Log::error(std::format("[UdpSender::init_socket] Cannot resolve target name [{}] with writing port [{}], error [{}].\n", targetName, port, error.what()));
        clean_socket();
        return false;
    }

    connection_state_signal(i->connected = true);
    return true;
}

auto UdpSender::clean_socket() -> void{

    if(i->socket){
        try{
            i->ioService.stop();
            i->socket->shutdown(boost::asio::ip::udp::socket::shutdown_send);
            std::this_thread::sleep_for (std::chrono::milliseconds(300));
            i->socket->close();
        }catch (const boost::system::system_error& error){
            Log::error(std::format("[UdpSender::clean_socket] Cannot shutdown socket with adress {}, error message: {}.\n",
                i->endpoint->endpoint().address().to_string(),
                error.what())
            );
        }
    }
    i->socket = nullptr;

    if(i->connected){
        connection_state_signal(i->connected = false);
    }
}

auto UdpSender::send_packet_data(std::span<const std::byte> packetData) -> size_t{

    size_t bytesNbSent=0;
    try{
        if(!i->simulateFailure){
            bytesNbSent = i->socket->send_to(boost::asio::buffer(packetData.data(), packetData.size()), i->endpoint->endpoint());
        }else{
            // failure simulation
            if(rand()%100 > i->percentageFailture){
                bytesNbSent = i->socket->send_to(boost::asio::buffer(packetData.data(), packetData.size()), i->endpoint->endpoint());
            }else{
                bytesNbSent = 0;
            }
        }
    } catch (const boost::system::system_error& error) {
        Log::error(std::format("[UdpSender::send_packet_data] Cannot sent data to endpoint {}, error code: [{}] message: [{}].\n",
            i->endpoint->endpoint().address().to_string(),
            error.code().value(),
            error.what())
        );


    }
    return bytesNbSent;   
}


auto UdpSender::generate_data_packets(MessageTypeId messageType, std::span<const std::byte> dataToSend, std::vector<std::vector<std::byte>> &dataPackets) -> void{

    Header commonHeader = generate_header(messageType);

    static constexpr size_t sizePacketHeader = sizeof (Header);
    size_t sizePacketData  = i->sizeUdpPacket - sizePacketHeader;
    size_t nbPacketsNeeded = dataToSend.size() / sizePacketData;
    size_t rest            = dataToSend.size() % sizePacketData;
    if(rest > 0){
        ++nbPacketsNeeded;
    }
    commonHeader.totalSizeBytes     = static_cast<std::uint32_t>(nbPacketsNeeded * sizePacketHeader + dataToSend.size());
    commonHeader.totalNumberPackets = static_cast<std::uint32_t>(nbPacketsNeeded);

    // resize data packets
    dataPackets.resize(nbPacketsNeeded);
    for(auto &dataPacket : dataPackets){
        dataPacket.resize(i->sizeUdpPacket);
    }
    if(rest != 0){
        dataPackets.back().resize(rest + sizePacketHeader);
    }

    std::vector<size_t> ids(nbPacketsNeeded);
    std::iota(ids.begin(), ids.end(), 0);
    std::for_each(std::execution::par_unseq, std::begin(ids), std::end(ids), [&](size_t idP){

        // init header informations
        Header header = commonHeader;
        header.currentPacketId          = static_cast<std::uint16_t>(idP);
        header.currentPacketSizeBytes   = static_cast<std::uint16_t>(dataPackets[idP].size());        
        header.dataOffset               = static_cast<std::uint32_t>(idP * sizePacketData);

        // retrieve data subpart
        auto dataS = std::span(dataToSend.data() + header.dataOffset, header.currentPacketSizeBytes - sizePacketHeader);

        // copy packet data
        std::copy(
            dataS.begin(), dataS.end(),
            dataPackets[idP].begin() + sizePacketHeader
        );

        // copy header
        header.creationTimestampNs = Time::nanoseconds_since_epoch().count();
        header.checkSum = 0;
        // header.checkSum = data::Checksum::gen_crc16(std::span(reinterpret_cast<std::byte*>(&header), sizeof(header)));

        std::copy(
            reinterpret_cast<std::byte*>(&header),
            reinterpret_cast<std::byte*>(&header) + sizePacketHeader,
            dataPackets[idP].begin()
        );
    });
}

auto UdpSender::send_data(Header &header, std::span<const std::byte> dataToSend) -> size_t {

    static constexpr size_t sizePacketHeader = sizeof (Header);
    size_t sizePacketData  = i->sizeUdpPacket - sizePacketHeader;
    size_t nbPacketsNeeded = dataToSend.size() / sizePacketData;


    size_t rest= dataToSend.size() % sizePacketData;
    if(rest > 0){
        ++nbPacketsNeeded;
    }

    header.totalSizeBytes     = static_cast<std::uint32_t>(nbPacketsNeeded * sizePacketHeader + dataToSend.size());
    header.totalNumberPackets = static_cast<std::uint32_t>(nbPacketsNeeded);

    if(header.messageId == 2){
        Log::fmessage("SEND DATA SIZE: {} {}\n", header.totalSizeBytes, header.totalNumberPackets);
    }

    size_t nbBytesSent = 0;
    for(size_t idP = 0; idP < header.totalNumberPackets; ++idP){

        // update header informations
        header.currentPacketId = static_cast<std::uint16_t>(idP);
        if(idP < header.totalNumberPackets -1){
            header.currentPacketSizeBytes = static_cast<std::uint16_t>(i->sizeUdpPacket);
        }else{
            header.currentPacketSizeBytes = static_cast<std::uint16_t>(rest + sizePacketHeader);
        }

        // copy data
        auto dataS = std::span(dataToSend.data() + header.dataOffset, header.currentPacketSizeBytes - sizePacketHeader);
        std::copy(
            dataS.begin(), dataS.end(),
            i->packetBuffer.begin() + sizePacketHeader
        );

        // copy header
        header.creationTimestampNs = Time::nanoseconds_since_epoch().count();
        header.checkSum = 0;
        header.checkSum = Checksum::gen_crc16(std::span(reinterpret_cast<std::byte*>(&header), sizeof(header)));

        std::copy(
            reinterpret_cast<std::byte*>(&header),
            reinterpret_cast<std::byte*>(&header) + sizePacketHeader,
            i->packetBuffer.begin()
        );

        // send packet
        nbBytesSent += send_packet_data(std::span(i->packetBuffer.data(), header.currentPacketSizeBytes));

        // update offset
        header.dataOffset += header.currentPacketSizeBytes - sizePacketHeader;
    }

    if(header.type != 10 && header.type != 9 && header.type != 6 && header.type != 8){
        Log::fmessage("Send data {} {} {} {} {} {} {}\n",
            header.type, header.totalSizeBytes, sizePacketHeader, sizePacketData, nbPacketsNeeded, rest, nbBytesSent
        );
    }


    return nbBytesSent;
}

auto UdpSender::update_size_packets(size_t newUdpPacketSize) -> void{
    i->sizeUdpPacket = newUdpPacketSize;
    i->packetBuffer.resize(i->sizeUdpPacket);
}

auto UdpSender::simulate_failure(bool enabled, int percentage) -> void{
    i->simulateFailure      = enabled;
    i->percentageFailture   = percentage;
}

auto UdpSender::set_sender_id(size_t idClient) -> void{
    Log::fmessage("########### UdpSender {}\n", idClient);
    i->senderId = idClient;
}

auto UdpSender::send_message(MessageTypeId messageType, std::span<const std::byte> data) -> size_t{

    if(!is_connected()){
        Log::error("[UdpSender::send_message] Sender not opened, message canceled.\n"sv);
        return 0;
    }

    // init header
    if(!data.empty()){        

        Header header = generate_header(messageType);

        // send data
        size_t nbBytesSent = send_data(header, data);
        if(nbBytesSent != header.totalSizeBytes){
            Log::error(std::format("[UdpSender::send_message] Invalid nb of bytes send, {} instead of {}.\n"sv, nbBytesSent, header.totalSizeBytes));
        }        

        return nbBytesSent;

    }else{

        Header header = generate_dataless_header(messageType);

        // copy data
        auto headerD = reinterpret_cast<const std::byte*>(&header);
        std::copy(headerD, headerD + sizeof(Header), i->packetBuffer.begin());

        return send_packet_data(std::span(i->packetBuffer.data(), header.totalSizeBytes));
    }
}

auto UdpSender::generate_header(MessageTypeId type) -> Header{
    Header header;
    header.senderId = static_cast<std::uint32_t>(i->senderId);
    header.type     = type;
    if(i->currentIdMessages.contains(header.type)){
        header.messageId = i->currentIdMessages[header.type]++;
        if(header.messageId > 1000000){
            i->currentIdMessages[type] = 0;
        }
    }else{
        i->currentIdMessages[header.type] = 0;
        header.messageId = 0;
    }
    return header;
}


auto UdpSender::generate_dataless_header(MessageTypeId type) -> Header{

    Header header;
    header.senderId                 = static_cast<std::uint32_t>(i->senderId);
    header.type                     = type;
    header.totalSizeBytes           = static_cast<std::uint32_t>(sizeof(Header));
    header.totalNumberPackets       = 1;
    header.currentPacketId          = 0;
    header.currentPacketSizeBytes   = static_cast<std::uint16_t>(header.totalSizeBytes);
    header.creationTimestampNs = Time::nanoseconds_since_epoch().count();
    header.dataOffset               = 0;

    if(i->currentIdMessages.contains(type)){
        header.messageId = i->currentIdMessages[type]++;
        if(header.messageId > 100000){
            i->currentIdMessages[type] = 0;
        }
    }else{
        i->currentIdMessages[type] = 0;
        header.messageId = 0;
    }

    return header;
}

