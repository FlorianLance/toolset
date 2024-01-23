/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#include "k2_scaner_tcp_sender.hpp"

// std
#include <thread>
#include <chrono>

// base
#include "utility/types.hpp"
#include "camera/impl/k2_network.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;
using boost::asio::ip::tcp;
using boost::asio::ip::address;
using namespace tool;
using namespace tool::cam;
using namespace tool::net;

struct K2ScanerTcpSender::Impl{

    size_t id;

    // TCP
    boost::asio::io_service ioService;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    K2TcpPacket packet;

    // data
    std::array<std::uint8_t, 4> sizeData;
    std::vector<std::uint8_t> data;
    int sizeUdpPackets = 512;

    std::vector<net::Interface> *localInterfaces = nullptr;
    size_t readingInterface = 0;    

    bool connectionValid = false;

    std::mutex locker;
};

K2ScanerTcpSender::K2ScanerTcpSender(size_t id, std::vector<Interface> *localInterfaces): m_p(std::make_unique<Impl>()){

    m_p->id = id;
    m_p->localInterfaces = localInterfaces;
    m_p->packet.idGrabber = static_cast<std::uint8_t>(m_p->id);
    m_p->data.resize(sizeof(K2TcpPacket));

    std::int32_t size = static_cast<std::int32_t>(m_p->data.size());
    auto sd = reinterpret_cast<char*>(&size);
    std::copy(sd, sd + 4, m_p->sizeData.begin());
}

K2ScanerTcpSender::~K2ScanerTcpSender(){
    disable_writing();
}


auto K2ScanerTcpSender::enable_writing(std::string targetName, int writingPort, size_t readingInterface, int readingPort) -> bool{

    std::unique_lock<std::mutex> lock(m_p->locker);

    m_p->readingInterface = readingInterface;
    clean_socket();

    auto writingPortStr = std::to_string(writingPort);

    std::string solvedAddress ="";
    using namespace boost::asio;
    try{

        m_p->socket = std::make_unique<boost::asio::ip::tcp::socket>(m_p->ioService);
        boost::asio::ip::tcp::resolver resolver(m_p->ioService);

        if(ip::host_name() == targetName){
            auto endpoint = resolver.resolve(tcp::resolver::query("localhost", writingPortStr, tcp::resolver::canonical_name));
            boost::asio::connect(*m_p->socket.get(), endpoint);
            solvedAddress = endpoint->endpoint().address().to_string();
        }else{
            auto endpoint = resolver.resolve(tcp::resolver::query(targetName, writingPortStr));
            boost::asio::connect(*m_p->socket.get(), endpoint);
            solvedAddress = endpoint->endpoint().address().to_string();
        }

    }catch (const boost::system::system_error&){
        error_signal("TcpSenderWorker:enable_writing");
        connection_state_signal(m_p->connectionValid = false);
        clean_socket();
        return false;
    }

    // send init command
    if(m_p->readingInterface < m_p->localInterfaces->size()){
        init_command(readingPort, (*m_p->localInterfaces)[to_size_t(m_p->readingInterface)].ipAddress);
    }else{
        error_signal("Invalid interface to be sent to grabber.");
        connection_state_signal(m_p->connectionValid = false);
        clean_socket();
        return false;
    }

    message_signal(std::string("Start writing: (") + solvedAddress.c_str() + ":"+ writingPortStr + ")");
    connection_state_signal(m_p->connectionValid = true);
    return true;
}

auto K2ScanerTcpSender::disable_writing() -> void{

    std::unique_lock<std::mutex> lock(m_p->locker);

    clean_socket();
    m_p->connectionValid = false;

    message_signal("Disconnected");
    connection_state_signal(m_p->connectionValid);
}

auto K2ScanerTcpSender::init_command(int targetWritingPort, std::string targetWritingIpAddress) -> void{

    m_p->packet.cmd = K2TcpCommand::Init;
    m_p->packet.writingPort = targetWritingPort;
    if(targetWritingIpAddress.size() < 46){
        std::copy(std::begin(targetWritingIpAddress), std::end(targetWritingIpAddress), m_p->packet.writingIpv6Address.data());
    }
    write_data();
}

auto K2ScanerTcpSender::open_camera_command(K2FrameRequest mode) -> void{

    std::unique_lock<std::mutex> lock(m_p->locker);

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::OpenCamera;
    m_p->packet.frameMode = mode;
    write_data();
}

auto K2ScanerTcpSender::close_camera_command() -> void{

    std::unique_lock<std::mutex> lock(m_p->locker);

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::CloseCamera;
    write_data();
}

auto K2ScanerTcpSender::update_settings_command(K2Settings parameters) -> void{

    std::unique_lock<std::mutex> lock(m_p->locker);

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::UpdateParameters;
    m_p->packet.p   = parameters;
    write_data();
}

auto K2ScanerTcpSender::ask_new_frame_command() -> void{

    std::unique_lock<std::mutex> lock(m_p->locker);

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::AskNewFrame;
    m_p->packet.t   = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    m_p->packet.sizeUdpPackets =static_cast<uint16_t>(m_p->sizeUdpPackets);
    write_data();
}

auto K2ScanerTcpSender::set_size_udp_packets(int size) -> void{
    std::unique_lock<std::mutex> lock(m_p->locker);
    m_p->sizeUdpPackets = size;
}

auto K2ScanerTcpSender::reading_interface() const -> size_t{
    return m_p->readingInterface;
}

auto K2ScanerTcpSender::clean_socket() -> void{

    if(m_p->socket){
        try{
            m_p->ioService.stop();
            m_p->socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send);
            std::this_thread::sleep_for (std::chrono::milliseconds(300));
            m_p->socket->close();
        }catch (const boost::system::system_error&){
            error_signal("TcpSender::clean_socket");
        }
    }
    m_p->socket = nullptr;
}

auto K2ScanerTcpSender::write_data() -> void{

    // set byte array
    std::copy(reinterpret_cast<char*>(&m_p->packet), reinterpret_cast<char*>(&m_p->packet) + sizeof(K2TcpPacket), m_p->data.begin());
    try{
        m_p->socket->send(boost::asio::buffer(m_p->sizeData.data(),m_p->sizeData.size()));
        m_p->socket->send(boost::asio::buffer(m_p->data.data(),m_p->data.size()));
    }catch (const boost::system::system_error& error){

        error_signal("TcpSender::write_data");
        switch(error.code().value()){
            case boost::asio::error::connection_aborted:
                connection_state_signal(m_p->connectionValid = false);
            break;
            default:
            break;
        }
    }
}
