

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

#include "dc_udp_reader.hpp"

// local
#include "depth-camera/dc_compressed_frame.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"

using namespace tool::cam;
using namespace tool::net;
using namespace std::chrono;


auto DCClientUdpReader::process_packet(std::span<std::int8_t> packet) -> void{

    auto headerData    = packet.first(sizeof(Header));
    auto dataToProcess = packet.subspan(sizeof(Header));

    Header header(headerData);

    switch (static_cast<MessageType>(header.type)) {
    case MessageType::init_network_infos:{
        Logger::message("[DCClientUdpReader] init_network_infos message received.\n");
        UdpNetworkSendingSettings initNetworkInfo;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(UdpNetworkSendingSettings), reinterpret_cast<std::int8_t*>(&initNetworkInfo));
        init_network_infos_signal(std::move(header), std::make_shared<UdpNetworkSendingSettings>(std::move(initNetworkInfo)));
    }break;
    case MessageType::update_device_settings:{
        Logger::message("[DCClientUdpReader] update_device_settings message received.\n");
        DCDeviceSettings deviceSettings;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(DCDeviceSettings), reinterpret_cast<std::int8_t*>(&deviceSettings));
        update_device_settings_signal(std::move(header), std::make_shared<DCDeviceSettings>(std::move(deviceSettings)));
    }break;
    case MessageType::update_filters:{

        filtersReception.update(header, dataToProcess, messagesBuffer);

        if(auto info = filtersReception.message_fully_received(header); info.has_value()){
            Logger::message("[DCClientUdpReader] update_filters message received.\n");
            size_t offset = 0;
            update_filters_signal(std::move(header), std::make_shared<cam::DCFiltersSettings>(info->messageData.data(), offset, info->messageData.size_bytes()));
        }

        if(auto nbMessageTimeout = filtersReception.check_timeout_frames(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

    }break;
    case MessageType::update_color_settings:{
        Logger::message("[DCClientUdpReader] update_color_settings message received.\n");
        DCColorSettings colorSettings;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(DCColorSettings), reinterpret_cast<std::int8_t*>(&colorSettings));
        update_color_settings_signal(std::move(header), std::make_shared<DCColorSettings>(std::move(colorSettings)));
    }break;
    case MessageType::delay:{
        Logger::message("[DCClientUdpReader] delay message received.\n");
        DCDelaySettings delaySettings;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(DCDelaySettings), reinterpret_cast<std::int8_t*>(&delaySettings));
        update_delay_signal(std::move(header), delaySettings);
    }break;
    case MessageType::command:{
        Logger::message("[DCClientUdpReader] command message received.\n");
        Command command;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(Command), reinterpret_cast<std::int8_t*>(&command));
        command_signal(std::move(header), command);
    }break;
    default:
        break;
    }
}


auto DCServerUdpReader::process_packet(std::span<std::int8_t> packet) -> void{

    auto headerData    = packet.first(sizeof(Header));
    auto dataToProcess = packet.subspan(sizeof(Header));

    Header header(headerData);

    // check integrity
    if(header.currentPacketSizeBytes != packet.size_bytes()){
        // drop packet
        Logger::error(std::format("[DCServerUdpReader::process_packet] Invalid packet size: [{}], expected: [{}]\n", header.currentPacketSizeBytes, packet.size_bytes()));
        return;
    }

    switch (static_cast<MessageType>(header.type)) {
    case MessageType::synchro:{
        synchro.update_average_difference(header.currentPacketTimestampNs);
        synchro_signal(synchro.averageDiffNs);
    }break;
    case MessageType::feedback:{
        Feedback feedback;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(feedback), reinterpret_cast<std::int8_t*>(&feedback));
        feedback_signal(std::move(header), feedback);
    }break;
    case MessageType::compressed_frame_data:{

        cFramesReception.update(header, dataToProcess, messagesBuffer);

        if(auto info = cFramesReception.message_fully_received(header); info.has_value()){

            // create compressed frame from data
            auto cFrame = std::make_shared<DCCompressedFrame>();

            // init compressed frame from data packets
            size_t offset = 0;
            cFrame->init_from_data(info->messageData, offset);

            // update received TS with first packet received TS
            auto diffCaptureSending = (info->firstPacketSentTS.count() - cFrame->afterCaptureTS);
            cFrame->receivedTS = info->firstPacketReceivedTS.count() - diffCaptureSending;

            // add average diff to capture timestamp
            cFrame->afterCaptureTS += synchro.averageDiffNs;

            framerate.add_frame();

            // update latency
            latency.update_average_latency(Time::difference_micro_s(nanoseconds(cFrame->afterCaptureTS), Time::nanoseconds_since_epoch()).count());

            // send compressed frame
            compressed_frame_signal(std::move(header), std::move(cFrame));
        }

        if(auto nbMessageTimeout = cFramesReception.check_timeout_frames(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

        status_signal(UdpReceivedStatus{cFramesReception.get_percentage_success(), framerate.get_framerate(), latency.averageLatency});

    }break;  
    default:
        break;
    }
}


