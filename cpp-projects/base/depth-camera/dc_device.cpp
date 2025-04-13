
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

// std
#include <unordered_map>

// local
#include "depth-camera/impl/azure_kinect_device_impl.hpp"
#include "depth-camera/impl/femto_bolt_device_impl.hpp"
#include "depth-camera/impl/femto_mega_ethernet_device_impl.hpp"
#include "depth-camera/impl/femto_mega_usb_device_impl.hpp"
#include "depth-camera/impl/gemini_215_device_impl.hpp"
#include "depth-camera/impl/recording_device_impl.hpp"
#include "utility/logger.hpp"

using namespace tool::geo;
using namespace tool::cam;

// enum class ActionType : std::int8_t{
//     Device,
//     Colors,
//     Filters,
//     Delay,
//     None
// };

// struct DeviceAction{

    // ActionType type = ActionType::None;

    // bool cleanDevice   = false;
    // bool createDevice  = false;
    // bool closeDevice   = false;
    // bool openDevice    = false;
    // bool resetColorSettings = false;

    // bool updateColors  = false;
    // bool updateFilters  = false;
    // bool updateDelay   = false;

// };

struct DCDevice::Impl{


    // settings
    std::mutex locker;
    std::optional<DCDeviceSettings> nDeviceS = std::nullopt;
    std::optional<DCFiltersSettings> nFiltersS = std::nullopt;
    std::optional<DCColorSettings> nColorsS = std::nullopt;
    std::optional<DCMiscSettings> nMiscS = std::nullopt;
    DCDeviceSettings deviceS;
    DCFiltersSettings filtersS;
    DCColorSettings colorsS;
    DCMiscSettings miscS;

    // device
    std::unique_ptr<DCDeviceImpl> device = nullptr;

    // threads/tasks
    std::unique_ptr<std::thread> loopT = nullptr;
    tf::Taskflow readFramesT; // inused

    // states
    // std::atomic<size_t> devicesNb = 0;
    std::atomic_bool deviceInitialized = false;
    std::atomic_bool deviceOpened = false;
    // std::atomic_bool readFrames = false;

    // AverageBuffer procB;
    // AverageBuffer sleepB;
    std::atomic<double> procUsage = 0; // inused

    // actions
    std::atomic_bool doLoopA = false;
    // bool localReadFrames = false;

    // std::vector<DeviceAction> actions;
    // std::unordered_map<ActionType, DeviceAction> actions;

    // last frames
    std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>> lastFrames = {nullptr,nullptr};

    // test
    // std::vector<DeviceAction> localActions;


};

DCDevice::DCDevice(): i(std::make_unique<Impl>()){

    // auto hasLock

    // si pas de lock
    //  read frame
    //  sleep

    // auto copyActionsT = i->readFramesT.emplace([&](){
    //     Log::fmessage("#################################################### copyActionsT\n");
    //     i->locker.lock();
    //     i->localActions = i->actions;
    //     i->actions.clear();
    //     i->localReadFrames = i->readFrames;
    //     i->locker.unlock();
    // }).name("copyActions");

    // auto processActionsT = i->readFramesT.emplace([&](){

    //     // Log::fmessage("#################################################### processActionsT {}\n", i->localActions.size());
    //     for(const auto &dAction : i->localActions){

    //         if(dAction.type == ActionType::Device){
    //             Log::fmessage("#################################################### DEVICE\n");
    //             // close device
    //             if((i->device != nullptr) && dAction.closeDevice){
    //                 if(i->device->is_opened()){
    //                     i->device->close();
    //                     i->deviceOpened = false;
    //                     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //                 }
    //             }

    //             // clean device
    //             if((i->device != nullptr) && dAction.cleanDevice){
    //                 i->device = nullptr;
    //                 i->deviceInitialized = false;
    //                 std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //             }

    //             // create device
    //             if((i->device == nullptr) && dAction.createDevice){

    //                 Log::fmessage("#################################################### CREATE\n");
    //                 auto type = i->deviceS.configS.typeDevice;
    //                 if(type == DCType::AzureKinect){
    //                     auto lg = LogG("DCDevice::DCDevice AzureKinectDeviceImpl"sv);
    //                     i->device = std::make_unique<AzureKinectDeviceImpl>();
    //                 }else if(type == DCType::FemtoBolt){
    //                     auto lg = LogG("DCDevice::DCDevice FemtoBoltDeviceImpl"sv);
    //                     i->device = std::make_unique<FemtoBoltDeviceImpl>();
    //                 }else if(type == DCType::FemtoMegaEthernet){
    //                     auto lg = LogG("DCDevice::DCDevice FemtoMegaEthernetDeviceImpl"sv);
    //                     i->device = std::make_unique<FemtoMegaEthernetDeviceImpl>();
    //                 }else if(type == DCType::FemtoMegaUSB){
    //                     auto lg = LogG("DCDevice::DCDevice FemtoMegaUSBDeviceImpl"sv);
    //                     i->device = std::make_unique<FemtoMegaUSBDeviceImpl>();
    //                 }else if(type == DCType::Recording){
    //                     auto lg = LogG("DCDevice::DCDevice RecordingDeviceImpl"sv);
    //                     i->device = std::make_unique<RecordingDeviceImpl>();
    //                 }

    //                 if(i->device != nullptr){

    //                     // set connections
    //                     i->device->new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
    //                         new_frame_signal(std::move(frame));
    //                     });
    //                     i->device->new_data_frame_signal.connect([&](std::shared_ptr<DCDataFrame> frame){
    //                         new_data_frame_signal(std::move(frame));
    //                     });

    //                     i->deviceInitialized = true;
    //                     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //                 }
    //             }

    //             // open device
    //             if((i->device != nullptr) && dAction.openDevice){

    //                 if(!i->device->is_opened()){

    //                     if(i->device->open(i->deviceS.configS)){

    //                         Log::fmessage("#################################################### OPEN\n");
    //                         i->deviceOpened = true;

    //                         // update settings
    //                         {
    //                             std::unique_lock<std::mutex> lock(i->locker);
    //                             i->device->set_data_settings(i->deviceS.dataS.server);
    //                             i->device->set_filters_settings(i->filtersS);
    //                             i->device->set_delay_settings(i->delayS);

    //                             if(dAction.resetColorSettings){
    //                                 i->colorsS.set_default_values(i->deviceS.configS.typeDevice);
    //                                 color_settings_reset_signal(i->colorsS);
    //                             }

    //                             i->device->set_color_settings(i->colorsS);
    //                             i->device->update_from_colors_settings();
    //                         }

    //                         // update device name
    //                         update_device_name_signal(i->deviceS.configS.idDevice, std::format("[{}] [{}]", i->deviceS.configS.idDevice, i->device->device_name()));
    //                     }

    //                 }
    //             }
    //         }

    //         // update settings
    //         if(i->device != nullptr){

    //             if(i->device->is_opened()){

    //                 std::unique_lock<std::mutex> lock(i->locker);
    //                 i->device->set_data_settings(i->deviceS.dataS.server);

    //                 if(dAction.type == ActionType::Colors){
    //                     Log::message("################################ COLOR\n ");
    //                     i->device->set_color_settings(i->colorsS);
    //                     i->device->update_from_colors_settings();
    //                 }
    //                 if(dAction.type == ActionType::Filters){
    //                     Log::message("################################ FILTERS\n ");
    //                     i->device->set_filters_settings(i->filtersS);
    //                 }
    //                 if(dAction.type == ActionType::Delay){
    //                     Log::message("################################ DELAY\n ");
    //                     i->device->set_delay_settings(i->delayS);
    //                 }

    //             }
    //         }
    //     }

    // }).name("processActions");

    // // readFrames
    // auto checkReadFrameT = i->readFramesT.emplace([&](){
    //     Log::message("################################ CHECK\n ");
    //     if(i->device != nullptr){
    //         if(i->device->is_opened()){
    //             if(i->localReadFrames ){
    //                 Log::message("################################ READ\n ");
    //                 return 0;
    //             }
    //         }
    //     }
    //     // Log::message("################################ WAIT\n ");
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //     return 1;
    // }).name("checkReadFrame");

    // auto readFrameModuleT = i->readFramesT.composed_of(*i->device->process_frames_task()).name("readFrameModule");

    // auto end = i->readFramesT.emplace([&](){
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //                              // Log::fmessage("#################################################### end\n");
    // }).name("end");

    // copyActionsT.precede(processActionsT);
    // processActionsT.precede(checkReadFrameT);
    // checkReadFrameT.precede(readFrameModuleT, end);

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
        // tf::Executor executor;
        // Log::fmessage("CREATE EXECUTOR\n");

        Log::message("[DCDevice::start_thread]\n"sv);
        i->doLoopA = true;
        // size_t loopCounter = 0;
        while(i->doLoopA){

            // ################## NEW
            // Log::fmessage("WAIT\n");
            // executor.run(*mainTF).wait();
            // i->lastFrames = {i->device->frame, i->device->dFrame};
            // if(std::get<0>(i->lastFrames)){
            //     Log::fmessage("frame {}\n", std::get<0>(i->lastFrames)->idCapture);
            // }

            // ################## CURRENT
            // auto lastFrames = read_frames();
            // i->lastFrames = std::move(lastFrames);

            // ################## OLD
            auto tBefore = Time::nanoseconds_since_epoch();
            i->lastFrames = read_frames();
            auto tAfter = Time::nanoseconds_since_epoch();

            auto diffMs = Time::difference_ms(tBefore, tAfter);
            if(diffMs.count() < 5){
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }


            // // Log::fmessage("Duration {}\n", diffMs);

            // i->lastFrames = std::move(lastFrames);

            // // std::this_thread::sleep_for(std::chrono::milliseconds(5));
            // // sleep

            // // if(loopCounter%20 == 0){
            // //     auto v1 = i->procB.get();
            // //     auto v2 = i->sleepB.get();
            // //     i->procUsage = v1 / (v1 + v2);
            // //     loopCounter = 0;
            // // }
            // // ++loopCounter;
            // // Log::fmessage("[{}]", Time::now_difference_micro_s(t1));
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

auto DCDevice::read_frames_from_external_thread() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>{

    if(i->loopT != nullptr){
        return {nullptr,nullptr};
    }

    i->lastFrames = read_frames();

    return i->lastFrames;
}

auto DCDevice::read_frames_task() -> tf::Taskflow*{
    return &i->readFramesT;
}

auto DCDevice::read_frames() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>{

    // auto tStart = Time::nanoseconds_since_epoch();

    i->locker.lock();
    std::optional<DCDeviceSettings> nDeviceS   = i->nDeviceS;
    std::optional<DCColorSettings> nColorS     = i->nColorsS;
    std::optional<DCFiltersSettings> nFiltersS = i->nFiltersS;
    std::optional<DCMiscSettings> nMisc        = i->nMiscS;
    i->nDeviceS  = std::nullopt;
    i->nColorsS  = std::nullopt;
    i->nFiltersS = std::nullopt;
    i->nMiscS   = std::nullopt;
    i->locker.unlock();

    if(nDeviceS.has_value()){

        // check changes
        const auto &newConfigS  = nDeviceS->configS;
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
            (newConfigS.colorAlignmentRotEuler          != currConfigS.colorAlignmentRotEuler);


        // # device must be deleted
        bool cleanDevice     = i->deviceInitialized && deviceTypeChanged;
        // # device must be created
        bool createDevice    = (!i->deviceInitialized && (newConfigS.typeDevice != DCType::Undefined)) || deviceTypeChanged;
        // # device must be closed
        bool closeDevice     = !newConfigS.openDevice || deviceChanged || syncChanged || cameraSettingsChanged ||cleanDevice;
        // # device must be opened
        bool openDevice      = newConfigS.openDevice;
        // # colors resetings must be resetted
        bool resetColorSettings = deviceTypeChanged;

        // update settings
        i->deviceS = *nDeviceS;
        // i->readFrames = newConfigS.startReading;

        // close device
        if((i->device != nullptr) && closeDevice){
            if(i->device->is_opened()){
                i->device->close();
                i->deviceOpened = false;
            }
        }

        // clean device
        if((i->device != nullptr) && cleanDevice){
            i->device = nullptr;
            i->deviceInitialized = false;
        }

        // create device
        if((i->device == nullptr) && createDevice){

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
            }else if(type == DCType::Gemini215){
                auto lg = LogG("DCDevice::DCDevice Gemini215DeviceImpl"sv);
                i->device = std::make_unique<Geimini215DeviceImpl>();
            }else if(type == DCType::Recording){
                auto lg = LogG("DCDevice::DCDevice RecordingDeviceImpl"sv);
                i->device = std::make_unique<RecordingDeviceImpl>();
            }else{
                return {nullptr,nullptr};
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
        if((i->device != nullptr) && openDevice){

            if(!i->device->is_opened()){

                if(i->device->open(i->deviceS.configS)){

                    i->deviceOpened = true;

                    // set others settings
                    // # data
                    // i->device->set_data_settings(i->deviceS.dataS.server);
                    // # filters
                    i->device->set_filters_settings(i->filtersS);
                    // # misc
                    i->device->set_misc_settings(i->miscS);
                    // # color
                    if(resetColorSettings){
                        i->colorsS.set_default_values(i->deviceS.configS.typeDevice);
                        color_settings_reset_signal(i->colorsS);
                    }
                    i->device->set_color_settings(i->colorsS);
                    i->device->update_from_colors_settings();

                    // update device name
                    update_device_name_signal(i->deviceS.configS.idDevice, std::format("[{}] [{}]", i->deviceS.configS.idDevice, i->device->device_name()));
                }
            }
        }

        // update data
        if((i->device != nullptr)){
            i->device->set_data_settings(i->deviceS.dataS.server);
        }
    }

    if(nColorS.has_value()){
        i->colorsS = *nColorS;
        if(i->device != nullptr){
            if(i->device->is_opened()){
                i->device->set_color_settings(i->colorsS);
                i->device->update_from_colors_settings();
            }
        }
    }

    if(nFiltersS.has_value()){
        i->filtersS = *nFiltersS;
        if(i->device != nullptr){
            if(i->device->is_opened()){
                i->device->set_filters_settings(i->filtersS);
            }
        }
    }

    if(nMisc.has_value()){
        i->miscS = *nMisc;
        if(i->device != nullptr){
            if(i->device->is_opened()){
                i->device->set_misc_settings(i->miscS);
            }
        }
    }


    // process frame
    if(i->device != nullptr){
        if(i->device->is_opened()){
            if(i->deviceS.configS.startReading ){
                return i->device->process_frames();
                // Log::fmessage("[{}]", processed);
                // if(!i->device->process()){
                    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
                // }
                // return true;
            }
        }
    }

    return {nullptr,nullptr};

    // auto tEnd = Time::nanoseconds_since_epoch();
    // auto diff = Time::difference_ms(tStart,tEnd);
    // int targetFPS = 30;
    // int timeToWait = 0;
    // if(diff.count() < targetFPS){
    //     timeToWait = static_cast<int>((targetFPS - diff.count())*0.85);
    //     // if(timeToWait > 5){
    //     //     timeToWait = 5;
    //     // }
    //     std::this_thread::sleep_for(std::chrono::milliseconds(timeToWait));
    // }
    // // Log::fmessage("[{}-{}]", timeToWait, Time::now_difference_micro_s(tEnd));

    // i->procB.add_value(diff.count());
    // i->sleepB.add_value(Time::difference_ms(tEnd,Time::nanoseconds_since_epoch()).count());


    // return processed;
}

auto DCDevice::is_opened() const noexcept -> bool{
    return i->deviceOpened;
}

auto DCDevice::update_device_settings(const DCDeviceSettings &deviceS) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    i->nDeviceS = deviceS;
}

auto DCDevice::update_color_settings(const DCColorSettings &colorS) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    i->nColorsS = colorS;
}

auto DCDevice::update_filters_settings(const DCFiltersSettings &filtersS) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    i->nFiltersS = filtersS;
}

auto DCDevice::update_misc_settings(const DCMiscSettings &miscS) -> void{
    std::unique_lock<std::mutex> lock(i->locker);
    i->nMiscS = miscS;
}

auto DCDevice::get_duration_ms(std::string_view id) noexcept -> double{
    if(i->deviceOpened){
        if(auto duration = i->device->get_duration_micro_s(id); duration.has_value()){
            return duration.value().count()*0.001;
        }
    }
    return 0.0;
}

auto DCDevice::get_duration_micro_s(std::string_view id) noexcept -> int64_t{
    if(i->deviceOpened){
        if(auto duration = i->device->get_duration_micro_s(id); duration.has_value()){
            return duration.value().count();
        }
    }
    return 0;
}

auto DCDevice::get_capture_duration_ms() noexcept -> double{
    return get_duration_ms("CAPTURE_FRAME"sv);
}

auto DCDevice::get_processing_duration_ms() noexcept -> double{
    return get_duration_ms("PROCESSING_DATA"sv);
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

// auto DCDevice::tf() -> tf::Taskflow*{
//     i->device = std::make_unique<AzureKinectDeviceImpl>();
//     DCConfigSettings cs;
//     cs.mode = DCMode::AK_C1280x720_DI640x576_NV12_F30;

//     i->device->open(cs);
//     return i->device->tf();
// }
