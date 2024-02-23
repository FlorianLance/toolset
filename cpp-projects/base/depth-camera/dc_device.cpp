
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
#include "utility/logger.hpp"

using namespace tool::geo;
using namespace tool::cam;

struct DCDevice::Impl{
    std::unique_ptr<DCDeviceImpl> dd;
};

DCDevice::DCDevice(DCType type): i(std::make_unique<Impl>()){

    auto lg = LogGuard("DCDevice::DCDevice"sv);
    if(type == DCType::AzureKinect){
        i->dd = std::make_unique<AzureKinectDeviceImpl>();
        i->dd->dcDevice = this;
    }else if(type == DCType::FemtoBolt){
        i->dd = std::make_unique<FemtoBoltDeviceImpl>();
        i->dd->dcDevice = this;
    }else if(type == DCType::FemtoMega){
        i->dd = std::make_unique<FemtoMegaDeviceImpl>();
        i->dd->dcDevice = this;
    }
}

DCDevice::~DCDevice(){
    clean();
}

auto DCDevice::open(std::uint32_t deviceId) -> bool{

    auto lg = LogGuard("DCDevice::open"sv);
    if(is_opened()){
        return false;
    }

    // cameras
    auto deviceCount = nb_devices();
    if(deviceCount == 0){
        Logger::error("[DCDevice::open] No device found\n");
        return false;
    }
    Logger::message(std::format("[DCDevice] Devices found: {}\n", deviceCount));
    if(deviceId >= deviceCount){
        Logger::error("[DCDevice::open] Invalid device id\n");
        return false;
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
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
    return i->dd->readFramesFromCameras;
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
    return i->dd->start_reading(configS);
}

auto DCDevice::stop_reading() -> void{

    auto lg = LogGuard("DCDevice::stop_reading"sv);
    if(!is_reading() || !is_opened()){
        return;
    }
    i->dd->stop_reading();
}

auto DCDevice::set_color_settings(const DCColorSettings &colorS) -> void{    
    auto lg = LogGuard("DCDevice::set_color_settings"sv);
    i->dd->set_color_settings(colorS);
}

auto DCDevice::set_data_settings(const DCDataSettings &dataS) -> void{
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

