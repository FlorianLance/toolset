
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

#pragma once

// local
#include "dc_capture_settings.hpp"
#include "dc_frame_generation_settings.hpp"
#include "dc_frame_compression_settings.hpp"

namespace tool::cam {

struct DCDeviceDataSettings{
    DCCaptureSettings capture;
    DCFrameGenerationSettings generation;
    DCFrameCompressionSettings compression;
    [[nodiscard]] constexpr auto capture_cloud() const noexcept -> bool{return capture.depth && (generation.cloud || compression.cloud);}

    auto apply_remote_profile() -> void{
        // capture
        capture.color                      = true;
        capture.depth                      = true;
        capture.infra                      = false;
        capture.audio                      = false;
        capture.imu                        = false;
        capture.bodyTracking               = false;
        // generation
        generation.calibration             = false;
        generation.depth                   = false;
        generation.depthSizedColorImage    = false;
        generation.cloud                   = false;
        generation.depthImage              = false;
        generation.infra                   = false;
        generation.infraImage              = false;
        generation.colorImage              = false;
        generation.bodyIdMapImage          = false;
        generation.bodyTracking            = false;
        generation.imu                     = false;
        generation.audio                   = false;
        generation.cloudGenMode            = CloudGenerationMode::FromDepth;
        generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
        // compression
        compression.jpegCompressionRate    = 80;
        compression.calibration            = true;
        compression.depth                  = true;
        compression.depthSizedColor        = true;
        compression.color                  = false;
        compression.infra                  = false;
        compression.bodyIdMap              = false;
        compression.cloud                  = false;
        compression.bodyTracking           = false;
        compression.audio                  = false;
        compression.imu                    = false;
        compression.cloudColorMode         = CloudColorMode::FromDepthSizedColorImage;
    }

    auto apply_local_profile() -> void{
        // capture
        capture.color                      = true;
        capture.depth                      = true;
        capture.infra                      = false;
        capture.audio                      = false;
        capture.imu                        = false;
        capture.bodyTracking               = false;
        // generation
        generation.calibration             = true;
        generation.depth                   = true;
        generation.depthSizedColorImage    = true;
        generation.cloud                   = true;
        generation.depthImage              = true;
        generation.infra                   = false;
        generation.infraImage              = false;
        generation.colorImage              = false;
        generation.bodyIdMapImage          = false;
        generation.bodyTracking            = false;
        generation.imu                     = false;
        generation.audio                   = false;
        generation.cloudGenMode            = CloudGenerationMode::FromDepth;
        generation.cloudColorMode          = CloudColorMode::FromDepthSizedColorImage;
        // compression
        compression.jpegCompressionRate    = 80;
        compression.calibration            = false;
        compression.depth                  = false;
        compression.depthSizedColor        = false;
        compression.color                  = false;
        compression.infra                  = false;
        compression.bodyIdMap              = false;
        compression.cloud                  = false;
        compression.bodyTracking           = false;
        compression.audio                  = false;
        compression.imu                    = false;
        compression.cloudColorMode         = CloudColorMode::FromDepthSizedColorImage;
    }
};
}
