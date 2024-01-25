
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

// std
#include <format>

// local
#include "dc_device.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;
using namespace std::chrono;

struct DCDeviceManager::Impl{

    // device
    std::unique_ptr<DCDevice> device = nullptr;

    // settings
    DCDeviceSettings deviceS;
    DCFiltersSettings filters;
    DCColorSettings colorsS;
    DCDelaySettings delayS;
};

DCDeviceManager::DCDeviceManager(): i(std::make_unique<Impl>()){
}

DCDeviceManager::~DCDeviceManager(){
    clean();
}

auto DCDeviceManager::clean() -> void{

    if(is_device_initialized()){
        i->device->clean();
        i->device = nullptr;
    }
}

auto DCDeviceManager::initialize_device(DCType typeDevice) -> void {

    // init device
    i->device = std::make_unique<DCDevice>(typeDevice);

    // set connections
    i->device->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
        new_frame_signal(std::move(frame));
    });
    i->device->new_compressed_frame_signal.connect([&](std::shared_ptr<DCCompressedFrame> frame){        
        new_compressed_frame_signal(std::move(frame));
    });
    i->device->new_imu_sample_signal.connect([&](DCImuSample sample){
        new_imu_sample_signal(sample);
    });

    for(size_t ii = 0; ii < i->device->nb_devices(); ++ii){
        update_device_name_signal(static_cast<int>(ii), std::format("Cam {}", ii));
    }
}

auto DCDeviceManager::update_delay_settings(const DCDelaySettings &delayS) -> void{
    i->delayS = delayS;
    if(is_device_initialized()){
        i->device->set_delay_settings(i->delayS);
    }
}

auto DCDeviceManager::is_device_initialized() const noexcept -> bool{
    return i->device!= nullptr;
}

auto DCDeviceManager::is_opened() const noexcept -> bool{
    if(is_device_initialized()){
        return i->device->is_opened();
    }
    return false;
}

auto DCDeviceManager::is_reading() const noexcept -> bool{
    if(is_device_initialized()){
        return i->device->is_reading();
    }
    return false;
}

auto DCDeviceManager::update_filters_settings(const DCFiltersSettings &filters) -> void {
    i->filters = filters;
    if(is_device_initialized()){
        i->device->set_filters_settings(i->filters);
    }
}

auto DCDeviceManager::update_color_settings(const DCColorSettings &colorS) -> void {
    i->colorsS = colorS;
    if(is_device_initialized()){
        i->device->set_color_settings(i->colorsS);
    }
}

auto DCDeviceManager::get_capture_duration_ms() -> int64_t{
    if(is_device_initialized()){
        return i->device->get_capture_duration_ms();
    }
    return 0;
}

auto DCDeviceManager::get_nb_capture_per_second() -> float{
    if(is_device_initialized()){
        return i->device->get_nb_capture_per_second();
    }
    return 0;
}

auto DCDeviceManager::get_processing_duration_ms() -> int64_t{
    if(is_device_initialized()){
        return i->device->get_processing_duration_ms();
    }
    return 0;
}

auto DCDeviceManager::get_compressing_duration_ms() -> int64_t{
    if(is_device_initialized()){
        return i->device->get_compressing_duration_ms();
    }
    return 0;
}

auto DCDeviceManager::get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> int64_t{
    if(is_device_initialized()){
        return i->device->get_duration_between_ms(from, to);
    }
    return 0;
}

auto DCDeviceManager::get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> int64_t{    
    if(is_device_initialized()){
        return i->device->get_duration_between_micro_s(from, to);
    }
    return 0;
}

auto DCDeviceManager::update_device_settings(const DCDeviceSettings &deviceS) -> void{

    const auto &newActionsS = deviceS.actionsS;
    const auto &newConfigS  = deviceS.configS;
    const auto &currConfigS = i->deviceS.configS;

    bool deviceChanged   = currConfigS.typeDevice != newConfigS.typeDevice;
    bool deviceIdChanged = currConfigS.idDevice != newConfigS.idDevice;
    bool cameraSettingsChanged =
        // device
        (newConfigS.mode                            != currConfigS.mode) ||
        // synch
        (newConfigS.synchronizeColorAndDepth        != currConfigS.synchronizeColorAndDepth) ||
        (newConfigS.delayBetweenColorAndDepthUsec   != currConfigS.delayBetweenColorAndDepthUsec) ||
        (newConfigS.subordinateDelayUsec            != currConfigS.subordinateDelayUsec) ||
        (newConfigS.synchMode                       != currConfigS.synchMode) ||
        // body tracking
        (newConfigS.enableBodyTracking              != currConfigS.enableBodyTracking) ||
        (newConfigS.btGPUId                         != currConfigS.btGPUId) ||
        (newConfigS.btOrientation                   != currConfigS.btOrientation) ||
        (newConfigS.btProcessingMode                != currConfigS.btProcessingMode) ||
        // others
        (newConfigS.disableLED                      != currConfigS.disableLED);

    bool closeDevice = false;
    bool stopReading = false;
    if(is_opened()){
        closeDevice = deviceChanged || deviceIdChanged || !newActionsS.openDevice;
    }
    if(is_reading()){
        stopReading = closeDevice || !newActionsS.startReading || cameraSettingsChanged;
    }

    // stop / close camera
    if(is_device_initialized()){
        if(stopReading){
            i->device->stop_reading();
        }
        if(closeDevice){
            i->device->close();
        }
    }
    if(closeDevice || stopReading){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // generate device
    if(!is_device_initialized() || deviceChanged){
        initialize_device(newConfigS.typeDevice);
    }

    bool openDevice  = newActionsS.openDevice    && (!i->device->is_opened());
    bool startCamera = newActionsS.startReading  && (!i->device->is_reading());

    // update device settings
    i->deviceS = deviceS;

    // set device data settings
    i->device->set_data_settings(i->deviceS.dataS);

    // open device camera
    auto idDevice = i->deviceS.configS.idDevice;
    if(openDevice){
        if(i->device->open(idDevice)){
            i->device->set_filters_settings(i->filters);

            if(deviceChanged){
                tool::Logger::message("DEVICE CHANGED COLOR SETTINGS DEFAULTED\n");
                i->colorsS.set_default_values(i->deviceS.configS.typeDevice);
                color_settings_reset_signal(i->colorsS);
            }
            i->device->set_color_settings(i->colorsS);
            i->device->set_delay_settings(i->delayS);                        
            update_device_name_signal(idDevice, std::format("Id:{} Num:{}", idDevice, i->device->device_name()));
        }
    }

    // start device camera
    if(startCamera){
        i->device->start_reading(i->deviceS.configS);
    }
}


