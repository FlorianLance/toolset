
/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/


#include "k2_grabber_controller.hpp"

// std
#include <fstream>

// local
#include "utility/logger.hpp"
#include "utility/format.hpp"

using namespace tool;
using namespace tool::scan;
using namespace tool::network;
using namespace tool::camera;
using namespace tool::geo;

K2GrabberController::K2GrabberController(K2UdpReaderManager *udpReaderManager, std::vector<Interface> *localInterfaces, size_t id, K2GrabberTargetInfo info) : idC(id), m_info(info){

    // init workers
    m_udpReaderManager  = udpReaderManager;
    m_tcpSender         = std::make_unique<K2ScanerTcpSender>(id, localInterfaces);
    m_dataProcess       = std::make_unique<K2ProcessData>(id);
    m_frameReader       = std::make_unique<K2FrameReader>();

    // set connections
    init_connections();   
}

void K2GrabberController::init_connections(){

    // from tcp
    m_tcpSender->connection_state_signal.connect([&](bool state){
        writing_connected_state_signal(idC, state);
    });
    m_tcpSender->message_signal.connect([&](std::string message){
        Logger::message(std::format("[TCP : id {}] {}", idC, message));
    });
    m_tcpSender->error_signal.connect([&](std::string error){
        Logger::error(std::format("[TCP : id {}] {}", idC, error));
    });

    // from udp manager
    m_udpReaderManager->packet_received_signal.connect([&](size_t idInterface, size_t idGrabber, size_t){

        if(idInterface == m_tcpSender->reading_interface() && idGrabber == idC){
            packet_received_signal(idC, ++m_packetsReceived);
        }
    });
    m_udpReaderManager->frame_dropped_signal.connect([&](size_t idInterface, size_t idGrabber, size_t idFrame, size_t packetsDropped){
        if(idInterface == m_tcpSender->reading_interface() && idGrabber == idC){

            // send dropped frame info
            m_packetsLost += packetsDropped;
            frame_dropped_signal(idC, idFrame, ++m_framesLost, m_packetsLost);
        }
    });
    m_udpReaderManager->all_packets_received_signal.connect([&](size_t idInterface, size_t idGrabber, std::vector<std::vector<char>*> *packets){

        if(idInterface == m_tcpSender->reading_interface() &&idGrabber == idC){

            if(packets->size() == 0){
                return;
            }

            // read current frame
            if(m_frameReaderT != nullptr){
                if(m_frameReaderT->joinable()){
                    m_frameReaderT->join();
                }
            }

            m_frameReaderT = std::make_unique<std::thread>(&K2FrameReader::process_packets, m_frameReader.get(), packets);
        }
    });    
    m_udpReaderManager->message_signal.connect([&](std::string message){
        Logger::message(message);
    });
    m_udpReaderManager->timeout_signal.connect([&](size_t interface){
        (void)interface;
//        message_signal("Timeout from interface " + std::to_string(interface));
    });
    m_udpReaderManager->connected_state_signal.connect([&](size_t interface, bool state){
        reading_connected_state_signal(interface,state);
    });

    // from frame reader
    m_frameReader->frame_received_signal.connect([&](K2UdpHeader header, std::shared_ptr<camera::K2Frame> frame){

        // process current frame
        if(m_dataProcessT != nullptr){
            if(m_dataProcessT->joinable()){
                m_dataProcessT->join();
            }
        }
        m_dataProcessT = std::make_unique<std::thread>(&K2ProcessData::process_data_from_udp_frame, m_dataProcess.get(), header, frame);
    });
    m_frameReader->stack_trace_message_signal.connect([&](std::string trace){
        stack_trace_message_signal(trace + "_c" + std::to_string(idC));
    });

    // from data process
    m_dataProcess->update_cloud_data_signal.connect([&](camera::K2CloudDisplayData *cloudData){
        update_cloud_data_signal(idC, cloudData);
    });
    m_dataProcess->update_mesh_data_signal.connect([&](camera::K2MeshDisplayData *meshData){
        update_mesh_data_signal(idC, meshData);
    });
    m_dataProcess->update_bodies_data_signal.connect([&](camera::K2BodiesDisplayData *bodiesData){
        update_bodies_data_signal(idC, bodiesData);
    });
    m_dataProcess->end_processing_signal.connect([&](std::int64_t timeStampGet, std::int64_t timeProcessing){
        frame_processed_signal(idC, ++m_framesReceived, timeStampGet, timeProcessing);
    });
    m_dataProcess->stack_trace_message_signal.connect([&](std::string trace){
        stack_trace_message_signal(std::format("{}_c{}", trace, idC));
    });

    update_size_udp_packet(9000);
}


void K2GrabberController::quit_threads(){

    m_tcpSender->disable_writing();

    if(m_frameReaderT != nullptr){
        if(m_frameReaderT->joinable()){
            m_frameReaderT->join();
        }
        m_frameReaderT = nullptr;
    }
    if(m_dataProcessT != nullptr){
        if(m_dataProcessT->joinable()){
            m_dataProcessT->join();
        }
        m_dataProcessT = nullptr;
    }
}

bool K2GrabberController::connect_grabber(){

    if(!m_tcpSender->enable_writing(m_info.name, m_info.targetWritingPort, to_size_t(m_info.targetPreferredInterface), m_info.targetReadingPort)){
        Logger::error(std::format("Cannot connect to grabber {}.", idC));
        return false;
    }
    return true;
}

void K2GrabberController::disconnect_grabber(){
    m_tcpSender->disable_writing();
}

void K2GrabberController::open_camera_mode(K2FrameRequest cameraMode){
    m_tcpSender->open_camera_command(cameraMode);
}

void K2GrabberController::close_camera_mode(){
    m_tcpSender->close_camera_command();
}

void K2GrabberController::set_camera_parameters(K2Settings parameters){
    m_tcpSender->update_settings_command(parameters);
}

void K2GrabberController::update_size_udp_packet(int size){
    m_tcpSender->set_size_udp_packets(size);
}

void K2GrabberController::ask_new_frame(){
    m_tcpSender->ask_new_frame_command();
}
