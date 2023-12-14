

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

#include "dc_server_local_device.hpp"

// std
#include <format>

// local
#include "camera/dc_device_manager.hpp"
// #include "dc_udp_sender.hpp"
// #include "dc_udp_reader.hpp"
// #include "utility/logger.hpp"

using namespace tool::network;

struct DCServerLocalDevice::Impl{

    // size_t id = 0;
    // ReadSendNetworkInfos infos;
    // bool remoteDeviceConnected = false;

    // std::int64_t initTs;
    // std::atomic<size_t> totalReceivedBytes = 0;
    // DCServerUdpSender udpSender;
    // DCServerUdpReader udpReader;

    // static constexpr std::uint16_t maxSizeUpdMessage = 9000;
};

DCServerLocalDevice::DCServerLocalDevice() : i(std::make_unique<DCServerLocalDevice::Impl>()){

}

DCServerLocalDevice::~DCServerLocalDevice(){

}

auto DCServerLocalDevice::initialize(size_t id, const ReadSendNetworkInfos &infos) -> bool{
    return true;
}

auto DCServerLocalDevice::clean() -> void {

}
