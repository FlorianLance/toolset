

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
    AverageSynch synchro;
    AverageBandwidth bandwidth;
    UdpMessageReception cFramesReception;
    std::atomic<size_t> totalReceivedBytes = 0;

    std::int64_t averageTimestampDiffNs = 0;    
};

DCClientRemoteDevice::DCClientRemoteDevice() : i(std::make_unique<DCClientRemoteDevice::Impl>()){
    i->messagesBuffer.resize(100, 0);
}

DCClientRemoteDevice::~DCClientRemoteDevice(){
    Logger::log("[DCClientRemoteDevice::clean] 3\n");
    DCClientRemoteDevice::clean();
}

auto DCClientRemoteDevice::initialize(const DCDeviceConnectionSettings &connectionS) -> bool {

    auto lg = LogGuard("[DCClientRemoteDevice::initialize]");
    i->remoteServerS = connectionS;

    Logger::message(
        std::format(
            "DCServerRemoteDevice: initialize:"
            "\n\tReading interface id: [{}]"
            "\n\tReading address: [{}]"
            "\n\tReading port: [{}]"
            "\n\tSending address: [{}]"
            "\n\tSending port: [{}]"
            "\n\tProtocol: [{}].\n",
            i->remoteServerS.idReadingInterface,
            i->remoteServerS.readingAddress,
            i->remoteServerS.readingPort,
            i->remoteServerS.sendingAddress,
            i->remoteServerS.sendingPort,
            static_cast<int>(i->remoteServerS.protocol)
        )
    );


    // init reader
    Logger::message(std::format("DCClientRemoteDevice init reader: {} {} {}\n", i->remoteServerS.readingAddress, std::to_string(i->remoteServerS.readingPort), static_cast<int>(i->remoteServerS.protocol)));
    if(!i->udpReader.init_socket(i->remoteServerS.readingAddress, i->remoteServerS.readingPort, i->remoteServerS.protocol)){
        Logger::error("[DCClientRemoteDevice]: Cannot init udp reader.\n");
        return false;
    }

    if(i->remoteServerS.startReadingThread){
        i->udpReader.start_threads();
    }

    // init sender
    Logger::message(std::format("DCClientRemoteDevice init sender: {} {} {}\n", i->remoteServerS.sendingAddress, std::to_string(i->remoteServerS.sendingPort), static_cast<int>(i->remoteServerS.protocol)));
    if(!i->udpSender.init_socket(i->remoteServerS.sendingAddress, std::to_string(i->remoteServerS.sendingPort), i->remoteServerS.protocol)){
        Logger::error("[DCClientRemoteDevice::initialize] Cannot init udp sender.\n");
        return false;
    }

    // set connection
    i->udpReader.packed_received_signal.connect(&DCClientRemoteDevice::process_received_packet, this);

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

    auto lg = LogGuard("[DCClientRemoteDevice::clean]"sv);

    // clean sender
    // # disconnect client
    if(i->udpSender.is_connected()){
        Logger::log("[DCClientRemoteDevice::clean] Send disconnect message.\n"sv);
        Command command = Command::disconnect;
        i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::command), std::span(reinterpret_cast<const std::byte*>(&command), sizeof(Command)));
    }
    // # clean socket
    Logger::log("[DCClientRemoteDevice::clean] Clean sender socket.\n"sv);
    i->udpSender.clean_socket();

    // clean reader
    // # remove connections
    Logger::log("[DCClientRemoteDevice::clean] Remove connection.\n"sv);
    i->udpReader.packed_received_signal.disconnect(&DCClientRemoteDevice::process_received_packet, this);

    i->remoteDeviceConnected = false;
    // # stop reading loop
    if(i->udpReader.are_threads_started()){
        Logger::log("[DCClientRemoteDevice::clean] Stop reading loop.\n"sv);
        i->udpReader.stop_threads();
    }
    // # clean socket
    Logger::log("[DCClientRemoteDevice::clean] Clean reader socket.\n"sv);
    i->udpReader.clean_socket();
}

auto DCClientRemoteDevice::apply_command(Command command) -> void{

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

auto DCClientRemoteDevice::update_delay_settings(const cam::DCDelaySettings &delayS) -> void{
    auto bData = delayS.convert_to_json_binary();
    i->udpSender.send_message(static_cast<MessageTypeId>(DCMessageType::update_delay_settings), std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
}

auto DCClientRemoteDevice::device_connected() const noexcept -> bool {
    return i->remoteDeviceConnected;
}

auto DCClientRemoteDevice::read_data_from_network() -> size_t{
    return i->udpReader.receive_data_from_external_thread();
}

auto DCClientRemoteDevice::trigger_received_packets() -> void{
    i->udpReader.trigger_received_packets_from_external_thread();
}

auto DCClientRemoteDevice::process_received_packet(EndPointId endpoint, Header header, std::span<const std::byte> dataToProcess) -> void{

    switch (static_cast<DCMessageType>(header.type)) {
    case DCMessageType::synchro:{

        // update synchro
        i->synchro.update_average_difference(header.currentPacketTimestampNs);

        // trigger synchro signal
        remote_synchro_signal(i->synchro.averageDiffNs);

    }break;
    case DCMessageType::feedback:{

        Feedback feedback;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(feedback), reinterpret_cast<std::byte*>(&feedback));

        // Logger::message(std::format("RECEIVED FEEDBACK {} {} {} {}\n", (int)feedback.receivedMessageType, (int)feedback.feedback, dataToProcess.size_bytes(), dataToProcess.size()));

        // process feedback
        receive_feedback(std::move(header), feedback);

    }break;
    case DCMessageType::compressed_frame_data:{

        i->cFramesReception.update(header, dataToProcess, i->messagesBuffer);

        if(auto info = i->cFramesReception.message_fully_received(header); info.has_value()){

            // create compressed frame from data
            auto cFrame = std::make_shared<DCCompressedFrame>();

            // init compressed frame from data packets
            size_t offset = 0;
            cFrame->init_from_data(info->messageData, offset);

            // update received TS with first packet received TS
            auto diffCaptureSending = (info->firstPacketSentTS.count() - cFrame->afterCaptureTS);
            cFrame->receivedTS = info->firstPacketReceivedTS.count() - diffCaptureSending;

            // add average diff to capture timestamp
            cFrame->afterCaptureTS += i->synchro.averageDiffNs;

            // update bandwitdh
            i->bandwidth.add_size(info->totalBytesReceived);

            // send compressed frame
            receive_compressed_frame(std::move(header), std::move(cFrame));
        }

        if(auto nbMessageTimeout = i->cFramesReception.check_message_timeout(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

        receive_network_status(UdpNetworkStatus{i->cFramesReception.get_percentage_success(), i->bandwidth.get_bandwidth()});

    }break;
    default:
        break;
    }
}

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
        Logger::message("DCServerRemoteDevice: device disconnected\n");
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

    remote_feedback_signal(std::move(message));
}

auto DCClientRemoteDevice::receive_compressed_frame(Header h, std::shared_ptr<cam::DCCompressedFrame> cFrame) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;
    if(cFrame){

        // update framerate
        framerate.add_frame();

        // update latency
        latency.update_average_latency(Time::difference_micro_s(std::chrono::nanoseconds(cFrame->afterCaptureTS), Time::nanoseconds_since_epoch()).count());

        // send frame
        remote_frame_signal(std::move(cFrame));

        // send status
        data_status_signal(UdpDataStatus{framerate.get_framerate(), latency.averageLatency});
    }
}

auto DCClientRemoteDevice::receive_network_status(UdpNetworkStatus status) -> void{
    remote_network_status_signal(status);
}
