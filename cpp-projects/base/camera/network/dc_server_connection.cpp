

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

#include "dc_server_connection.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

using namespace std::chrono;
using namespace tool::network;
using namespace tool::camera;

DCServerConnection::~DCServerConnection(){
    clean();
}

auto DCServerConnection::initialize(size_t id, const ReadSendNetworkInfos &infos) -> bool {

    m_id    = id;
    m_infos = infos;    

    // init reader
    if(!udpReader.init_socket(m_infos.readingAdress, m_infos.readingPort)){
        Logger::error("DCServerConnection: Cannot init udp reader.\n");
        return false;
    }
    udpReader.start_reading();

    // init sender
    if(!udpSender.init_socket(m_infos.sendingAdress, std::to_string(m_infos.sendingPort))){
        Logger::error("DCServerConnection: Cannot init udp sender.\n");
        return false;
    }

    // connections
    udpReader.synchro_signal.connect([&](std::int64_t timestamp){
        if(m_useGlobalsSignals){
            synchro_signal(m_id, timestamp);
        }else{
            grabber_synchro_signal(timestamp);
        }
    });
    udpReader.feedback_signal.connect(&DCServerConnection::receive_feedback, this);
    udpReader.compressed_frame_signal.connect(&DCServerConnection::receive_compressed_frame, this);

    return true;
}

auto DCServerConnection::clean() -> void {

    udpReader.feedback_signal.disconnect(&DCServerConnection::receive_feedback, this);
    udpReader.compressed_frame_signal.disconnect(&DCServerConnection::receive_compressed_frame, this);
    m_grabberConnectedToServer = false;

    // manager size
    if(udpReader.is_reading()){
        udpReader.stop_reading();
    }    
    udpReader.clean_socket();
    udpSender.clean_socket();
}

auto DCServerConnection::init_connection_with_grabber() -> void {

    Logger::message(std::format("DCServerConnection: infos read: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}].\n",
        m_infos.idReadingInterface, m_infos.readingAdress, m_infos.readingPort, m_infos.sendingAdress, m_infos.sendingPort));

    m_initTs = udpSender.send_init_message(
        UdpNetworkSendingSettings(m_infos.readingAdress, m_infos.readingPort, maxSizeUpdMessage)
    ).currentPacketTime;
}

auto DCServerConnection::disconnect_grabber() -> void {
    udpSender.send_command_message(Command::Disconnect);
}

auto DCServerConnection::quit_grabber() -> void {
    udpSender.send_command_message(Command::Quit);
}

auto DCServerConnection::shutdown_grabber_computer() -> void {
    udpSender.send_command_message(Command::Shutdown);
}

auto DCServerConnection::restart_grabber_computer() -> void {
    udpSender.send_command_message(Command::Restart);
}

auto DCServerConnection::update_device_list() -> void {
    udpSender.send_command_message(Command::UpdateDeviceList);
}

auto DCServerConnection::receive_feedback(Header h, UdpMonoPacketMessage<Feedback> message) -> void {

    // from reader thread:
    m_totalReceivedBytes += h.totalSizeBytes;

    switch(message.data.feedback){
    case FeedbackType::message_received:
        if(message.data.receivedMessageType == MessageType::init_network_infos){
            m_grabberConnectedToServer = true;
        }
        break;
    case FeedbackType::disconnect:
        Logger::message("grabbed disconnected\n");
        m_grabberConnectedToServer = false;
        break;
    case FeedbackType::quit:
        m_grabberConnectedToServer = false;
        break;
    case FeedbackType::shutdown:
        m_grabberConnectedToServer = false;
        break;
    case FeedbackType::restart:
        m_grabberConnectedToServer = false;
        break;
    default:
        break;
    }

    if(m_useGlobalsSignals){
        feedback_signal(m_id, std::move(message.data));
    }else{
        grabber_feedback_signal(std::move(message.data));
    }
}

auto DCServerConnection::receive_compressed_frame(Header h, std::shared_ptr<camera::DCCompressedFrame> compressedFrame) -> void {

    // from reader thread:
    m_totalReceivedBytes += h.totalSizeBytes;
    if(compressedFrame){
        if(m_useGlobalsSignals){
            compressed_frame_signal(m_id, compressedFrame);
        }else{
            grabber_compressed_frame_signal(compressedFrame);
        }
    }
}




