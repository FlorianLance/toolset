
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

#include "dc_device_manager.hpp"

// local
// # utility
#include "utility/format.hpp"
//#include "utility/thread.hpp"
// # camera
#include "camera/kinect4/k4_device.hpp"
#include "camera/orbbec/orbbec_device.hpp"

using namespace tool::camera;
using namespace std::chrono;

struct DCDeviceManager::Impl{
    // device
    std::unique_ptr<DCDevice> device = nullptr;
};

DCDeviceManager::DCDeviceManager(): i(std::make_unique<Impl>()){
}

DCDeviceManager::~DCDeviceManager(){
    clean();
}

auto DCDeviceManager::initialize(DCType type) -> void {

    if(type == DCType::Kinect4){
        i->device = std::make_unique<K4Device>();
    }else if(type == DCType::Orbbec){
        i->device = std::make_unique<OrbbecDevice>();
    }

    init_connections();

    for(size_t ii = 0; ii < i->device->nb_devices(); ++ii){
        auto t = std::format("Cam {}", ii);
        update_device_name_signal(static_cast<int>(ii), t);
    }
}

auto DCDeviceManager::clean() -> void {
    i->device->clean();
}

auto DCDeviceManager::update_delay(DCDelaySettings delayMs) -> void{
    i->device->set_delay(delayMs);
}

auto DCDeviceManager::update_filters(const DCFilters &filters) -> void {
    i->device->set_filters(filters);
}

auto DCDeviceManager::update_color_settings(const DCColorSettings &colorS) -> void {
    i->device->set_color_settings(colorS);
}

auto DCDeviceManager::get_capture_duration_ms() -> int64_t{
    return i->device->get_capture_duration_ms();
}
auto DCDeviceManager::get_nb_capture_per_second() -> float{
    return i->device->get_nb_capture_per_second();
}

auto DCDeviceManager::get_processing_duration_ms() -> int64_t{
    return i->device->get_processing_duration_ms();
}

auto DCDeviceManager::get_compressing_duration_ms() -> int64_t{
    return i->device->get_compressing_duration_ms();
}

auto DCDeviceManager::get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> int64_t{
    return i->device->get_duration_between_ms(from, to);
}

auto DCDeviceManager::get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> int64_t{
    return i->device->get_duration_between_micro_s(from, to);
}

auto DCDeviceManager::update_device_list() -> void{

    i->device->refresh_devices_list();

    if(i->device->nb_devices() > 0){
        for(size_t ii = 0; ii < i->device->nb_devices(); ++ii){
            update_device_name_signal(static_cast<int>(ii), std::format("Id:{} Num:...", ii));
        }
    }else{
        update_device_name_signal(-1, "");
    }
}

auto DCDeviceManager::update_settings(const DCDeviceSettings &settings) -> void{

    i->device->set_data_settings(settings.dataS);

    bool stopDevice =
        ((i->device->is_opened() ? (i->device->device_id() != settings.configS.idDevice) : false) || !settings.actionsS.startDevice);

    bool stopCamera =
        stopDevice ||
        !settings.actionsS.openCamera ||
        (settings.configS.mode != i->device->mode()) ||
        (settings.configS.disableLED != i->device->is_LED_disabled()) ||
        (settings.configS.synchronizeColorAndDepth != i->device->color_and_depth_synchronized()) ||
        (settings.configS.delayBetweenColorAndDepthUsec != i->device->delay_between_color_and_depth_usec()) ||
        (settings.configS.subordinateDelayUsec != i->device->subordinate_delay_usec()) ||
        (settings.configS.synchMode != i->device->synch_mode());

    // stop / close
    if(i->device->is_opened()){
        if(stopCamera){
            i->device->stop_cameras();
        }
        if(stopDevice){
            i->device->close();
        }
    }

    if(stopDevice || stopCamera){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // open device
    if(settings.actionsS.startDevice){
        if(i->device->open(settings.configS.idDevice)){
            update_device_name_signal(settings.configS.idDevice, fmt("Id:{} Num:{}",  settings.configS.idDevice, i->device->device_name()));
        }
    }

    // start camera
    if(settings.actionsS.openCamera){
        i->device->start_cameras(settings.configS);
    }
}

auto DCDeviceManager::init_connections() -> void {

    i->device->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
        new_frame_signal(std::move(frame));
    });

    i->device->new_compressed_frame_signal.connect([&](std::shared_ptr<DCCompressedFrame> frame){
        new_compressed_frame_signal(std::move(frame));
    });

    i->device->new_imu_sample_signal.connect([&](DCImuSample sample){
        new_imu_sample_signal(sample);
    });
}

