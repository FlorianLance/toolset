

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

#include "dc_client_remote_device.hpp"

// std
#include <format>

// local
#include "network/udp_sender.hpp"
#include "network/udp_reader.hpp"
#include "network/settings/udp_connection_settings.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"

using namespace tool::net;
using namespace tool::cam;
using namespace std::chrono;

struct DCClientRemoteDevice::Impl{

    // connection
    DCDeviceConnectionSettings remoteServerS;
    bool remoteDeviceConnected = false;

    // send
    UdpSender udpSender;
    std::uint16_t maxSizeUpdMessage = 9000;

    // read
    UdpReader udpReader;
    DoubleRingBuffer<std::byte> messagesBuffer;
    AverageSynchBuffer synchro;
    AverageBandwidthBuffer bandwidth;
    AverageBuffer receptionDurationNS;

    UdpMessageReception dFramesReception;
    std::atomic<size_t> totalReceivedBytes = 0;
};

DCClientRemoteDevice::DCClientRemoteDevice() : i(std::make_unique<DCClientRemoteDevice::Impl>()){
    i->messagesBuffer.resize(100, 0);
}

DCClientRemoteDevice::~DCClientRemoteDevice(){
    DCClientRemoteDevice::clean();
}

auto DCClientRemoteDevice::initialize(const DCDeviceConnectionSettings &connectionS) -> bool {
    
    auto lg = LogG("[DCClientRemoteDevice::initialize]");
    i->remoteServerS = connectionS;
    
    Log::message(
        std::format(
            "DCServerRemoteDevice: initialize:"
            "\n\tReading interface id: [{}]"
            "\n\tAny reading interface: [{}]"
            "\n\tReading address: [{}]"
            "\n\tReading port: [{}]"
            "\n\tSending address: [{}]"
            "\n\tSending port: [{}]"
            "\n\tProtocol: [{}].\n",
            i->remoteServerS.idReadingInterface,
            i->remoteServerS.anyReadingInterface,
            i->remoteServerS.readingAddress,
            i->remoteServerS.readingPort,
            i->remoteServerS.processedSendingAddress,
            i->remoteServerS.sendingPort,
            static_cast<int>(i->remoteServerS.protocol)
        )
    );

    // init reader
    if(!i->udpReader.init_socket(i->remoteServerS.anyReadingInterface ? "" : i->remoteServerS.readingAddress, i->remoteServerS.readingPort, i->remoteServerS.protocol)){
        Log::error("[DCClientRemoteDevice]: Cannot init udp reader.\n");
        return false;
    }

    if(i->remoteServerS.startReadingThread){
        i->udpReader.start_threads();
    }

    // init sender
    Log::message(std::format("DCClientRemoteDevice init sender: {} {} {}\n", i->remoteServerS.processedSendingAddress, std::to_string(i->remoteServerS.sendingPort), static_cast<int>(i->remoteServerS.protocol)));
    if(!i->udpSender.init_socket(i->remoteServerS.processedSendingAddress, std::to_string(i->remoteServerS.sendingPort), i->remoteServerS.protocol)){
        Log::error("[DCClientRemoteDevice::initialize] Cannot init udp sender.\n");
        return false;
    }

    // set connection
    i->udpReader.packets_received_signal.connect(&DCClientRemoteDevice::process_received_packets, this);

    return true;
}

auto DCClientRemoteDevice::init_remote_connection(size_t idClient) -> void{

    UdpConnectionSettings connectionSettings;
    connectionSettings.address          = i->remoteServerS.readingAddress;
    connectionSettings.port             = i->remoteServerS.readingPort;
    connectionSettings.maxPacketSize    = i->maxSizeUpdMessage;

    auto bData = connectionSettings.convert_to_json_binary();
    i->udpSender.set_sender_id(idClient);
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::init_server_client_connection),  std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
}

auto DCClientRemoteDevice::clean() -> void{
    
    auto lg = LogG("[DCClientRemoteDevice::clean]"sv);

    // clean sender
    // # disconnect client
    if(i->udpSender.is_connected()){
        Log::log("[DCClientRemoteDevice::clean] Send disconnect message.\n"sv);
        Command command = Command::disconnect;
        i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::command), std::span(reinterpret_cast<const std::byte*>(&command), sizeof(Command)));
    }
    // # clean socket
    Log::log("[DCClientRemoteDevice::clean] Clean sender socket.\n"sv);
    i->udpSender.clean_socket();

    // clean reader
    // # remove connections
    Log::log("[DCClientRemoteDevice::clean] Remove connection.\n"sv);
    i->udpReader.packets_received_signal.disconnect(&DCClientRemoteDevice::process_received_packets, this);

    i->remoteDeviceConnected = false;
    // # stop reading loop
    if(i->udpReader.are_threads_started()){
        Log::log("[DCClientRemoteDevice::clean] Stop reading loop.\n"sv);
        i->udpReader.stop_threads();
    }
    // # clean socket
    Log::log("[DCClientRemoteDevice::clean] Clean reader socket.\n"sv);
    i->udpReader.clean_socket();
}

auto DCClientRemoteDevice::apply_command(Command command) -> void{
    
    Log::log("DCClientRemoteDevice::apply_command\n");

    bool isCommandValid = false;
    switch(command){
    case Command::disconnect:
        isCommandValid = true;
        break;
    case Command::quit:
        isCommandValid = true;
        break;
    case Command::shutdown:
        isCommandValid = true;
        break;
    case Command::restart:
        isCommandValid = true;
        break;
    }

    if(isCommandValid){
        i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::command), std::span(reinterpret_cast<const std::byte*>(&command), sizeof(Command)));
    }
    
    Log::log("DCClientRemoteDevice::apply_command end\n");
}

auto DCClientRemoteDevice::ping() -> void{
    Feedback feedback;
    feedback.type = FeedbackType::ping;
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::feedback), std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback)));
}

auto DCClientRemoteDevice::update_device_settings(const cam::DCDeviceSettings &deviceS) -> void{
    auto bData = deviceS.convert_to_json_binary();
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::update_device_settings), std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
}

auto DCClientRemoteDevice::update_color_settings(const cam::DCColorSettings &colorS) -> void{
    auto bData = colorS.convert_to_json_binary();
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::update_color_settings), std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
}

auto DCClientRemoteDevice::update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void{
    auto bData = filtersS.convert_to_json_binary();
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::update_filters_settings), std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
}

auto DCClientRemoteDevice::update_misc_settings(const cam::DCMiscSettings &miscS) -> void{
    auto bData = miscS.convert_to_json_binary();
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::update_misc_settings), std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
}

auto DCClientRemoteDevice::device_connected() const noexcept -> bool {
    return i->remoteDeviceConnected;
}

auto DCClientRemoteDevice::read_data_from_external_thread() -> size_t{
    return i->udpReader.receive_data_from_external_thread();
}

auto DCClientRemoteDevice::trigger_received_packets() -> void{
    i->udpReader.trigger_received_packets_from_external_thread();
}

auto DCClientRemoteDevice::process_received_packets(std::span<net::PData> packetsToProcess) -> void{

    // read synchro packets
    bool triggerSynch = false;
    for(const auto &packet : packetsToProcess){

        if(static_cast<DCMessageType>(packet.header.type) == DCMessageType::synchro){
            // update synchro
            i->synchro.update_average_difference(nanoseconds(packet.header.receptionTimestampNs) - nanoseconds(packet.header.creationTimestampNs));
            triggerSynch = true;
        }
    }

    // read data frames
    bool triggerNetwork = false;
    size_t nbTimeout = 0;
    for(const auto &packet : packetsToProcess){

        if(static_cast<DCMessageType>(packet.header.type) == DCMessageType::data_frame){

            i->dFramesReception.update(packet.header, packet.data, i->messagesBuffer);

            if(auto info = i->dFramesReception.message_fully_received(packet.header); info.has_value()){

                // create compressed frame from data
                auto dFrame = std::make_shared<DCDataFrame>();

                // init compressed frame from data packets
                size_t offset = 0;
                dFrame->init_from_data(info->messageData, offset);

                // update received TS with first packet received TS
                dFrame->receivedTS = info->firstPacketReceptionTS.count();

                // auto diff = dFrame->afterCaptureTS;
                // add average diff to capture timestamp
                dFrame->afterCaptureTS = (nanoseconds(dFrame->afterCaptureTS) + i->synchro.averageDiffNS).count();

                // auto now = Time::nanoseconds_since_epoch();

                // Log::message(std::format("[{}] [{}] [{}] [{}] [{}] [{}]\n",
                //     i->synchro.averageDiffNS,
                //     Time::difference_micro_s(nanoseconds(diff),                         now).count(),
                //     Time::difference_micro_s(nanoseconds(dFrame->afterCaptureTS),       now).count(),
                //     Time::difference_micro_s(nanoseconds(info->firstPacketEmissionTS),  now).count(),
                //     Time::difference_micro_s(nanoseconds(info->firstPacketReceptionTS), now).count(),
                //     duration_cast<microseconds>(nanoseconds(i->synchro.averageDiffNS))
                // ));

                // auto receptionDurationNs  =
                //     Time::nanoseconds_since_epoch().count() - info->firstPacketEmissionTS.count() + i->synchro.averageDiffNS;
                // auto receptionDurationNs  =
                    // Time::nanoseconds_since_epoch().count() - info->firstPacketEmissionTS.count() - 15000;

                auto firstPacketEmissionTS = info->firstPacketEmissionTS + i->synchro.averageDiffNS;
                // auto firstPacketEmissionTS = info->firstPacketReceptionTS;// + nanoseconds(i->synchro.averageDiffNS);
                auto receptionDurationNs = Time::nanoseconds_since_epoch() - firstPacketEmissionTS;

                // send compressed frame
                receive_data_frame(std::move(packet.header), std::move(dFrame));

                // update duration reception
                i->receptionDurationNS.add_value(receptionDurationNs.count());

                // update bandwitdh
                i->bandwidth.add_size(info->totalBytesReceived);
            }

            nbTimeout += i->dFramesReception.check_message_timeout();
            triggerNetwork = true;
        }
    }

    // read feedbacks
    for(const auto &packet : packetsToProcess){

        if(static_cast<DCMessageType>(packet.header.type) == DCMessageType::feedback){

            Feedback feedback;
            std::copy(packet.data.data(), packet.data.data() + sizeof(feedback), reinterpret_cast<std::byte*>(&feedback));

            // process feedback
            receive_feedback(std::move(packet.header), feedback);
        }
    }

    // trigger
    if(triggerSynch){
        remote_synchro_signal(i->synchro.averageDiffNS.count());
    }

    if(nbTimeout != 0){
        timeout_messages_signal(nbTimeout);
    }

    if(triggerNetwork){
        receive_network_status(UdpNetworkStatus{i->dFramesReception.get_percentage_success(), i->bandwidth.get_bandwidth(), i->receptionDurationNS.get()});
    }
}

// auto DCClientRemoteDevice::process_received_packet(net::PData packetToProcess) -> void{

//     switch (static_cast<DCMessageType>(packetToProcess.header.type)) {
//     case DCMessageType::synchro:{

//         // update synchro
//         i->synchro.update_average_difference(packetToProcess.header.receptionTimestampNs - packetToProcess.header.creationTimestampNs);

//         // trigger synchro signal
//         // remote_synchro_signal(i->synchro.averageDiffNs);
//         remote_synchro_signal(i->synchro.averageDiffNS);

//         // Log::message(std::format("[S-{}|{}] ", header.messageId, Time::difference_micro_s(std::chrono::nanoseconds(header.currentPacketTimestampNs), Time::nanoseconds_since_epoch()).count()));


//     }break;
//     case DCMessageType::feedback:{

//         Feedback feedback;
//         std::copy(packetToProcess.data.data(), packetToProcess.data.data() + sizeof(feedback), reinterpret_cast<std::byte*>(&feedback));

//         // Log::message(std::format("RECEIVED FEEDBACK {} {} {} {}\n", (int)feedback.receivedMessageType, (int)feedback.type, dataToProcess.size_bytes(), dataToProcess.size()));

//         // process feedback
//         receive_feedback(std::move(packetToProcess.header), feedback);

//     }break;
//     case DCMessageType::data_frame:{

//         i->dFramesReception.update(packetToProcess.header, packetToProcess.data, i->messagesBuffer);

//         if(auto info = i->dFramesReception.message_fully_received(packetToProcess.header); info.has_value()){

//             // create compressed frame from data
//             auto dFrame = std::make_shared<DCDataFrame>();

//             // init compressed frame from data packets
//             size_t offset = 0;
//             dFrame->init_from_data(info->messageData, offset);

//             // update received TS with first packet received TS
//             dFrame->receivedTS = info->firstPacketReceivedTS.count();
//             // auto diffCaptureSending = (info->firstPacketSentTS.count() - dFrame->afterCaptureTS);
//             // dFrame->receivedTS = info->firstPacketReceivedTS.count() - diffCaptureSending;

//             // add average diff to capture timestamp
//             // dFrame->afterCaptureTS += i->synchro.averageDiffNs;
//             dFrame->afterCaptureTS += i->synchro.averageDiffNS;

//             // update bandwitdh
//             i->bandwidth.add_size(info->totalBytesReceived);

//             // send compressed frame
//             receive_data_frame(std::move(packetToProcess.header), std::move(dFrame));
//         }

//         if(auto nbMessageTimeout = i->dFramesReception.check_message_timeout(); nbMessageTimeout != 0){
//             timeout_messages_signal(nbMessageTimeout);
//         }

//         receive_network_status(UdpNetworkStatus{i->dFramesReception.get_percentage_success(), i->bandwidth.get_bandwidth()});

//     }break;
//     default:
//         break;
//     }
// }

auto DCClientRemoteDevice::receive_feedback(Header h, Feedback message) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;

    switch(message.type){
    case FeedbackType::message_received:
        if(message.receivedMessageType == static_cast<MessageTypeId>(DCMessageType::init_server_client_connection)){
            i->remoteDeviceConnected = true;
        }
        break;
    case FeedbackType::disconnect:
        Log::message("DCServerRemoteDevice: device disconnected\n");
        i->remoteDeviceConnected = false;
        break;
    case FeedbackType::quit:
        i->remoteDeviceConnected = false;
        break;
    case FeedbackType::shutdown:
        i->remoteDeviceConnected = false;
        break;
    case FeedbackType::restart:
        i->remoteDeviceConnected = false;
        break;
    default:
        break;
    }

    remote_feedback_signal(idClient, std::move(message));
}

auto DCClientRemoteDevice::receive_data_frame(Header h, std::shared_ptr<cam::DCDataFrame> dFrame) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;
    if(dFrame){

        // update framerate
        framerate.add_frame();

        // update latency
        latency.update_average_latency(Time::difference_micro_s(std::chrono::nanoseconds(dFrame->afterCaptureTS), Time::nanoseconds_since_epoch()).count());
        // Log::message(std::format("[{}] ", Time::difference_micro_s(std::chrono::nanoseconds(dFrame->afterCaptureTS), Time::nanoseconds_since_epoch()).count()));

        // send frame
        remote_data_frame_signal(idClient, std::move(dFrame));

        // send status
        data_status_signal(UdpDataStatus{framerate.get_framerate(), latency.averageLatency});
    }
}

auto DCClientRemoteDevice::receive_network_status(UdpNetworkStatus status) -> void{
    remote_network_status_signal(status);
}
