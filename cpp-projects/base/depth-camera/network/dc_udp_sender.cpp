


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
#include "utility/time.hpp"

using namespace tool::net;

auto DCServerUdpSender::send_init_binary_message(const UdpNetworkSendingSettings &network) -> bool{

    if(!is_opened()){
        Logger::error("[DCServerUdpSender::send_init_binary_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_header(MessageType::init_network_infos);

    // convert to data
    auto bData = std::span(reinterpret_cast<const std::byte*>(&network), sizeof(UdpNetworkSendingSettings));

    // send data
    size_t nbBytesSent = send_data(header, bData);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_feedback_binary_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}


auto DCServerUdpSender::send_delay_settings_binary_message(cam::DCDelaySettings delay) -> bool{

    if(!is_opened()){
        Logger::error("[DCServerUdpSender::send_delay_settings_binary_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_header(MessageType::delay);

    // convert to data
    auto bData = std::span(reinterpret_cast<const std::byte*>(&delay), sizeof(cam::DCDelaySettings));

    // send data
    size_t nbBytesSent = send_data(header, bData);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_feedback_binary_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}

auto DCClientUdpSender::send_synchronisation_binary_message() -> bool{

    if(!is_opened()){
        Logger::error("[DCClientUdpSender::send_synchronisation_binary_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_dataless_header(MessageType::synchro);

    // copy data
    if(packetBuffer.size() < header.totalSizeBytes){
        packetBuffer.resize(header.totalSizeBytes);
    }
    auto headerD = reinterpret_cast<const std::byte*>(&header);
    std::copy(headerD, headerD + sizeof(Header), packetBuffer.begin());

    // send data
    return send_mono_packet(header) != 0;
}

auto DCClientUdpSender::send_feedback_binary_message(Feedback feedback) -> bool{

    if(!is_opened()){
        Logger::error("[DCClientUdpSender::send_feedback_binary_message] Sender not opened, message canceled.\n");
        return false;
    }

    // init header
    Header header = generate_header(MessageType::feedback);

    // convert to data
    auto bData = std::span(reinterpret_cast<const std::byte*>(&feedback), sizeof(Feedback));

    // send data
    size_t nbBytesSent = send_data(header, bData);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_feedback_binary_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}

auto DCServerUdpSender::send_command_binary_message(net::Command command) -> bool{

    if(!is_opened()){
        Logger::error("[DCServerUdpSender::send_command_binary_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_header(MessageType::command);

    // convert to data
    auto bData = std::span(reinterpret_cast<const std::byte*>(&command), sizeof(Command));

    // send data
    size_t nbBytesSent = send_data(header, bData);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_command_binary_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}


auto DCServerUdpSender::send_update_device_settings_bson_message(const cam::DCDeviceSettings &device) -> bool{

    if(!is_opened()){
        Logger::error("[DCServerUdpSender::send_update_device_settings_bson_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_header(MessageType::update_device_settings);

    // convert to data
    auto bData = device.convert_to_json_binary();

    // send data
    size_t nbBytesSent = send_data(header, std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_update_device_settings_bson_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}


auto DCServerUdpSender::send_update_filters_settings_bson_message(const cam::DCFiltersSettings &filters) -> bool{

    if(!is_opened()){
        Logger::error("[DCServerUdpSender::send_update_filters_settings_bson_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_header(MessageType::update_filters);

    // convert to data
    auto bData = filters.convert_to_json_binary();

    // send data
    size_t nbBytesSent = send_data(header, std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_update_filters_settings_bson_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}

auto DCServerUdpSender::send_update_color_settings_bson_message(const cam::DCColorSettings &color) -> bool{

    if(!is_opened()){
        Logger::error("[DCServerUdpSender::send_update_color_settings_bson_message] Sender not opened, message canceled.\n"sv);
        return false;
    }

    // init header
    Header header = generate_header(MessageType::update_color_settings);

    // convert to data
    auto bData = color.convert_to_json_binary();

    // send data
    size_t nbBytesSent = send_data(header, std::span(reinterpret_cast<const std::byte*>(bData.data()), bData.size()));
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCServerUdpSender::send_update_color_settings_bson_message] Invalid nb of bytes send, {} instead of {}.\n"sv,
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    return true;
}


auto DCClientUdpSender::send_compressed_frame_binary_message(std::shared_ptr<cam::DCCompressedFrame> frame) -> bool{

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
    auto bData = std::span(bufferToSend.data(), totalDataSizeBytes);
    frame->write_to_data(bData, offset);

    // send data
    size_t nbBytesSent = send_data(header, bData);
    if(nbBytesSent != header.totalSizeBytes){
        Logger::error(std::format("[DCClientUdpSender::send_compressed_frame_binary_message] Invalid nb of bytes send, {} instead of {}.\n",
            nbBytesSent, header.totalSizeBytes));
        return false;
    }

    ++idLastFrameMutliPacketsMessageSent;
    return true;

}

