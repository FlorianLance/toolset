


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

#include "gemini_215_device_impl.hpp"

// utility
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

Geimini215DeviceImpl::Geimini215DeviceImpl(){
    auto lg = LogG("Geimini215DeviceImpl::Geimini215DeviceImpl"sv);
    orbbecD  = std::make_unique<FemtoBaseDevice>(DCType::Gemini215);
}

auto Geimini215DeviceImpl::open(const DCConfigSettings &newConfigS) -> bool{

    auto lg = LogG("Geimini215DeviceImpl::open"sv);
    initialize(newConfigS);

    if(orbbecD->open(mInfos, settings.config, settings.color)){
        fData.binaryCalibration = orbbecD->read_calibration();
        return true;
    }
    Log::error("[Geimini215DeviceImpl::open] Cannot open device\n");
    return false;
}

auto Geimini215DeviceImpl::close() -> void{
    auto lg = LogG("Geimini215DeviceImpl::close"sv);
    orbbecD->close();
}

auto Geimini215DeviceImpl::update_from_colors_settings() -> void{
    orbbecD->update_from_colors_settings(settings.color);
}

auto Geimini215DeviceImpl::is_opened() const noexcept -> bool{
    return orbbecD->is_opened();
}

auto Geimini215DeviceImpl::device_name() const noexcept -> std::string{
    return orbbecD->device_name();
}

auto Geimini215DeviceImpl::capture_frame(int32_t timeoutMs) -> bool{
    return orbbecD->capture_frame(timeoutMs);
}

auto Geimini215DeviceImpl::read_color_image(bool enable) -> bool{
    if(enable){
        return !(fData.rawColor = orbbecD->read_color_image()).empty();
    }
    fData.rawColor = {};
    return true;
}

auto Geimini215DeviceImpl::read_depth_image(bool enable) -> bool{
    if(enable){
        return !(fData.depth = orbbecD->read_depth_image()).empty();
    }
    fData.depth = {};
    return true;
}

auto Geimini215DeviceImpl::read_infra_image(bool enable) -> bool{
    if(enable){
        return !(fData.infra = orbbecD->read_infra_image()).empty();
    }
    fData.infra = {};
    return true;
}


auto Geimini215DeviceImpl::resize_images() -> void{

    // if(!fData.originalSizeColor.empty() && !fData.depth.empty()){
    //     fData.depthSizedColor = orbbecD->resize_color_image_to_depth_size(mInfos, fData.originalSizeColor, fData.depth);
    // }else{
    //     fData.depthSizedColor = {};
    // }
}

auto Geimini215DeviceImpl::generate_cloud(bool enable) -> void{

    // if(enable && !fData.depth.empty()){
    //     fData.depthCloud = orbbecD->generate_cloud(mInfos, fData.depth);
    // }else{
    //     fData.depthCloud = {};
    // }
}

