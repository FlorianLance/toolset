

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

#include "tcp_reader.hpp"

// std
#include <iostream>

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// base
#include "utility/logger.hpp"

using namespace boost::asio;
using namespace boost::asio::detail;
using namespace boost::asio::ip;

using namespace tool;
using namespace tool::net;



struct TcpReader::Impl{

    // udp connection
    io_service ioService;
    std::unique_ptr<tcp::socket> socket = nullptr;
    tcp::endpoint endpoint;

    // reading thread
    std::unique_ptr<std::thread> thread = nullptr;
//    size_t currentBufferId = 0;
//    size_t buffersCount = 10000;
//    std::vector<std::vector<char>> buffers;

    // states
    std::atomic_bool connectionValid = false;
    std::atomic_bool isReading = false;
};

TcpReader::TcpReader() : i(std::make_unique<Impl>()){

}

TcpReader::~TcpReader(){

    if(is_reading()){
        stop_reading();
    }
    if(is_connected()){
        clean_socket();
    }
}


bool TcpReader::init_socket(std::string serverName, int readingPort){

    if(is_reading()){
        Log::error(std::format("TcpReader: Cannot init socket while reading thread is still active.\n"));
        return false;
    }

    // reset socket if necessary
    if(i->connectionValid){
        clean_socket();
    }

    try {
        
        
        Log::message("TCP query\n");
        tcp::resolver::query query(serverName, std::to_string(readingPort));

        tcp::resolver resolver(i->ioService);
        Log::message("TCP resolve\n");
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        i->socket   = std::make_unique<tcp::socket>(i->ioService);
        
        Log::message(std::format("TCP connect {} {} \n", endpoint_iterator->service_name(), endpoint_iterator->host_name()));
        boost::asio::connect(*i->socket, endpoint_iterator);
        
        
        Log::message("connect\n");
//        i->endpoint = tcp::endpoint(address::from_string(readingAdress), static_cast<unsigned short>(readingPort));


        // bind socket

//        i->socket->bind(i->endpoint);


    }catch (const boost::system::system_error &error){
        Log::error(std::format("TcpReader: Cannot connect {}, {}, error message: {}.\n",
            serverName, readingPort, error.what()));
        clean_socket();
        return false;
    }

    connection_state_signal(i->connectionValid = true);
    return true;
}

void TcpReader::clean_socket(){

    if(i->socket){
        try {
            i->socket->close();
            i->ioService.stop();
        }catch (const boost::system::system_error &error){
            Log::error(std::format("TcpReader::clean_socket: Cannot shutdown socket, error message: {}.\n", error.what()));
        }
    }
    i->socket = nullptr;

    if(i->connectionValid){
        connection_state_signal(i->connectionValid = false);
    }
}

void TcpReader::start_reading(){

    if(is_reading()){
        Log::error("TcpReader::start_reading: Reading already started.\n");
        return;
    }

    if(!is_connected()){
        Log::error("TcpReader::start_reading: Socket not connected.\n");
        return;
    }

    if(i->thread == nullptr){
        i->thread = std::make_unique<std::thread>(&TcpReader::read_data, this);
    }else{
        Log::error("TcpReader::start_reading: Cannot start reading, thread not cleaned.\n");
        return;
    }
}

void TcpReader::stop_reading(){

    if(!is_reading()){
        Log::error("TcpReader::stop_reading: Reading not started.\n");
        return;
    }

    if(i->thread != nullptr){
        if(i->thread->joinable()){
            i->isReading = false;
            i->thread->join();
        }
        i->thread = nullptr;
    }
}

void TcpReader::process_packet(std::vector<char> *, size_t ){

}

bool TcpReader::is_reading() const noexcept{return i->isReading;}

bool TcpReader::is_connected() const noexcept{return i->connectionValid;}


void TcpReader::read_data(){

    i->isReading = true;

    std::vector<char> buffer(65535);
    while(i->isReading){

        if(!i->connectionValid){
            break;
        }

        size_t nbBytesReceived = 0;
        tcp::endpoint senderEndpoint;
        std::cout << "r";
        boost::system::error_code err;
        try {
            nbBytesReceived = i->socket->receive(boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())));
//            nbBytesReceived = read(*i->socket, boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())));

        } catch (const boost::system::system_error &error) {

            if(error.code() == boost::asio::error::timed_out){
                timeout_packet_signal();
            }else{
                Log::error(std::format("TcpReader::read_data: Cannot read from socket, error message: {}  code: {}\n",
                    error.what(),
                    error.code().value()));
                 connection_state_signal(i->connectionValid = false);
            }
            continue;
        }

        if(nbBytesReceived == 0){
            Log::warning("TcpReader::read_data: No bytes received.");
            continue;
        }

        //        senderEndpoint.address().to_v4().to_bytes();
        //        senderEndpoint.address().to_v6().to_bytes();
        //        senderEndpoint.protocol();
        //        endP.port();

        process_packet(&buffer, nbBytesReceived);
    }

    i->isReading = false;
}

