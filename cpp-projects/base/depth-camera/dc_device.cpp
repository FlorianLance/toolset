
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

struct DCDevice::Impl{
    DCType type;
    std::unique_ptr<DCDeviceImpl> dd = nullptr;
};

DCDevice::DCDevice(DCType type): i(std::make_unique<Impl>()){

    i->type = type;
    auto lg = LogGuard("DCDevice::DCDevice"sv);
    if(type == DCType::AzureKinect){
        auto lg = LogGuard("DCDevice::DCDevice AzureKinectDeviceImpl"sv);
        i->dd = std::make_unique<AzureKinectDeviceImpl>();
    }else if(type == DCType::FemtoBolt){
        auto lg = LogGuard("DCDevice::DCDevice FemtoBoltDeviceImpl"sv);
        i->dd = std::make_unique<FemtoBoltDeviceImpl>();
    }else if(type == DCType::FemtoMega){
        auto lg = LogGuard("DCDevice::DCDevice FemtoMegaDeviceImpl"sv);
        i->dd = std::make_unique<FemtoMegaDeviceImpl>();
    }else if(type == DCType::Recording){
        auto lg = LogGuard("DCDevice::DCDevice RecordingDeviceImpl"sv);
        i->dd = std::make_unique<RecordingDeviceImpl>();
    }
    i->dd->set_parent_device(this);
}

DCDevice::~DCDevice(){
    clean();
}

auto DCDevice::open(std::uint32_t deviceId) -> bool{

    auto lg = LogGuard("DCDevice::open"sv);
    if(is_opened()){
        return false;
    }

    if(i->type != DCType::FemtoMega){
        // cameras
        auto deviceCount = nb_devices();
        if(deviceCount == 0){
            Logger::warning("[DCDevice::open] No device found\n");
            return false;
        }
        Logger::message(std::format("[DCDevice] Devices found: {}\n", deviceCount));
        if(deviceId >= deviceCount){
            Logger::error("[DCDevice::open] Invalid device id\n");
            return false;
        }
    }

    return i->dd->open(deviceId);
}

auto DCDevice::close() -> void{

    auto lg = LogGuard("DCDevice::close"sv);
    if(!is_opened()){
        return;
    }
    if(is_reading()){
        Logger::error("[DCDevice::close] Device reading must be stopped before closing it.\n");
        return;
    }
    i->dd->close();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
}

auto DCDevice::clean() -> void{

    auto lg = LogGuard("DCDevice::clean"sv);
    stop_reading();
    close();
}

auto DCDevice::nb_devices() const noexcept -> uint32_t{
    return i->dd->nb_devices();
}

auto DCDevice::device_name() const noexcept -> std::string{
    return i->dd->device_name();
}

auto DCDevice::is_opened() const noexcept -> bool{
    return i->dd->is_opened();
}

auto DCDevice::is_reading() const noexcept -> bool{
    return i->dd->is_reading_frames_from_camera();
}


auto DCDevice::get_capture_duration_ms() noexcept -> int64_t{
    if(auto duration = i->dd->get_duration_ms("CAPTURE_FRAME"sv); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto DCDevice::get_processing_duration_ms() noexcept -> int64_t{
    if(auto duration = i->dd->get_duration_ms("PROCESSING_DATA"sv); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto DCDevice::get_duration_ms(std::string_view id) noexcept -> int64_t{
    if(auto duration = i->dd->get_duration_ms(id); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto DCDevice::get_duration_micro_s(std::string_view id) noexcept -> int64_t{
    if(auto duration = i->dd->get_duration_micro_s(id); duration.has_value()){
        return duration.value().count();
    }
    return -1;
}

auto DCDevice::start_reading(const DCConfigSettings &configS) -> bool{

    auto lg = LogGuard("DCDevice::start_reading"sv);
    if(is_reading() || !is_opened()){
        return false;
    }
    if(i->dd->start(configS)){
        i->dd->start_reading_thread();
        return true;
    }
    return false;
}

auto DCDevice::stop_reading() -> void{

    auto lg = LogGuard("DCDevice::stop_reading"sv);   
    if(!is_reading() || !is_opened()){
        return;
    }
    i->dd->stop_reading_thread();
    i->dd->stop();
}

auto DCDevice::set_color_settings(const DCColorSettings &colorS) -> void{    
    auto lg = LogGuard("DCDevice::set_color_settings"sv);
    i->dd->set_color_settings(colorS);
}

auto DCDevice::set_data_settings(const DCDeviceDataSettings &dataS) -> void{
    auto lg = LogGuard("DCDevice::set_data_settings"sv);
    i->dd->set_data_settings(dataS);
}

auto DCDevice::set_filters_settings(const DCFiltersSettings &filtersS) -> void{
    auto lg = LogGuard("DCDevice::set_filters_settings"sv);
    i->dd->set_filters_settings(filtersS);
}

auto DCDevice::set_delay_settings(const DCDelaySettings &delayS) -> void{
    auto lg = LogGuard("DCDevice::set_delay_settings"sv);
    i->dd->set_delay_settings(delayS);
}


struct DeviceAction{
    bool cleanDevice   = false;
    bool createDevice  = false;
    bool closeDevice   = false;
    bool openDevice    = false;
    bool loopDataReset = false;
    // bool readData      = false;
};

struct DCDevice2::Impl{

    // settings
    DCDeviceSettings deviceS;
    DCFiltersSettings filters;
    DCColorSettings colorsS;
    DCDataSettings dataS;
    DCDelaySettings delayS;

    // device
    std::unique_ptr<DCDeviceImpl> device = nullptr;

    // threads/tasks
    std::unique_ptr<std::thread> loopT = nullptr;

    // states
    std::atomic_bool isDeviceInitializedS   = false;
    std::atomic_bool isOpenedS              = false;
    std::atomic_bool readDataS              = false;

    // actions
    bool doLoopA = false;
    std::mutex locker;
    std::optional<DeviceAction> dAction      = std::nullopt;
};

DCDevice2::DCDevice2(): i(std::make_unique<Impl>()){

    // start thread
    i->loopT = std::make_unique<std::thread>(&DCDevice2::thread_loop, this);
}

DCDevice2::~DCDevice2(){

    // stop thread
    i->doLoopA = false;
    if(i->loopT->joinable()){
        i->loopT->join();
    }
    i->loopT = nullptr;
}

auto DCDevice2::update_device_settings(const DCDeviceSettings &deviceS) -> void{

    const auto &newConfigS  = deviceS.configS;
    const auto &currConfigS = i->deviceS.configS;

    // if(deviceChanged && i->device != nullptr){
    //     for(size_t ii = 0; ii < i->device->nb_devices(); ++ii){
    //         update_device_name_signal(static_cast<int>(ii), std::format("Cam {}", ii));
    //     }
    // }

    bool deviceChanged   = currConfigS.typeDevice != newConfigS.typeDevice;

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
    dAction.cleanDevice     = (i->device != nullptr) && deviceChanged;
    // device must be created
    dAction.createDevice    =  ((i->device == nullptr) && (newConfigS.typeDevice != DCType::Undefined)) || deviceChanged;
    // device must be closed
    dAction.closeDevice     = !newConfigS.openDevice || deviceIdChanged || syncChanged || dAction.cleanDevice;
    // device must be opened
    dAction.openDevice      = newConfigS.openDevice;
    // loop data must be reset
    dAction.loopDataReset   = cameraSettingsChanged || dAction.createDevice || dAction.openDevice;

    // replace current config
    i->locker.lock();
    i->deviceS = deviceS;
    i->locker.unlock();

    // update reading data state
    i->readDataS = newConfigS.startReading;
}

auto DCDevice2::thread_loop() -> void{

    i->doLoopA = true;

    while(i->doLoopA){

        i->locker.lock();
        auto dAction = i->dAction;
        i->locker.unlock();

        if(dAction.has_value()){

            // close device
            if(i->isDeviceInitializedS && i->isOpenedS && dAction->closeDevice){
                i->device->stop();
                i->device->close();
                i->isOpenedS = false;
            }

            // clean device
            if(i->isDeviceInitializedS && dAction->cleanDevice){
                i->device = nullptr;
                i->isDeviceInitializedS = false;
            }

            // create device
            if(!i->isDeviceInitializedS && dAction->createDevice){
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
                    // ...
                    break;
                }
                // device->set_parent_device(this);
                i->isDeviceInitializedS = true;

                // update_device_name_signal(idDevice, std::format("Id:{} Num:{}", idDevice, i->device->device_name()));

                // // set connections
                // i->device->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
                //     new_frame_signal(std::move(frame));
                // });
                // i->device->new_compressed_frame_signal.connect([&](std::shared_ptr<DCCompressedFrame> frame){
                //     new_compressed_frame_signal(std::move(frame));
                // });
            }

            // open device
            if(i->isDeviceInitializedS && !i->isOpenedS && dAction->openDevice){

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
                    if(i->device->open(i->deviceS.configS.idDevice)){

                        // if(deviceChanged){
                        //     tool::Logger::message("Device changed, colors settings set to default.\n");
                        //     i->colorsS.set_default_values(i->deviceS.configS.typeDevice);
                        //     color_settings_reset_signal(i->colorsS);
                        // }

                        // set current settings
                        i->device->set_color_settings(i->colorsS);
                        i->device->set_filters_settings(i->filters);
                        i->device->set_data_settings(i->deviceS.dataS.server);
                        i->device->set_delay_settings(i->delayS);

                        i->isOpenedS = true;

                        // update_device_name_signal(idDevice, std::format("Id:{} Num:{}", idDevice, i->device->device_name()));
                    }
                }
            }

            // reset loop data
            if(i->isDeviceInitializedS && i->isOpenedS && dAction->loopDataReset){
                i->device->loop_initialization();
            }

            dAction = std::nullopt;
        }


        // if(readData){

        // }

        // if(openDevice && !isOpened){

        // }else if(!openDevice && isOpened){

        // }

        // if(initDevice && !isDeviceInitialized){

        // }else if(!initDevice && isDeviceInitialized){

        // }

    }
}

