
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
#include "files/binary_settings.hpp"

namespace tool::camera {

struct K4DataSettings : files::SubBinarySettings{

    // capture
    bool captureAudio             = true;
    bool captureIMU               = true;
    bool captureBodies            = false;

    // send
    bool sendColor                = true;
    bool sendDepth                = false;
    bool sendInfra                = false;
    bool sendCloud                = true;
    bool sendIMU                  = true;
    bool sendAudio                = true;
    bool sendBodies               = false;

    // display locally on grabber
    bool generateRGBLocalFrame    = false;
    bool generateDepthLocalFrame  = false;
    bool generateInfraLocalFrame  = false;
    bool generateCloudLocal       = false;

    // body tracking
    float btTemporalSmoothing       = 0.f;

    static auto default_init_for_grabber() -> K4DataSettings;
    static auto default_init_for_manager() -> K4DataSettings;

    // i/o
    auto init_from_data(std::int8_t *data) -> void override;
    auto convert_to_data(std::int8_t *data) const -> void override;
    auto total_data_size() const noexcept-> size_t override{
        return
            sizeof(captureAudio) +
            sizeof(captureIMU) +
            sizeof(captureBodies) +
            sizeof(generateRGBLocalFrame) +
            sizeof(generateDepthLocalFrame) +
            sizeof(generateInfraLocalFrame) +
            sizeof(generateCloudLocal) +
            sizeof(sendColor) +
            sizeof(sendDepth) +
            sizeof(sendInfra) +
            sizeof(sendCloud) +
            sizeof(sendIMU) +
            sizeof(sendAudio) +
            sizeof(sendBodies)+
            sizeof(btTemporalSmoothing);
    }
};
}
