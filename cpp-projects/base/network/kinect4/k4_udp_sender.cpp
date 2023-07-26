


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

#include "k4_udp_sender.hpp"

// local
#include "utility/logger.hpp"
#include "utility/time.hpp"


using namespace tool::network;

auto K4ServerUdpSender::send_init_message(const K4NetworkSendingSettings &network) -> Header{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_init_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::init_network_infos), sizeof(K4NetworkSendingSettings));

    // init data
    UdpMonoPacketData::copy_to_data<network::K4NetworkSendingSettings>(header, &network, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto K4ServerUdpSender::send_update_device_settings_message(const camera::K4DeviceSettings &device) -> Header{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_update_device_settings_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::update_device_settings), sizeof(camera::K4DeviceSettings));

    // init data
    UdpMonoPacketData::copy_to_data<camera::K4DeviceSettings>(header, &device, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto K4ServerUdpSender::send_update_color_settings_message(const camera::K4ColorSettings &color) -> Header{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_update_color_settings_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::update_color_settings), sizeof(camera::K4ColorSettings));

    // init data
    UdpMonoPacketData::copy_to_data<camera::K4ColorSettings>(header, &color, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto K4ServerUdpSender::send_delay_settings_message(camera::K4Delay delay) -> Header{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_delay_settings_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::delay), sizeof(camera::K4Delay));

    // init data
    UdpMonoPacketData::copy_to_data<camera::K4Delay>(header, &delay, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto K4ServerUdpSender::send_command_message(network::K4Command command) -> Header{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_command_message: sender not opened, message canceled.\n");
        return {};
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::command), sizeof(K4Command));

    // init data
    UdpMonoPacketData::copy_to_data<network::K4Command>(header, &command, packetBuffer);

    // send data
    send_mono_packet(header);

    return header;
}

auto K4ServerUdpSender::send_update_filters_settings_message(const camera::K4Filters &filters) -> bool{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_update_filters_settings_message: sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header;
    header.type      = to_i8(MessageType::update_filters);
    header.idMessage = static_cast<std::int32_t>(idLastMasksFiltersMessageSent);

    // init data
    auto sizeMessage = filters.total_data_size();
    if(bufferToSend.size() < sizeMessage){
        bufferToSend.resize(sizeMessage);
    }
    filters.convert_to_data(bufferToSend.data());

    // send data
    size_t nbBytesSent = send_packets(header, sizeMessage);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("K4SMUdpSender::send_update_filters_settings_message: Invalid nb of bytes send, {} instead of {}.\n",
                nbBytesSent, header.totalSizeBytes));
        return false;
    }

    ++idLastMasksFiltersMessageSent;
    return true;
}

auto K4ClientUdpSender::send_synchronisation_message() -> bool{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_synchronisation_message: sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::synchro), 0);

    // init data
    UdpMonoPacketData::copy_only_header_to_data(header, packetBuffer);

    // send data
    return send_mono_packet(header) != 0;
}

auto K4ClientUdpSender::send_feedback_message(K4Feedback feedback) -> bool{

    if(!is_opened()){
        Logger::error("K4SMUdpSender::send_feedback_message: sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header = Header::generate_mono_packet(to_i8(MessageType::feedback), sizeof(K4UdpFeedback));

    // init data
    UdpMonoPacketData::copy_to_data<K4Feedback>(header, &feedback, packetBuffer);

    // send data
    return send_mono_packet(header) != 0;
}

auto K4ClientUdpSender::send_compressed_frame_message(std::shared_ptr<camera::K4CompressedFrame> frame) -> bool{

    if(!is_opened()){
        return false;
    }    

    // init header
    Header header;
    header.type      = to_i8(MessageType::compressed_frame_data);
    header.idMessage = static_cast<std::int32_t>(idLastFrameMutliPacketsMessageSent);

    // init data
    size_t totalDataSizeBytes = frame->convert_to_data(bufferToSend);

    // send data
    size_t nbBytesSent = send_packets(header, totalDataSizeBytes);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[K4ClientUdpSender::send_compressed_frame_message] Invalid nb of bytes send, {} instead of {}.\n",
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    ++idLastFrameMutliPacketsMessageSent;
    return true;

}

