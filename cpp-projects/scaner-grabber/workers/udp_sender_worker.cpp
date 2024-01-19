
/*******************************************************************************
** Toolset-scaner-grabber                                                     **
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

#include "udp_sender_worker.hpp"

// std
#include <execution>
#include <iostream>
#include <chrono>

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

// Qt
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QDateTime>

// base
#include "camera/impl/k2_network.hpp"
#include "utility/types.hpp"

// qt-utility
#include "qt_logger.hpp"

using namespace std::chrono;
using namespace boost::asio::ip;

using namespace tool::network;
using namespace tool::geo;
using namespace tool::camera;

struct UdpSenderWorker::Impl{

    std::uint8_t grabberId = 0;
//    std::mutex locker;

    // io
    boost::asio::io_service ioService;
    std::unique_ptr<udp::socket> socket = nullptr;
    udp::endpoint endpoint;    
    QString writingAddress;
    int writingPort;
    std::atomic_bool connectionValid = false;

    // camera
    std::shared_ptr<K2Frame> lastFrame = nullptr;

    // buffer data to send
    uint32_t frameId = 0;
    std::vector<unsigned char> bufferData;
    std::vector<QByteArray> packets;
    K2UdpHeader header;
};

UdpSenderWorker::UdpSenderWorker() : m_p(std::make_unique<Impl>()){
}

UdpSenderWorker::~UdpSenderWorker(){
    disable_writing();
}

void UdpSenderWorker::set_grabber_id(uint8_t id){
    m_p->grabberId = id;
}

void UdpSenderWorker::enable_writing(QString writingAddress, int writingPort){

//    std::unique_lock<std::mutex> lock(m_p->locker);
    m_p->writingAddress = writingAddress;
    m_p->writingPort    = writingPort;

    clean_socket();
    if(!init_socket(writingAddress, static_cast<quint16>(writingPort))){
        clean_socket();
        emit connected_state_signal(m_p->writingAddress, m_p->writingPort,  m_p->connectionValid = false);
    }else{
        emit connected_state_signal(m_p->writingAddress, m_p->writingPort, m_p->connectionValid = true);
    }
}

void UdpSenderWorker::disable_writing(){

//    std::unique_lock<std::mutex> lock(m_p->locker);
    clean_socket();
    emit connected_state_signal(m_p->writingAddress, m_p->writingPort, m_p->connectionValid = false);
}

bool UdpSenderWorker::init_socket(QString address, quint16 port){

    try{
        // init socket
        m_p->socket   = std::make_unique<udp::socket>(m_p->ioService);

        // init endpoint
        m_p->endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address.toStdString()), port);

        // open socket
        m_p->socket->open(udp::v4());
        m_p->socket->set_option(boost::asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{ 5 });
        m_p->socket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
        m_p->socket->set_option(udp::socket::send_buffer_size(9000*1000));
        m_p->socket->set_option(udp::socket::receive_buffer_size(9000*1000));


    }catch (const boost::system::system_error& error){
        QtLogger::error(QSL("[UDP] init_socket: ") % QString(error.what()));
        return false;
    }

    return true;
}

void UdpSenderWorker::clean_socket(){

    if(m_p->socket){
        try{
            m_p->ioService.stop();
            if(m_p->socket->is_open()){
                m_p->socket->close();
            }
        }catch (const boost::system::system_error& error){
            QtLogger::error(QSL("[UDP] clean_socket: ") % QString(error.what()));
        }
    }
    m_p->socket = nullptr;
}


void UdpSenderWorker::send_frame(K2TcpPacket command, std::shared_ptr<K2Frame> frame){

    if(!m_p->socket || !m_p->connectionValid){
        QtLogger::error("Can't send frame, socket valid.");
        return;
    }

    // get last frame
    m_p->lastFrame = frame;

    if(m_p->lastFrame == nullptr){ // no frame to send
        return;
    }

    // start timer
    auto timeStart = high_resolution_clock::now();

    // check mode
    if(m_p->lastFrame->mode != command.frameMode){
        QtLogger::error("TCP packet mode and last frame mode different.");
        return;
    }

    // store camera id
    m_p->header.idGrabber = m_p->grabberId;

    // set intrinsics
    m_p->header.intrinsics1 = m_p->lastFrame->intrinsics[0]; // focalLengthX
    m_p->header.intrinsics2 = m_p->lastFrame->intrinsics[1]; // focalLengthY
    m_p->header.intrinsics3 = m_p->lastFrame->intrinsics[2]; // principalPointX
    m_p->header.intrinsics4 = m_p->lastFrame->intrinsics[3]; // principalPointY

    // data to send
    char *data = nullptr;

    const auto mode = m_p->lastFrame->mode;
    if(mode == K2FrameRequest::compressed_color_1920x1080 || mode == K2FrameRequest::compressed_color_512x424){

        data = reinterpret_cast<char*>(m_p->lastFrame->compressedImage.data());
        m_p->header.sizeFullData = m_p->lastFrame->jpegColorSize;

    }else if(mode == K2FrameRequest::depth_512x424){

        data = reinterpret_cast<char*>(m_p->lastFrame->depthData->data());
        m_p->header.sizeFullData = m_p->lastFrame->depthData->size()*sizeof(std::uint16_t);

    }else if(mode == K2FrameRequest::infra_512x424 || mode == K2FrameRequest::long_exposure_infra_512x424){

        data = reinterpret_cast<char*>(m_p->lastFrame->infraData->data());
        m_p->header.sizeFullData = m_p->lastFrame->infraData->size()*sizeof(std::uint16_t);

    }else if(mode == K2FrameRequest::compressed_color_cloud || mode == K2FrameRequest::compressed_color_mesh){

        // store compressed data sizes
        m_p->header.size1 = m_p->lastFrame->compressedDepthSize;
        m_p->header.size2 = m_p->lastFrame->jpegColorSize;

        const size_t sizeDepth16    = m_p->lastFrame->compressedDepthSize*2;
        const size_t sizeDepth8     = sizeDepth16*2;
        const size_t sizeColor8     = m_p->lastFrame->jpegColorSize;

        const size_t bufferSize8 = sizeDepth8 + sizeColor8 + k2_bodies_joints_data_size8;
        if(m_p->bufferData.size() < bufferSize8){
            m_p->bufferData.resize(bufferSize8);
        }

        // depth
        std::copy(
            m_p->lastFrame->compressedDepthData.data(),
            m_p->lastFrame->compressedDepthData.data() + sizeDepth16,
            reinterpret_cast<std::uint16_t*>(m_p->bufferData.data())
        );

        // color
        std::copy(
            m_p->lastFrame->compressedImage.data(),
            m_p->lastFrame->compressedImage.data() + m_p->lastFrame->jpegColorSize,
            m_p->bufferData.data() + sizeDepth8
        );

        // joints
        size_t id = 0;
        K2Joint4x64 *bodyP = reinterpret_cast<K2Joint4x64*>(&m_p->bufferData[sizeDepth8 + sizeColor8]);
        for(const auto &body : m_p->lastFrame->bodiesData){
            for(const auto &joint : body.joints){
                bodyP[id++] = joint_to_int(body, joint.first, joint.second);
            }
        }

        m_p->header.sizeFullData = bufferSize8 * sizeof (std::uint8_t);
        data = reinterpret_cast<char*>(m_p->bufferData.data());

    }else{
        QtLogger::error("Camera mode not managed.");
        return;
    }


    // define packet sizes
    const int sizePacket       = command.sizeUdpPackets;
    const int sizePacketHeader = sizeof (K2UdpHeader);
    const int sizePacketData   = sizePacket - sizePacketHeader;

    unsigned short nbPacketsNeeded = static_cast<unsigned short>(m_p->header.sizeFullData/to_size_t(sizePacketData));
    const unsigned short rest      = static_cast<unsigned short>(m_p->header.sizeFullData%to_size_t(sizePacketData));
    if(rest > 0){
        ++nbPacketsNeeded;
    }

    // resize packets array
    if(m_p->packets.size() < nbPacketsNeeded){
       m_p->packets.resize(nbPacketsNeeded);
       for(auto &p : m_p->packets){
           if(p.size() != sizePacket){
               p.resize(sizePacket);
           }
       }
    }

    // init header
    m_p->header.frameId        = m_p->frameId;
    m_p->header.frameMode      = command.frameMode;
    m_p->header.offset         = 0;
    m_p->header.totalNbPackets = nbPacketsNeeded;

    // fill packets with data
    for(size_t idP = 0; idP < m_p->header.totalNbPackets; ++idP){

        // set packet timestamp relative to kinect time
        m_p->header.timeStamp = (std::chrono::high_resolution_clock::now().time_since_epoch().count()-m_p->lastFrame->timeStampGetFrame);

        // get current data paquet size and id
        m_p->header.sizePacketData = to_size_t(sizePacketData);
        if(idP == nbPacketsNeeded-1){ // last case
            if(rest > 0){
                m_p->header.sizePacketData = rest;
            }
        }
        m_p->header.idPacket = static_cast<uint16_t>(idP);

        // write header into packet
        std::copy(reinterpret_cast<char*>(&m_p->header), reinterpret_cast<char*>(&m_p->header) + sizeof(K2UdpHeader), m_p->packets[idP].begin());

        // write data into packet
//        std::copy(data + m_p->header.offset, data + m_p->header.offset + m_p->header.sizePacketData, m_p->packets[idP].begin() + sizeof(UdpHeader));
        std::move(data + m_p->header.offset, data + m_p->header.offset + m_p->header.sizePacketData, m_p->packets[idP].begin() + sizeof(K2UdpHeader));
//        std::move(data + header.offset, data + header.offset + header.sizePacketData, m_p->packets[idP].begin() + sizeof(UdpHeader));

        // increment data offset
        m_p->header.offset += m_p->header.sizePacketData;
    }

    // send packets
    size_t countFailure = 0;
    for(size_t idP = 0; idP < m_p->header.totalNbPackets; ++idP){
        size_t bytesNbSent=0;
        try{
            bytesNbSent = m_p->socket->send_to(boost::asio::buffer(m_p->packets[idP].data(), static_cast<size_t>(sizePacket)), m_p->endpoint);
        } catch (const boost::system::system_error& error) {
            QtLogger::error(QStringLiteral("[UDP] send_frame: ") + QString(error.what()));
            ++countFailure;
        }
        emit nb_bytes_sent_signal(QDateTime::currentMSecsSinceEpoch(), bytesNbSent);
    }

    // increment frame counter
    ++m_p->frameId;

    emit packets_failure_signal(countFailure);
    emit frame_sent_signal(duration_cast<microseconds>(high_resolution_clock::now() - timeStart).count());
}


#include "moc_udp_sender_worker.cpp"
