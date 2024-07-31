
#pragma once

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// local
#include "network/network_enums.hpp"
#include "utility/ring_buffer.hpp"

namespace tool::net{

class UdpServer{
public:

    UdpServer(){
        io_service = std::make_unique<boost::asio::io_service>();
    }

    auto initialize_socket(Protocol protocol, int port, int timeoutMs) -> bool{
        using namespace boost::asio;
        using namespace boost::asio::detail;
        using namespace boost::asio::ip;

        clean_socket();

        try {
            socket = std::make_unique<udp::socket>(*io_service, udp::endpoint(protocol == Protocol::ipv4 ? udp::v4() : udp::v6(), port));
            socket->set_option(socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{timeoutMs});
            socket->set_option(udp::socket::reuse_address(true));
            socket->set_option(udp::socket::receive_buffer_size(receiveBufferSize));
        }catch (const boost::system::system_error &error){
            // Logger::error(std::format("[UdpReader::init_connection] Cannot bind endpoint {}, {}, error message: {}.\n",
                                      // readingAdress, port, error.what()));
            clean_socket();
            return false;
        }

        connected = true;
        // connection_state_signal(true);
        return true;
    }

    auto clean_socket() -> void{

        using namespace boost::asio;
        using namespace boost::asio::detail;
        using namespace boost::asio::ip;

        if(socket){
            try {
                io_service->stop();
                socket->shutdown(udp::socket::shutdown_receive);
                std::this_thread::sleep_for (std::chrono::milliseconds(75));
                socket->close();
            }catch (const boost::system::system_error &error){
                // Logger::error(std::format("[UdpReader::clean_connection] Cannot shutdown socket, error message: {}.\n", error.what()));
            }
            socket = nullptr;
        }

        if(connected){
            connected = false;
            // connection_state_signal(false);
        }

    }

    auto start_reading_thread() -> void{
        if(is_reading_thread_started()){
            // Logger::error("[UdpReader::start_reading_thread] Reading thread already started.\n");
            return;
        }

        if(!is_connected()){
            // Logger::error("[UdpReader::start_reading_thread] Socket not connected.\n");
            return;
        }

        if(thread == nullptr){
            thread = std::make_unique<std::thread>(&UdpServer::read_data_thread, this);
        }else{
            // Logger::error("[UdpReader::start_reading_thread] Cannot start reading, thread not cleaned.\n");
            return;
        }
    }

    auto stop_reading_thread() -> void{

        if(!is_reading_thread_started()){
            // Logger::error("[UdpReader::stop_reading_thread] Reading thread not started.\n");
            return;
        }

        if(thread != nullptr){
            if(thread->joinable()){
                readingThreadStarted = false;
                thread->join();
            }
            thread = nullptr;
        }
    }

    auto is_connected() const noexcept -> bool{return connected;}
    auto is_reading_thread_started() const noexcept -> bool { return readingThreadStarted;}

protected:

    virtual auto process_packet(std::span<const std::byte> packet) -> void{
        static_cast<void>(packet);
    }

private:

    auto read_data_thread() -> void{

        readingThreadStarted = true;

        while(readingThreadStarted){
            read_packet();
        }

        readingThreadStarted = false;
    }

    auto read_packet() -> size_t{


        using namespace boost::asio;
        using namespace boost::asio::detail;
        using namespace boost::asio::ip;

        if(!connected){
            return 0;
        }

        auto buffer = packetsRingBuffer.current_span(packetSize);

        size_t nbBytesReceived = 0;
        udp::endpoint senderEndpoint;
        try {
            // receive data
            nbBytesReceived = socket->receive_from(
                boost::asio::buffer(buffer.data(),static_cast<size_t>(buffer.size())),
                senderEndpoint
            );
            packetsRingBuffer.increment();

        } catch (const boost::system::system_error &error) {
            if(error.code() == boost::asio::error::timed_out){
                // timeout_packet_signal();
            }else{
                // Logger::error("[UdpReader::read_packet] Cannot read from socket, error message: {}\n", error.what());
                connected = false;
                // connection_state_signal(false);
            }
            return 0;
        }

        if(nbBytesReceived == 0){
            // Logger::warning("[UdpReader::read_packet] No bytes received.");
            return 0;
        }

        process_packet(std::span(reinterpret_cast<const std::byte*>(buffer.data()), nbBytesReceived));

        return nbBytesReceived;
    }


    static constexpr size_t packetSize = 9000;
    static constexpr size_t receiveBufferSize = packetSize * 50;

    std::unique_ptr<boost::asio::io_service> io_service = nullptr;
    std::unique_ptr<boost::asio::ip::udp::socket> socket = nullptr;

    std::unique_ptr<std::thread> thread = nullptr;
    std::atomic_bool connected = false;
    std::atomic_bool readingThreadStarted = false;

    DoubleRingBuffer<std::uint8_t> packetsRingBuffer;
    // DoubleRingBuffer<std::byte> messagesBuffer;
};
}

// using boost::asio::ip::udp;

// std::string make_daytime_string()
// {
//     using namespace std; // For time_t, time and ctime;
//     time_t now = time(0);
//     return ctime(&now);
// }

// int main()
// {
//     try
//     {
//         boost::asio::io_service io_service;

//         udp::socket socket(io_service, udp::endpoint(udp::v4(), 13));

//         for (;;)
//         {
//             boost::array<char, 1> recv_buf;
//             udp::endpoint remote_endpoint;
//             boost::system::error_code error;
//             socket.receive_from(boost::asio::buffer(recv_buf),
//                                 remote_endpoint, 0, error);

//             if (error && error != boost::asio::error::message_size)
//                 throw boost::system::system_error(error);

//             std::string message = make_daytime_string();

//             boost::system::error_code ignored_error;
//             socket.send_to(boost::asio::buffer(message),
//                            remote_endpoint, 0, ignored_error);
//         }
//     }
//     catch (std::exception& e)
//     {
//         std::cerr << e.what() << std::endl;
//     }

//     return 0;
// }
