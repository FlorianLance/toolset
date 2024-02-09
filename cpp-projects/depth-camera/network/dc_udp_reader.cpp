

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
#include "utility/time.hpp"
#include "camera/dc_compressed_frame.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;
using namespace tool::net;


auto DCClientUdpReader::process_packet(std::vector<char> *packet, size_t nbBytes) -> void{

    auto packetData = reinterpret_cast<std::int8_t*>(packet->data());

    Header header(packetData);
    packetData += sizeof(Header);

    switch (static_cast<MessageType>(header.type)) {
    case MessageType::init_network_infos:{
        Logger::message("[DCClientUdpReader] init_network_infos message received.\n");
        init_network_infos_signal(header, UdpMonoPacketData::generate_data_from_packet<UdpNetworkSendingSettings>(packetData, sizeof(UdpNetworkSendingSettings)));
    }break;
    case MessageType::update_device_settings:{
        Logger::message("[DCClientUdpReader] update_device_settings message received.\n");
        update_device_settings_signal(std::move(header), std::make_shared<UdpMonoPacketMessage<DCDeviceSettings>>(packetData));
    }break;
    case MessageType::update_filters:{

        if(!filtersMessage.copy_packet_to_data(header, nbBytes, packetData, m_data)){
            break;
        }
        if(filtersMessage.all_received(header)){
            Logger::message("[DCClientUdpReader] update_filters message received.\n");
            size_t offset = 0;
            update_filters_signal(std::move(header), std::make_shared<cam::DCFiltersSettings>(m_data.data(), offset, header.total_size_data_bytes()));
        }
    }break;
    case MessageType::update_color_settings:{
        Logger::message("[DCClientUdpReader] update_color_settings message received.\n");
        update_color_settings_signal(std::move(header), std::make_shared<UdpMonoPacketMessage<DCColorSettings>>(packetData));
    }break;
    case MessageType::delay:{
        Logger::message("[DCClientUdpReader] delay message received.\n");
        update_delay_signal(std::move(header), UdpMonoPacketMessage<DCDelaySettings>(packetData));
    }break;
    case MessageType::command:{
        Logger::message("[DCClientUdpReader] command message received.\n");
        command_signal(header, UdpMonoPacketData::generate_data_from_packet_raw<Command>(packetData));
    }break;
    default:
        break;
    }
}

auto DCServerUdpReader::process_packet(std::vector<char> *packet, size_t nbBytes) -> void{

    using namespace std::chrono;

    auto packetData = reinterpret_cast<std::int8_t*>(packet->data());

    Header header(packetData);
    packetData += sizeof(Header);

    switch (static_cast<MessageType>(header.type)) {
    case MessageType::synchro:{
        synchro.update_average_difference(header.currentPacketTimestampNs);
        synchro_signal(synchro.averageDiffNs);

    }break;
    case MessageType::feedback:{
        feedback_signal(std::move(header), UdpMonoPacketMessage<Feedback>(packetData));
    }break;
    case MessageType::compressed_frame_data:{

        if(header.currentPacketId == 0){
            firstPacketReceivedCompressedFrameTS = Time::nanoseconds_since_epoch().count();
            firstPacketSentCompressedFrameTS     = header.currentPacketTimestampNs;
        }

        if(!compressedFrameMessage.copy_packet_to_data(header, nbBytes, packetData, m_data)){
            break;
        }

        if(compressedFrameMessage.all_received(header)){

            // create compressed frame from data
            auto cFrame = std::make_shared<DCCompressedFrame>();

            // init compressed frame from data packets
            size_t offset = 0;
            cFrame->init_from_data(std::span<std::int8_t>(m_data.data(), header.total_size_data_bytes()), offset);

            // update received TS with first packet received TS
            auto diffCaptureSending = (firstPacketSentCompressedFrameTS - cFrame->afterCaptureTS);
            cFrame->receivedTS = firstPacketReceivedCompressedFrameTS - diffCaptureSending;

            // add average diff to capture timestamp
            cFrame->afterCaptureTS += synchro.averageDiffNs;

            // send compressed frame
            compressed_frame_signal(std::move(header), std::move(cFrame));
        }

    }break;  
    default:
        break;
    }
}

Synchro::Synchro(){
    diffNs.resize(nbMaxValues);
    std::fill(std::begin(diffNs), std::end(diffNs), std::chrono::nanoseconds(0));
}

auto Synchro::update_average_difference(int64_t timestampNS) -> void{

    using namespace std::chrono;
    nanoseconds currentTimestampNS = Time::nanoseconds_since_epoch();
    diffNs[currentId++] = currentTimestampNS - nanoseconds(timestampNS);
    if(currentId >= nbMaxValues){
        currentId = 0;
    }

    averageDiffNs = static_cast<std::int64_t>(
        1.0 * std::accumulate(std::begin(diffNs), std::end(diffNs), nanoseconds(0)).count() / nbMaxValues
    );
}
