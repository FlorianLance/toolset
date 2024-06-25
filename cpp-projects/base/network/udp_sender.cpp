
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


#include "udp_sender.hpp"

// std
#include <format>

// local
#include "boost_asio.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"
#include "data/checksum.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

using namespace boost::asio;

using namespace tool;
using namespace tool::net;

struct UdpSender::Impl{
    io_service ioService;
    std::unique_ptr<ip::udp::socket> socket = nullptr;
    boost::asio::ip::basic_resolver_results<ip::udp> endpoint;
    std::string solvedAdress;

    bool simulateFailure = false;
    int percentageFailture = 0;
};

UdpSender::UdpSender(): i(std::make_unique<Impl>()){
    update_size_packets(sizeUdpPacket);
}

UdpSender::~UdpSender(){
    clean_socket();
}

auto UdpSender::is_opened() const noexcept -> bool{
    return i->socket != nullptr;
}

auto UdpSender::init_socket(std::string targetName, std::string port, Protocol protocol) -> bool{

    // init socket and service
    try{
        i->socket = std::make_unique<ip::udp::socket>(i->ioService);
        i->socket->open(protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4());
        i->socket->set_option(detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{ 5 });
        i->socket->set_option(ip::udp::socket::reuse_address(true));
        i->socket->set_option(ip::udp::socket::send_buffer_size(9000*1000));
        i->socket->set_option(ip::udp::socket::receive_buffer_size(9000*1000));

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
                port)
            );
        }

    }catch (const boost::system::system_error& error){
        Logger::error(std::format("[UdpSender::init_socket] Cannot resolve target name [{}] with writing port [{}], error [{}].\n", targetName, port, error.what()));
        clean_socket();
        return false;
    }

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
            Logger::error(std::format("UdpSender: Cannot shutdown socket with adress {}, error message: {}.\n",
                i->solvedAdress, error.what()));
        }
    }
    i->socket = nullptr;
}

auto UdpSender::send_packet_data(std::span<const std::byte> packetData) -> size_t{

    size_t bytesNbSent=0;
    try{
        bytesNbSent = i->socket->send_to(boost::asio::buffer(packetData.data(), packetData.size()), i->endpoint->endpoint());
    } catch (const boost::system::system_error& error) {
        Logger::error(std::format("UdpSender::send_data: Cannot sent data to endpoint {}, error message: {}.\n",
            i->endpoint->endpoint().address().to_string(),
            error.what())
        );
    }
    return bytesNbSent;
}

auto UdpSender::send_mono_packet(Header &header) -> size_t{
    return send_packet_data(std::span(packetBuffer.data(), header.totalSizeBytes));
}

auto UdpSender::send_data(Header &header, std::span<const std::byte> dataToSend) -> size_t {

    static constexpr size_t sizePacketHeader = sizeof (Header);
    size_t sizePacketData  = sizeUdpPacket - sizePacketHeader;
    size_t nbPacketsNeeded = dataToSend.size() / sizePacketData;
    size_t rest            = dataToSend.size() % sizePacketData;
    if(rest > 0){
        ++nbPacketsNeeded;
    }

    header.totalSizeBytes     = static_cast<std::uint32_t>(nbPacketsNeeded * sizePacketHeader + dataToSend.size());
    header.totalNumberPackets = static_cast<std::uint32_t>(nbPacketsNeeded);

    size_t nbBytesSent = 0;
    for(size_t idP = 0; idP < header.totalNumberPackets; ++idP){

        header.currentPacketId = static_cast<std::uint16_t>(idP);

        if(idP < header.totalNumberPackets -1){
            header.currentPacketSizeBytes = static_cast<std::uint16_t>(sizeUdpPacket);
        }else{
            header.currentPacketSizeBytes = static_cast<std::uint16_t>(rest + sizePacketHeader);
        }
        
        header.currentPacketTimestampNs = Time::nanoseconds_since_epoch().count();

        // copy data
        auto dataS = std::span(dataToSend.data() + header.dataOffset, header.currentPacketSizeBytes - sizePacketHeader);
        std::copy(
            dataS.begin(), dataS.end(),
            packetBuffer.begin() + sizePacketHeader
        );

        // compute data checksum
        header.checkSum = data::Checksum::gen_crc16(dataS);

        // copy header
        std::copy(
            reinterpret_cast<std::byte*>(&header),
            reinterpret_cast<std::byte*>(&header) + sizePacketHeader,
            packetBuffer.begin()
        );

        // send packet
        if(!i->simulateFailure){
            nbBytesSent += send_packet_data(std::span(packetBuffer.data(), header.currentPacketSizeBytes));
        }else{
            // failure simulation
            if(rand()%100 > i->percentageFailture){
                nbBytesSent += send_packet_data(std::span(packetBuffer.data(), header.currentPacketSizeBytes));
            }else{
                nbBytesSent += header.currentPacketSizeBytes;
            }
        }

        // update offset
        header.dataOffset += header.currentPacketSizeBytes - sizePacketHeader;
    }
    return nbBytesSent;
}

auto UdpSender::update_size_packets(size_t newUdpPacketSize) -> void{
    sizeUdpPacket = newUdpPacketSize;
    packetBuffer.resize(sizeUdpPacket);
}

auto UdpSender::simulate_failure(bool enabled, int percentage) -> void{
    i->simulateFailure      = enabled;
    i->percentageFailture   = percentage;
}

auto UdpSender::generate_header(MessageType type) -> Header{
    Header header;
    header.type  = type;
    if(currentIdMessages.contains(header.type)){
        header.idMessage = currentIdMessages[header.type]++;
        if(header.idMessage > 1000000){
            currentIdMessages[type] = 0;
        }
    }else{
        currentIdMessages[header.type] = 0;
        header.idMessage = 0;
    }
    return header;
}


auto UdpSender::generate_dataless_header(MessageType type) -> Header{

    Header header;
    header.type                     = type;
    header.totalSizeBytes           = static_cast<std::uint32_t>(sizeof(Header));
    header.totalNumberPackets       = 1;
    header.currentPacketId          = 0;
    header.currentPacketSizeBytes   = static_cast<std::uint16_t>(header.totalSizeBytes);
    header.currentPacketTimestampNs = Time::nanoseconds_since_epoch().count();
    header.dataOffset               = 0;

    if(currentIdMessages.contains(type)){
        header.idMessage = currentIdMessages[type]++;
        if(header.idMessage > 100000){
            currentIdMessages[type] = 0;
        }
    }else{
        currentIdMessages[type] = 0;
        header.idMessage = 0;
    }

    return header;
}

