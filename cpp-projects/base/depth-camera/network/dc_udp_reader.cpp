

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
#include "data/checksum.hpp"

using namespace tool::cam;
using namespace tool::net;
using namespace std::chrono;

auto DCClientUdpReader::process_packet(std::span<const std::byte> packet) -> void{

    auto headerData    = packet.first(sizeof(Header));
    auto dataToProcess = packet.subspan(sizeof(Header));

    Header header(headerData);

    // check integrity
    if(header.currentPacketSizeBytes != packet.size_bytes()){
        // drop packet
        Logger::error(std::format("[DCClientUdpReader::process_packet] Invalid packet size: [{}], expected: [{}]\n", header.currentPacketSizeBytes, packet.size_bytes()));
        return;
    }

    if(header.checkSum != 0){
        auto checksum = data::Checksum::gen_crc16(dataToProcess);
        if(checksum != header.checkSum){
            // drop packet
            Logger::error(std::format("[DCClientUdpReader::process_packet] Invalid checksum size: [{}], expected: [{}]\n", checksum, header.checkSum));
            return;
        }
    }

    switch (static_cast<MessageType>(header.type)) {
    case MessageType::init_network_infos:{
        Logger::message("[DCClientUdpReader] init_network_infos message received.\n");
        UdpNetworkSendingSettings network;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(UdpNetworkSendingSettings), reinterpret_cast<std::byte*>(&network));
        init_network_infos_signal(std::move(header), std::move(network));
    }break;
    case MessageType::delay:{

        Logger::message("[DCClientUdpReader] delay message received.\n");
        DCDelaySettings delay;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(DCDelaySettings), reinterpret_cast<std::byte*>(&delay));
        update_delay_signal(std::move(header), std::move(delay));

    }break;
    case MessageType::command:{

        Logger::message("[DCClientUdpReader] command message received.\n");
        Command command;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(Command), reinterpret_cast<std::byte*>(&command));
        command_signal(std::move(header), command);

    }break;
    case MessageType::update_device_settings:{

        deviceReception.update(header, dataToProcess, messagesBuffer);

        if(auto info = deviceReception.message_fully_received(header); info.has_value()){
            Logger::message("[DCClientUdpReader] update_device_settings message received.\n");
            update_device_settings_signal(std::move(header), std::make_shared<cam::DCDeviceSettings>(
                std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
            ));
        }

        if(auto nbMessageTimeout = deviceReception.check_message_timeout(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

    }break;
    case MessageType::update_filters:{

        filtersReception.update(header, dataToProcess, messagesBuffer);

        if(auto info = filtersReception.message_fully_received(header); info.has_value()){
            Logger::message("[DCClientUdpReader] update_filters message received.\n");
            update_filters_signal(std::move(header), std::make_shared<cam::DCFiltersSettings>(
                std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
            ));
        }

        if(auto nbMessageTimeout = filtersReception.check_message_timeout(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

    }break;
    case MessageType::update_color_settings:{

        colorReception.update(header, dataToProcess, messagesBuffer);

        if(auto info = colorReception.message_fully_received(header); info.has_value()){
            Logger::message("[DCClientUdpReader] update_color_settings message received.\n");
            update_color_settings_signal(std::move(header), std::make_shared<cam::DCColorSettings>(
                std::span<const std::uint8_t>(reinterpret_cast<const std::uint8_t*>(info->messageData.data()),info->messageData.size_bytes())
            ));
        }

        if(auto nbMessageTimeout = colorReception.check_message_timeout(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

    }break;
    default:
        break;
    }
}


auto DCServerUdpReader::process_packet(std::span<const std::byte> packet) -> void{

    auto headerData    = packet.first(sizeof(Header));
    auto dataToProcess = packet.subspan(sizeof(Header));

    Header header(headerData);

    // check integrity
    if(header.currentPacketSizeBytes != packet.size_bytes()){
        // drop packet
        Logger::error(std::format("[DCServerUdpReader::process_packet] Invalid packet size: [{}], expected: [{}]\n", header.currentPacketSizeBytes, packet.size_bytes()));
        return;
    }

    if(header.checkSum != 0){
        auto checksum = data::Checksum::gen_crc16(dataToProcess);
        if(checksum != header.checkSum){
            // drop packet
            Logger::error(std::format("[DCServerUdpReader::process_packet] Invalid checksum size: [{}], expected: [{}]\n", checksum, header.checkSum));
            return;
        }
    }


    switch (static_cast<MessageType>(header.type)) {
    case MessageType::synchro:{
        synchro.update_average_difference(header.currentPacketTimestampNs);
        synchro_signal(synchro.averageDiffNs);
    }break;
    case MessageType::feedback:{
        Feedback feedback;
        std::copy(dataToProcess.data(), dataToProcess.data() + sizeof(feedback), reinterpret_cast<std::byte*>(&feedback));
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

            // update bandwitdh
            bandwidth.add_size(info->totalBytesReceived);

            // send compressed frame
            compressed_frame_signal(std::move(header), std::move(cFrame));
        }

        if(auto nbMessageTimeout = cFramesReception.check_message_timeout(); nbMessageTimeout != 0){
            timeout_messages_signal(nbMessageTimeout);
        }

        network_status_signal(UdpNetworkStatus{cFramesReception.get_percentage_success(), bandwidth.get_bandwidth()});

    }break;  
    default:
        break;
    }
}


