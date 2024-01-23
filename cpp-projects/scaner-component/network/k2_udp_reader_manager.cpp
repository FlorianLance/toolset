/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#include "k2_udp_reader_manager.hpp"

// std
#include <thread>
#include <chrono>
#include <string>

// boost
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>

// base
#include "utility/time.hpp"
//#include "camera/kinect2/k2_network.hpp"

// scaner-lib
#include "k2_scaner_udp_reader.hpp"

using namespace std::chrono_literals;
using namespace std::chrono;
using namespace tool;
using namespace tool::net;
//using namespace tool::camera;

using boost::asio::ip::udp;
using boost::asio::ip::address;

struct K2UdpReaderManager::Impl{
    std::vector<std::unique_ptr<K2ScanerUdpReader>> readers;
};

K2UdpReaderManager::K2UdpReaderManager(const std::vector<Interface> &interfaces) : m_p(std::make_unique<Impl>()){

    // init interface readers and threads
    for(size_t ii = 0; ii < interfaces.size(); ++ii){
        m_p->readers.emplace_back(std::make_unique<K2ScanerUdpReader>(ii, interfaces[ii]));
    }

    // set connection for each reader
    for(size_t ii = 0; ii < m_p->readers.size(); ++ii){

        auto reader = m_p->readers[ii].get();
        reader->message_signal.connect([=](std::string message){
            message_signal("[UDP : interface " + std::to_string(reader->id()) +"] " + message);
        });
        reader->error_signal.connect([=](std::string error){
            message_signal("[UDP : interface " + std::to_string(reader->id()) +"] " + error);
        });

        reader->connected_state_signal.connect([=](bool state){
            connected_state_signal(reader->id(), state);
        });
        reader->timeout_packet_signal.connect([=](){
            timeout_signal(reader->id());
        });
        reader->packet_received_signal.connect([=](size_t idInterface, size_t idGrabber, size_t idFrame){
            packet_received_signal(idInterface, idGrabber, idFrame);
        });
        reader->frame_dropped_signal.connect([=](size_t idInterface, size_t idGrabber, size_t idFrame, size_t packetsDropped){
            frame_dropped_signal(idInterface, idGrabber, idFrame, packetsDropped);
        });
        reader->all_packets_received_signal.connect([=](size_t idInterface, size_t idGrabber, std::vector<std::vector<char>*> *buffers){
            all_packets_received_signal(idInterface, idGrabber, buffers);
        });
    }
}

K2UdpReaderManager::~K2UdpReaderManager(){
}

auto K2UdpReaderManager::enable_reading() -> bool{

    for(auto &reader : m_p->readers) {
        if(!reader->enable_reading(8888)){
            error_signal("[UDP] Cannot enable reading from interface " + reader->interface().ipAddress);
            return false;
        }
    }
    return true;
}

auto K2UdpReaderManager::disable_reading() -> void{

    for(auto &reader : m_p->readers) {
        reader->disable_reading();
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(50));
}

