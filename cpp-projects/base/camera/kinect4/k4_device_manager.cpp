
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

#include "k4_device_manager.hpp"

// local
// # utility
#include "utility/format.hpp"
//#include "utility/thread.hpp"
// # camera
#include "camera/kinect4/k4_device.hpp"

using namespace tool::camera;
using namespace std::chrono;

struct K4DeviceManager::Impl{

    // device
    std::unique_ptr<K4Device> kinect = nullptr;

//    // frames
//    SpinLock compressedFrameL;
//    std::shared_ptr<K4CompressedFrame> lastCompressedFrame = nullptr;
};

K4DeviceManager::K4DeviceManager(): i(std::make_unique<Impl>()){
}

K4DeviceManager::~K4DeviceManager(){
    clean();
}

auto K4DeviceManager::initialize() -> void {

    i->kinect = std::make_unique<K4Device>();

    init_connections();

    for(size_t ii = 0; ii < i->kinect->nb_devices(); ++ii){
        auto t = std::format("Cam {}", ii);
        update_device_name_signal(static_cast<int>(ii), t);
    }
}

auto K4DeviceManager::clean() -> void {
    i->kinect->clean();
}

auto K4DeviceManager::update_delay(K4Delay delayMs) -> void{
    i->kinect->set_delay(delayMs);
}

auto K4DeviceManager::update_filters(const K4Filters &filters) -> void {
    i->kinect->set_filters(filters);
}

auto K4DeviceManager::update_color_settings(const K4ColorSettings &colorS) -> void {
    i->kinect->set_color_settings(colorS);
}

auto K4DeviceManager::get_capture_duration_ms() -> int64_t{
    return i->kinect->get_capture_duration_ms();
}
auto K4DeviceManager::get_nb_capture_per_second() -> float{
    return i->kinect->get_nb_capture_per_second();
}

auto K4DeviceManager::get_processing_duration_ms() -> int64_t{
    return i->kinect->get_processing_duration_ms();
}

auto K4DeviceManager::get_compressing_duration_ms() -> int64_t{
    return i->kinect->get_compressing_duration_ms();
}

auto K4DeviceManager::get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> int64_t{
    return i->kinect->get_duration_between_ms(from, to);
}

auto K4DeviceManager::get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> int64_t{
    return i->kinect->get_duration_between_micro_s(from, to);
}

auto K4DeviceManager::update_device_list() -> void{

    i->kinect->refresh_devices_list();

    if(i->kinect->nb_devices() > 0){
        for(size_t ii = 0; ii < i->kinect->nb_devices(); ++ii){
            update_device_name_signal(static_cast<int>(ii), std::format("Id:{} Num:...", ii));
        }
    }else{
        update_device_name_signal(-1, "");
    }
}

auto K4DeviceManager::update_settings(const K4DeviceSettings &settings) -> void{

    i->kinect->set_data_settings(settings.dataS);

    bool stopDevice =
        ((i->kinect->is_opened() ? (i->kinect->device_id() != settings.configS.idDevice) : false) || !settings.actionsS.startDevice);

    bool stopCamera =
        stopDevice ||
        !settings.actionsS.openCamera ||
        (settings.configS.mode != i->kinect->mode()) ||
        (settings.configS.disableLED != i->kinect->is_LED_disabled()) ||
        (settings.configS.synchronizeColorAndDepth != i->kinect->color_and_depth_synchronized()) ||
        (settings.configS.delayBetweenColorAndDepthUsec != i->kinect->delay_between_color_and_depth_usec()) ||
        (settings.configS.subordinateDelayUsec != i->kinect->subordinate_delay_usec()) ||
        (settings.configS.synchMode != i->kinect->synch_mode());

    // stop / close
    if(i->kinect->is_opened()){
        if(stopCamera){
            i->kinect->stop_cameras();
        }
        if(stopDevice){
            i->kinect->close();
        }
    }

    if(stopDevice || stopCamera){
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // open device
    if(settings.actionsS.startDevice){
        if(i->kinect->open(settings.configS.idDevice)){
            update_device_name_signal(settings.configS.idDevice, fmt("Id:{} Num:{}",  settings.configS.idDevice, i->kinect->device_name()));
        }
    }

    // start camera
    if(settings.actionsS.openCamera){
        i->kinect->start_cameras(settings.configS);
    }
}

auto K4DeviceManager::init_connections() -> void {

    i->kinect->new_frame_signal.connect([&](std::shared_ptr<K4Frame> frame){
        new_frame_signal(frame);
    });

    i->kinect->new_compressed_frame_signal.connect([&](std::shared_ptr<K4CompressedFrame> frame){
        new_compressed_frame_signal(frame);
//        std::lock_guard l(i->compressedFrameL);
//        i->lastCompressedFrame = frame;
    });

    i->kinect->new_imu_sample_signal.connect([&](K4ImuSample sample){
        new_imu_sample_signal(sample);
    });
}

//auto K4DeviceManager::get_last_compressed_frame() -> std::shared_ptr<K4CompressedFrame>{

//    if(!i->compressedFrameL.try_lock()){
//        return nullptr;
//    }

//    std::shared_ptr<K4CompressedFrame> frame = nullptr;
//    std::swap(frame, i->lastCompressedFrame);
//    i->compressedFrameL.unlock();

//    return frame;
//}
