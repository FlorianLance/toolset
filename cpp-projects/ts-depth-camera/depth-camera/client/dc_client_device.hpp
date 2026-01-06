
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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

// sigslot
#include <sigslot/signal.hpp>

// base
#include "utility/monitoring.hpp"
#include "depth-camera/network/dc_network_types.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_misc_settings.hpp"

// local
#include "depth-camera/settings/dc_device_connection_settings.hpp"

namespace tool::cam {

class DCClientDevice{
public:
    virtual ~DCClientDevice(){}
    virtual auto initialize(const DCDeviceConnectionSettings &connectionS) -> bool{static_cast<void>(connectionS);return false;}
    virtual auto clean() -> void{}

    virtual auto apply_command(net::Command command) -> void{static_cast<void>(command);}
    virtual auto update_device_settings(const cam::DCDeviceSettings &deviceS) -> void{static_cast<void>(deviceS);}
    virtual auto update_color_settings(const cam::DCColorSettings &colorS) -> void{static_cast<void>(colorS);}
    virtual auto update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void{static_cast<void>(filtersS);}
    virtual auto update_misc_settings(const cam::DCMiscSettings &miscS) -> void{static_cast<void>(miscS);}

    virtual constexpr auto type() const noexcept -> DCClientType {return DCClientType::Undefined;}
    virtual auto device_connected() const noexcept -> bool{return false;}

    auto set_id_client(size_t idClient) noexcept -> void{
        this->idClient = idClient;
    }

    sigslot::signal<net::UdpDataStatus> data_status_signal;

    FramerateBuffer framerate;
    AverageLatencyBuffer latency;
    size_t idClient = 0;
};


}
