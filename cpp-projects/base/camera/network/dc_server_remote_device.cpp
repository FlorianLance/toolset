

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

using namespace tool::network;

struct DCServerRemoteDevice::Impl{

    // size_t id = 0;
    ReadSendNetworkInfos infos;
    bool remoteDeviceConnected = false;

    std::int64_t initTs;
    std::atomic<size_t> totalReceivedBytes = 0;
    DCServerUdpSender udpSender;
    DCServerUdpReader udpReader;

    static constexpr std::uint16_t maxSizeUpdMessage = 9000;
};

DCServerRemoteDevice::DCServerRemoteDevice() : i(std::make_unique<DCServerRemoteDevice::Impl>()){

}

DCServerRemoteDevice::~DCServerRemoteDevice(){
    DCServerRemoteDevice::clean();
}

auto DCServerRemoteDevice::initialize(const ReadSendNetworkInfos &infos) -> bool {

    // i->id    = id;
    i->infos = infos;

    // init reader
    if(!i->udpReader.init_socket(i->infos.readingAdress, i->infos.readingPort, i->infos.protocol)){
        Logger::error("DCServerRemoteDevice: Cannot init udp reader.\n");
        return false;
    }
    i->udpReader.start_reading();

    // init sender
    if(!i->udpSender.init_socket(i->infos.sendingAdress, std::to_string(i->infos.sendingPort), i->infos.protocol)){
        Logger::error("DCServerRemoteDevice: Cannot init udp sender.\n");
        return false;
    }

    // connections
    i->udpReader.synchro_signal.connect([&](std::int64_t timestamp){
        remote_synchro_signal(timestamp);
    });
    i->udpReader.feedback_signal.connect(&DCServerRemoteDevice::receive_feedback, this);
    i->udpReader.compressed_frame_signal.connect(&DCServerRemoteDevice::receive_compressed_frame, this);

    return true;
}

auto DCServerRemoteDevice::init_remote_connection() -> void{

    Logger::message(
        std::format("DCServerRemoteDevice: init remote connection infos: RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}].\n",
        i->infos.idReadingInterface,
        i->infos.readingAdress,
        i->infos.readingPort,
        i->infos.sendingAdress,
        i->infos.sendingPort)
    );

    i->initTs = i->udpSender.send_init_message(
        UdpNetworkSendingSettings(
            i->infos.readingAdress,
            i->infos.readingPort,
            i->maxSizeUpdMessage
        )
    ).currentPacketTime;
}

auto DCServerRemoteDevice::clean() -> void{

    i->udpReader.feedback_signal.disconnect(&DCServerRemoteDevice::receive_feedback, this);
    i->udpReader.compressed_frame_signal.disconnect(&DCServerRemoteDevice::receive_compressed_frame, this);
    i->remoteDeviceConnected = false;

    // manager size
    if(i->udpReader.is_reading()){
        i->udpReader.stop_reading();
    }
    i->udpReader.clean_socket();
    i->udpSender.clean_socket();
}

auto DCServerRemoteDevice::apply_command(Command command) -> void{

    switch(command){
    case Command::Disconnect:
        i->udpSender.send_command_message(Command::Disconnect);
        break;
    case Command::Quit:
        i->udpSender.send_command_message(Command::Quit);
        break;
    case Command::Shutdown:
        i->udpSender.send_command_message(Command::Shutdown);
        break;
    case Command::Restart:
        i->udpSender.send_command_message(Command::Restart);
        break;
    case Command::UpdateDeviceList:
        i->udpSender.send_command_message(Command::UpdateDeviceList);
        break;
    }
}

auto DCServerRemoteDevice::update_device_settings(const camera::DCDeviceSettings &deviceS) -> void{
    i->udpSender.send_update_device_settings_message(deviceS);
}

auto DCServerRemoteDevice::update_color_settings(const camera::DCColorSettings &colorS) -> void{
    i->udpSender.send_update_color_settings_message(colorS);
}

auto DCServerRemoteDevice::update_filters_settings(const camera::DCFiltersSettings &filtersS) -> void{
    i->udpSender.send_update_filters_settings_message(filtersS);
}

auto DCServerRemoteDevice::update_delay_settings(const camera::DCDelaySettings &delayS) -> void{
    i->udpSender.send_delay_settings_message(delayS);
}

auto DCServerRemoteDevice::device_connected() const noexcept -> bool {
    return i->remoteDeviceConnected;
}

auto DCServerRemoteDevice::receive_feedback(Header h, UdpMonoPacketMessage<Feedback> message) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;

    switch(message.data.feedback){
    case FeedbackType::message_received:
        if(message.data.receivedMessageType == MessageType::init_network_infos){
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

    remote_feedback_signal(std::move(message.data));
}

auto DCServerRemoteDevice::receive_compressed_frame(Header h, std::shared_ptr<camera::DCCompressedFrame> compressedFrame) -> void{

    // from reader thread:
    i->totalReceivedBytes += h.totalSizeBytes;
    if(compressedFrame){
        remote_frame_signal(std::move(compressedFrame));
    }
}
