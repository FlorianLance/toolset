
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

#include "udp_reader.hpp"


// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// local
#include "utility/logger.hpp"

using namespace boost::asio;
using namespace boost::asio::detail;
using namespace boost::asio::ip;

using namespace tool;
using namespace tool::net;


struct UdpReader::Impl{

    // udp connection
    io_service ioService;
    std::unique_ptr<udp::socket> socket = nullptr;
    udp::endpoint endPoint;
    std::atomic_bool connected = false;

    // reading thread
    std::unique_ptr<std::thread> thread = nullptr;
    std::atomic_bool readingThreadStarted = false;

    // packets
    DoubleRingBuffer<char> packetsRingBuffer;
};

UdpReader::UdpReader() : i(std::make_unique<Impl>()){

    i->packetsRingBuffer.resize(10000, packetSize);
    messagesBuffer.resize(100, 0);
}

UdpReader::~UdpReader(){

    if(is_reading_thread_started()){
        stop_reading_thread();
    }
    if(is_connected()){
        clean_connection();
    }
}

auto UdpReader::init_connection(std::string readingAdress, int port, Protocol protocol) -> bool{

    if(is_reading_thread_started()){
        Logger::error(std::format("[UdpReader::init_connection] Cannot init socket while reading thread is still active.\n"));
        return false;
    }

    // reset socket if necessary
    if(i->connected){
        clean_connection();
    }

    try {
        // open socket
        i->socket   = std::make_unique<udp::socket>(i->ioService);
        i->socket->open(protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4());
        i->socket->set_option(socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{timeoutMs});
        i->socket->set_option(udp::socket::reuse_address(true));
        i->socket->set_option(udp::socket::receive_buffer_size(receiveBufferSize));

        // bind socket
        i->endPoint = udp::endpoint(address::from_string(readingAdress), static_cast<unsigned short>(port));
        i->socket->bind(i->endPoint);

    }catch (const boost::system::system_error &error){
        Logger::error(std::format("[UdpReader::init_connection] Cannot bind endpoint {}, {}, error message: {}.\n",
            readingAdress, port, error.what()));
        clean_connection();
        return false;
    }

    connection_state_signal(i->connected = true);
    return true;
}

auto UdpReader::clean_connection() -> void{

    if(i->socket){
        try {
            i->ioService.stop();
            i->socket->shutdown(udp::socket::shutdown_receive);
            std::this_thread::sleep_for (std::chrono::milliseconds(75));
            i->socket->close();
        }catch (const boost::system::system_error &error){
            Logger::error(std::format("[UdpReader::clean_connection] Cannot shutdown socket, error message: {}.\n", error.what()));
        }
    }
    i->socket = nullptr;

    if(i->connected){
        connection_state_signal(i->connected = false);
    }
}

auto UdpReader::start_reading_thread() -> void{

    if(is_reading_thread_started()){
        Logger::error("[UdpReader::start_reading_thread] Reading thread already started.\n");
        return;
    }

    if(!is_connected()){
        Logger::error("[UdpReader::start_reading_thread] Socket not connected.\n");
        return;
    }

    if(i->thread == nullptr){
        i->thread = std::make_unique<std::thread>(&UdpReader::read_data_thread, this);
    }else{
        Logger::error("[UdpReader::start_reading_thread] Cannot start reading, thread not cleaned.\n");
        return;
    }
}

auto UdpReader::stop_reading_thread() -> void{

    if(!is_reading_thread_started()){
        Logger::error("[UdpReader::stop_reading_thread] Reading thread not started.\n");
        return;
    }

    if(i->thread != nullptr){
        if(i->thread->joinable()){
            i->readingThreadStarted = false;
            i->thread->join();
        }
        i->thread = nullptr;
    }
}

auto UdpReader::is_reading_thread_started() const noexcept -> bool {
    return i->readingThreadStarted;
}

auto UdpReader::is_connected() const noexcept -> bool{
    return i->connected;
}

auto UdpReader::process_packet(std::span<std::int8_t> packet) -> void{
    static_cast<void>(packet);
}

auto UdpReader::read_packet() -> size_t{

    if(!i->connected){
        return 0;
    }

    auto buffer = i->packetsRingBuffer.current_span(packetSize);

    size_t nbBytesReceived = 0;
    udp::endpoint senderEndpoint;
    try {
        // receive data
        nbBytesReceived = i->socket->receive_from(
            boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())),
            senderEndpoint
        );
        i->packetsRingBuffer.increment();

    } catch (const boost::system::system_error &error) {
        if(error.code() == boost::asio::error::timed_out){
            // timeout_packet_signal();
        }else{
            Logger::error("[UdpReader::read_packet] Cannot read from socket, error message: {}\n", error.what());
            connection_state_signal(i->connected = false);
        }
        return 0;
    }

    if(nbBytesReceived == 0){
        Logger::warning("[UdpReader::read_packet] No bytes received.");
        return 0;
    }

    process_packet(std::span<std::int8_t>(reinterpret_cast<std::int8_t*>(buffer.data()), nbBytesReceived));

    return nbBytesReceived;
}

auto UdpReader::read_data_thread() -> void{

    i->readingThreadStarted = true;

    while(i->readingThreadStarted){
        read_packet();
    }

    i->readingThreadStarted = false;
}

