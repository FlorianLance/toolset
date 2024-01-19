

/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#include "k2_scaner_udp_reader.hpp"

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// base
#include "camera/impl/k2_network.hpp"

using namespace boost::asio;
using namespace boost::asio::detail;
using namespace boost::asio::ip;

using namespace tool;
using namespace tool::network;
using namespace tool::camera;

struct K2ScanerUdpReader::Impl{

    size_t idInterface;

    // udp connection
    io_service ioService;
    std::unique_ptr<udp::socket> socket = nullptr;
    udp::endpoint senderEndpoint;

    // received
    K2UdpHeader header;
    size_t currentBufferId = 0;
    size_t buffersNb = 10000;
    std::vector<std::vector<char>> buffers;
    std::vector<std::unique_ptr<std::mutex>> lockers;

    network::Interface interface;
    std::atomic_bool connectionValid = false;

    std::thread thread;
};

K2ScanerUdpReader::K2ScanerUdpReader(size_t id, Interface interface) : m_p(std::make_unique<Impl>()){

    m_p->idInterface = id;
    m_p->interface   = interface;

    // init buffers
    m_p->buffers.resize(m_p->buffersNb);
    for(auto &buffer : m_p->buffers){
        buffer.resize(9000);
        m_p->lockers.emplace_back(std::make_unique<std::mutex>());
    }
}

K2ScanerUdpReader::~K2ScanerUdpReader(){
    disable_reading();
}

size_t K2ScanerUdpReader::id() const{
    return m_p->idInterface;
}

Interface K2ScanerUdpReader::interface() const{
    return m_p->interface;
}

auto K2ScanerUdpReader::enable_reading(int readingPort) -> bool{

    disable_reading();

    auto readingPortStr = std::to_string(readingPort);
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

    }catch (const boost::system::system_error&){
        connected_state_signal(m_p->connectionValid = false);
        error_signal("InterfaceUdpReader::init_reading_connection");
        return false;
    }

    connected_state_signal(m_p->connectionValid = m_p->socket->is_open());

    if(m_p->connectionValid){
        message_signal(std::string("Start reading: (") + (m_p->interface.ipAddress) + ":" + readingPortStr + ")" );

        // process current frame
        if(m_p->thread.joinable()){
            m_p->thread.join();
        }
        m_p->thread = std::thread(&K2ScanerUdpReader::read_data, this);

    }else{
        error_signal(std::string("Open failure"));
        return false;
    }

    return true;
}

auto K2ScanerUdpReader::disable_reading() -> void{

    // stop reading loop
    connected_state_signal(m_p->connectionValid = false);
    std::this_thread::sleep_for (std::chrono::milliseconds(20));

    // stop thread
    if(m_p->thread.joinable()){
        m_p->thread.join();
    }

    // clean
    clean_socket();

    message_signal("Disconnected");
}

auto K2ScanerUdpReader::clean_socket() -> void{

    if(m_p->socket){
        try {
            m_p->ioService.stop();
            m_p->socket->shutdown(udp::socket::shutdown_receive);
            std::this_thread::sleep_for (std::chrono::milliseconds(300));
            m_p->socket->close();
        }catch (const boost::system::system_error&){
            error_signal("InterfaceUdpReader::clean_socket");
        }
    }
    m_p->socket = nullptr;
}


auto K2ScanerUdpReader::read_data() -> void{

    struct RawFrameReceived{
        std::vector<std::vector<char>*> buffers;
        size_t received  = 0;
        size_t totalToReceived = 0;
        std::int64_t timeStamp = 0;
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
                timeout_packet_signal();
            }else{
                message_signal("InterfaceUdpReader::read_data");
                connected_state_signal(m_p->connectionValid = false);
            }
            continue;
        }
        static_cast<void>(nbBytesReceived);


        // retrieve header
        std::copy(buffer->begin(), buffer->begin() + sizeof (m_p->header), reinterpret_cast<char*>(&m_p->header));

        const size_t idGrabber  = m_p->header.idGrabber;
        const size_t idFrame    = m_p->header.frameId;

        // packet received
        buffersToProcess[idGrabber][idFrame].buffers.emplace_back(buffer);
        buffersToProcess[idGrabber][idFrame].received++;
        buffersToProcess[idGrabber][idFrame].totalToReceived  = m_p->header.totalNbPackets;
        buffersToProcess[idGrabber][idFrame].timeStamp        = m_p->header.timeStamp;
        packet_received_signal(id(), idGrabber, idFrame);

        if(buffersToProcess[idGrabber][idFrame].totalToReceived == buffersToProcess[idGrabber][idFrame].received){

            // all packets received
            all_packets_received_signal(id(), idGrabber, &buffersToProcess[idGrabber][idFrame].buffers);
            // buffersToProcess[idGrabber].erase(idFrame);

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
                    frame_dropped_signal(id(), idGrabber, frameToRemove, droppedPackets);
                }
            }
        }
    }
}

