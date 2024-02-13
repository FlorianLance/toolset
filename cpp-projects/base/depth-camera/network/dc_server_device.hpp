
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
#include "network/network_types.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"


namespace tool::net {


enum class DCServerType{
    local,remote,undefined
};

class DCServerDevice{
public:
    virtual ~DCServerDevice(){}
    virtual auto initialize(const ReadSendNetworkInfos &infos) -> bool{static_cast<void>(infos);return false;}
    virtual auto clean() -> void{}    

    virtual auto apply_command(Command command) -> void{static_cast<void>(command);}
    virtual auto update_device_settings(const cam::DCDeviceSettings &deviceS) -> void{static_cast<void>(deviceS);}
    virtual auto update_color_settings(const cam::DCColorSettings &colorS) -> void{static_cast<void>(colorS);}
    virtual auto update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void{static_cast<void>(filtersS);}
    virtual auto update_delay_settings(const cam::DCDelaySettings &delayS) -> void{static_cast<void>(delayS);}

    virtual constexpr auto type() const noexcept -> DCServerType {return DCServerType::undefined;}
    virtual auto device_connected() const noexcept -> bool{return false;}
};


}
