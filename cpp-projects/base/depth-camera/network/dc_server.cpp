

/*******************************************************************************
** Toolset-dc-grabber                                                         **
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

#include "dc_server.hpp"

// std
#include <variant>
#include <execution>

// local
#include "utility/ring_buffer.hpp"
#include "utility/safe_queue.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"
#include "utility/unordered_map.hpp"
#include "network/udp_reader.hpp"
#include "network/udp_sender.hpp"
#include "depth-camera/network/dc_network_enums.hpp"
#include "network/settings/udp_connection_settings.hpp"

using namespace tool::net;
using namespace tool::cam;
using namespace std::chrono;

template<class T>
using EndVal = std::pair<EndPointId, T>;

template<class T>
using EndPtr = std::pair<EndPointId, std::shared_ptr<T>>;

using RMessageV =
    std::variant<
        EndVal<UdpConnectionSettings>,
    EndVal<DCMiscSettings>,
        EndVal<Command>,
        EndVal<Feedback>,
        EndPtr<DCDeviceSettings>,
        EndPtr<DCColorSettings>,
        EndPtr<DCFiltersSettings>
    >;

using SMessageV =
    std::variant<
        EndPtr<DCDataFrame>,
        EndVal<UdpConnectionSettings>,
        EndVal<Feedback>
    >;

template<typename ... Callable>
struct Visitor : Callable...{
    using Callable::operator()...;
};

struct DCServer::Impl{

    Impl();

    auto start_reading_thread(const std::string &readingAdress, int readingPort, Protocol protocol) -> bool;
    auto stop_reading_thread() -> void;
    auto start_sending_thread() -> void;
    auto stop_sending_thread() -> void;

    auto disconnect_sender(const std::string &id) -> void{
        Log::message(std::format("[DCServer] Client disconnected [{}].\n", id));
        udpSenders[id]->clean_socket();
        udpSenders.erase(id);
        client_disconnected_signal(id);
    }

    // reading
    // # reader
    UdpReader udpReader;
    // # buffer
    DoubleRingBuffer<std::byte> messagesBuffer;
    // # reception
    umap<std::int8_t, UdpMessageReception> receptions;
    // # lock
    std::mutex messagesL;
    // # rMessages
    std::unique_ptr<std::vector<RMessageV>> rMessages = nullptr;
    std::unique_ptr<std::vector<RMessageV>> rMessagesSwap = nullptr;

    // sending
    // # messages
    SafeQueue<SMessageV> sMessages;
    // # data
    std::vector<std::vector<std::byte>> frameDataPackets;

    // signals
    sigslot::signal<size_t> timeout_messages_signal;
    sigslot::signal<std::string> client_connected_signal;
    sigslot::signal<std::string> client_disconnected_signal;
    sigslot::signal<size_t, std::chrono::nanoseconds, std::int64_t> last_frame_sent_signal;


private:

    Protocol protocol = Protocol::unknow;

    // sending
    // # thread
    std::atomic_bool sendMessages = false;
    std::unique_ptr<std::thread> sendMessagesT = nullptr;
    // # senders
    umap<std::string, std::unique_ptr<UdpSender>> udpSenders;
    // # data
    std::vector<std::byte> senderBuffer;

private:

    auto send_messages_loop() -> void;
};


DCServer::Impl::Impl(){

    messagesBuffer.resize(100, 0);

    rMessages     = std::make_unique<std::vector<RMessageV>>();
    rMessagesSwap = std::make_unique<std::vector<RMessageV>>();

    receptions[static_cast<std::int8_t>(DCMessageType::init_server_client_connection)] = {};
    receptions[static_cast<std::int8_t>(DCMessageType::update_device_settings)] = {};
    receptions[static_cast<std::int8_t>(DCMessageType::update_filters_settings)] = {};
    receptions[static_cast<std::int8_t>(DCMessageType::update_color_settings)] = {};

    udpReader.packets_received_signal.connect([&](std::span<PData> packets){

        for(const auto &packet : packets){

            switch (static_cast<DCMessageType>(packet.header.type)) {
            case DCMessageType::init_server_client_connection:{

                auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::init_server_client_connection)];
                reception.update(packet.header, packet.data, messagesBuffer);

                if(auto info = reception.message_fully_received(packet.header); info.has_value()){

                    Log::message("[DCServer] init_server_client_connection message received.\n");
                    UdpConnectionSettings cs(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes()));

                    std::lock_guard l(messagesL);
                    rMessages->push_back(EndVal<UdpConnectionSettings>{std::move(packet.endPoint), cs});
                }

                if(auto nbMessageTimeout = reception.check_message_timeout(); nbMessageTimeout != 0){
                    timeout_messages_signal(nbMessageTimeout);
                }

            }break;
            case DCMessageType::command:{

                Log::message("[DCServer] command message received.\n");
                Command command;
                std::copy(packet.data.data(), packet.data.data() + sizeof(Command), reinterpret_cast<std::byte*>(&command));

                std::lock_guard l(messagesL);
                rMessages->push_back(EndVal<Command>{std::move(packet.endPoint), command});

            }break;
            case DCMessageType::feedback:{

                Feedback feedback;
                std::copy(packet.data.data(), packet.data.data() + sizeof(Feedback), reinterpret_cast<std::byte*>(&feedback));

                std::lock_guard l(messagesL);
                rMessages->push_back(EndVal<Feedback>{std::move(packet.endPoint), feedback});

            }break;
            case DCMessageType::update_device_settings:{

                auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::update_device_settings)];
                reception.update(packet.header, packet.data, messagesBuffer);

                if(auto info = reception.message_fully_received(packet.header); info.has_value()){

                    Log::message("[DCServer] update_device_settings message received.\n");

                    std::lock_guard l(messagesL);
                    rMessages->push_back(EndPtr<DCDeviceSettings>{std::move(packet.endPoint),  std::make_shared<DCDeviceSettings>(
                        std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                    )});
                }

                if(auto nbMessageTimeout =  reception.check_message_timeout(); nbMessageTimeout != 0){
                    timeout_messages_signal(nbMessageTimeout);
                }

            }break;
            case DCMessageType::update_filters_settings:{

                auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::update_filters_settings)];
                reception.update(packet.header, packet.data, messagesBuffer);

                if(auto info = reception.message_fully_received(packet.header); info.has_value()){
                    Log::message("[DCServer] update_filters_settings message received.\n");

                    std::lock_guard l(messagesL);
                    rMessages->push_back(EndPtr<DCFiltersSettings>{std::move(packet.endPoint),  std::make_shared<DCFiltersSettings>(
                        std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                    )});
                }

                if(auto nbMessageTimeout = reception.check_message_timeout(); nbMessageTimeout != 0){
                    timeout_messages_signal(nbMessageTimeout);
                }

            }break;
            case DCMessageType::update_color_settings:{

                auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::update_color_settings)];
                reception.update(packet.header, packet.data, messagesBuffer);

                if(auto info = reception.message_fully_received(packet.header); info.has_value()){

                    Log::message("[DCServer] update_color_settings message received.\n");

                    std::lock_guard l(messagesL);
                    rMessages->push_back(EndPtr<DCColorSettings>{std::move(packet.endPoint),  std::make_shared<DCColorSettings>(
                        std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                    )});
                }

                if(auto nbMessageTimeout = reception.check_message_timeout(); nbMessageTimeout != 0){
                    timeout_messages_signal(nbMessageTimeout);
                }

            }break;
            case DCMessageType::update_misc_settings:{

                Log::message("[DCServer] update_delay_settings message received.\n");

                DCMiscSettings delay;
                std::copy(packet.data.data(), packet.data.data() + sizeof(DCMiscSettings), reinterpret_cast<std::byte*>(&delay));

                std::lock_guard l(messagesL);
                rMessages->push_back(EndVal<DCMiscSettings>{std::move(packet.endPoint), std::move(delay)});

            }break;
            default:
                break;
            }
        }
    });
}

auto DCServer::Impl::start_reading_thread(const std::string &readingAdress, int readingPort, Protocol protocol) -> bool{

    this->protocol = protocol;
    if(!udpReader.init_socket(readingAdress, readingPort, protocol)){
        return false;
    }
    udpReader.start_threads();

    return true;
}

auto DCServer::Impl::stop_reading_thread() -> void{
    if(udpReader.are_threads_started()){
        udpReader.stop_threads();
    }
    udpReader.clean_socket();
}

auto DCServer::Impl::start_sending_thread() -> void{
    sendMessagesT = std::make_unique<std::thread>(&DCServer::Impl::send_messages_loop, this);
}

auto DCServer::Impl::stop_sending_thread() -> void{

    // stop thread
    sendMessages = false;
    if(sendMessagesT != nullptr){
        if(sendMessagesT->joinable()){
            sendMessagesT->join();
        }
        sendMessagesT = nullptr;
    }

    // clean sockets
    for(auto &sender : udpSenders){
        sender.second->clean_socket();
        sender.second = nullptr;
    }
}

auto DCServer::Impl::send_messages_loop() -> void{

    sendMessages = true;

    while(sendMessages){

        auto message = sMessages.pop_front();

        // send synch if no message to send
        if(!message.has_value()){
            for(auto &sender : udpSenders){
                sender.second->send_message(static_cast<MessageTypeId>(DCMessageType::synchro));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        bool frameSent = false;

        std::visit(
            Visitor{
                [&](EndVal<UdpConnectionSettings> m){

                    const auto &endPoint = m.first;
                    if(!udpSenders.contains(endPoint.id)){

                        // add new sender
                        udpSenders[endPoint.id] = std::make_unique<UdpSender>();

                        // init socket
                        if(udpSenders[endPoint.id]->init_socket(m.second.address, std::to_string(m.second.port), protocol)){

                            Feedback feedback;
                            feedback.type = FeedbackType::message_received;
                            feedback.receivedMessageType = static_cast<MessageTypeId>(DCMessageType::init_server_client_connection);
                            udpSenders[endPoint.id]->send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));
                            
                            Log::message(std::format("[DCServer] New client connected [{}].\n", endPoint.id));
                            client_connected_signal(endPoint.id);
                        }else{
                            udpSenders.erase(endPoint.id);
                            Log::error(std::format("[DCServer] Cannot connect to client [{}].\n", endPoint.id));
                        }
                    }else{
                        Log::warning(std::format("[DCServer] Client already connected [{}].\n", endPoint.id));
                    }

                },
                [&](EndVal<Feedback> m){

                    Feedback feedback = m.second;
                    Log::message(std::format("[DCServer] Send feedback of type [{}].\n", static_cast<int>(feedback.receivedMessageType)));
                    const auto &endPoint = m.first;

                    if(endPoint.id.empty()){ // if empty, apply feedback to all clients

                        // command received
                        if(feedback.receivedMessageType == static_cast<MessageTypeId>(DCMessageType::command)){
                            if(feedback.type == FeedbackType::disconnect){

                                // disconnect all
                                for(auto &udpSender : udpSenders){
                                    udpSender.second->send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));
                                    udpSender.second->clean_socket();
                                    client_disconnected_signal(udpSender.first);
                                }
                                udpSenders.clear();
                            }
                        }

                    } else if(udpSenders.contains(endPoint.id)){

                        // send message received feedback
                        udpSenders[endPoint.id]->send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));

                        // command received
                        if(feedback.receivedMessageType == static_cast<MessageTypeId>(DCMessageType::command)){

                            // disconnect command received
                            if(feedback.type == FeedbackType::disconnect){
                                disconnect_sender(endPoint.id);
                            }
                        }
                    }
                },
                [&](EndPtr<DCDataFrame> m){

                    if(!udpSenders.empty()){

                        // auto t1 = Time::nanoseconds_since_epoch();
                        auto beforeSendingFrameTS = Time::nanoseconds_since_epoch();

                        std::shared_ptr<cam::DCDataFrame> f = std::move(m.second);

                        // resize buffer if necessary
                        size_t totalDataSizeBytes = f->data_size();
                        if(senderBuffer.size() < totalDataSizeBytes){
                            senderBuffer.resize(totalDataSizeBytes);
                        }
                        // auto t2 = Time::nanoseconds_since_epoch();

                        // write data to buffer
                        size_t offset = 0;
                        auto bData = std::span(senderBuffer.data(), totalDataSizeBytes);
                        f->write_to_data(bData, offset);

                        // auto t3 = Time::nanoseconds_since_epoch();

                        // generate all packets
                        (*udpSenders.begin()).second->generate_data_packets(static_cast<MessageTypeId>(DCMessageType::data_frame), bData, frameDataPackets);

                        // auto t4 = Time::nanoseconds_since_epoch();

                        // send all packets
                        std::atomic<size_t> count = 0;
                        std::for_each(std::execution::par_unseq, std::begin(udpSenders), std::end(udpSenders), [&](auto &senderP){
                            size_t nbBytesSent = 0;
                            for(const auto &dataPacket : frameDataPackets){
                                auto nbBytesS = senderP.second->send_packet_data(dataPacket);
                                nbBytesSent += nbBytesS;
                            }
                            if(nbBytesSent != 0){
                                ++count;
                            }
                        });

                        // auto t5 = Time::nanoseconds_since_epoch();

                        // update last frame id sent if at least on frame sent
                        if(count != 0){
                            frameSent = true;

                            std::chrono::nanoseconds lastFrameSentTS = Time::nanoseconds_since_epoch();
                            last_frame_sent_signal(f->idCapture, lastFrameSentTS, std::chrono::duration_cast<std::chrono::microseconds>(lastFrameSentTS - beforeSendingFrameTS).count());
                        }

                        // Log::message(std::format("TIME SEND: {} {} {} {}\n",
                        //     Time::difference_micro_s(t1,t2).count(),
                        //     Time::difference_micro_s(t2,t3).count(),Time::difference_micro_s(t3,t4).count(),Time::difference_micro_s(t4,t5).count()));
                    }
                }
            },
            message.value()
        );


        // send synchro
        for(auto &sender : udpSenders){
            sender.second->send_message(static_cast<MessageTypeId>(DCMessageType::synchro));
        }

        // wait
        if(!frameSent){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

DCServer::DCServer(): i(std::make_unique<Impl>()){

    i->timeout_messages_signal.connect(&DCServer::timeout_messages_signal, this);
    i->client_connected_signal.connect([&](std::string id){
        std::unique_lock lg(settings.cInfos.lock);
        settings.cInfos.clientsConnected[id] = Time::nanoseconds_since_epoch();
    });
    i->client_disconnected_signal.connect([&](std::string id){
        std::unique_lock lg(settings.cInfos.lock);
        settings.cInfos.clientsConnected.erase(id);
    });
    i->last_frame_sent_signal.connect([&](size_t idFrame, std::chrono::nanoseconds ts, std::int64_t sendingDurationMicroS){
        std::unique_lock lg(settings.cInfos.lock);
        settings.cInfos.lastFrameIdSent = idFrame;
        settings.cInfos.lastFrameSentTS = ts;
        settings.cInfos.lastFrameSentDurationMicroS = sendingDurationMicroS;
    });
}

DCServer::~DCServer(){
    clean();
}

auto DCServer::clean() -> void{

    // stop threads
    i->stop_sending_thread();
    i->stop_reading_thread();
}


auto DCServer::reset_network() -> void{

    i->stop_reading_thread();

    settings.update_reading_interface();

    if(!i->start_reading_thread(settings.udpServerS.anyReadingInterface ? "" : settings.udpReadingInterface.ipAddress, settings.udpServerS.readingPort, settings.udpReadingInterface.protocol)){
        Log::error(std::format("[DCServer::initialize] Error cann start reading thread with address [{}].\n", settings.udpReadingInterface.ipAddress));
        return;
    }
}

auto DCServer::disconnect_clients() -> void{
    i->sMessages.push_back(std::make_pair(EndPointId{}, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::disconnect}));
}

auto DCServer::initialize(const std::string &serverSettingsPath) -> bool{

    if(!settings.load_from_file(serverSettingsPath)){
        Log::error(std::format("[DCServer::initialize] Error while reading server settings file at path [{}].\n", serverSettingsPath));
        return false;
    }
    settings.globalFilePath = serverSettingsPath;

    if(!i->start_reading_thread(settings.udpServerS.anyReadingInterface ? "" : settings.udpReadingInterface.ipAddress, settings.udpServerS.readingPort, settings.udpReadingInterface.protocol)){
        Log::error(std::format("[DCServer::initialize] Error cann start reading thread with address [{}].\n", settings.udpReadingInterface.ipAddress));
        return false;
    }

    i->start_sending_thread();

    return true;
}

auto DCServer::legacy_initialize(const std::string &legacyNetworkSettingsFilePath) -> bool{

    if(!settings.udpServerS.load_from_file(legacyNetworkSettingsFilePath)){
        return false;
    }
    settings.update_reading_interface();

    if(!i->start_reading_thread(settings.udpReadingInterface.ipAddress, settings.udpServerS.readingPort, settings.udpReadingInterface.protocol)){
        // ...
        return false;
    }

    i->start_sending_thread();
    // ...

    return true;
}

auto DCServer::update() -> void{

    i->rMessagesSwap->clear();

    bool hasMessages = false;
    if(i->messagesL.try_lock()){
        if(!i->rMessages->empty()){
            std::swap(i->rMessages, i->rMessagesSwap);
            hasMessages = true;
        }
        i->messagesL.unlock();
    }

    if(hasMessages){
        for(const auto &message : *i->rMessagesSwap){
            std::visit(
                Visitor{
                    [&](EndVal<UdpConnectionSettings> m){
                        i->sMessages.push_back(std::make_pair(m.first, std::move(m.second)));;
                    },
                    [&](EndVal<DCMiscSettings> m){
                        i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_misc_settings), FeedbackType::message_received}));
                        settings.miscS = std::move(m.second);
                        delay_settings_received_signal();
                    },
                    [&](EndVal<Command> m){

                        switch(m.second){
                        case Command::shutdown:
                            i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::shutdown}));
                            shutdown_signal();
                            break;
                        case Command::restart:
                            i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::restart}));
                            restart_signal();
                            break;
                        case Command::disconnect:
                            i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::disconnect}));
                            break;
                        case Command::quit:
                            i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::quit}));
                            quit_signal();
                            break;
                        }
                    },
                    [&](EndVal<Feedback> m){
                        if(m.second.type == FeedbackType::ping){
                            if(settings.cInfos.clientsConnected.contains(m.first.id)){
                                settings.cInfos.clientsConnected[m.first.id] = Time::nanoseconds_since_epoch();
                                // Log::message("ping\n");
                            }
                        }
                    },
                    [&](EndPtr<DCDeviceSettings> m){
                        i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_device_settings), FeedbackType::message_received}));
                        settings.deviceS = std::move(*m.second);
                        device_settings_received_signal();
                    },
                    [&](EndPtr<DCColorSettings> m){
                        i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_color_settings), FeedbackType::message_received}));
                        settings.colorS = std::move(*m.second);
                        color_settings_received_signal();
                    },
                    [&](EndPtr<DCFiltersSettings> m){
                        i->sMessages.push_back(std::make_pair(m.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_filters_settings), FeedbackType::message_received}));
                        settings.filtersS = std::move(*m.second);
                        filters_settings_received_signal();
                    }},
                message
            );
        }
    }

    // check for disconnected clients
    for(auto &client : settings.cInfos.clientsConnected){
        if(Time::now_difference_ms(client.second).count() > 10000){
            i->disconnect_sender(client.first);
        }
    }
}


auto DCServer::send_frame(std::shared_ptr<cam::DCDataFrame> frame) -> void{
    settings.cInfos.sizeDataFrame = frame->data_size();
    i->sMessages.push_back(std::make_pair(EndPointId{}, std::move(frame)));
}


auto DCServer::simulate_sending_failure(bool enabled, int percentage) -> void{    
    //m_udpServerSender.simulate_failure(enabled, percentage);
}



