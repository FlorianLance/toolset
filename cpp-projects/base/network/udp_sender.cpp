
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

// boost
#include <boost/asio.hpp>

// local
#include "utility/logger.hpp"
#include "utility/time.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;

using namespace boost::asio;

using namespace tool;
using namespace tool::network;

struct UdpSender::Impl{
    io_service ioService;
    std::unique_ptr<ip::udp::socket> socket = nullptr;
    boost::asio::ip::basic_resolver_results<ip::udp> endpoint;
    std::string solvedAdress;
};

UdpSender::UdpSender(): i(std::make_unique<Impl>()){
    update_size_packets(sizeUdpPacket);
}

UdpSender::~UdpSender(){
    clean_socket();
}

auto UdpSender::is_opened() const -> bool{
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
                targetName = "localhost", port, ip::udp::resolver::canonical_name));
        }else{
            i->endpoint = resolver.resolve(
                ip::udp::resolver::query(targetName, port));
        }

    }catch (const boost::system::system_error& error){

        Logger::error(std::format("UdpSender::init_socket: Cannot resolve target name {} with writing port {}, error message: {}.\n",
            targetName, port, error.what()));
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

auto UdpSender::send_packet_data(int8_t *packetData, size_t nbBytes) -> size_t{

    size_t bytesNbSent=0;
    try{
        bytesNbSent = i->socket->send_to(boost::asio::buffer(packetData, nbBytes), i->endpoint->endpoint());
    } catch (const boost::system::system_error& error) {
        Logger::error(std::format("UdpSender::send_data: Cannot sent data to endpoint {}, error message: {}.\n",
            i->endpoint->endpoint().address().to_string(),
            error.what())
        );
    }
    return bytesNbSent;
}

auto UdpSender::send_mono_packet(Header &header) -> size_t{
    return send_packet_data(packetBuffer.data(), header.totalSizeBytes);
}

auto UdpSender::send_packets(Header &header, size_t allPacketsNbBytes) -> size_t{

    constexpr size_t sizePacketHeader = sizeof (Header);
    size_t sizePacketData   = sizeUdpPacket - sizePacketHeader;

    size_t nbPacketsNeeded = allPacketsNbBytes / sizePacketData;
    size_t rest            = allPacketsNbBytes % sizePacketData;
    if(rest > 0){
        ++nbPacketsNeeded;
    }

    header.totalSizeBytes     = static_cast<std::uint32_t>(nbPacketsNeeded * sizeof(Header) + allPacketsNbBytes);
    header.totalNumberPackets = static_cast<std::uint32_t>(nbPacketsNeeded);

    size_t nbBytesSent = 0;
    for(size_t idP = 0; idP < header.totalNumberPackets; ++idP){

        header.currentPacketId = static_cast<std::uint16_t>(idP);

        if(idP < header.totalNumberPackets -1){
            header.currentPacketSizeBytes = static_cast<std::uint16_t>(sizeUdpPacket);
        }else{
            header.currentPacketSizeBytes = static_cast<std::uint16_t>(rest + sizeof (Header));
        }

        header.currentPacketTime = Time::nanoseconds_since_epoch().count();

        // copy header
        std::copy(
            reinterpret_cast<std::int8_t*>(&header),
            reinterpret_cast<std::int8_t*>(&header) + sizeof(Header),
            packetBuffer.begin()
        );

        // copy data
        std::copy(
            bufferToSend.data() + header.dataOffset,
            bufferToSend.data() + header.dataOffset + header.currentPacketSizeBytes - sizeof(Header),
            packetBuffer.begin() + sizeof(Header)
        );

        // send packet
        nbBytesSent += send_packet_data(packetBuffer.data(), header.currentPacketSizeBytes);

        // update offset
        header.dataOffset += header.currentPacketSizeBytes - sizeof(Header);
    }
    return nbBytesSent;
}

auto UdpSender::update_size_packets(size_t newUdpPacketSize) -> void{
    sizeUdpPacket = newUdpPacketSize;
    packetBuffer.resize(sizeUdpPacket);
}
