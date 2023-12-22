

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

// local
#include "camera/dc_device_manager.hpp"

using namespace tool::camera;
using namespace tool::network;

struct DCServerLocalDevice::Impl{
    std::unique_ptr<DCDeviceManager>  deviceM = nullptr;
};

DCServerLocalDevice::DCServerLocalDevice() : i(std::make_unique<DCServerLocalDevice::Impl>()){
}

DCServerLocalDevice::~DCServerLocalDevice(){
    clean();
}

auto DCServerLocalDevice::initialize(const ReadSendNetworkInfos &infos) -> bool{
    i->deviceM = std::make_unique<DCDeviceManager>();
    i->deviceM->new_frame_signal.connect(&DCServerLocalDevice::local_frame_signal, this);
    return true;
}

auto DCServerLocalDevice::clean() -> void {
    i->deviceM = nullptr;
}

auto DCServerLocalDevice::apply_command(Command command) -> void{
    static_cast<void>(command);
    // switch(command){
    // case Command::Disconnect:
    //     break;
    // case Command::Quit:
    //     break;
    // case Command::Shutdown:
    //     break;
    // case Command::Restart:
    //     break;
    // case Command::UpdateDeviceList:
    //     break;
    // }
}

auto DCServerLocalDevice::update_device_settings(const camera::DCDeviceSettings &deviceS) -> void{
    if(i->deviceM){
        i->deviceM->update_device_settings(deviceS);
    }
}

auto DCServerLocalDevice::update_color_settings(const camera::DCColorSettings &colorS) -> void{
    if(i->deviceM){
        i->deviceM->update_color_settings(colorS);
    }
}

auto DCServerLocalDevice::update_filters_settings(const camera::DCFiltersSettings &filtersS) -> void{
    if(i->deviceM){
        i->deviceM->update_filters_settings(filtersS);
    }
}

auto DCServerLocalDevice::update_delay_settings(const camera::DCDelaySettings &delayS) -> void{
    if(i->deviceM){
        i->deviceM->update_delay_settings(delayS);
    }
}

auto DCServerLocalDevice::device_connected() const noexcept -> bool{
    if(i->deviceM){
        return i->deviceM->is_opened();
    }
    return false;
}
