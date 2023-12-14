
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
#include "dc_server_device.hpp"
// #include "camera/dc_compressed_frame.hpp"
// #include "network/udp_header.hpp"
// #include "thirdparty/sigslot/signal.hpp"

namespace tool::network {


class DCServerLocalDevice : public DCServerDevice{
public:

    DCServerLocalDevice();
    ~DCServerLocalDevice() override;

    auto initialize(size_t id, const ReadSendNetworkInfos &infos) -> bool override;
    auto clean() -> void override;

    auto udpate_device_settings(const camera::DCDeviceSettings &deviceS) -> void override{}
    auto udpate_color_settings(const camera::DCColorSettings &colorS) -> void override{}
    auto udpate_filters_settings(const camera::DCFiltersSettings &filters) -> void override{}
    auto udpate_delay_settings(camera::DCDelaySettings delay) -> void override{}

    constexpr auto type() const noexcept -> DCServerType override {return DCServerType::local;}
private:

    struct Impl;
    std::unique_ptr<Impl> i;
};


}
