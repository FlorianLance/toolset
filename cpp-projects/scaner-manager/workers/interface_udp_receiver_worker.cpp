
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

#include "interface_udp_receiver_worker.hpp"

// Qt
#include <QCoreApplication>
#include <QThread>
#include <QUdpSocket>
#include <QDateTime>
#include <QByteArray>
#include <QTimer>

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// base
#include "camera/kinect2/k2_network.hpp"

// qt_utility
#include "qt_ui.hpp"
#include "qt_str.hpp"
#include "qt_process.hpp"

using namespace boost::asio;
using namespace boost::asio::detail;
using namespace boost::asio::ip;

using namespace tool::network;
using namespace tool::ui;
using namespace tool::camera;


struct InterfaceUdpReceiverWorker::Impl{

    size_t idInterface;

    // udp connection
    io_service ioService;
    std::unique_ptr<udp::socket> socket = nullptr;
    udp::endpoint senderEndpoint;

    // received
    K2UdpHeader header;
    size_t currentBufferId = 0;
    size_t buffersNb = 10000;
    std::vector<QByteArray> buffers;
//    std::vector<std::unique_ptr<std::mutex>> lockers;

    network::Interface interface;
    std::atomic_bool connectionValid = false;

//    std::mutex locker;
};

InterfaceUdpReceiverWorker::InterfaceUdpReceiverWorker(size_t id, Interface interface) : m_p(std::make_unique<Impl>()){

    m_p->idInterface = id;
    m_p->interface = interface;
    connect(this, &InterfaceUdpReceiverWorker::start_reading_signal, this, &InterfaceUdpReceiverWorker::read_data);

    // init buffers
    m_p->buffers.resize(m_p->buffersNb);
    for(auto &buffer : m_p->buffers){
        buffer.resize(9000);
//        m_p->lockers.emplace_back(std::make_unique<std::mutex>());
    }
}

InterfaceUdpReceiverWorker::~InterfaceUdpReceiverWorker(){
    disable_reading();
    m_p = nullptr;
}

size_t InterfaceUdpReceiverWorker::id() const{
    return m_p->idInterface;
}

Interface InterfaceUdpReceiverWorker::interface() const{
    return m_p->interface;
}

void InterfaceUdpReceiverWorker::enable_reading(int readingPort){

//    std::unique_lock<std::mutex> lock(m_p->locker);
    clean_socket();

    try {
        // open socket
        m_p->socket   = std::make_unique<udp::socket>(m_p->ioService);
        m_p->socket->open(udp::v4());
        m_p->socket->set_option(socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{500});
        m_p->socket->set_option(udp::socket::reuse_address(true));
        m_p->socket->set_option(udp::socket::receive_buffer_size(9000*10));

        // bind socket
        m_p->senderEndpoint = udp::endpoint(address::from_string(m_p->interface.ipAddress), static_cast<unsigned short>(readingPort));
        m_p->socket->bind(m_p->senderEndpoint);

    }catch (const boost::system::system_error& error){
        clean_socket();
        emit message_signal(QStringLiteral("[UDP] init_reading_connection: ") % QString(error.what()));
        return;
    }

    emit connected_state_signal(m_p->connectionValid = m_p->socket->is_open());

    if(m_p->connectionValid){
        emit message_signal(QStringLiteral("[UDP] Start reading: (") + (m_p->interface.ipAddress).c_str() + ":" + QString::number(readingPort) + ")" );
        emit start_reading_signal();
    }else{
        emit message_signal(QStringLiteral("[UDP] Open failure"));
    }
}

void InterfaceUdpReceiverWorker::disable_reading(){

    {
//        std::unique_lock<std::mutex> lock(m_p->locker);
        clean_socket();
    }

    emit message_signal(QStringLiteral("[UDP] Disconnected"));
}

void InterfaceUdpReceiverWorker::clean_socket(){

    if(m_p->socket){
        try {
            m_p->ioService.stop();
            m_p->socket->shutdown(udp::socket::shutdown_receive);
            wait_process(300);
            m_p->socket->close();
        }catch (const boost::system::system_error& error){
            emit message_signal(QStringLiteral("[UDP] clean_socket: ") + QString(error.what()));
        }
    }
    m_p->socket = nullptr;
    emit connected_state_signal(m_p->connectionValid = false);
}


void InterfaceUdpReceiverWorker::read_data(){

    struct RawFrameReceived{
        std::vector<QByteArray*> buffers;
        size_t received  = 0;
        size_t totalToReceived = 0;
    };

    std::unordered_map<size_t, std::unordered_map<size_t,RawFrameReceived>> buffersToProcess;

    while(m_p->connectionValid){

        auto buffer = &m_p->buffers[m_p->currentBufferId];
        size_t nbBytesReceived = 0;
        try {
            // receive data from grabber
            udp::endpoint endP;
            nbBytesReceived = m_p->socket->receive_from(boost::asio::buffer(buffer->data(),static_cast<size_t>(buffer->size())), endP);
            m_p->currentBufferId = (m_p->currentBufferId + 1)%m_p->buffersNb;

        } catch (const boost::system::system_error& error) {

            if(error.code() == boost::asio::error::timed_out){
                emit timeout_packet_signal();
            }else{
                emit message_signal(QStringLiteral("[UDP] read_data: ") + error.what());
                emit connected_state_signal(m_p->connectionValid = false);
            }
            continue;
        }


        // retrieve header
        std::copy(buffer->begin(), buffer->begin() + sizeof (m_p->header), reinterpret_cast<char*>(&m_p->header));

        const size_t idGrabber      = m_p->header.idGrabber;
        const size_t idFrame      = m_p->header.frameId;

        if(buffersToProcess[idGrabber][idFrame].received == 0){
            // store local timestamp;
        }

        // packet received
        buffersToProcess[idGrabber][idFrame].buffers.emplace_back(buffer);
        buffersToProcess[idGrabber][idFrame].received++;
        buffersToProcess[idGrabber][idFrame].totalToReceived  = m_p->header.totalNbPackets;
        // TODO timestamp

        emit packet_received_signal(id(), m_p->header.idGrabber, idFrame);

        if(buffersToProcess[idGrabber][idFrame].totalToReceived == buffersToProcess[idGrabber][idFrame].received){            

            // all packets received
            emit all_packets_received_signal(id(), m_p->header.idGrabber, buffersToProcess[idGrabber][idFrame].buffers);
            buffersToProcess[idGrabber].erase(idFrame);

            // search for old frames
            std::vector<size_t> framesToRemove;
            for(auto &frame : buffersToProcess[idGrabber]){
                if(frame.first < idFrame){
                    framesToRemove.emplace_back(frame.first);
                }
            }

            // remove them
            for(const auto &frameToRemove : framesToRemove){
                size_t droppedPackets = buffersToProcess[idGrabber][idFrame].totalToReceived - buffersToProcess[idGrabber][idFrame].received;
                buffersToProcess[idGrabber].erase(frameToRemove);
                if(droppedPackets != 0){
                    emit frame_dropped_signal(id(), m_p->header.idGrabber, frameToRemove, droppedPackets);
                }
            }

            continue;
        }
    }
}

#include "moc_interface_udp_receiver_worker.cpp"


