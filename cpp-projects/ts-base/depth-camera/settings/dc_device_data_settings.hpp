
/*******************************************************************************
** Toolset-ts-base                                                            **
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

#pragma once

// local
#include "dc_capture_settings.hpp"
#include "dc_frame_generation_settings.hpp"
#include "dc_data_frame_generation_settings.hpp"

namespace tool::cam {

struct DCDeviceDataSettings{

    DCCaptureSettings capture;
    DCDataFrameGenerationSettings sending;
    DCFrameGenerationSettings generation;    

    [[nodiscard]] constexpr auto capture_cloud() const noexcept -> bool{return capture.depth && (generation.cloud || sending.addCloud);}

    auto apply_remote_profile() -> void{
        // capture
        capture.color                      = true;
        capture.depth                      = true;
        capture.infra                      = false;
        capture.audio                      = false;
        capture.imu                        = false;
        capture.bodyTracking               = false;
        // sending
        sending.originalSizeColorJPEGCQ   = 90;
        sending.depthSizedColorJPEGCQ     = 90;
        sending.addDepth                  = true;
        sending.addDepthSizedColor        = true;
        sending.addOriginalSizeColor      = false;
        sending.addInfra                  = false;
        sending.addBodiesId              = false;
        sending.addCloud                  = false;
        sending.addBodiesSkeleton           = false;
        sending.addAudio                  = false;
        sending.addImu                    = false;
        sending.cloudColorMode            = CloudColorMode::FromDepthSizedColorImage;
        // generation
        generation.cloud                   = false;
        generation.depthSizedColorImage    = false;        
        generation.depthImage              = false;
        generation.infraImage              = false;
        generation.originalSizeColorImage  = false;
        generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
    }

    auto apply_local_profile() -> void{
        // capture
        capture.color                      = true;
        capture.depth                      = true;
        capture.infra                      = false;
        capture.audio                      = false;
        capture.imu                        = false;
        capture.bodyTracking               = false;
        // sending
        sending.originalSizeColorJPEGCQ     = 80;
        sending.depthSizedColorJPEGCQ       = 90;
        sending.addDepth                    = false;
        sending.addDepthSizedColor          = false;
        sending.addOriginalSizeColor        = false;
        sending.addInfra                    = false;
        sending.addBodiesId                = false;
        sending.addCloud                    = false;
        sending.addBodiesSkeleton             = false;
        sending.addAudio                    = false;
        sending.addImu                      = false;
        sending.cloudColorMode              = CloudColorMode::FromDepthSizedColorImage;
        // generation        
        generation.cloud                   = true;
        generation.depthImage              = true;
        generation.depthSizedColorImage    = true;
        generation.infraImage              = false;
        generation.originalSizeColorImage  = false;
        generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
    }
};
}
