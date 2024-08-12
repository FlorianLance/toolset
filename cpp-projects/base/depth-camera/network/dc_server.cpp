

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

#include "dc_server.hpp"

// std
#include <variant>
#include <execution>

// local
#include "utility/thread.hpp"
#include "utility/ring_buffer.hpp"
#include "utility/safe_queue.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"
#include "utility/unordered_map.hpp"
#include "network/udp_reader.hpp"
#include "network/udp_sender.hpp"
#include "data/json_utility.hpp"
#include "depth-camera/network/dc_network_enums.hpp"


using namespace tool::net;
using namespace tool::cam;
using namespace std::chrono;
using namespace tool::data;
using json = nlohmann::json;

DCServerSettings::DCServerSettings(){

    sType   = io::SettingsType::Dc_server;
    version = io::SettingsVersion::LastVersion;

    // retrieve interfaces
    ipv4Interfaces = Interface::list_local_interfaces(Protocol::ipv4);
    if(ipv4Interfaces.size() == 0){
        Logger::warning("[DCServerSettings::reset_interfaces] Cannot find any ipv4 interface.\n"sv);
    }
    ipv6Interfaces = Interface::list_local_interfaces(Protocol::ipv6);
    if(ipv6Interfaces.size() == 0){
        Logger::warning("[DCServerSettings::reset_interfaces] Cannot find any ipv6 interface.\n"sv);
    }
}

auto DCServerSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_object(json, unreadCount, "base"sv));
    // local
    udpServerS.init_from_json(read_object(json, unreadCount, "udp_server"sv));
    deviceS.init_from_json(read_object(json, unreadCount, "device"sv));
    filtersS.init_from_json(read_object(json, unreadCount, "filters"sv));
    calibrationFiltersS.init_from_json(read_object(json, unreadCount, "calibration_filters"sv));
    colorS.init_from_json(read_object(json, unreadCount, "color"sv));
    modelS.init_from_json(read_object(json, unreadCount, "model"sv));
    // delayS.init_from_json(read_object(json, unreadCount, "delay"sv))

    if(unreadCount != 0){
        Logger::warning(std::format("[DCServerSettings::init_from_json] [{}] values have not been initialized from json data.\n", unreadCount));
    }

    update_reading_interface();
}

auto DCServerSettings::convert_to_json() const -> nlohmann::json{
    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // local
    add_value(json, "udp_server"sv, udpServerS.convert_to_json());
    add_value(json, "device"sv, deviceS.convert_to_json());
    add_value(json, "filters"sv, filtersS.convert_to_json());
    add_value(json, "calibration_filters"sv, calibrationFiltersS.convert_to_json());
    add_value(json, "color"sv, colorS.convert_to_json());
    add_value(json, "model"sv, modelS.convert_to_json());
    // add_value(json, "delay"sv, delayS.convert_to_json());

    return json;
}

auto DCServerSettings::update_reading_interface() -> void{
    const auto &interfaces = (udpServerS.protocol == Protocol::ipv6) ? ipv6Interfaces : ipv4Interfaces;
    if(udpServerS.udpReadingInterfaceId < interfaces.size()){
        udpReadingInterface = interfaces[udpServerS.udpReadingInterfaceId];
    }else{
        Logger::error("[DCServerSettings::update_reading_interface] Invalid reading interface id, not enough interfaces.\n");
    }
}

template<class T>
using EndOpt = std::pair<EndPoint, std::optional<T>>;

template<class T>
using EndPtr = std::pair<EndPoint, std::shared_ptr<T>>;

using Mess =
    std::variant<
        EndOpt<UdpConnectionSettings>,
        EndOpt<DCDelaySettings>,
        EndOpt<Command>,
        EndPtr<DCDeviceSettings>,
        EndPtr<DCColorSettings>,
        EndPtr<DCFiltersSettings>
    >;

struct Ping{
};

struct DCServer::Impl{

    Impl();

    auto operator()(EndOpt<UdpConnectionSettings> s) -> void{

    }

    auto start_reading_thread(const std::string &readingAdress, int readingPort, Protocol protocol) -> bool;
    auto stop_reading_thread() -> void;
    auto start_sending_thread() -> void;
    auto stop_sending_thread() -> void;

    // reading
    // # reader
    UdpReader udpReader;    
    // # lock
    SpinLock readerL;
    // # buffer
    DoubleRingBuffer<std::byte> messagesBuffer;
    // # reception
    umap<std::int8_t, UdpMessageReception> receptions;

    // # messages
    std::vector<Mess> messages;
    EndOpt<UdpConnectionSettings> initServerClientConnectionMessage;
    EndOpt<DCDelaySettings> updateDelaySettingsMessage;
    EndOpt<Command> commandMessage;
    EndPtr<DCDeviceSettings> updateDeviceSettingsMessage;
    EndPtr<DCColorSettings> updateColorSettingsMessage;
    EndPtr<DCFiltersSettings> updateFiltersSettingsMessage;

    // sending
    // # messages
    SafeQueue<std::pair<EndPoint, std::variant<std::shared_ptr<cam::DCCompressedFrame>, Feedback, Ping>>> messagesToSend;
    // # monitoring
    std::atomic<size_t> lastFrameIdSent = 0;
    std::atomic<std::int64_t> lastFrameSendingDurationMicrosS = 0;
    std::atomic<std::chrono::nanoseconds> lastFrameSentTS;    

    // signals
    sigslot::signal<size_t> timeout_messages_signal;

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

    initServerClientConnectionMessage   = std::make_pair<EndPoint, std::optional<UdpConnectionSettings>>({},    std::nullopt);
    updateDelaySettingsMessage          = std::make_pair<EndPoint, std::optional<DCDelaySettings>>({},          std::nullopt);
    commandMessage                      = std::make_pair<EndPoint, std::optional<Command>>({},                  std::nullopt);

    updateDeviceSettingsMessage         = std::make_pair<EndPoint, std::shared_ptr<DCDeviceSettings>>({},  nullptr);
    updateColorSettingsMessage          = std::make_pair<EndPoint, std::shared_ptr<DCColorSettings>>({},   nullptr);
    updateFiltersSettingsMessage        = std::make_pair<EndPoint, std::shared_ptr<DCFiltersSettings>>({}, nullptr);

    receptions[static_cast<std::int8_t>(DCMessageType::init_server_client_connection)] = {};
    receptions[static_cast<std::int8_t>(DCMessageType::update_device_settings)] = {};
    receptions[static_cast<std::int8_t>(DCMessageType::update_filters_settings)] = {};
    receptions[static_cast<std::int8_t>(DCMessageType::update_color_settings)] = {};

    udpReader.packed_received_signal.connect([&](EndPoint endpoint, Header header, std::span<const std::byte> dataToProcess){

        switch (static_cast<DCMessageType>(header.type)) {
        case DCMessageType::init_server_client_connection:{

            auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::init_server_client_connection)];
            reception.update(header, dataToProcess, messagesBuffer);

            if(auto info = reception.message_fully_received(header); info.has_value()){

                Logger::message("[DCServer] init_server_client_connection message received.\n");
                UdpConnectionSettings cs(std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes()));

                std::lock_guard l(readerL);
                initServerClientConnectionMessage  = {std::move(endpoint), cs};
            }

            if(auto nbMessageTimeout = reception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::command:{

            Logger::message("[DCServer] command message received.\n");
            Command command;
            std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(Command), reinterpret_cast<std::byte*>(&command));

            std::lock_guard l(readerL);
            commandMessage  = {std::move(endpoint), command};

        }break;
        case DCMessageType::update_device_settings:{

            auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::update_device_settings)];
            reception.update(header, dataToProcess, messagesBuffer);

            if(auto info = reception.message_fully_received(header); info.has_value()){

                Logger::message("[DCServer] update_device_settings message received.\n");
                std::lock_guard l(readerL);
                updateDeviceSettingsMessage  = {std::move(endpoint), std::make_shared<cam::DCDeviceSettings>(
                    std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                )};
            }

            if(auto nbMessageTimeout =  reception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::update_filters_settings:{

            auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::update_filters_settings)];
            reception.update(header, dataToProcess, messagesBuffer);

            if(auto info = reception.message_fully_received(header); info.has_value()){
                Logger::message("[DCServer] update_filters_settings message received.\n");
                std::lock_guard l(readerL);
                updateFiltersSettingsMessage  = {std::move(endpoint), std::make_shared<cam::DCFiltersSettings>(
                    std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                )};
            }

            if(auto nbMessageTimeout = reception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::update_color_settings:{

            auto &reception = receptions[static_cast<std::int8_t>(DCMessageType::update_color_settings)];
            reception.update(header, dataToProcess, messagesBuffer);

            if(auto info = reception.message_fully_received(header); info.has_value()){
                Logger::message("[DCServer] update_color_settings message received.\n");
                std::lock_guard l(readerL);
                updateColorSettingsMessage  = {std::move(endpoint), std::make_shared<cam::DCColorSettings>(
                    std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
                )};
            }

            if(auto nbMessageTimeout = reception.check_message_timeout(); nbMessageTimeout != 0){
                timeout_messages_signal(nbMessageTimeout);
            }

        }break;
        case DCMessageType::update_delay_settings:{

            Logger::message("[DCServer] update_delay_settings message received.\n");
            DCDelaySettings delay;
            std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(DCDelaySettings), reinterpret_cast<std::byte*>(&delay));

            std::lock_guard l(readerL);
            updateDelaySettingsMessage  = {std::move(endpoint), std::move(delay)};

        }break;
        default:
            break;
        }
    });
}

auto DCServer::Impl::start_reading_thread(const std::string &readingAdress, int readingPort, Protocol protocol) -> bool{

    this->protocol = protocol;
    if(!udpReader.init_socket(readingAdress,readingPort, protocol)){
        return false;
    }
    udpReader.start_reading_thread();

    return true;
}

auto DCServer::Impl::stop_reading_thread() -> void{
    udpReader.stop_reading_thread();
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

        auto message = messagesToSend.pop_front();

        // send synch if no message to send
        if(!message.has_value()){
            for(auto &sender : udpSenders){
                sender.second->send_message(static_cast<MessageTypeId>(DCMessageType::synchro));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        bool frameSent = false;

        if(auto feedbackPtr = std::get_if<Feedback>(&message.value().second); feedbackPtr != nullptr){

            Feedback feedback = *feedbackPtr;
            Logger::message(std::format("[DCClientConnection] Send feedback of type [{}].\n", static_cast<int>(feedback.receivedMessageType)));
            const auto &endPoint = message.value().first;

            if(udpSenders.contains(endPoint.id)){

                udpSenders[endPoint.id]->send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));

                // check if disconnect command received
                if(feedback.receivedMessageType == static_cast<MessageTypeId>(DCMessageType::command)){
                    udpSenders[endPoint.id]->clean_socket();
                    udpSenders.erase(endPoint.id);
                }

            }else if(feedback.receivedMessageType == static_cast<MessageTypeId>(DCMessageType::init_server_client_connection)){

                // add new sender
                udpSenders[endPoint.id] = std::make_unique<UdpSender>();

                // init socket
                if(udpSenders[endPoint.id]->init_socket(endPoint.address, std::to_string(endPoint.port), protocol)){
                    udpSenders[endPoint.id]->send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));
                }
            }

        }else if(auto frame = std::get_if<std::shared_ptr<cam::DCCompressedFrame>>(&message.value().second); frame != nullptr){

            auto beforeSendingFrameTS = Time::nanoseconds_since_epoch();

            std::shared_ptr<cam::DCCompressedFrame> f = *frame;

            // resize buffer if necessary
            size_t totalDataSizeBytes = f->data_size();
            if(senderBuffer.size() < totalDataSizeBytes){
                senderBuffer.resize(totalDataSizeBytes);
            }

            // write data to buffer
            size_t offset = 0;
            auto bData = std::span(senderBuffer.data(), totalDataSizeBytes);
            f->write_to_data(bData, offset);

            // send to all clients
            std::atomic<size_t> count = 0;
            std::for_each(std::execution::par_unseq, std::begin(udpSenders), std::end(udpSenders), [&](auto &senderP){
                if(senderP.second->send_message(static_cast<MessageTypeId>(DCMessageType::compressed_frame_data), bData)){
                    ++count;
                }
            });

            // update last frame id sent if at least on frame sent
            if(count != 0){
                lastFrameIdSent = f->idCapture;
                frameSent = true;
            }

            // update timestamp
            auto afterSendintFrameTS = Time::nanoseconds_since_epoch();
            lastFrameSendingDurationMicrosS = std::chrono::duration_cast<std::chrono::microseconds>(afterSendintFrameTS - beforeSendingFrameTS).count();
            lastFrameSentTS = afterSendintFrameTS;

        }else if(auto pingPtr = std::get_if<Ping>(&message.value().second); pingPtr != nullptr){

            for(auto &sender : udpSenders){
                Feedback pingFeedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received};
                sender.second->send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&pingFeedback), sizeof(Feedback)));
            }
        }

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
}

DCServer::~DCServer(){
    clean();
}

auto DCServer::clean() -> void{

    // stop threads
    i->stop_sending_thread();
    i->stop_reading_thread();
}


auto DCServer::initialize(const std::string &serverSettingsPath) -> bool{

    if(!settings.load_from_file(serverSettingsPath)){
        // ...
        return false;
    }
    settings.globalFilePath = serverSettingsPath;

    if(!i->start_reading_thread(settings.udpReadingInterface.ipAddress, settings.udpServerS.udpReadingPort, settings.udpReadingInterface.protocol)){
        // ...
        return false;
    }

    i->start_sending_thread();

    // ...

    return true;
}

auto DCServer::legacy_initialize(const std::string &legacyNetworkSettingsFilePath) -> bool{

    if(!settings.udpServerS.load_from_file(legacyNetworkSettingsFilePath)){
        return false;
    }
    settings.update_reading_interface();


    if(!i->start_reading_thread(settings.udpReadingInterface.ipAddress, settings.udpServerS.udpReadingPort, settings.udpReadingInterface.protocol)){
        // ...
        return false;
    }

    return true;
}


auto DCServer::update() -> void{

    if(!i->readerL.try_lock()){
        return;
    }

    // check if messages received
    auto initServerClientConnectionM = std::move(i->initServerClientConnectionMessage);
    i->initServerClientConnectionMessage.second = std::nullopt;

    auto commandMessageM = std::move(i->commandMessage);
    i->commandMessage.second = std::nullopt;

    auto updateDelaySettingsMessageM = std::move(i->updateDelaySettingsMessage);
    i->updateDelaySettingsMessage.second = std::nullopt;

    auto updateDeviceSettingsMessageM = std::move(i->updateDeviceSettingsMessage);
    i->updateDeviceSettingsMessage.second = nullptr;

    auto updateColorSettingsMessageM = std::move(i->updateColorSettingsMessage);
    i->updateColorSettingsMessage.second = nullptr;

    auto updateFiltersMessageM = std::move(i->updateFiltersSettingsMessage);
    i->updateFiltersSettingsMessage.second = nullptr;

    i->readerL.unlock();

    // server client connection
    if(initServerClientConnectionM.second.has_value()){
        i->messagesToSend.push_back(std::make_pair(initServerClientConnectionM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::init_server_client_connection), FeedbackType::message_received}));;
        receive_init_server_client_connection_signal(std::move(initServerClientConnectionM.second.value()));
    }

    // delay
    if(updateDelaySettingsMessageM.second.has_value()){
        i->messagesToSend.push_back(std::make_pair(updateDelaySettingsMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_delay_settings), FeedbackType::message_received}));
        receive_delay_settings_signal(updateDelaySettingsMessageM.second.value());
    }

    // command
    if(commandMessageM.second.has_value()){

        switch(commandMessageM.second.value()){
        case Command::shutdown:
            i->messagesToSend.push_back(std::make_pair(commandMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::shutdown}));
            shutdown_signal();
            break;
        case Command::restart:
            i->messagesToSend.push_back(std::make_pair(commandMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::restart}));
            restart_signal();
            break;
        case Command::disconnect:
            i->messagesToSend.push_back(std::make_pair(commandMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::disconnect}));
            break;
        case Command::quit:
            i->messagesToSend.push_back(std::make_pair(commandMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::command), FeedbackType::quit}));
            quit_signal();
            break;
        }
    }

    // device settings
    if(updateDeviceSettingsMessageM.second){
        i->messagesToSend.push_back(std::make_pair(updateDeviceSettingsMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_device_settings), FeedbackType::message_received}));
        settings.deviceS = *updateDeviceSettingsMessageM.second;
        receive_device_settings_signal(updateDeviceSettingsMessageM.second);
    }

    // color settings
    if(updateColorSettingsMessageM.second){
        i->messagesToSend.push_back(std::make_pair(updateColorSettingsMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_color_settings), FeedbackType::message_received}));
        settings.colorS = *updateColorSettingsMessageM.second;
        receive_color_settings_signal(updateColorSettingsMessageM.second);
    }

    // filters settings
    if(updateFiltersMessageM.second){
        i->messagesToSend.push_back(std::make_pair(updateFiltersMessageM.first, Feedback{static_cast<MessageTypeId>(DCMessageType::update_filters_settings), FeedbackType::message_received}));
        settings.filtersS = *updateFiltersMessageM.second;
        receive_filters_signal(updateFiltersMessageM.second);
    }
}

auto DCServer::load_device_settings_file(const std::string &settingsFilePath) -> bool{

    if(settings.deviceS.load_from_file(settingsFilePath)){
        settings.deviceFilePath = settingsFilePath;
    }else{
        Logger::error("[DCServer::load_device_settings_file No device settings file found for device n°[{}]], default parameters used instead.\n");
        return false;
    }

    return true;
}

auto DCServer::load_filters_settings_file(const std::string &settingsFilePath) -> bool{

    if(settings.filtersS.load_from_file(settingsFilePath)){
        settings.filtersFilePath = settingsFilePath;
    }else{
        Logger::error("[DCServer::load_filters_settings_file No device settings file found for device n°[{}]], default parameters used instead.\n");
        return false;
    }

    return true;
}

auto DCServer::load_calibration_filters_settings_file(const std::string &settingsFilePath) -> bool{

    if(settings.calibrationFiltersS.load_from_file(settingsFilePath)){
        settings.calibrationFiltersFilePath = settingsFilePath;
    }else{
        Logger::error("[DCServer::load_calibration_filters_settings_file No device settings file found for device n°[{}]], default parameters used instead.\n");
        return false;
    }

    return true;
}

auto DCServer::load_color_settings_file(const std::string &settingsFilePath) -> bool{

    if(settings.colorS.load_from_file(settingsFilePath)){
        settings.colorFilePath = settingsFilePath;
    }else{
        Logger::error("[DCServer::load_color_settings_file No device settings file found for device n°[{}]], default parameters used instead.\n");
        return false;
    }

    return true;
}

auto DCServer::load_model_settings_file(const std::string &settingsFilePath) -> bool{

    if(settings.modelS.load_from_file(settingsFilePath)){
        settings.modelFilePath = settingsFilePath;
    }else{
        Logger::error("[DCServer::load_model_settings_file No device settings file found for device n°[{}]], default parameters used instead.\n");
        return false;
    }

    return true;
}

auto DCServer::send_frame(std::shared_ptr<cam::DCCompressedFrame> frame) -> void{
    i->messagesToSend.push_back(std::make_pair(EndPoint{}, std::move(frame)));
}

auto DCServer::ping_server() -> void{
    i->messagesToSend.push_back(std::make_pair(EndPoint{}, Ping{}));
}

auto DCServer::last_frame_id_sent() const -> size_t{
    return i->lastFrameIdSent;
}

auto DCServer::last_frame_sent_timestamp_nanosecond() const -> nanoseconds {
    return i->lastFrameSentTS;
}

auto DCServer::last_frame_sending_duration_micros_s() const -> int64_t{
    return i->lastFrameSendingDurationMicrosS;
}

auto DCServer::simulate_sending_failure(bool enabled, int percentage) -> void{    
    //m_udpServerSender.simulate_failure(enabled, percentage);
}





