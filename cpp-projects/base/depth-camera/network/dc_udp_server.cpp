

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

#include "dc_udp_server.hpp"

// base
#include "utility/logger.hpp"
#include "utility/format.hpp"
#include "utility/time.hpp"
#include "depth-camera/network/dc_network_types.hpp"

using namespace tool::net;
using namespace tool::cam;
using namespace std::chrono;



auto DCUdpServer::init_connections() -> void{

    m_udpServerReader.packed_received_signal.connect([&](EndPoint endpoint, Header header, std::span<const std::byte> dataToProcess){

        switch (static_cast<DCMessageType>(header.type)) {
        case DCMessageType::init_server_client_connection:{

            initServerClientConnectionReception.update(header, dataToProcess, messagesBuffer);

            if(auto info = initServerClientConnectionReception.message_fully_received(header); info.has_value()){

                Logger::message("[DCUdpServer] init_server_client_connection message received.\n");
                UdpConnectionSettings cs(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes()));

                std::lock_guard l(m_readerL);
                m_initServerClientConnectionMessage  = {std::move(endpoint), cs};
            }

            if(auto nbMessageTimeout = initServerClientConnectionReception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::command:{

            Logger::message("[DCUdpServer] command message received.\n");
            Command command;
            std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(Command), reinterpret_cast<std::byte*>(&command));

            std::lock_guard l(m_readerL);
            m_commandMessage  = {std::move(endpoint), command};

        }break;
        case DCMessageType::update_device_settings:{

            deviceSettingsReception.update(header, dataToProcess, messagesBuffer);

            if(auto info = deviceSettingsReception.message_fully_received(header); info.has_value()){

                Logger::message("[DCUdpServer] update_device_settings message received.\n");
                std::lock_guard l(m_readerL);
                m_updateDeviceSettingsMessage  = {std::move(endpoint), std::make_shared<cam::DCDeviceSettings>(
                    std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                )};
            }

            if(auto nbMessageTimeout = deviceSettingsReception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::update_filters_settings:{

            filtersSettingsReception.update(header, dataToProcess, messagesBuffer);

            if(auto info = filtersSettingsReception.message_fully_received(header); info.has_value()){
                Logger::message("[DCUdpServer] update_filters_settings message received.\n");
                std::lock_guard l(m_readerL);
                m_updateFiltersSettingsMessage  = {std::move(endpoint), std::make_shared<cam::DCFiltersSettings>(
                    std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                )};
            }

            if(auto nbMessageTimeout = filtersSettingsReception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::update_color_settings:{

            colorSettingsReception.update(header, dataToProcess, messagesBuffer);

            if(auto info = colorSettingsReception.message_fully_received(header); info.has_value()){
                Logger::message("[DCUdpServer] update_color_settings message received.\n");
                std::lock_guard l(m_readerL);
                m_updateColorSettingsMessage  = {std::move(endpoint), std::make_shared<cam::DCColorSettings>(
                    std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                )};
            }

            if(auto nbMessageTimeout = colorSettingsReception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::update_delay_settings:{

            Logger::message("[DCUdpServer] update_delay_settings message received.\n");
            DCDelaySettings delay;
            std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(DCDelaySettings), reinterpret_cast<std::byte*>(&delay));

            std::lock_guard l(m_readerL);
            m_updateDelaySettingsMessage  = {std::move(endpoint), std::move(delay)};

        }break;
        default:
            break;
        }
    });

    m_udpServerReader.timeout_packet_signal.connect([&](){
        // Logger::warning("Packet timeout\n");
    });

}

auto DCUdpServer::start_reading_thread(UdpServerSettings *networkS) -> bool{

    // stop reading
    if(m_udpServerReader.is_reading_thread_started()){
        m_udpServerReader.stop_reading_thread();
    }

    // init reader
    if(!m_udpServerReader.init_socket(networkS->udpReadingInterface.ipAddress, networkS->udpReadingPort, m_protocol = networkS->protocol)){
        return false;
    }

    // start reading
    m_udpServerReader.start_reading_thread();


    return true;
}

auto DCUdpServer::init_sender(UdpServerSettings *networkS) -> void{
    
    if(m_udpServerSender.is_connected()){
        Logger::warning("[DCClientConnection::init_sender] Sender already initialized. Call clean before.\n");
        messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});
        return;
    }
    
    Logger::message(std::format("[DCClientConnection::init_sender] Init sender with sending adress [{}] port [{}] and protocol [{}].\n", networkS->clientConnectionSettings.address, networkS->clientConnectionSettings.port, static_cast<int>(networkS->protocol)));
    if(m_udpServerSender.init_socket(networkS->clientConnectionSettings.address, std::to_string(networkS->clientConnectionSettings.port), networkS->protocol)){

        // start sending
        sendMessages = false;
        if(sendMessagesT != nullptr){
            if(sendMessagesT->joinable()){
                sendMessagesT->join();
            }
            sendMessagesT = nullptr;
        }
        sendMessagesT = std::make_unique<std::thread>(&DCUdpServer::send_messages_loop, this);
        messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});

    }else{
        Logger::error(fmt("[DCClientConnection::init_sender] Cannot init sender with address [{]] and port [{}]\n", networkS->clientConnectionSettings.address,std::to_string(networkS->clientConnectionSettings.port)));
    }
}

auto DCUdpServer::ping_server() -> void{
    if(m_udpServerSender.is_connected()){
        messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});
    }
}

auto DCUdpServer::clean() -> void{

    // stop sending
    disconnect_sender();

    // stop reading
    if(m_udpServerReader.is_reading_thread_started()){
        m_udpServerReader.stop_reading_thread();
    }
    m_udpServerReader.clean_socket();
}

auto DCUdpServer::update() -> void{

    if(!m_readerL.try_lock()){
        return;
    }

    // check if messages received
    auto initServerClientConnectionM = std::move(m_initServerClientConnectionMessage);
    m_initServerClientConnectionMessage.second = std::nullopt;

    auto commandMessageM = std::move(m_commandMessage);
    m_commandMessage.second = std::nullopt;

    auto updateDelaySettingsMessageM = std::move(m_updateDelaySettingsMessage);
    m_updateDelaySettingsMessage.second = std::nullopt;

    auto updateDeviceSettingsMessageM = std::move(m_updateDeviceSettingsMessage);
    m_updateDeviceSettingsMessage.second = nullptr;

    auto updateColorSettingsMessageM = std::move(m_updateColorSettingsMessage);
    m_updateColorSettingsMessage.second = nullptr;

    auto updateFiltersMessageM = std::move(m_updateFiltersSettingsMessage);
    m_updateFiltersSettingsMessage.second = nullptr;

    m_readerL.unlock();

    if(initServerClientConnectionM.second.has_value()){

        // init new sender
        if(!m_udpSenders.contains(initServerClientConnectionM.first.id)){

            m_udpSenders[initServerClientConnectionM.first.id] = std::make_unique<UdpSender>();

        }

        auto sender = m_udpSenders[initServerClientConnectionM.first.id].get();
        if(!sender->is_connected()){

            const auto &initCoSettings = initServerClientConnectionM.second.value();
            Logger::message(std::format("[DCUdpServer::update] Init sender with sending address [{}] port [{}] and protocol [{}].\n",initCoSettings.address, initCoSettings.port, static_cast<int>(m_protocol)));
            if(sender->init_socket(initCoSettings.address, std::to_string(initCoSettings.port), m_protocol)){

            }

        }else{
            Logger::warning("[DCUdpServer::update] Sender already opened.\n");
            messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});
        }



            // sender already exists

        // }

        // initServerClientConnectionM.first.id;


        receive_init_server_client_connection_signal(std::move(initServerClientConnectionM.second.value()));


        // if(m_udpServerSender.is_opened()){
        //     Logger::warning("[DCClientConnection::init_sender] Sender already initialized. Call clean before.\n");
        //     messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});
        //     return;
        // }

        // Logger::message(std::format("[DCClientConnection::init_sender] Init sender with sending adress [{}] port [{}] and protocol [{}].\n", networkS->clientConnectionSettings.address, networkS->clientConnectionSettings.port, static_cast<int>(networkS->protocol)));
        // if(m_udpServerSender.init_socket(networkS->clientConnectionSettings.address, std::to_string(networkS->clientConnectionSettings.port), networkS->protocol)){

        //     // start sending
        //     sendMessages = false;
        //     if(sendMessagesT != nullptr){
        //         if(sendMessagesT->joinable()){
        //             sendMessagesT->join();
        //         }
        //         sendMessagesT = nullptr;
        //     }
        //     sendMessagesT = std::make_unique<std::thread>(&DCUdpServer::send_messages_loop, this);
        //     messagesToSend.push_back(Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received});

        // }else{
        //     Logger::error(fmt("[DCClientConnection::init_sender] Cannot init sender with address [{]] and port [{}]\n", networkS->clientConnectionSettings.address,std::to_string(networkS->clientConnectionSettings.port)));
        // }

        //

    }

    if(updateDelaySettingsMessageM.second.has_value()){
        send_feedback({static_cast<MessageTypeId>(DCMessageType::update_delay_settings), FeedbackType::message_received});
        receive_delay_settings_signal(updateDelaySettingsMessageM.second.value());
    }

    if(commandMessageM.second.has_value()){

        switch(commandMessageM.second.value()){
        case Command::shutdown:
            send_feedback({static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::shutdown});
            shutdown_signal();
            break;
        case Command::restart:
            send_feedback({static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::restart});
            restart_signal();
            break;
        case Command::disconnect:
            send_feedback({static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::disconnect});
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            disconnect_sender();
            disconnect_signal();
            break;
        case Command::quit:
            send_feedback({static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::quit});
            quit_signal();
            break;
        }
    }

    if(updateDeviceSettingsMessageM.second){
        send_feedback({static_cast<MessageTypeId>(DCMessageType::update_device_settings), FeedbackType::message_received});
        receive_device_settings_signal(updateDeviceSettingsMessageM.second);
    }

    if(updateColorSettingsMessageM.second){
        send_feedback({static_cast<MessageTypeId>(DCMessageType::update_color_settings), FeedbackType::message_received});
        receive_color_settings_signal(updateColorSettingsMessageM.second);        
    }

    if(updateFiltersMessageM.second){
        send_feedback({static_cast<MessageTypeId>(DCMessageType::update_filters_settings), FeedbackType::message_received});
        receive_filters_signal(updateFiltersMessageM.second);        
    }


}

auto DCUdpServer::disconnect_sender() -> void{

    // quit sending message loop thread
    sendMessages = false;
    if(sendMessagesT != nullptr){
        if(sendMessagesT->joinable()){
            sendMessagesT->join();
        }
        sendMessagesT = nullptr;
    }

    // clean sender sockets
    m_udpServerSender.clean_socket();    
    for(auto &sender : m_udpSenders){
        sender.second->clean_socket();
    }
    m_udpSenders.clear();


    // clean messages
    messagesToSend.clean();
}

auto DCUdpServer::send_frame(std::shared_ptr<cam::DCCompressedFrame> frame) -> void{
    messagesToSend.push_back(frame);
}

auto DCUdpServer::send_feedback(Feedback feedback) -> void{
    messagesToSend.push_back(feedback);
}

auto DCUdpServer::last_frame_id_sent() const -> size_t{
    return m_lastFrameIdSent;
}

auto DCUdpServer::simulate_sending_failure(bool enabled, int percentage) -> void{
    m_udpServerSender.simulate_failure(enabled, percentage);
}


auto DCUdpServer::send_messages_loop() -> void{

    sendMessages = true;

    while(sendMessages){

        auto message = messagesToSend.pop_front();
        if(!message.has_value()){
            std::this_thread::sleep_for (std::chrono::milliseconds(1));
            m_udpServerSender.send_message(static_cast<MessageTypeId>(DCMessageType::synchro));
            continue;
        }

        if(auto feedbackPtr = std::get_if<Feedback>(&message.value()); feedbackPtr != nullptr){
            Feedback feedback = *feedbackPtr;
            Logger::message(std::format("[DCClientConnection] Send feedback of type [{}].\n", static_cast<int>(feedback.receivedMessageType)));
            m_udpServerSender.send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));
        }

        if(auto frame = std::get_if<std::shared_ptr<cam::DCCompressedFrame>>(&message.value()); frame != nullptr){

            auto beforeSendingFrameTS = Time::nanoseconds_since_epoch();

            std::shared_ptr<cam::DCCompressedFrame> f = *frame;
            size_t totalDataSizeBytes = f->data_size();
            if(senderBuffer.size() < totalDataSizeBytes){
                senderBuffer.resize(totalDataSizeBytes);
            }
            size_t offset = 0;
            auto bData = std::span(senderBuffer.data(), totalDataSizeBytes);
            f->write_to_data(bData, offset);
            if(m_udpServerSender.send_message(static_cast<MessageTypeId>(DCMessageType::compressed_frame_data), bData)){
                m_lastFrameIdSent = f->idCapture;
            }
            

            auto afterSendintFrameTS = Time::nanoseconds_since_epoch();
            lastFrameSendingDurationMicrosS = std::chrono::duration_cast<std::chrono::microseconds>(afterSendintFrameTS - beforeSendingFrameTS).count();

            m_lastFrameSentTS = afterSendintFrameTS;
        }
        
        m_udpServerSender.send_message(static_cast<MessageTypeId>(DCMessageType::synchro));
    }
}

