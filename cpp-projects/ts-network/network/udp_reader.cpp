
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

#include "udp_reader.hpp"

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// base
#include "utility/logger.hpp"
#include "utility/time.hpp"



using namespace boost::asio;
using namespace boost::asio::detail;
using namespace boost::asio::ip;

using namespace tool;
using namespace tool::net;


struct UdpReader::Impl{

    // udp connection
    std::atomic_bool connected = false;
    io_service ioService;
    std::unique_ptr<udp::socket> socket = nullptr;
    udp::endpoint endPoint;    

    // reading readPacketsT
    std::unique_ptr<std::thread> readPacketsT = nullptr;
    std::unique_ptr<std::thread> processPacketsT = nullptr;
    std::atomic_bool threadsStarted = false;

    // packets   
    DoubleRingBuffer<std::uint8_t> packetsRingBuffer;
    std::mutex locker;
    Buffer<PData> packetsReceived;
    Buffer<PData> swapPackets;

    size_t maxPacketSize = 9000;
    size_t timeoutMs = 100;
};

UdpReader::UdpReader() : i(std::make_unique<Impl>()){

}

UdpReader::~UdpReader(){

    if(are_threads_started()){
        stop_threads();
    }
    if(is_connected()){
        clean_socket();
    }
}

auto UdpReader::init_socket(std::string readingAdress, int port, Protocol protocol, size_t maxPacketSize) -> bool{

    i->maxPacketSize    = maxPacketSize;
    i->packetsRingBuffer.resize(10000, i->maxPacketSize);

    if(are_threads_started()){
        Log::error(std::format("[UdpReader::init_socket] Cannot init socket while reading thread is still active.\n"sv));
        return false;
    }

    // reset socket if necessary
    if(i->connected){
        clean_socket();
    }

    try {
        // open socket
        i->socket   = std::make_unique<udp::socket>(i->ioService);
        i->socket->open(protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4());
        i->socket->set_option(socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{static_cast<int>(i->timeoutMs)});
        i->socket->set_option(udp::socket::reuse_address(true));

        size_t receiveBufferSize = i->maxPacketSize * 300;
        i->socket->set_option(udp::socket::receive_buffer_size(static_cast<int>(receiveBufferSize)));

        // bind socket
        if(readingAdress.empty()){
            i->endPoint = udp::endpoint(protocol == Protocol::ipv6 ? ip::udp::v6() : ip::udp::v4(), static_cast<unsigned short>(port));
            Log::message(std::format("[UdpReader::init_socket] Init socket with binded enpoint [any_address], [{}].\n"sv, port));
        }else{
            i->endPoint = udp::endpoint(address::from_string(readingAdress), static_cast<unsigned short>(port));
            Log::message(std::format("[UdpReader::init_socket] Init socket with binded enpoint [{}], [{}].\n"sv, readingAdress, port));
        }
        i->socket->bind(i->endPoint);        

    }catch (const boost::system::system_error &error){
        Log::error(std::format("[UdpReader::init_socket] Cannot bind endpoint [{}], [{}], error message: [{}].\n"sv, readingAdress, port, error.what()));
        clean_socket();
        return false;
    }

    connection_state_signal(i->connected = true);
    return true;
}

auto UdpReader::clean_socket() -> void{

    if(i->socket){
        try {
            i->ioService.stop();
            i->socket->shutdown(udp::socket::shutdown_receive);
            std::this_thread::sleep_for (std::chrono::milliseconds(75));
            i->socket->close();
        }catch (const boost::system::system_error &error){
            Log::error(std::format("[UdpReader::clean_socket] Cannot shutdown socket, error message: [{}].\n", error.what()));
        }
    }
    i->socket = nullptr;

    if(i->connected){
        connection_state_signal(i->connected = false);
    }
}

auto UdpReader::start_threads() -> void{

    if(are_threads_started()){
        Log::error("[UdpReader::start_threads] Reading thread already started.\n");
        return;
    }

    if(!is_connected()){
        Log::error("[UdpReader::start_threads] Socket not connected.\n");
        return;
    }

    if((i->readPacketsT == nullptr) && (i->processPacketsT == nullptr)){
        
        auto lg = LogG("[UdpReader::start_threads] Start threads"sv);
        i->threadsStarted = true;
        i->readPacketsT  = std::make_unique<std::thread>([&](){
            auto lgS = LogG("[UdpReader::start_threads] Receiving thread"sv);
            while(i->threadsStarted){
                receive_data();
            }
        });
        i->processPacketsT = std::make_unique<std::thread>([&](){
            auto lgS = LogG("[UdpReader::start_threads] Triggering thread"sv);
            while(i->threadsStarted){
                trigger_received_packets();
            }
        });

    }else{
        Log::error("[UdpReader::start_threads] Cannot start reading, thread not cleaned.\n");
        return;
    }
}

auto UdpReader::stop_threads() -> void{

    if(!are_threads_started()){
        Log::error("[UdpReader::stop_threads] Reading thread not started.\n");
        return;
    }

    i->threadsStarted = false;

    if(i->readPacketsT != nullptr){        
        if(i->readPacketsT->joinable()){
            Log::log("[UdpReader::stop_threads] Join readPacketsT.\n"sv);
            i->readPacketsT->join();
        }
        Log::log("[UdpReader::stop_threads] Destroy readPacketsT.\n"sv);
        i->readPacketsT = nullptr;
    }

    if(i->processPacketsT != nullptr){
        if(i->processPacketsT->joinable()){
            Log::log("[UdpReader::stop_threads] Join processPacketsT.\n"sv);
            i->processPacketsT->join();
        }
        Log::log("[UdpReader::stop_threads] Destroy processPacketsT.\n"sv);
        i->processPacketsT = nullptr;
    }
}

auto UdpReader::are_threads_started() const noexcept -> bool {
    return i->threadsStarted;
}


auto UdpReader::receive_data() -> size_t{

    if(!i->connected){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return 0;
    }

    size_t nbBytesReceived = 0;
    udp::endpoint senderEndpoint;

    auto packetData = i->packetsRingBuffer.current_span(i->maxPacketSize);

    std::chrono::nanoseconds receptionTimestampNs;

    // receive data
    try {
        nbBytesReceived = i->socket->receive_from(
            boost::asio::buffer(packetData.data(),static_cast<size_t>(packetData.size())),
            senderEndpoint
        );
        receptionTimestampNs = Time::nanoseconds_since_epoch();

    } catch (const boost::system::system_error &error) {
        if(error.code() != boost::asio::error::timed_out){
            Log::ferror("[UdpReader::receive_data] Cannot read from socket, error message: [{}]\n"sv, error.what());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));            
        }
        return 0;
    }

    if(nbBytesReceived != 0){

        auto pkd = std::span(reinterpret_cast<const std::byte*>(packetData.data()), nbBytesReceived);
        auto headerData     = pkd.first(sizeof(Header));
        auto dataToProcess  = pkd.subspan(sizeof(Header));
        Header header(headerData);

        if(header.type != 10 && header.type != 9 && header.type != 6 && header.type != 8){
            Log::fmessage("Receive data {} {} {} \n",
                header.type, header.totalSizeBytes, nbBytesReceived
            );
        }


        // check packet checksum
        // if(header.checkSum != 0){
        //     auto currentCS = header.checkSum;
        //     header.checkSum = 0;
        //     auto checksum = data::Checksum::gen_crc16(std::span(reinterpret_cast<std::byte*>(&header), sizeof(header)));
        //     if(checksum != currentCS){
        //         invalid_checksum_signal();
        //         Log::error(std::format("[UdpReader::receive_data] Invalid checksum size: [{}], expected: [{}]\n"sv, checksum, currentCS));
        //         return nbBytesReceived;
        //     }
        // }

        header.receptionTimestampNs = receptionTimestampNs.count();

        // check packet size
        if(header.currentPacketSizeBytes != pkd.size_bytes()){
            invalid_packet_size_signal();
            Log::ferror("[UdpReader::receive_data] Invalid packet size: [{}], expected: [{}]\n"sv, header.currentPacketSizeBytes, pkd.size_bytes());
            return nbBytesReceived;
        }


        // increment ring buffer for next data reading
        i->packetsRingBuffer.increment();

        // generate endpoint
        auto endPoint = EndPointId(senderEndpoint.address().to_string(), header.senderId);

        // store received packet
        i->locker.lock();
        i->packetsReceived.push_back(PData{std::move(endPoint), std::move(header), std::move(dataToProcess)});
        if(i->packetsReceived.size() > 2000){
            i->packetsReceived.remove_first();
            Log::error("[UdpReader::receive_data] To many packets received, oldest packet dropped.\n"sv);
        }
        i->locker.unlock();

        // Log::log_unf("[/R0]");
    }else{
        Log::fmessage("Received empty\n");
    }

    return nbBytesReceived;
}

auto UdpReader::trigger_received_packets() -> void{

    i->swapPackets.clear();
    if(i->locker.try_lock()){
        std::swap(i->packetsReceived, i->swapPackets);
        i->locker.unlock();
    }

    if(!i->swapPackets.empty()){
        // Log::log_unf("[PRS]");
        packets_received_signal(i->swapPackets.span());
        // Log::log_unf("[/PRS]\n");
    }else{
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

auto UdpReader::is_connected() const noexcept -> bool{
    return i->connected;
}

auto UdpReader::receive_data_from_external_thread() -> size_t{

    if(are_threads_started()){
        Log::error("[UdpReader::receive_data_from_external_thread] Thread is already started, cannot be called from main thread.\n");
        return 0;
    }

    return receive_data();
}

auto UdpReader::trigger_received_packets_from_external_thread() -> void{

    if(are_threads_started()){
        Log::error("[UdpReader::generate_packets_from_external_thread] Thread is already started, cannot be called from main thread.\n");
        return;
    }
    trigger_received_packets();
}



