

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

#include "dc_color_settings.hpp"

// std
#include <format>


// local
#include "utility/io_data.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;

DCColorSettings::DCColorSettings(){
    sType   = io::SettingsType::Color;
    version = io::Version::v1_0;
}

auto DCColorSettings::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void {

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCColorSettings::init_from_data: Not enought data space for initializing data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::init_from_data(data, offset, sizeData);
    read(autoExposureTime, data, offset, sizeData);
    read(exposureTime, data, offset, sizeData);
    read(brightness, data, offset, sizeData);
    read(contrast, data, offset, sizeData);
    read(saturation, data, offset, sizeData);
    read(sharpness, data, offset, sizeData);
    read(autoWhiteBalance, data, offset, sizeData);
    read(whiteBalance, data, offset, sizeData);
    read(backlightCompensation, data, offset, sizeData);
    read(gain, data, offset, sizeData);
    read(powerlineFrequency, data, offset, sizeData);
    read(hdr, data, offset, sizeData);
}

auto DCColorSettings::write_to_data(std::int8_t * const data, size_t &offset, size_t sizeData) const -> void{

    if(offset + total_data_size() > sizeData){
        tool::Logger::error(std::format("DCColorSettings::write_to_data: Not enought data space for writing to data: [{}] required [{}]\n", sizeData-offset, total_data_size()));
        return;
    }

    BaseSettings::write_to_data(data, offset, sizeData);
    write(autoExposureTime, data, offset, sizeData);
    write(exposureTime, data, offset, sizeData);
    write(brightness, data, offset, sizeData);
    write(contrast, data, offset, sizeData);
    write(saturation, data, offset, sizeData);
    write(sharpness, data, offset, sizeData);
    write(autoWhiteBalance, data, offset, sizeData);
    write(whiteBalance, data, offset, sizeData);
    write(backlightCompensation, data, offset, sizeData);
    write(gain, data, offset, sizeData);
    write(powerlineFrequency, data, offset, sizeData);
    write(hdr, data, offset, sizeData);
}

auto DCColorSettings::total_data_size() const noexcept -> size_t{
    return
        BaseSettings::total_data_size() +
        sizeof(autoExposureTime) +
        sizeof(exposureTime) +
        sizeof(brightness) +
        sizeof(contrast) +
        sizeof(saturation) +
        sizeof(sharpness) +
        sizeof(autoWhiteBalance) +
        sizeof(whiteBalance) +
        sizeof(backlightCompensation) +
        sizeof(gain) +
        sizeof(powerlineFrequency) +
        sizeof(hdr);
}

auto DCColorSettings::set_default_values(DCType type) -> void{
    whiteBalance            = static_cast<std::uint16_t>(default_value(ColorSettingsType::White_balance, type));
    exposureTime            = static_cast<std::uint16_t>(default_value(ColorSettingsType::Exposure, type));
    brightness              = static_cast<std::uint8_t>(default_value(ColorSettingsType::Brightness, type));
    contrast                = static_cast<std::uint8_t>(default_value(ColorSettingsType::Contrast, type));
    saturation              = static_cast<std::uint8_t>(default_value(ColorSettingsType::Saturation, type));
    sharpness               = static_cast<std::uint8_t>(default_value(ColorSettingsType::Sharpness, type));
    gain                    = static_cast<std::uint8_t>(default_value(ColorSettingsType::Gain, type));
    powerlineFrequency      = static_cast<DCPowerlineFrequency>(default_value(ColorSettingsType::Power_line_frequency, type));
    backlightCompensation   = static_cast<bool>(default_value(ColorSettingsType::Backlight_compensation, type));
    autoExposureTime        = static_cast<bool>(default_value(ColorSettingsType::Auto_exposure, type));
    autoWhiteBalance        = static_cast<bool>(default_value(ColorSettingsType::Auto_white_balance, type));
    hdr                     = static_cast<bool>(default_value(ColorSettingsType::HDR, type));
}
