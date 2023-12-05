

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
#include "camera/kinect4/k4_compressed_frame.hpp"

using namespace tool::camera;
using namespace tool::network;

auto DCClientUdpReader::process_packet(std::vector<char> *packet, size_t nbBytes) -> void{

    auto packetData = reinterpret_cast<std::int8_t*>(packet->data());

    Header header(packetData);
    packetData += sizeof(Header);

//    Logger::message(std::format(
//        "[pp {} {} {} {} {} {} {}] ",
//        nbBytes, header.idMessage, header.currentPacketSizeBytes, header.currentPacketId,
//        header.totalSizeBytes, header.dataOffset, header.idMessage));
    switch (static_cast<MessageType>(header.type)) {
    case MessageType::init_network_infos:{
        init_network_infos_signal(header, UdpMonoPacketData::generate_data_from_packet<UdpNetworkSendingSettings>(packetData));
    }break;
    case MessageType::update_device_settings:{
        update_device_settings_signal(std::move(header), std::make_shared<UdpMonoPacketMessage<DCDeviceSettings>>(packetData));
    }break;
    case MessageType::update_filters:{
        if(!filtersMessage.copy_packet_to_data(header, nbBytes, packetData, m_data)){
            break;
        }
        if(filtersMessage.all_received(header)){
            update_filters_signal(std::move(header), std::make_shared<camera::DCFiltersSettings>(m_data.data()));
        }
    }break;
    case MessageType::update_color_settings:{        
        update_color_settings_signal(std::move(header), std::make_shared<UdpMonoPacketMessage<DCColorSettings>>(packetData));
    }break;
    case MessageType::delay:{
        update_delay_signal(std::move(header), UdpMonoPacketMessage<DCDelaySettings>(packetData));
    }break;
    case MessageType::command:{
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

        nanoseconds currentTS = Time::nanoseconds_since_epoch();
        nanoseconds diffNs    = currentTS - nanoseconds(header.currentPacketTime);

        synchro.diffNs[synchro.currentId++] = diffNs;
        if(synchro.currentId >= synchro.nbMaxValues){
            synchro.currentId = 0;
        }

        auto totalNs = std::accumulate(std::begin(synchro.diffNs), std::end(synchro.diffNs), nanoseconds(0));
        synchro.averageDiffNs = static_cast<std::int64_t>(1.0 * totalNs.count() / synchro.nbMaxValues);

        synchro_signal(synchro.averageDiffNs);

    }break;
    case MessageType::feedback:{
        feedback_signal(std::move(header), UdpMonoPacketMessage<Feedback>(packetData));
    }break;
    case MessageType::compressed_frame_data:{       

        if(!compressedFrameMessage.copy_packet_to_data(header, nbBytes, packetData, m_data)){
            break;
        }

        if(compressedFrameMessage.all_received(header)){

            // create compressed frame from data
            auto cFrame = std::make_shared<K4CompressedFrame>();
            cFrame->init_from_data(m_data.data());

            // TEST
            cFrame->afterCaptureTS = compressedFrameMessage.firstPacketTimestamp - cFrame->afterCaptureTS;

            // send compressed frame
            compressed_frame_signal(std::move(header), cFrame);
        }

    }break;  
    default:
        break;
    }
}
