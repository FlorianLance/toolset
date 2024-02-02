
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

#include "azure_kinect_device_impl.hpp"

// local
#include "utility/benchmark.hpp"

using namespace tool::geo;
using namespace tool::cam;

AzureKinectDeviceImpl::AzureKinectDeviceImpl(){
    azureD = std::make_unique<AzureBaseDevice>();
}

auto AzureKinectDeviceImpl::initialize_device_specific() -> void {
    azureD->initialize(mInfos, settings.config);
}

auto AzureKinectDeviceImpl::update_camera_from_colors_settings() -> void {
    azureD->update_camera_from_colors_settings(settings.color);
}

auto AzureKinectDeviceImpl::open(uint32_t deviceId) -> bool {    
    return azureD->open_device(settings.config.idDevice = deviceId);
}

auto AzureKinectDeviceImpl::start_reading(const DCConfigSettings &newConfigS) -> bool {

    settings.config = newConfigS;
    initialize();

    if(azureD->start_device(settings.config)){
        start_reading_thread();
        return true;
    }

    return false;
}

auto AzureKinectDeviceImpl::stop_reading() -> void{
    stop_reading_thread();
    azureD->stop_device();
}

auto AzureKinectDeviceImpl::close() -> void{    
    azureD->close_device();
}

auto AzureKinectDeviceImpl::is_opened() const noexcept -> bool{
    return azureD->is_opened();
}

auto AzureKinectDeviceImpl::nb_devices() const noexcept -> std::uint32_t {
    return static_cast<std::uint32_t>(azureD->nb_devices());
}

auto AzureKinectDeviceImpl::device_name() const noexcept -> std::string {
    return azureD->device_name();
}

auto AzureKinectDeviceImpl::read_calibration() -> void{
    fData.binaryCalibration = azureD->read_calibration();
}

auto AzureKinectDeviceImpl::capture_frame(std::int32_t timeoutMs) -> bool{
    return azureD->capture_frame(timeoutMs);
}

auto AzureKinectDeviceImpl::read_color_image(bool enable) -> bool {
    
    if(enable){
        fData.rawColor = azureD->read_color_image();
    }else{
        fData.rawColor = {};
    }
    return !fData.rawColor.empty();
}

auto AzureKinectDeviceImpl::read_depth_image(bool enable) -> bool {
    
    if(enable){
        fData.depth = azureD->read_depth_image();
    }else{
        fData.depth = {};
    }
    return !fData.depth.empty();
}

auto AzureKinectDeviceImpl::read_infra_image(bool enable) -> bool {

    if(enable){
        fData.infra = azureD->read_infra_image();
    }else{
        fData.infra = {};
    }
    return !fData.infra.empty();
}

auto AzureKinectDeviceImpl::read_from_microphones(bool enable) -> void{
    if(enable){
        fData.audioChannels = azureD->read_from_microphones();
    }else{
        fData.audioChannels = {0, {}};
    }
}

auto AzureKinectDeviceImpl::read_from_imu(bool enable) -> void {

    if(enable){
        fData.binaryIMU = azureD->read_from_imu();
    }else{
        fData.binaryIMU = {};
    }
}

auto AzureKinectDeviceImpl::read_bodies(bool enable) -> void{

    if(enable){
        auto bodiesD = azureD->read_bodies();
        fData.bodiesIdDepth = std::get<0>(bodiesD);
        fData.bodies        = std::get<1>(bodiesD);
    }else{
        fData.bodiesIdDepth = {};
        fData.bodies        = {};
    }
}

auto AzureKinectDeviceImpl::generate_cloud(bool enable) -> void{

    if(enable && !fData.depth.empty() && fData.validDepthValues > 0){
        fData.depthCloud = azureD->generate_cloud();
    }else{
        fData.depthCloud = {};
    }
}

auto AzureKinectDeviceImpl::resize_color_image_to_depth_size() -> void{
    if(!fData.color.empty() && !fData.depth.empty()){
        fData.dephtSizedColor = azureD->resize_color_image_to_depth_size(mInfos, fData.color);
    }else{
        fData.dephtSizedColor = {};
    }
}

auto AzureKinectDeviceImpl::create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame> {

    tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame]");

    auto dFrame  = std::make_unique<DCFrame>();
    update_infos(dFrame.get());
    update_color(dataS, dFrame.get());
    update_depth_sized_color(dataS, dFrame.get());
    update_depth(dataS, dFrame.get());
    update_infra(dataS, dFrame.get());
    update_cloud(dataS, dFrame.get());
    update_bodies(dataS, dFrame.get());
    update_calibration(dFrame.get());
    update_imu(dataS, dFrame.get());
    update_audio(dataS, dFrame.get());

    tool::Bench::stop();

    return dFrame;
}

auto AzureKinectDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame> {

    tool::Bench::start("[AzureKinectDeviceImpl::compress_frame]");

    auto cFrame = std::make_unique<DCCompressedFrame>();
    update_compressed_frame_infos(cFrame.get());
    update_compressed_frame_color(dataS, filtersS, cFrame.get());
    update_compressed_frame_depth_sized_color(dataS, filtersS, cFrame.get());
    update_compressed_frame_depth(dataS, cFrame.get());
    update_compressed_frame_infra(dataS, cFrame.get());
    update_compressed_frame_cloud(dataS, cFrame.get());
    update_compressed_frame_audio(dataS, cFrame.get());
    update_compressed_frame_imu(dataS, cFrame.get());
    update_compressed_frame_bodies(dataS, filtersS, cFrame.get());
    update_compressed_frame_calibration(cFrame.get());

    tool::Bench::stop();

    return cFrame;
}



