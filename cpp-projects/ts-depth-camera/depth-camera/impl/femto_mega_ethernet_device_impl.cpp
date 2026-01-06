
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "femto_mega_ethernet_device_impl.hpp"

// local
#include "utility/logger.hpp"

using namespace  std::string_literals;
using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

FemtoMegaEthernetDeviceImpl::FemtoMegaEthernetDeviceImpl(){
    
    auto lg = LogG("FemtoMegaEthernetDeviceImpl::FemtoMegaEthernetDeviceImpl"sv);
    orbbecD  = std::make_unique<FemtoBaseDevice>(DCType::FemtoMegaEthernet);
}

auto FemtoMegaEthernetDeviceImpl::open(const DCConfigSettings &newConfigS) -> bool{
    
    auto lg = LogG("FemtoMegaEthernetDeviceImpl::open"sv);
    initialize(newConfigS);

    if(mInfos.image_format() != DCImageFormat::MJPG){
        Log::warning("Femto Mega must use MJPG image format, format ignored.\n");
        mInfos.force_image_format(DCImageFormat::MJPG);
    }

    if(orbbecD == nullptr){
        orbbecD  = std::make_unique<FemtoBaseDevice>(DCType::FemtoMegaEthernet);
    }

    if(orbbecD->open(mInfos, settings.config, settings.color)){
        fData.binaryCalibration = orbbecD->read_calibration();
        return true;
    }
    
    Log::error("[FemtoMegaEthernetDeviceImpl::open] Cannot open device\n");
    return false;
}

auto FemtoMegaEthernetDeviceImpl::close() -> void {
    auto lg = LogG("FemtoMegaEthernetDeviceImpl::close"sv);
    if(orbbecD != nullptr){
        orbbecD->close();
        orbbecD = nullptr;
    }
}

auto FemtoMegaEthernetDeviceImpl::update_from_colors_settings() -> void{
    if(orbbecD != nullptr){
        orbbecD->update_from_colors_settings(settings.color);
    }
}

auto FemtoMegaEthernetDeviceImpl::is_opened() const noexcept -> bool{
    if(orbbecD != nullptr){
        return orbbecD->is_opened();
    }
    return false;
}

// auto FemtoMegaEthernetDeviceImpl::nb_devices() const noexcept -> uint32_t{
//     if(orbbecD != nullptr){
//         return static_cast<std::uint32_t>(orbbecD->nb_devices());
//     }
//     return 0;
// }

auto FemtoMegaEthernetDeviceImpl::device_name() const noexcept -> std::string{
    if(orbbecD != nullptr){
        return orbbecD->device_name();
    }
    return "";
}

auto FemtoMegaEthernetDeviceImpl::capture_frame(int32_t timeoutMs) -> bool{
    if(orbbecD != nullptr){
        return orbbecD->capture_frame(timeoutMs);
    }
    return false;
}

auto FemtoMegaEthernetDeviceImpl::read_color_image(bool enable) -> bool{
    if(enable){
        return !(fData.rawColor = orbbecD->read_color_image()).empty();
    }
    fData.rawColor = {};
    return true;
}

auto FemtoMegaEthernetDeviceImpl::read_depth_image(bool enable) -> bool{
    if(enable){
        return !(fData.depth = orbbecD->read_depth_image()).empty();
    }
    fData.depth = {};
    return true;
}

auto FemtoMegaEthernetDeviceImpl::read_infra_image(bool enable) -> bool{
    if(enable){
        return !(fData.infra = orbbecD->read_infra_image()).empty();
    }
    fData.infra = {};
    return true;
}

auto FemtoMegaEthernetDeviceImpl::read_IMU(bool enable) -> void {
    if(enable){
        fData.binaryIMU = orbbecD->read_from_imu();
    }else{
        fData.binaryIMU = {};
    }
}

auto FemtoMegaEthernetDeviceImpl::read_body_tracking(bool enable) -> void{

    if(enable){
        auto bodiesD = orbbecD->read_body_tracking();
        fData.bodiesId = std::get<0>(bodiesD);
        fData.bodies        = std::get<1>(bodiesD);
    }else{
        fData.bodiesId = {};
        fData.bodies   = {};
    }
}

auto FemtoMegaEthernetDeviceImpl::resize_images() -> void {

    if(!fData.originalSizeColor.empty() && !fData.depth.empty()){
        fData.depthSizedColor = orbbecD->resize_color_image_to_depth_size(mInfos, fData.originalSizeColor, fData.depth);
    }else{
        fData.depthSizedColor = {};
    }
}

auto FemtoMegaEthernetDeviceImpl::generate_cloud(bool enable) -> void{
    
    if(enable && !fData.depth.empty()){
        fData.depthCloud = orbbecD->generate_cloud(mInfos, fData.depth);
    }else{
        fData.depthCloud = {};
    }
}


