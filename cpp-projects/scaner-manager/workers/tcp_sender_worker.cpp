
/*******************************************************************************
** Toolset-scaner-manager                                                     **
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

#include "tcp_sender_worker.hpp"

// Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QByteArray>
#include <QHostAddress>
#include <QDataStream>

// std
#include <thread>
#include <chrono>

// boost
#include <boost/asio.hpp>

// base
#include "camera/kinect2/k2_network.hpp"
#include "network/network_utility.hpp"
#include "utility/types.hpp"

// qt-utility
#include "qt_str.hpp"

using boost::asio::ip::tcp;
using boost::asio::ip::address;

using namespace std::chrono_literals;

using namespace tool;
using namespace tool::network;
using namespace tool::camera;

struct TcpSenderWorker::Impl{

    size_t id;

    // TCP
    boost::asio::io_service ioService;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;
    K2TcpPacket packet;

    // data
    std::array<std::uint8_t, 4> sizeData;
    std::vector<std::uint8_t> data;
    int sizeUdpPackets = 512;

    std::vector<network::Interface> *localInterfaces = nullptr;
    size_t readingInterface = 0;

    bool connectionValid = false;

};

TcpSenderWorker::TcpSenderWorker(size_t id, std::vector<Interface> *localInterfaces) : m_p(std::make_unique<Impl>()){

    m_p->id = id;
    m_p->localInterfaces = localInterfaces;
    m_p->packet.idGrabber = static_cast<std::uint8_t>(m_p->id);
    m_p->data.resize(sizeof(K2TcpPacket));

    std::int32_t size = static_cast<std::int32_t>(m_p->data.size());
    auto sd = reinterpret_cast<char*>(&size);
    std::copy(sd, sd + 4, m_p->sizeData.begin());
}

TcpSenderWorker::~TcpSenderWorker(){
    disable_writing();
}

void TcpSenderWorker::enable_writing(QString targetName, int writingPort, int readingInterface, int readingPort){

    m_p->readingInterface = to_size_t(readingInterface);
    clean_socket();   

    std::string solvedAddress;
    using namespace boost::asio;       
    try{        

        m_p->socket = std::make_unique<boost::asio::ip::tcp::socket>(m_p->ioService);
        boost::asio::ip::tcp::resolver resolver(m_p->ioService);

        if(ip::host_name() == targetName.toStdString()){
            auto endpoint = resolver.resolve(tcp::resolver::query("localhost", QString::number(writingPort).toStdString(), tcp::resolver::canonical_name));
            boost::asio::connect(*m_p->socket.get(), endpoint);
            solvedAddress = endpoint->endpoint().address().to_string();
        }else{
            auto endpoint = resolver.resolve(tcp::resolver::query(targetName.toStdString(), QString::number(writingPort).toStdString()));
            boost::asio::connect(*m_p->socket.get(), endpoint);
            solvedAddress = endpoint->endpoint().address().to_string();
        }

    }catch (const boost::system::system_error& error){
        emit message_signal(QSL("[TCP] TcpSenderWorker:enable_writing: ") % QString(solvedAddress.c_str()) % QSL(" ") % QString(error.what()));
        clean_socket();

        return;
    }

    // send init command
    if(m_p->readingInterface < m_p->localInterfaces->size()){
        init_command(readingPort, (*m_p->localInterfaces)[to_size_t(m_p->readingInterface)].ipAddress);
    }else{        
        emit message_signal("[TCP] Invalid interface to be sent to grabber.");
        clean_socket();
        return;
    }

    emit message_signal(QString("[TCP] Start writing: (") % QString(solvedAddress.c_str()) % QSL(":") % QString::number(writingPort) % QSL(")"));
    emit connection_state_signal(m_p->connectionValid = true);
}

void TcpSenderWorker::disable_writing(){

    clean_socket();
    emit message_signal("[TCP] Disconnected");
}

void TcpSenderWorker::init_command(int targetWritingPort, std::string targetWritingIpAddress){    

    m_p->packet.cmd = K2TcpCommand::Init;
    m_p->packet.writingPort = targetWritingPort;
    if(targetWritingIpAddress.size() < 46){
        std::copy(std::begin(targetWritingIpAddress), std::end(targetWritingIpAddress), m_p->packet.writingIpv6Address.data());
    }
    write_data();
}

void TcpSenderWorker::open_camera_command(K2FrameRequest mode){

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::OpenCamera;
    m_p->packet.frameMode = mode;
    write_data();
}

void TcpSenderWorker::close_camera_command(){

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::CloseCamera;
    write_data();
}

void TcpSenderWorker::update_settings_command(K2Settings p){

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::UpdateParameters;
    m_p->packet.p   = p;
    write_data();
}

void TcpSenderWorker::ask_new_frame_command(){

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    m_p->packet.cmd = K2TcpCommand::AskNewFrame;
    m_p->packet.t   = QDateTime::currentMSecsSinceEpoch();
    m_p->packet.sizeUdpPackets =static_cast<uint16_t>(m_p->sizeUdpPackets);
    write_data();
}

void TcpSenderWorker::ask_to_save_settings_file_command(){

    if(!m_p->socket || !m_p->connectionValid){
        return;
    }

    emit message_signal("[TCP] AskToSaveSettingsFile");
    m_p->packet.cmd = K2TcpCommand::AskToSaveSettingsFile;
    m_p->packet.t   = QDateTime::currentMSecsSinceEpoch();
    m_p->packet.sizeUdpPackets =static_cast<uint16_t>(m_p->sizeUdpPackets);
    write_data();
}

void TcpSenderWorker::set_size_udp_packets(int size){
    m_p->sizeUdpPackets = size;
}

void TcpSenderWorker::shutdown_computer(){
    m_p->packet.cmd = K2TcpCommand::ShutdownComputer;
    m_p->packet.t   = QDateTime::currentMSecsSinceEpoch();
    m_p->packet.sizeUdpPackets =static_cast<uint16_t>(m_p->sizeUdpPackets);
    write_data();
}

void TcpSenderWorker::restart_computer(){
    m_p->packet.cmd = K2TcpCommand::RestartComputer;
    m_p->packet.t   = QDateTime::currentMSecsSinceEpoch();
    m_p->packet.sizeUdpPackets =static_cast<uint16_t>(m_p->sizeUdpPackets);
    write_data();
}

int TcpSenderWorker::reading_interface() const{
    return static_cast<int>(m_p->readingInterface);
}

void TcpSenderWorker::clean_socket(){

    if(m_p->socket != nullptr){
        try{
            m_p->ioService.stop();
            m_p->socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send);            
            m_p->socket->close();
        }catch (const boost::system::system_error& error){
            emit message_signal(QString("[TCP] TcpSenderWorker:clean_socket: ") + error.what());
        }
    }
    m_p->socket = nullptr;
    emit connection_state_signal(m_p->connectionValid = false);
}

void TcpSenderWorker::write_data(){

    // set byte array
    std::copy(reinterpret_cast<char*>(&m_p->packet), reinterpret_cast<char*>(&m_p->packet) + sizeof(K2TcpPacket), m_p->data.begin());
    try{
        m_p->socket->send(boost::asio::buffer(m_p->sizeData.data(),m_p->sizeData.size()));
        m_p->socket->send(boost::asio::buffer(m_p->data.data(),m_p->data.size()));
    }catch (const boost::system::system_error& error){

        emit message_signal(QString("[TCP] TcpSenderWorker:write_data: ") +error.what());
        switch(error.code().value()){
            case boost::asio::error::connection_aborted:
                emit connection_state_signal(m_p->connectionValid = false);
            break;
            default:
            break;
        }
    }
}

#include "moc_tcp_sender_worker.cpp"
