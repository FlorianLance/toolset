


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

#include "dc_udp_sender.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"


using namespace tool::net;

auto DCServerUdpSender::send_init_message(const UdpNetworkSendingSettings &network) -> Header{

    if(!is_opened()){
        Logger::error("DCServerUdpSender::send_init_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::init_network_infos, sizeof(UdpNetworkSendingSettings));

    // init data
    UdpMonoPacketData::copy_to_data<net::UdpNetworkSendingSettings>(header, &network, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto DCServerUdpSender::send_update_device_settings_message(const cam::DCDeviceSettings &device) -> Header{

    if(!is_opened()){
        Logger::error("DCServerUdpSender::send_update_device_settings_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::update_device_settings, sizeof(cam::DCDeviceSettings));

    // compute checksum
    // header.c
    // auto data = reinterpret_cast<std::uint16_t>(&device);

    // init data
    UdpMonoPacketData::copy_to_data(header, &device, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto DCServerUdpSender::send_update_color_settings_message(const cam::DCColorSettings &color) -> Header{

    if(!is_opened()){
        Logger::error("DCServerUdpSender::send_update_color_settings_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::update_color_settings, sizeof(cam::DCColorSettings));

    // init data
    UdpMonoPacketData::copy_to_data<cam::DCColorSettings>(header, &color, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto DCServerUdpSender::send_delay_settings_message(cam::DCDelaySettings delay) -> Header{

    if(!is_opened()){
        Logger::error("DCServerUdpSender::send_delay_settings_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::delay, sizeof(cam::DCDelaySettings));

    // init data
    UdpMonoPacketData::copy_to_data<cam::DCDelaySettings>(header, &delay, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto DCServerUdpSender::send_command_message(net::Command command) -> Header{

    if(!is_opened()){
        Logger::error("DCServerUdpSender::send_command_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::command, sizeof(Command));

    // init data
    UdpMonoPacketData::copy_to_data<net::Command>(header, &command, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto DCServerUdpSender::send_update_filters_settings_message(const cam::DCFiltersSettings &filters) -> bool{

    if(!is_opened()){
        Logger::error("DCServerUdpSender::send_update_filters_settings_message: sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header;
    header.type      = MessageType::update_filters;
    header.idMessage = static_cast<std::int32_t>(idLastMasksFiltersMessageSent);

    // init data
    auto totalDataSizeBytes = filters.total_data_size();
    if(bufferToSend.size() < totalDataSizeBytes){
        bufferToSend.resize(totalDataSizeBytes);
    }

    size_t offset = 0;
    filters.write_to_data(bufferToSend.data(), offset, totalDataSizeBytes);

    // send data
    size_t nbBytesSent = send_packets(header, totalDataSizeBytes);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("K4SMUdpSender::send_update_filters_settings_message: Invalid nb of bytes send, {} instead of {}.\n",
                nbBytesSent, header.totalSizeBytes));
        return false;
    }

    ++idLastMasksFiltersMessageSent;
    return true;
}

auto DCClientUdpSender::send_synchronisation_message() -> bool{

    if(!is_opened()){ Logger::error("DCClientUdpSender::send_synchronisation_message: sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::synchro, 0);

    // init data
    UdpMonoPacketData::copy_only_header_to_data(header, packetBuffer);

    // send data
    return send_mono_packet(header) != 0;
}

auto DCClientUdpSender::send_feedback_message(Feedback feedback) -> bool{

    if(!is_opened()){
        Logger::error("DCClientUdpSender::send_feedback_message: sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header = Header::generate_mono_packet(MessageType::feedback, sizeof(Feedback));

    // init data
    UdpMonoPacketData::copy_to_data<Feedback>(header, &feedback, packetBuffer);

    // send data
    return send_mono_packet(header) != 0;
}

auto DCClientUdpSender::send_compressed_frame_message(std::shared_ptr<cam::DCCompressedFrame> frame) -> bool{

    if(!is_opened()){
        return false;
    }    

    // init header
    Header header;
    header.type      = MessageType::compressed_frame_data;
    header.idMessage = static_cast<std::int32_t>(idLastFrameMutliPacketsMessageSent);

    // init data
    size_t totalDataSizeBytes = frame->data_size();
    if(bufferToSend.size() < totalDataSizeBytes){
        bufferToSend.resize(totalDataSizeBytes);
    }

    size_t offset = 0;
    frame->write_to_data(std::span<std::int8_t>(bufferToSend.data(), totalDataSizeBytes), offset);

    // send data
    size_t nbBytesSent = send_packets(header, totalDataSizeBytes);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("DCClientUdpSender::send_compressed_frame_message: Invalid nb of bytes send, {} instead of {}.\n",
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    ++idLastFrameMutliPacketsMessageSent;
    return true;

}

