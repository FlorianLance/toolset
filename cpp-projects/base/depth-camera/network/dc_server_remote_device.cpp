

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

#include "dc_server_remote_device.hpp"

// std
#include <format>

// local
#include "dc_udp_sender.hpp"
#include "dc_udp_reader.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"

using namespace tool::net;

struct DCServerRemoteDevice::Impl{

    // size_t id = 0;
    ReadSendNetworkInfos infos;
    bool remoteDeviceConnected = false;

    std::atomic<size_t> totalReceivedBytes = 0;
    DCServerUdpSender dcUdpSender;
    DCServerUdpReader dcUdpReader;

    std::int64_t averageTimestampDiffNs = 0;
    static constexpr std::uint16_t maxSizeUpdMessage = 9000;
};

DCServerRemoteDevice::DCServerRemoteDevice() : i(std::make_unique<DCServerRemoteDevice::Impl>()){

}

DCServerRemoteDevice::~DCServerRemoteDevice(){
    DCServerRemoteDevice::clean();
}

auto DCServerRemoteDevice::initialize(const ReadSendNetworkInfos &infos) -> bool {

    i->infos = infos;    

    // init reader
    Logger::message(std::format("DCServerRemoteDevice init reader: {} {} {}\n", i->infos.readingAdress, std::to_string(i->infos.readingPort), static_cast<int>(i->infos.protocol)));
    if(!i->dcUdpReader.init_connection(i->infos.readingAdress, i->infos.readingPort, i->infos.protocol)){
        Logger::error("[DCServerRemoteDevice]: Cannot init udp reader.\n");
        return false;
    }

    if(i->infos.startReadingThread){
        i->dcUdpReader.start_reading_thread();
    }

    // init sender
    Logger::message(std::format("DCServerRemoteDevice init sender: {} {} {}\n", i->infos.sendingAdress, std::to_string(i->infos.sendingPort), static_cast<int>(i->infos.protocol)));
    if(!i->dcUdpSender.init_socket(i->infos.sendingAdress, std::to_string(i->infos.sendingPort), i->infos.protocol)){
        Logger::error("[DCServerRemoteDevice]: Cannot init udp sender.\n");
        return false;
    }

    // connections
    i->dcUdpReader.synchro_signal.connect([&](std::int64_t averageTimestampDiffNs){
        remote_synchro_signal(i->averageTimestampDiffNs = averageTimestampDiffNs);
    });
    i->dcUdpReader.feedback_signal.connect(&DCServerRemoteDevice::receive_feedback, this);
    i->dcUdpReader.compressed_frame_signal.connect(&DCServerRemoteDevice::receive_compressed_frame, this);
    i->dcUdpReader.status_signal.connect(&DCServerRemoteDevice::receive_status, this);

    return true;
}

auto DCServerRemoteDevice::init_remote_connection() -> void{

    Logger::message(
        std::format("DCServerRemoteDevice: init remote connection infos: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}] PRO:[{}].\n",
        i->infos.idReadingInterface,
        i->infos.readingAdress,
        i->infos.readingPort,
        i->infos.sendingAdress,
        i->infos.sendingPort,
        static_cast<int>(i->infos.protocol))
    );

    i->dcUdpSender.send_init_binary_message(
        UdpNetworkSendingSettings(
            i->infos.readingAdress,
            i->infos.readingPort,
            i->maxSizeUpdMessage
        )
    );
}

auto DCServerRemoteDevice::clean() -> void{

    // clean sender
    // # disconnect client
    if(i->dcUdpSender.is_opened()){
        i->dcUdpSender.send_command_binary_message(Command::Disconnect);
    }
    // # clean socket
    i->dcUdpSender.clean_socket();

    // clean reader
    // # remove connections
    i->dcUdpReader.feedback_signal.disconnect(&DCServerRemoteDevice::receive_feedback, this);
    i->dcUdpReader.compressed_frame_signal.disconnect(&DCServerRemoteDevice::receive_compressed_frame, this);    
    i->dcUdpReader.status_signal.disconnect(&DCServerRemoteDevice::receive_status, this);

    i->remoteDeviceConnected = false;
    // # stop reading loop
    if(i->dcUdpReader.is_reading_thread_started()){
        i->dcUdpReader.stop_reading_thread();
    }
    // # clean socket
    i->dcUdpReader.clean_connection();
}

auto DCServerRemoteDevice::apply_command(Command command) -> void{

    switch(command){
    case Command::Disconnect:
        i->dcUdpSender.send_command_binary_message(Command::Disconnect);
        break;
    case Command::Quit:
        i->dcUdpSender.send_command_binary_message(Command::Quit);
        break;
    case Command::Shutdown:
        i->dcUdpSender.send_command_binary_message(Command::Shutdown);
        break;
    case Command::Restart:
        i->dcUdpSender.send_command_binary_message(Command::Restart);
        break;
    }
}

auto DCServerRemoteDevice::update_device_settings(const cam::DCDeviceSettings &deviceS) -> void{
    i->dcUdpSender.send_update_device_settings_bson_message(deviceS);
}

auto DCServerRemoteDevice::update_color_settings(const cam::DCColorSettings &colorS) -> void{
    i->dcUdpSender.send_update_color_settings_bson_message(colorS);
}

auto DCServerRemoteDevice::update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void{
    i->dcUdpSender.send_update_filters_settings_bson_message(filtersS);
}

auto DCServerRemoteDevice::update_delay_settings(const cam::DCDelaySettings &delayS) -> void{
    i->dcUdpSender.send_delay_settings_binary_message(delayS);
}

auto DCServerRemoteDevice::device_connected() const noexcept -> bool {
    return i->remoteDeviceConnected;
}

auto DCServerRemoteDevice::read_data_from_network() -> size_t{
    return i->dcUdpReader.read_packet();
}

auto DCServerRemoteDevice::receive_feedback(Header h, Feedback message) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;

    switch(message.feedback){
    case FeedbackType::message_received:
        if(message.receivedMessageType == MessageType::init_network_infos){
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

auto DCServerRemoteDevice::receive_compressed_frame(Header h, std::shared_ptr<cam::DCCompressedFrame> compressedFrame) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;
    if(compressedFrame){
        remote_frame_signal(std::move(compressedFrame));
    }
}

auto DCServerRemoteDevice::receive_status(UdpReceivedStatus status) -> void{
    remote_status_signal(status);
}
