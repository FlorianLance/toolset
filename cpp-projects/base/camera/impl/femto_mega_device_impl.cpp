
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

#include "femto_mega_device_impl.hpp"


// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"

using namespace  std::string_literals;
using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

FemtoMegaDeviceImpl::FemtoMegaDeviceImpl(){
    orbbecD  = std::make_unique<OrbbecBaseDevice>();
    orbbecD->query_devices("Femto Mega"sv, true);
}

auto FemtoMegaDeviceImpl::initialize_device_specific() -> void{

    if(mInfos.image_format() != DCImageFormat::MJPG){
        Logger::warning("Femto Mega must use MJPG image format, format ignored.\n");
        mInfos.force_image_format(DCImageFormat::MJPG);
    }
    orbbecD->initialize(mInfos);
}

auto FemtoMegaDeviceImpl::update_camera_from_colors_settings() -> void{
    orbbecD->update_camera_from_colors_settings(readFramesFromCameras, settings.color);
}

auto FemtoMegaDeviceImpl::open(uint32_t deviceId) -> bool{
    return orbbecD->open_device(settings.config.idDevice = deviceId);
}

auto FemtoMegaDeviceImpl::start_reading(const DCConfigSettings &newConfigS) -> bool{

    settings.config = newConfigS;
    initialize();

    if(orbbecD->start_pipeline(mInfos, settings.config)){
        start_reading_thread();
        return true;
    }

    return false;
}

auto FemtoMegaDeviceImpl::stop_reading() -> void{
    stop_reading_thread();
}

auto FemtoMegaDeviceImpl::close() -> void {
    return orbbecD->close_device();
}

auto FemtoMegaDeviceImpl::is_opened() const noexcept -> bool{
    return orbbecD->is_opened();
}

auto FemtoMegaDeviceImpl::nb_devices() const noexcept -> uint32_t{
    return static_cast<std::uint32_t>(orbbecD->nb_devices());
}

auto FemtoMegaDeviceImpl::device_name() const noexcept -> std::string{
    return orbbecD->device_name();
}

auto FemtoMegaDeviceImpl::read_calibration() -> void{
    fData.binaryCalibration = orbbecD->read_calibration();
}

auto FemtoMegaDeviceImpl::capture_frame(int32_t timeoutMs) -> bool{
    return orbbecD->capture_frame(timeoutMs);
}

auto FemtoMegaDeviceImpl::read_color_image(bool enable) -> bool{

    if(enable){
        fData.rawColor = orbbecD->read_color_image();
    }else{
        fData.rawColor = {};
    }
    return !fData.rawColor.empty();
}

auto FemtoMegaDeviceImpl::read_depth_image(bool enable) -> bool{

    if(enable){
        fData.depth = orbbecD->read_depth_image();
    }else{
        fData.depth = {};
    }
    return !fData.depth.empty();
}

auto FemtoMegaDeviceImpl::read_infra_image(bool enable) -> bool{

    if(enable){
        fData.infra = orbbecD->read_infra_image();
    }else{
        fData.infra = {};
    }
    return !fData.infra.empty();
}

auto FemtoMegaDeviceImpl::read_from_imu(bool enable) -> void {

    if(enable){
        fData.binaryIMU = orbbecD->read_from_imu();
    }else{
        fData.binaryIMU = {};
    }
}

auto FemtoMegaDeviceImpl::read_bodies(bool enable) -> void{

    if(enable){
        auto bodiesD = orbbecD->read_bodies();
        fData.bodiesIdDepth = std::get<0>(bodiesD);
        fData.bodies        = std::get<1>(bodiesD);
    }else{
        fData.bodiesIdDepth = {};
        fData.bodies        = {};
    }
}

auto FemtoMegaDeviceImpl::resize_color_image_to_depth_size() -> void {

    if(!fData.color.empty() && !fData.depth.empty()){
        fData.dephtSizedColor = orbbecD->resize_color_image_to_depth_size(mInfos, fData.color, fData.depth);
    }else{
        fData.dephtSizedColor = {};
    }
}

auto FemtoMegaDeviceImpl::generate_cloud(bool enable) -> void{
    
    if(enable && !fData.depth.empty() && fData.validDepthValues > 0){
        fData.depthCloud = orbbecD->generate_cloud(mInfos, fData.depth);
    }else{
        fData.depthCloud = {};
    }
}

auto FemtoMegaDeviceImpl::create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>{

    tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame]");

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

    tool::Bench::stop();

    return dFrame;
}

auto FemtoMegaDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{

    tool::Bench::start("[FemtoMegaDeviceImpl::compress_frame]");

    auto cFrame = std::make_unique<DCCompressedFrame>();

    update_compressed_frame_infos(cFrame.get());
    update_compressed_frame_color(dataS, filtersS, cFrame.get());
    update_compressed_frame_depth_sized_color(dataS, filtersS, cFrame.get());
    update_compressed_frame_depth(dataS, cFrame.get());
    update_compressed_frame_infra(dataS, cFrame.get());
    update_compressed_frame_cloud(dataS, cFrame.get());
    update_compressed_frame_imu(dataS, cFrame.get());
    update_compressed_frame_bodies(dataS, filtersS, cFrame.get());
    update_compressed_frame_calibration(cFrame.get());

    tool::Bench::stop();

    return cFrame;
}
