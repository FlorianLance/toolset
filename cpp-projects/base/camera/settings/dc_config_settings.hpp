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
#include "camera/dc_enums.hpp"
#include "io/binary_settings.hpp"

namespace tool::camera {

struct DCConfigSettings : io::BinaryFileSettings{
    
    DCType typeDevice = DCType::AzureKinect;
    std::uint32_t idDevice = 0;
    DCMode mode = DCMode::K4_CLOUD_C1280x720_DI640x576_MJPG_F30;
    bool synchronizeColorAndDepth = true;
    int delayBetweenColorAndDepthUsec = 0;
    DCSynchronisationMode synchMode = DCSynchronisationMode::Standalone;
    int subordinateDelayUsec = 0;
    bool disableLED = false;

    // body tracking
    // TODO: capture bodies
    DCBTSensorOrientation btOrientation = DCBTSensorOrientation::k4_default_orientation;
    DCBTProcessingMode btProcessingMode = DCBTProcessingMode::K4_GPU_DIRECTML;
    std::int8_t btGPUId = 0;

    static auto default_init_for_grabber() -> DCConfigSettings;
    static auto default_init_for_manager() -> DCConfigSettings;


    DCConfigSettings();
    DCConfigSettings(std::int8_t const * const data, size_t &offset, size_t sizeData){
        DCConfigSettings::init_from_data(data, offset, sizeData);
    }

    // i/o
    auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void override;
    auto write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void override;
    auto total_data_size() const noexcept-> size_t override;
};



}
