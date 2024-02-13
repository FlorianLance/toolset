

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

#pragma once

// local
#include "network/udp_sender.hpp"
#include "network/network_types.hpp"
#include "depth-camera/dc_compressed_frame.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"

namespace tool::net {

class DCClientUdpSender : public UdpSender{
public:

    auto send_synchronisation_message() -> bool;
    auto send_feedback_message(Feedback feedback) -> bool;
    auto send_compressed_frame_message(std::shared_ptr<cam::DCCompressedFrame> frame) -> bool;

private:
    size_t idLastFrameMutliPacketsMessageSent = 0;
};

class DCServerUdpSender : public UdpSender{
public:

    auto send_init_message(const UdpNetworkSendingSettings &network) -> Header;
    auto send_update_device_settings_message(const cam::DCDeviceSettings &device) -> Header;
    auto send_update_color_settings_message(const cam::DCColorSettings &color) -> Header;
    auto send_delay_settings_message(cam::DCDelaySettings delay) -> Header;
    auto send_command_message(net::Command command) -> Header;
    auto send_update_filters_settings_message(const cam::DCFiltersSettings &filters) -> bool;

private:
    size_t idLastMasksFiltersMessageSent = 0;
};
}
