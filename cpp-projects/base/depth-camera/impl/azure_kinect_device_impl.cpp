
/*******************************************************************************
** Toolset-base                                                               **
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

#include "azure_kinect_device_impl.hpp"

// utility
#include "utility/logger.hpp"

using namespace tool::geo;
using namespace tool::cam;

AzureKinectDeviceImpl::AzureKinectDeviceImpl(){
    auto lg = LogG("AzureKinectDeviceImpl::AzureKinectDeviceImpl"sv);
    azureD = std::make_unique<AzureBaseDevice>();
}

auto AzureKinectDeviceImpl::open(const DCConfigSettings &newConfigS) -> bool {
    
    auto lg = LogG("AzureKinectDeviceImpl::open"sv);
    initialize(newConfigS);

    if(azureD->open(mInfos, settings.config)){
        fData.binaryCalibration = azureD->read_calibration();
        return true;
    }
    Log::error("[AzureKinectDeviceImpl::open] Cannot open device\n");
    return false;
}

auto AzureKinectDeviceImpl::close() -> void{
    auto lg = LogG("AzureKinectDeviceImpl::close"sv);
    azureD->close();
}

auto AzureKinectDeviceImpl::update_from_colors_settings() -> void {
    azureD->update_from_colors_settings(settings.color);
}

auto AzureKinectDeviceImpl::is_opened() const noexcept -> bool{
    return azureD->is_opened();
}

// auto AzureKinectDeviceImpl::nb_devices() const noexcept -> std::uint32_t {
//     return static_cast<std::uint32_t>(azureD->nb_devices());
// }

auto AzureKinectDeviceImpl::device_name() const noexcept -> std::string {
    return azureD->device_name();
}

auto AzureKinectDeviceImpl::capture_frame(std::int32_t timeoutMs) -> bool{
    return azureD->capture_frame(timeoutMs);
}

auto AzureKinectDeviceImpl::read_color_image(bool enable) -> bool {
    if(enable){
        return !(fData.rawColor = azureD->read_color_image()).empty();
    }
    fData.rawColor = {};
    return true;
}

auto AzureKinectDeviceImpl::read_depth_image(bool enable) -> bool {
    if(enable){
        return !(fData.depth = azureD->read_depth_image()).empty();
    }
    fData.depth = {};
    return true;
}

auto AzureKinectDeviceImpl::read_infra_image(bool enable) -> bool {
    if(enable){
        return !(fData.infra = azureD->read_infra_image()).empty();
    }
    fData.infra = {};
    return true;
}

auto AzureKinectDeviceImpl::read_audio(bool enable) -> void{
    if(enable){
        fData.audioChannels = azureD->read_from_microphones();
    }else{
        fData.audioChannels = {0, {}};
    }
}

auto AzureKinectDeviceImpl::read_IMU(bool enable) -> void {

    if(enable){
        fData.binaryIMU = azureD->read_from_imu();
    }else{
        fData.binaryIMU = {};
    }
}

auto AzureKinectDeviceImpl::read_body_tracking(bool enable) -> void{

    if(enable){
        auto bodiesD    = azureD->read_body_tracking();
        fData.bodiesId  = std::get<0>(bodiesD);
        fData.bodies    = std::get<1>(bodiesD);
    }else{
        fData.bodiesId  = {};
        fData.bodies    = {};
    }
}

auto AzureKinectDeviceImpl::generate_cloud(bool enable) -> void{

    if(enable && !fData.depth.empty()){
        fData.depthCloud = azureD->generate_cloud();
    }else{
        fData.depthCloud = {};
    }
}

auto AzureKinectDeviceImpl::resize_color_image_to_depth_size() -> void{
    if(!fData.originalSizeColor.empty() && !fData.depth.empty()){
        fData.depthSizedColor = azureD->resize_color_image_to_depth_size(mInfos, fData.originalSizeColor);
    }else{
        fData.depthSizedColor = {};
    }
}


