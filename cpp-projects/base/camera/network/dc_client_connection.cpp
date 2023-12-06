

/*******************************************************************************
** Toolset-dc-grabber                                                  **
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

#include "dc_client_connection.hpp"

// base
#include "utility/logger.hpp"
#include "utility/format.hpp"
#include "utility/time.hpp"

using namespace tool::network;
using namespace tool::camera;
using namespace std::chrono;

auto DCClientConnection::init_connections() -> void{

    // connections
    m_udpReaderG.init_network_infos_signal.connect([&](Header h, std::shared_ptr<UdpNetworkSendingSettings> message){
        // from reader thread:
        std::lock_guard l(m_readerL);
        m_initNetworkInfosMessage  = {std::move(h), message};
    });
    m_udpReaderG.update_device_settings_signal.connect([&](Header h, std::shared_ptr<UdpMonoPacketMessage<camera::DCDeviceSettings>> message){
        // from reader thread:
        std::lock_guard l(m_readerL);
        m_updateDeviceSettingsMessage  = {std::move(h), message};
    });
    m_udpReaderG.update_color_settings_signal.connect([&](Header h, std::shared_ptr<UdpMonoPacketMessage<camera::DCColorSettings>> message){
        // from reader thread:
        std::lock_guard l(m_readerL);
        m_updateColorSettingsMessage  = {std::move(h), message};
    });
    m_udpReaderG.update_filters_signal.connect([&](Header h, std::shared_ptr<DCFiltersSettings> message){
        // from reader thread:
        std::lock_guard l(m_readerL);
        m_updateFiltersMessage  = {std::move(h), message};
    });
    m_udpReaderG.update_delay_signal.connect([&](Header h, UdpMonoPacketMessage<camera::DCDelaySettings> message){
        // from reader thread:
        std::lock_guard l(m_readerL);
        m_updateDelayMessage  = {std::move(h), message};
    });

    m_udpReaderG.command_signal.connect([&](Header h, Command message){
        // from reader thread:
        std::lock_guard l(m_readerL);
        m_commandMessage  = {std::move(h), message};
    });
    m_udpReaderG.timeout_packet_signal.connect([&](){
        // Logger::message("timeout\n");
    });
}

auto DCClientConnection::start_reading(UdpClientNetworkSettings *networkS) -> bool{

    // stop reading
    if(m_udpReaderG.is_reading()){
        m_udpReaderG.stop_reading();
    }

    // init reader
    if(!m_udpReaderG.init_socket(networkS->interfaces[networkS->udpReadingInterfaceId].ipAddress, networkS->udpReadingPort)){
        return false;
    }

    // start reading
    m_udpReaderG.start_reading();


    return true;
}

auto DCClientConnection::init_sender(UdpClientNetworkSettings *networkS) -> bool{

    if(m_udpSenderG.init_socket(networkS->udpSendingAdress, std::to_string(networkS->udpSendingPort))){

        // start sending
        sendMessages = false;
        if(sendMessagesT != nullptr){
            if(sendMessagesT->joinable()){
                sendMessagesT->join();
            }
            sendMessagesT = nullptr;
        }
        sendMessagesT = std::make_unique<std::thread>(&DCClientConnection::send_messages_loop, this);
        messagesToSend.push_back(Feedback{MessageType::init_network_infos, FeedbackType::message_received});

        return true;
    }


    Logger::error(fmt("[DCClientConnection::init_sender] Cannot init sender with address [{]] and port [{}]\n", networkS->udpSendingAdress,std::to_string(networkS->udpSendingPort)));
    return false;
}

auto DCClientConnection::ping_server() -> void{    
    if(m_udpSenderG.is_opened()){
        messagesToSend.push_back(Feedback{MessageType::init_network_infos, FeedbackType::message_received});
    }
}

auto DCClientConnection::clean() -> void{

    // stop sending
    disconnect_sender();

    // stop reading
    if(m_udpReaderG.is_reading()){
        m_udpReaderG.stop_reading();
    }
    m_udpReaderG.clean_socket();
}

auto DCClientConnection::update() -> void{

    if(!m_readerL.try_lock()){
        return;
    }

    // check if messages received
    auto initNetworkInfosM = std::move(m_initNetworkInfosMessage);
    m_initNetworkInfosMessage.second = nullptr;

    auto updateDeviceSettingsMessageM = std::move(m_updateDeviceSettingsMessage);
    m_updateDeviceSettingsMessage.second = nullptr;

    auto updateColorSettingsMessageM = std::move(m_updateColorSettingsMessage);
    m_updateColorSettingsMessage.second = nullptr;

    auto updateFiltersMessageM = std::move(m_updateFiltersMessage);
    m_updateFiltersMessage.second = nullptr;

    auto commandMessageM = std::move(m_commandMessage);
    m_commandMessage.second = std::nullopt;

    auto updateDelayMessageM = std::move(m_updateDelayMessage);
    m_updateDelayMessage.second = std::nullopt;

    m_readerL.unlock();

    if(initNetworkInfosM.second){     
        receive_init_network_sending_settings_signal(initNetworkInfosM.second);
    }

    if(updateDeviceSettingsMessageM.second){
        send_feedback({MessageType::update_device_settings, FeedbackType::message_received});
        receive_device_settings_signal(updateDeviceSettingsMessageM.second);
    }

    if(updateColorSettingsMessageM.second){
        send_feedback({MessageType::update_color_settings, FeedbackType::message_received});
        receive_color_settings_signal(updateColorSettingsMessageM.second);        
    }

    if(updateFiltersMessageM.second){
        send_feedback({MessageType::update_filters, FeedbackType::message_received});
        receive_filters_signal(updateFiltersMessageM.second);        
    }

    if(updateDelayMessageM.second.has_value()){
        send_feedback({MessageType::delay, FeedbackType::message_received});
        receive_delay_signal(updateDelayMessageM.second.value());
    }

    if(commandMessageM.second.has_value()){

        switch(commandMessageM.second.value()){
            case Command::Shutdown:
                send_feedback({MessageType::command, FeedbackType::shutdown});
                shutdown_signal();
                break;
            case Command::Restart:
                send_feedback({MessageType::command, FeedbackType::restart});
                restart_signal();
                break;
            case Command::Disconnect:
                send_feedback({MessageType::command, FeedbackType::disconnect});
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                disconnect_sender();
                disconnect_signal();
                break;
            case Command::Quit:
                send_feedback({MessageType::command, FeedbackType::quit});
                quit_signal();
                break;
            case Command::UpdateDeviceList:
                send_feedback({MessageType::command, FeedbackType::update_device_list});
                update_device_list_signal();
                break;
        }
    }
}

auto DCClientConnection::disconnect_sender() -> void{


    // quit thread
    sendMessages = false;
    if(sendMessagesT != nullptr){
        if(sendMessagesT->joinable()){
            sendMessagesT->join();
        }
        sendMessagesT = nullptr;
    }

    // clean socker
    m_udpSenderG.clean_socket();

    // clean messages
    messagesToSend.clean();
}

auto DCClientConnection::send_frame(std::shared_ptr<camera::DCCompressedFrame> frame) -> void{
    messagesToSend.push_back(frame);
}

auto DCClientConnection::send_feedback(Feedback feedback) -> void{
    messagesToSend.push_back(feedback);
}

auto DCClientConnection::last_frame_id_sent() const -> size_t{
    return m_lastFrameIdSent;
}

auto DCClientConnection::dummy_device_trigger() -> void {

    Header header;
    DCDeviceSettings settings;
    settings.actionsS.openCamera  = true;
    settings.actionsS.startDevice = true;
    settings.configS.typeDevice   = DCType::Kinect4;
    settings.configS.synchMode    = DCSynchronisationMode::K4_Standalone;

    std::lock_guard l(m_readerL);
    m_updateDeviceSettingsMessage  = {std::move(header), std::make_shared<UdpMonoPacketMessage<camera::DCDeviceSettings>>(settings)};
}

auto DCClientConnection::send_messages_loop() -> void{

    sendMessages = true;

    while(sendMessages){

        auto message = messagesToSend.pop_front();
        if(!message.has_value()){
            m_udpSenderG.send_synchronisation_message();
            std::this_thread::sleep_for (std::chrono::milliseconds(1));
            continue;
        }

        if(auto feedback = std::get_if<Feedback>(&message.value()); feedback != nullptr){
            m_udpSenderG.send_feedback_message(*feedback);
        }

        if(auto frame = std::get_if<std::shared_ptr<camera::DCCompressedFrame>>(&message.value()); frame != nullptr){

            auto beforeSendingFrameTS = Time::nanoseconds_since_epoch();

            if(m_udpSenderG.send_compressed_frame_message((*frame))){
                m_lastFrameIdSent = (*frame)->idCapture;
            }

            auto afterSendintFrameTS = Time::nanoseconds_since_epoch();
            lastFrameSendingDurationMicrosS = std::chrono::duration_cast<std::chrono::microseconds>(afterSendintFrameTS - beforeSendingFrameTS).count();

            m_lastFrameSentTS = afterSendintFrameTS;
        }

        m_udpSenderG.send_synchronisation_message();
    }
}

