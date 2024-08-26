
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
#include "utility/safe_queue.hpp"
#include "data/checksum.hpp"

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
    std::unique_ptr<std::vector<std::tuple<EndPointId, Header, std::span<const std::byte>>>> packetsReceived = nullptr;;
    std::unique_ptr<std::vector<std::tuple<EndPointId, Header, std::span<const std::byte>>>> swapPacketsReceived = nullptr;;
};

UdpReader::UdpReader() : i(std::make_unique<Impl>()){
    i->packetsRingBuffer.resize(10000, maxPacketSize);
    i->packetsReceived     = std::make_unique<std::vector<std::tuple<EndPointId, Header, std::span<const std::byte>>>>();
    i->swapPacketsReceived = std::make_unique<std::vector<std::tuple<EndPointId, Header, std::span<const std::byte>>>>();
}

UdpReader::~UdpReader(){

    if(is_reading_thread_started()){
        stop_reading_thread();
    }
    if(is_connected()){
        clean_socket();
    }
}

auto UdpReader::init_socket(std::string readingAdress, int port, Protocol protocol) -> bool{

    if(is_reading_thread_started()){
        Logger::error(std::format("[UdpReader::init_socket] Cannot init socket while reading thread is still active.\n"sv));
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
        i->socket->set_option(socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{timeoutMs});
        i->socket->set_option(udp::socket::reuse_address(true));
        i->socket->set_option(udp::socket::receive_buffer_size(receiveBufferSize));

        // bind socket
        i->endPoint = udp::endpoint(address::from_string(readingAdress), static_cast<unsigned short>(port));
        i->socket->bind(i->endPoint);

        Logger::message(std::format("[UdpReader::init_socket] Init socket with binded enpoint [{}], [{}].\n"sv, readingAdress, port));

    }catch (const boost::system::system_error &error){
        Logger::error(std::format("[UdpReader::init_socket] Cannot bind endpoint [{}], [{}], error message: [{}].\n"sv, readingAdress, port, error.what()));
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
            Logger::error(std::format("[UdpReader::clean_socket] Cannot shutdown socket, error message: [{}].\n", error.what()));
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

    if((i->readPacketsT == nullptr) && (i->processPacketsT == nullptr)){

        auto lg = LogGuard("[UdpReader::start_reading_thread] Start threads"sv);
        i->threadsStarted  = true;
        i->readPacketsT    = std::make_unique<std::thread>([&](){
            auto lgS = LogGuard("[UdpReader::start_reading_thread] Receiving thread"sv);
            while(i->threadsStarted){
                receive_data();
            }
        });
        i->processPacketsT    = std::make_unique<std::thread>([&](){            
            auto lgS = LogGuard("[UdpReader::start_reading_thread] Triggering thread"sv);
            while(i->threadsStarted){
                trigger_received_packets();
            }
        });

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

    i->threadsStarted = false;

    if(i->readPacketsT != nullptr){        
        if(i->readPacketsT->joinable()){
            Logger::log("[UdpReader::stop_reading_thread] Join readPacketsT.\n"sv);
            i->readPacketsT->join();
        }
        Logger::log("[UdpReader::stop_reading_thread] Destroy readPacketsT.\n"sv);
        i->readPacketsT = nullptr;
    }

    if(i->processPacketsT != nullptr){
        if(i->processPacketsT->joinable()){
            Logger::log("[UdpReader::stop_reading_thread] Join processPacketsT.\n"sv);
            i->processPacketsT->join();
        }
        Logger::log("[UdpReader::stop_reading_thread] Destroy processPacketsT.\n"sv);
        i->processPacketsT = nullptr;
    }
}

auto UdpReader::is_reading_thread_started() const noexcept -> bool {
    return i->threadsStarted;
}

auto UdpReader::receive_data() -> size_t{

    if(!i->connected){
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return 0;
    }

    size_t nbBytesReceived = 0;
    udp::endpoint senderEndpoint;

    auto packetData = i->packetsRingBuffer.current_span(maxPacketSize);

    // receive data
    try {
        nbBytesReceived = i->socket->receive_from(
            boost::asio::buffer(packetData.data(),static_cast<size_t>(packetData.size())),
            senderEndpoint
        );
    } catch (const boost::system::system_error &error) {
        if(error.code() != boost::asio::error::timed_out){
            Logger::error("[UdpReader::receive_data] Cannot read from socket, error message: [{}]\n"sv, error.what());
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return 0;
        }
    }

    if(nbBytesReceived != 0){

        auto pkd = std::span(reinterpret_cast<const std::byte*>(packetData.data()), nbBytesReceived);
        auto headerData     = pkd.first(sizeof(Header));
        auto dataToProcess  = pkd.subspan(sizeof(Header));
        Header header(headerData);

        // check packet size
        if(header.currentPacketSizeBytes != pkd.size_bytes()){
            invalid_packet_size_signal();
            Logger::error(std::format("[UdpReader::receive_data] Invalid packet size: [{}], expected: [{}]\n"sv, header.currentPacketSizeBytes, pkd.size_bytes()));
            return nbBytesReceived;
        }

        // check packet checksum
        if(header.checkSum != 0){
            auto checksum = data::Checksum::gen_crc16(dataToProcess);
            if(checksum != header.checkSum){
                invalid_checksum_signal();
                Logger::error(std::format("[UdpReader::receive_data] Invalid checksum size: [{}], expected: [{}]\n"sv, checksum, header.checkSum));
                return nbBytesReceived;
            }
        }

        // increment ring buffer for next data reading
        i->packetsRingBuffer.increment();

        // generate endpoint
        auto endPoint = EndPointId(senderEndpoint.address().to_string(), header.senderId);

        // store received packet
        i->locker.lock();
        if(i->packetsReceived->size() < 200){
            i->packetsReceived->push_back(std::make_tuple(endPoint, std::move(header), std::move(dataToProcess)));
        }else{
            Logger::error("[UdpReader::receive_data] To many packets received, last packet dropped.\n"sv);
        }
        i->locker.unlock();
    }

    return nbBytesReceived;
}

auto UdpReader::trigger_received_packets() -> void{

    i->swapPacketsReceived->clear();

    bool hasPacketsToGenerate = false;
    if(i->locker.try_lock()){
        if(!i->packetsReceived->empty()){
            std::swap(i->packetsReceived, i->swapPacketsReceived);
            hasPacketsToGenerate = true;
        }
        i->locker.unlock();
    }

    if(hasPacketsToGenerate){
        for(auto &pckR : *i->swapPacketsReceived){
            packed_received_signal(std::move(std::get<0>(pckR)), std::move(std::get<1>(pckR)), std::move(std::get<2>(pckR)));
        }
    }else{
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

auto UdpReader::is_connected() const noexcept -> bool{
    return i->connected;
}

auto UdpReader::receive_data_from_external_thread() -> size_t{

    if(is_reading_thread_started()){
        Logger::error("[UdpReader::receive_data_from_external_thread] Thread is already started, cannot be called from main thread.\n");
        return 0;
    }

    return receive_data();
}

auto UdpReader::trigger_received_packets_from_external_thread() -> void{

    if(is_reading_thread_started()){
        Logger::error("[UdpReader::generate_packets_from_external_thread] Thread is already started, cannot be called from main thread.\n");
        return;
    }
    trigger_received_packets();
}


// auto UdpReader::read_packets() -> size_t{

// if(!i->connected){
    //     return 0;
    // }

    // size_t nbBytesReceived = 0;

    // // receive data
    // try {
    //     nbBytesReceived = i->socket->receive_from(
    //         boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())),
    //         senderEndpoint
    //     );
    //     i->packetsRingBuffer.increment();
    // } catch (const boost::system::system_error &error) {
    //     if(error.code() != boost::asio::error::timed_out){
    //         Logger::error("[UdpReader::read_packet] Cannot read from socket, error message: [{}]\n", error.what());
    //         buffers.clear();
    //         break;
    //     }
    // }

    // // if(!i->connected){
    // //     return 0;
    // // }

    // // // receive packets
    // // std::vector<std::tuple<std::span<std::uint8_t>, size_t, std::string>> buffers;
    // // bool readPackets = true;
    // // while(readPackets){

    // //     auto buffer = i->packetsRingBuffer.current_span(maxPacketSize);
    // //     size_t nbBytesReceived = 0;
    // //     udp::endpoint senderEndpoint;

    // //     // receive data
    // //     try {
    // //         nbBytesReceived = i->socket->receive_from(
    // //             boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())),
    // //             senderEndpoint
    // //         );
    // //         i->packetsRingBuffer.increment();
    // //     } catch (const boost::system::system_error &error) {
    // //         if(error.code() != boost::asio::error::timed_out){
    // //             Logger::error("[UdpReader::read_packet] Cannot read from socket, error message: [{}]\n", error.what());
    // //             buffers.clear();
    // //             break;
    // //         }
    // //     }

    // //     // std::cout << "R " << nbBytesReceived << " | ";
    // //     if(nbBytesReceived != 0){
    // //         buffers.push_back({buffer, nbBytesReceived, senderEndpoint.address().to_string()});

    // //     }else{
    // //         break;
    // //     }
    // // }

    // // // process packets
    // // size_t totalNbBytesReceived = 0;
    // // for(const auto &buffer : buffers){

    // //     auto packet        = std::span(reinterpret_cast<const std::byte*>(std::get<0>(buffer).data()), std::get<1>(buffer));
    // //     auto headerData    = packet.first(sizeof(Header));
    // //     auto dataToProcess = packet.subspan(sizeof(Header));
    // //     Header header(headerData);

    // //     // check packet size
    // //     if(header.currentPacketSizeBytes != packet.size_bytes()){
    // //         invalid_packet_size_signal();
    // //         Logger::error(std::format("[UdpReader::read_packet] Invalid packet size: [{}], expected: [{}]\n", header.currentPacketSizeBytes, packet.size_bytes()));
    // //         return 0;
    // //     }

    // //     // check packet checksum
    // //     if(header.checkSum != 0){
    // //         auto checksum = data::Checksum::gen_crc16(dataToProcess);
    // //         if(checksum != header.checkSum){
    // //             invalid_checksum_signal();
    // //             Logger::error(std::format("[UdpReader::read_packet] Invalid checksum size: [{}], expected: [{}]\n", checksum, header.checkSum));
    // //             return 0;
    // //         }
    // //     }

    // //     // send received packet
    // //     auto endPoint = EndPoint(std::get<2>(buffer), header.senderId);
    // //     packed_received_signal(std::move(endPoint), std::move(header), dataToProcess);

    // //     totalNbBytesReceived += std::get<1>(buffer);
    // // }

    // // return totalNbBytesReceived;

//     if(!i->connected){
//         return 0;
//     }

//     auto buffer = i->packetsRingBuffer.current_span(maxPacketSize);

//     size_t nbBytesReceived = 0;
//     udp::endpoint senderEndpoint;

//     try {
//         // receive data
//         nbBytesReceived = i->socket->receive_from(
//             boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())),
//             senderEndpoint
//         );
//         i->packetsRingBuffer.increment();

//     } catch (const boost::system::system_error &error) {
//         if(error.code() == boost::asio::error::timed_out){
//             timeout_packet_signal();
//         }else{
//             Logger::error("[UdpReader::read_packet] Cannot read from socket, error message: [{}]\n", error.what());
//         }
//         return 0;
//     }

//     if(nbBytesReceived == 0){
//         Logger::warning("[UdpReader::read_packet] No bytes received.");
//         return 0;
//     }

//     auto packet        = std::span(reinterpret_cast<const std::byte*>(buffer.data()), nbBytesReceived);
//     auto headerData    = packet.first(sizeof(Header));
//     auto dataToProcess = packet.subspan(sizeof(Header));
//     Header header(headerData);

//     // check packet size
//     if(header.currentPacketSizeBytes != packet.size_bytes()){
//         invalid_packet_size_signal();
//         Logger::error(std::format("[UdpReader::read_packet] Invalid packet size: [{}], expected: [{}]\n", header.currentPacketSizeBytes, packet.size_bytes()));
//         return 0;
//     }

//     // check packet checksum
//     if(header.checkSum != 0){
//         auto checksum = data::Checksum::gen_crc16(dataToProcess);
//         if(checksum != header.checkSum){
//             invalid_checksum_signal();
//             Logger::error(std::format("[UdpReader::read_packet] Invalid checksum size: [{}], expected: [{}]\n", checksum, header.checkSum));
//             return 0;
//         }
//     }

//     // send received packet
//     auto endPoint = EndPoint(senderEndpoint.address().to_string(), header.senderId);
//     packed_received_signal(std::move(endPoint), std::move(header), dataToProcess);

//     return nbBytesReceived;
// }



