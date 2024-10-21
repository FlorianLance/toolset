
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
#include "depth-camera/impl/femto_mega_ethernet_device_impl.hpp"
#include "depth-camera/impl/femto_mega_usb_device_impl.hpp"
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

    bool resetColorSettings = false;
};

struct DCDevice::Impl{

    // settings
    DCDeviceSettings deviceS;
    DCFiltersSettings filtersS;
    DCColorSettings colorsS;
    DCMiscSettings delayS;

    // device
    std::unique_ptr<DCDeviceImpl> device = nullptr;

    // threads/tasks
    std::unique_ptr<std::thread> loopT = nullptr;

    // states
    std::atomic<size_t> devicesNb = 0;
    std::atomic_bool deviceInitialized = false;
    std::atomic_bool deviceOpened = false;
    std::atomic_bool readFrames = false;

    AverageBuffer procB;
    AverageBuffer sleepB;
    std::atomic<double> procUsage = 0;

    // actions
    bool doLoopA = false;
    std::mutex locker;
    std::vector<DeviceAction> actions;
};

DCDevice::DCDevice(): i(std::make_unique<Impl>()){
}

DCDevice::~DCDevice(){
    stop_thread();
}

auto DCDevice::start_thread() -> void{

    if(i->loopT != nullptr){
        Log::error("[DCDevice::start_thread] Thread already running."sv);
        return;
    }

    i->loopT = std::make_unique<std::thread>([&](){
        Log::message("[DCDevice::start_thread]\n"sv);
        i->doLoopA = true;
        size_t loopCounter = 0;
        while(i->doLoopA){

            process();

            if(loopCounter%20 == 0){
                auto v1 = i->procB.get();
                auto v2 = i->sleepB.get();
                i->procUsage = v1 / (v1 + v2);
                loopCounter = 0;
            }
            ++loopCounter;
        }
    });
}

auto DCDevice::stop_thread() -> void{

    i->doLoopA = false;
    if(i->loopT != nullptr){
        if(i->loopT->joinable()){
            i->loopT->join();
        }
        i->loopT = nullptr;
    }
}

auto DCDevice::process() -> void{

    auto tStart = Time::nanoseconds_since_epoch();

    i->locker.lock();
    auto dActions = i->actions;
    i->actions.clear();
    bool readFrames = i->readFrames;
    i->locker.unlock();

    for(const auto &dAction : dActions){

        // close device
        if((i->device != nullptr) && dAction.closeDevice){
            if(i->device->is_opened()){
                i->device->close();
                i->deviceOpened = false;
            }
        }

        // clean device
        if((i->device != nullptr) && dAction.cleanDevice){
            i->device = nullptr;
            i->deviceInitialized = false;
        }

        // create device
        if((i->device == nullptr) && dAction.createDevice){

            auto type = i->deviceS.configS.typeDevice;
            if(type == DCType::AzureKinect){
                auto lg = LogG("DCDevice::DCDevice AzureKinectDeviceImpl"sv);
                i->device = std::make_unique<AzureKinectDeviceImpl>();
            }else if(type == DCType::FemtoBolt){
                auto lg = LogG("DCDevice::DCDevice FemtoBoltDeviceImpl"sv);
                i->device = std::make_unique<FemtoBoltDeviceImpl>();
            }else if(type == DCType::FemtoMegaEthernet){
                auto lg = LogG("DCDevice::DCDevice FemtoMegaEthernetDeviceImpl"sv);
                i->device = std::make_unique<FemtoMegaEthernetDeviceImpl>();
            }else if(type == DCType::FemtoMegaUSB){
                auto lg = LogG("DCDevice::DCDevice FemtoMegaUSBDeviceImpl"sv);
                i->device = std::make_unique<FemtoMegaUSBDeviceImpl>();
            }else if(type == DCType::Recording){
                auto lg = LogG("DCDevice::DCDevice RecordingDeviceImpl"sv);
                i->device = std::make_unique<RecordingDeviceImpl>();
            }else{
                return;
            }

            // set connections
            i->device->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
                new_frame_signal(std::move(frame));
            });
            i->device->new_data_frame_signal.connect([&](std::shared_ptr<DCDataFrame> frame){
                new_data_frame_signal(std::move(frame));
            });

            i->deviceInitialized = true;
        }

        // open device
        if((i->device != nullptr) && dAction.openDevice){

            if(!i->device->is_opened()){

                if(i->device->open(i->deviceS.configS)){

                    i->deviceOpened = true;

                    // update settings
                    {
                        std::unique_lock<std::mutex> lock(i->locker);
                        i->device->set_data_settings(i->deviceS.dataS.server);
                        i->device->set_filters_settings(i->filtersS);
                        i->device->set_delay_settings(i->delayS);

                        if(dAction.resetColorSettings){
                            i->colorsS.set_default_values(i->deviceS.configS.typeDevice);
                            color_settings_reset_signal(i->colorsS);
                        }

                        i->device->set_color_settings(i->colorsS);
                        i->device->update_from_colors_settings();
                    }

                    // update device name
                    update_device_name_signal(i->deviceS.configS.idDevice, std::format("[{}] [{}]", i->deviceS.configS.idDevice, i->device->device_name()));
                }

            }
        }

        // update settings
        if(i->device != nullptr){

            if(i->device->is_opened()){

                std::unique_lock<std::mutex> lock(i->locker);
                i->device->set_data_settings(i->deviceS.dataS.server);

                bool updateSettings = dAction.updateColors || dAction.updateFilters || dAction.updateDelay;
                if(updateSettings){
                    if(dAction.updateColors){
                        i->device->set_color_settings(i->colorsS);
                        i->device->update_from_colors_settings();
                    }
                    if(dAction.updateFilters){
                        i->device->set_filters_settings(i->filtersS);
                    }
                    if(dAction.updateDelay){
                        i->device->set_delay_settings(i->delayS);
                    }
                }
            }
        }
    }

    // process frame
    if(i->device != nullptr){
        if(i->device->is_opened()){
            if(readFrames ){
                i->device->process();
            }
        }
    }

    auto tEnd = Time::nanoseconds_since_epoch();
    auto diff = Time::difference_ms(tStart,tEnd);
    int targetFPS = 30;
    int timeToWait = 0;
    if(diff.count() < targetFPS){
        timeToWait = static_cast<int>((targetFPS - diff.count())*0.75);
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToWait));
    }
    i->procB.add_value(diff.count());
    i->sleepB.add_value(Time::difference_ms(tEnd,Time::nanoseconds_since_epoch()).count());

}

auto DCDevice::update_device_settings(const DCDeviceSettings &deviceS) -> void{

    const auto &newConfigS  = deviceS.configS;
    const auto &currConfigS = i->deviceS.configS;

    bool deviceTypeChanged   = currConfigS.typeDevice != newConfigS.typeDevice;
    if(deviceTypeChanged && i->deviceInitialized){
        update_device_name_signal(i->deviceS.configS.idDevice, std::format("[{}] [...]"sv, i->deviceS.configS.idDevice));
    }

    bool deviceChanged =
        (newConfigS.useSerialNumber != currConfigS.useSerialNumber) ||
        (newConfigS.idDevice        != currConfigS.idDevice)        ||
        (newConfigS.serialNumber    != currConfigS.serialNumber)    ||
        (newConfigS.ipv4Address     != currConfigS.ipv4Address);

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
    dAction.cleanDevice     = i->deviceInitialized && deviceTypeChanged;
    // device must be created
    dAction.createDevice    = (!i->deviceInitialized && (newConfigS.typeDevice != DCType::Undefined)) || deviceTypeChanged;
    // device must be closed
    dAction.closeDevice     = !newConfigS.openDevice || deviceChanged || syncChanged || cameraSettingsChanged || dAction.cleanDevice;
    // device must be opened
    dAction.openDevice      = newConfigS.openDevice;
    // colors resetings must be resetted
    dAction.resetColorSettings = deviceTypeChanged;

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->deviceS = deviceS;
    i->actions.push_back(dAction);
    i->readFrames = newConfigS.startReading;

    // TODO: add timing to action?
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
    i->actions.push_back(dAction);
}

auto DCDevice::update_filters_settings(const DCFiltersSettings &filtersS) -> void{

    i->filtersS = filtersS;

    DeviceAction dAction;
    dAction.updateFilters = true;

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->actions.push_back(dAction);
}

auto DCDevice::update_misc_settings(const DCMiscSettings &miscS) -> void{

    i->delayS = miscS;

    DeviceAction dAction;
    dAction.updateDelay = true;

    // update actions
    std::unique_lock<std::mutex> lock(i->locker);
    i->actions.push_back(dAction);
}

auto DCDevice::get_capture_duration_ms() noexcept -> int64_t{
    if(i->deviceOpened){
        if(auto duration = i->device->get_duration_ms("CAPTURE_FRAME"sv); duration.has_value()){
            return duration.value().count();
        }
    }
    return 0;
}

auto DCDevice::get_processing_duration_ms() noexcept -> int64_t{
    if(i->deviceOpened){
        if(auto duration = i->device->get_duration_ms("PROCESSING_DATA"sv); duration.has_value()){
            return duration.value().count();
        }
    }
    return 0;
}

auto DCDevice::get_duration_ms(std::string_view id) noexcept -> int64_t{
    if(i->deviceOpened){
        if(auto duration = i->device->get_duration_ms(id); duration.has_value()){
            return duration.value().count();
        }
    }
    return 0;
}

auto DCDevice::get_duration_micro_s(std::string_view id) noexcept -> int64_t{
    if(i->deviceOpened){
        if(auto duration = i->device->get_duration_micro_s(id); duration.has_value()){
            return duration.value().count();
        }
    }
    return 0;
}

auto DCDevice::get_average_framerate() -> float{
    if(i->deviceOpened){
        return i->device->get_average_framerate();
    }
    return 0.f;
}

auto DCDevice::get_proc_usage() const -> double{
    return i->procUsage;
}
