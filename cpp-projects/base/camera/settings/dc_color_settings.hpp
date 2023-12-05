
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
#include "files/binary_settings.hpp"

namespace tool::camera {

struct DCColorSettings : files::BinaryFileSettings{

    std::uint16_t whiteBalance = 4500; // 2500 - 12500
    std::uint8_t brightness = 128;          // 0 - 255
    std::uint8_t contrast = 5;              // 0 - 10
    std::uint8_t saturation = 32;           // 0 - 63
    std::uint8_t sharpness = 4;             // 0 - 4
    std::uint8_t gain = 128; // 0 - 255
    std::int8_t exposureTimeAbsolute = 5;   // 0 - 5
    PowerlineFrequency powerlineFrequency = PowerlineFrequency::F60;

    bool backlightCompensation = false;
    bool autoExposureTime = true;
    bool autoWhiteBalance = true;

    DCColorSettings() = default;
    DCColorSettings(std::int8_t *data){DCColorSettings::init_from_data(data);}

    // i/o
    auto init_from_data(std::int8_t *data) -> void override;
    auto convert_to_data(std::int8_t *data) const -> void override;
    auto total_data_size() const noexcept -> size_t override{
        return
            sizeof(std::uint16_t)*1+
            sizeof(std::uint8_t)*5+
            sizeof(std::int8_t)*2+
            sizeof(bool)*3;
    }
    auto type() const noexcept  -> std::int32_t override {return static_cast<std::int32_t>(tool::files::FileSettingsType::Color);};
};
}
