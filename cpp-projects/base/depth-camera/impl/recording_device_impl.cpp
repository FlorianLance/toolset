
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

#include "recording_device_impl.hpp"

// local
#include "utility/logger.hpp"

using namespace  std::string_literals;
using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

RecordingDeviceImpl::RecordingDeviceImpl(){
    // playerD  = std::make_unique<DCVideoPlayer>();
    DCVideoPlayerSettings videoPlayerS;
    videoPlayerS.doLoop = true;
    // videoPlayerS.generation;
    playerD.update_settings(videoPlayerS);
}

auto RecordingDeviceImpl::open(const DCConfigSettings &newConfigS) -> bool {

    initialize(newConfigS);

    Logger::error("[RecordingDeviceImpl::open] Not implemented"sv);
    return false;

    // if(!video.load_from_file(path)){
    //     return false;
    // }

    playerD.set_video(video);
    playerD.start_video();

    return true;

}


auto RecordingDeviceImpl::close() -> void {
    playerD.stop_video();
}

auto RecordingDeviceImpl::is_opened() const noexcept -> bool{
    return false;
}

// auto RecordingDeviceImpl::nb_devices() const noexcept -> uint32_t{
//     return 1;
// }

auto RecordingDeviceImpl::device_name() const noexcept -> std::string{
    return "";
}

// auto RecordingDeviceImpl::read_calibration() -> void{
//     fData.binaryCalibration = orbbecD->read_calibration();
// }

// auto RecordingDeviceImpl::capture_frame(int32_t timeoutMs) -> bool{
//     return orbbecD->capture_frame(timeoutMs);
// }

// auto RecordingDeviceImpl::read_color_image(bool enable) -> bool{
//     if(enable){
//         return !(fData.rawColor = orbbecD->read_color_image()).empty();
//     }
//     fData.rawColor = {};
//     return true;
// }

// auto RecordingDeviceImpl::read_depth_image(bool enable) -> bool{
//     if(enable){
//         return !(fData.depth = orbbecD->read_depth_image()).empty();
//     }
//     fData.depth = {};
//     return true;
// }

// auto RecordingDeviceImpl::read_infra_image(bool enable) -> bool{
//     if(enable){
//         return !(fData.infra = orbbecD->read_infra_image()).empty();
//     }
//     fData.infra = {};
//     return true;
// }

// auto RecordingDeviceImpl::read_IMU(bool enable) -> void {
//     if(enable){
//         fData.binaryIMU = orbbecD->read_from_imu();
//     }else{
//         fData.binaryIMU = {};
//     }
// }

// auto RecordingDeviceImpl::read_body_tracking(bool enable) -> void{

//     if(enable){
//         auto bodiesD = orbbecD->read_bodies();
//         fData.bodiesIdMap = std::get<0>(bodiesD);
//         fData.bodies        = std::get<1>(bodiesD);
//     }else{
//         fData.bodiesIdMap = {};
//         fData.bodies        = {};
//     }
// }

// auto RecordingDeviceImpl::resize_color_image_to_depth_size() -> void {

//     if(!fData.color.empty() && !fData.depth.empty()){
//         fData.depthSizedColor = orbbecD->resize_color_image_to_depth_size(mInfos, fData.color, fData.depth);
//     }else{
//         fData.depthSizedColor = {};
//     }
// }

// auto RecordingDeviceImpl::generate_cloud(bool enable) -> void{

//     if(enable && !fData.depth.empty() && fData.validDepthValues > 0){
//         fData.depthCloud = orbbecD->generate_cloud(mInfos, fData.depth);
//     }else{
//         fData.depthCloud = {};
//     }
// }


