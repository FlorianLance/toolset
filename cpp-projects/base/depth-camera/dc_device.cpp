
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

#include "dc_device.hpp"

// local
#include "depth-camera/impl/azure_kinect_device_impl.hpp"
#include "depth-camera/impl/femto_bolt_device_impl.hpp"
#include "depth-camera/impl/femto_mega_device_impl.hpp"
#include "depth-camera/impl/recording_device_impl.hpp"
#include "utility/logger.hpp"

using namespace tool::geo;
using namespace tool::cam;

struct DeviceAction{

    bool cleanDevice   = false;
    bool createDevice  = false;
    bool closeDevice   = false;
    bool openDevice    = false;

    bool updateColors  = false;
    bool updateFilters  = false;
    bool updateDelay   = false;
};

struct DCDevice::Impl{

    // settings
    DCDeviceSettings deviceS;
    DCFiltersSettings filtersS;
    DCColorSettings colorsS;
    DCDelaySettings delayS;

    // device
    std::unique_ptr<DCDeviceImpl> device = nullptr;

    // threads/tasks
    std::unique_ptr<std::thread> loopT = nullptr;

    // states
    std::atomic<size_t> devicesNb = 0;
    std::atomic_bool deviceInitialized = false;
    std::atomic_bool deviceOpened = false;
    std::atomic_bool readFrames = false;

    // actions
    bool doLoopA = false;
    std::mutex locker;
    std::optional<DeviceAction> dAction  = std::nullopt;
};

DCDevice::DCDevice(): i(std::make_unique<Impl>()){

    // start thread
    i->loopT = std::make_unique<std::thread>(&DCDevice::thread_loop, this);
}

DCDevice::~DCDevice(){

    // stop thread
    i->doLoopA = false;
    if(i->loopT->joinable()){
        i->loopT->join();
    }
    i->loopT = nullptr;
}

auto DCDevice::process() -> void{

    i->locker.lock();
    auto dAction    = i->dAction;
    bool readFrames = i->readFrames;
    i->locker.unlock();

    if(dAction.has_value()){

        // close device
        if((i->device != nullptr) && dAction->closeDevice){
            if(i->device->is_opened()){
                i->device->close();
                i->deviceOpened = false;
            }
        }

        // clean device
        if((i->device != nullptr) && dAction->cleanDevice){
            i->device = nullptr;
            i->deviceInitialized = false;
        }

        // create device
        if((i->device == nullptr) && dAction->createDevice){

            auto type = i->deviceS.configS.typeDevice;
            if(type == DCType::AzureKinect){
                auto lg = LogGuard("DCDevice::DCDevice AzureKinectDeviceImpl"sv);
                i->device = std::make_unique<AzureKinectDeviceImpl>();
            }else if(type == DCType::FemtoBolt){
                auto lg = LogGuard("DCDevice::DCDevice FemtoBoltDeviceImpl"sv);
                i->device = std::make_unique<FemtoBoltDeviceImpl>();
            }else if(type == DCType::FemtoMega){
                auto lg = LogGuard("DCDevice::DCDevice FemtoMegaDeviceImpl"sv);
                i->device = std::make_unique<FemtoMegaDeviceImpl>();
            }else if(type == DCType::Recording){
                auto lg = LogGuard("DCDevice::DCDevice RecordingDeviceImpl"sv);
                i->device = std::make_unique<RecordingDeviceImpl>();
            }else{
                return;
            }

            // set connections
            i->device->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
                new_frame_signal(std::move(frame));
            });
            i->device->new_compressed_frame_signal.connect([&](std::shared_ptr<DCCompressedFrame> frame){
                new_compressed_frame_signal(std::move(frame));
            });

            i->deviceInitialized = true;
        }

        // open device
        if((i->device != nullptr) && dAction->openDevice){

            if(!i->device->is_opened()){

                bool canOpenDevice = true;
                if(i->deviceS.configS.typeDevice != DCType::FemtoMega){
                    auto deviceCount = i->device->nb_devices();
                    if(deviceCount == 0){
                        canOpenDevice = false;
                        Logger::error("[DCDevice::open] No device found\n");
                    } else if(i->deviceS.configS.idDevice >= deviceCount){
                        canOpenDevice = false;
                        Logger::error(std::format("[DCDevice::open] Invalid device id: [{}], available: [{}]\n", i->deviceS.configS.idDevice, deviceCount));
                    }
                }

                if(canOpenDevice){
                    if(i->device->open(i->deviceS.configS)){

                        i->deviceOpened = true;

                        // update settings
                        {
                            std::unique_lock<std::mutex> lock(i->locker);
                            i->device->set_data_settings(i->deviceS.dataS.server);
                            i->device->update_from_colors_settings();
                            i->device->set_color_settings(i->colorsS);
                            i->device->update_from_colors_settings();
                            i->device->set_filters_settings(i->filtersS);
                            i->device->set_delay_settings(i->delayS);

                        }

                        // update device name
                        update_device_name_signal(i->deviceS.configS.idDevice, std::format("Id:{} Num:{}", i->deviceS.configS.idDevice, i->device->device_name()));
                    }
                }
            }
        }

        // update settings
        if(i->device != nullptr){
            if(i->device->is_opened()){

                std::unique_lock<std::mutex> lock(i->locker);
                i->device->set_data_settings(i->deviceS.dataS.server);

                bool updateSettings = dAction->updateColors || dAction->updateFilters || dAction->updateDelay;
                if(updateSettings){
                    if(dAction->updateColors){
                        i->device->set_color_settings(i->colorsS);
                        i->device->update_from_colors_settings();
                    }
                    if(dAction->updateFilters){
                        i->device->set_filters_settings(i->filtersS);
                    }
                    if(dAction->updateDelay){
                        i->device->set_delay_settings(i->delayS);
                    }
                }
            }
        }

        // reset action
        i->dAction = std::nullopt;
    }

    // process frame
    if(i->device != nullptr){
        if(i->device->is_opened()){
            if(readFrames ){
                i->device->process();
                return;
            }
        }
    }

    // wait
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

auto DCDevice::update_device_settings(const DCDeviceSettings &deviceS) -> void{

    Logger::message("update_device_settings");

    const auto &newConfigS  = deviceS.configS;
    const auto &currConfigS = i->deviceS.configS;

    bool deviceChanged   = currConfigS.typeDevice != newConfigS.typeDevice;
    if(deviceChanged && i->deviceInitialized){
        update_device_name_signal(i->deviceS.configS.idDevice, std::format("Id:{} Cam: ...", i->deviceS.configS.idDevice));
    }

    bool deviceIdChanged = currConfigS.idDevice != newConfigS.idDevice;

    bool syncChanged  =
        (newConfigS.synchronizeColorAndDepth        != currConfigS.synchronizeColorAndDepth) ||
        (newConfigS.delayBetweenColorAndDepthUsec   != currConfigS.delayBetweenColorAndDepthUsec) ||
        (newConfigS.subordinateDelayUsec            != currConfigS.subordinateDelayUsec) ||
        (newConfigS.synchMode                       != currConfigS.synchMode);

    bool cameraSettingsChanged =
        syncChanged ||
        // device
        (newConfigS.mode                            != currConfigS.mode) ||
        // body tracking
        (newConfigS.btEnabled                       != currConfigS.btEnabled) ||
        (newConfigS.btGPUId                         != currConfigS.btGPUId) ||
        (newConfigS.btOrientation                   != currConfigS.btOrientation) ||
        (newConfigS.btProcessingMode                != currConfigS.btProcessingMode) ||
        // others
        (newConfigS.disableLED                      != currConfigS.disableLED) ||
        // color - depth calibration
        (newConfigS.colorAlignmentTr                != currConfigS.colorAlignmentTr) ||
        (newConfigS.colorAlignmentRot               != currConfigS.colorAlignmentRot);


    // device must be deleted
    DeviceAction dAction;    
    dAction.cleanDevice     = i->deviceInitialized && deviceChanged;
    // device must be created
    dAction.createDevice    = (!i->deviceInitialized && (newConfigS.typeDevice != DCType::Undefined)) || deviceChanged;
    // device must be closed
    dAction.closeDevice     = !newConfigS.openDevice || deviceIdChanged || syncChanged || cameraSettingsChanged || dAction.cleanDevice;
    // device must be opened
    dAction.openDevice      = newConfigS.openDevice;

    Logger::message(std::format("{} {} {} {}\n",dAction.cleanDevice, dAction.createDevice, dAction.closeDevice, dAction.openDevice));

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->deviceS = deviceS;
    i->dAction = dAction;
    i->readFrames = newConfigS.startReading;
    // if(deviceChanged){
    //     tool::Logger::message("Device changed, colors settings set to default.\n");
    //     i->colorsS.set_default_values(i->deviceS.configS.typeDevice);
    //     color_settings_reset_signal(i->colorsS);
    // }
    Logger::message("end");
}


auto DCDevice::thread_loop() -> void{

    i->doLoopA = true;
    while(i->doLoopA){
        process();
    }
}

auto DCDevice::is_opened() const noexcept -> bool{
    return i->deviceOpened;
}

auto DCDevice::update_color_settings(const DCColorSettings &colorS) -> void{

    i->colorsS = colorS;

    DeviceAction dAction;
    dAction.updateColors = true;

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->dAction = dAction;
}

auto DCDevice::update_filters_settings(const DCFiltersSettings &filtersS) -> void{

    i->filtersS = filtersS;

    DeviceAction dAction;
    dAction.updateFilters = true;

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->dAction = dAction;
}

auto DCDevice::update_delay_settings(const DCDelaySettings &delayS) -> void{

    i->delayS = delayS;

    DeviceAction dAction;
    dAction.updateDelay = true;

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->dAction = dAction;
}

auto DCDevice::get_capture_duration_ms() noexcept -> int64_t{
    if(i->deviceInitialized){
        if(auto duration = i->device->get_duration_ms("CAPTURE_FRAME"sv); duration.has_value()){
            return duration.value().count();
        }
    }
    return -1;
}

auto DCDevice::get_processing_duration_ms() noexcept -> int64_t{
    if(i->deviceInitialized){
        if(auto duration = i->device->get_duration_ms("PROCESSING_DATA"sv); duration.has_value()){
            return duration.value().count();
        }
    }
    return -1;
}

auto DCDevice::get_duration_ms(std::string_view id) noexcept -> int64_t{
    if(i->deviceInitialized){
        if(auto duration = i->device->get_duration_ms(id); duration.has_value()){
            return duration.value().count();
        }
    }
    return -1;
}

auto DCDevice::get_duration_micro_s(std::string_view id) noexcept -> int64_t{
    if(i->deviceInitialized){
        if(auto duration = i->device->get_duration_micro_s(id); duration.has_value()){
            return duration.value().count();
        }
    }
    return -1;
}
