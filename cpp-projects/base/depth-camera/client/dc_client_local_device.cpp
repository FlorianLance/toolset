

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

#include "dc_client_local_device.hpp"

// local
#include "depth-camera/dc_device_manager.hpp"
#include "utility/time.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;
using namespace tool::net;

struct DCClientLocalDevice::Impl{
    std::unique_ptr<DCDeviceManager>  deviceM = nullptr;
};

DCClientLocalDevice::DCClientLocalDevice() : i(std::make_unique<DCClientLocalDevice::Impl>()){
}

DCClientLocalDevice::~DCClientLocalDevice(){
    DCClientLocalDevice::clean();
}

auto DCClientLocalDevice::initialize(DCDeviceConnectionSettings *deviceS) -> bool{
    
    if(auto devS = dynamic_cast<DCLocalDeviceConnectionSettings*>(deviceS)){

        i->deviceM = std::make_unique<DCDeviceManager>();
        i->deviceM->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){

            if(frame){

                // update framerate
                framerate.add_frame();

                // update latency
                latency.update_average_latency(Time::difference_micro_s(std::chrono::nanoseconds(frame->afterCaptureTS), Time::nanoseconds_since_epoch()).count());

                // send frame
                local_frame_signal(std::move(frame));

                // send status
                data_status_signal(UdpDataStatus{framerate.get_framerate(), latency.averageLatency});
            }
        });

        return true;

    }else{
        Logger::error("[DCClientLocalDevice::initialize] Invalid settings file.");
        return false;
    }
}

auto DCClientLocalDevice::clean() -> void {
    i->deviceM = nullptr;
}

auto DCClientLocalDevice::apply_command(Command command) -> void{
    static_cast<void>(command);
}

auto DCClientLocalDevice::update_device_settings(const cam::DCDeviceSettings &deviceS) -> void{
    if(i->deviceM){
        i->deviceM->update_device_settings(deviceS);
    }
}

auto DCClientLocalDevice::update_color_settings(const cam::DCColorSettings &colorS) -> void{
    if(i->deviceM){
        i->deviceM->update_color_settings(colorS);
    }
}

auto DCClientLocalDevice::update_filters_settings(const cam::DCFiltersSettings &filtersS) -> void{
    if(i->deviceM){
        i->deviceM->update_filters_settings(filtersS);
    }
}

auto DCClientLocalDevice::update_delay_settings(const cam::DCDelaySettings &delayS) -> void{
    if(i->deviceM){
        i->deviceM->update_delay_settings(delayS);
    }
}

auto DCClientLocalDevice::device_connected() const noexcept -> bool{
    if(i->deviceM){
        return i->deviceM->is_opened();
    }
    return false;
}