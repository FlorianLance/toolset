

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
#include "data/json_utility.hpp"

using namespace tool::cam;
using namespace tool::data;
using json = nlohmann::json;

auto DCColorSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, io::Settings::convert_to_json());
    // colors
    add_value(json, "white_balance"sv,             whiteBalance);
    add_value(json, "exposure_time"sv,             exposureTime);
    add_value(json, "brightness"sv,                brightness);
    add_value(json, "contrast"sv,                  contrast);
    add_value(json, "saturation"sv,                saturation);
    add_value(json, "sharpness"sv,                 sharpness);
    add_value(json, "gain"sv,                      gain);
    add_value(json, "powerline_frequency"sv,       static_cast<int>(powerlineFrequency));
    add_value(json, "backlight_compensation"sv,    backlightCompensation);
    add_value(json, "auto_exposure_time"sv,        autoExposureTime);
    add_value(json, "auto_white_balance"sv,        autoWhiteBalance);
    add_value(json, "hdr"sv,                       hdr);

    return json;
}

auto DCColorSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    io::Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
    // colors
    read_and_update_value(json, unreadCount, "white_balance"sv, whiteBalance);
    read_and_update_value(json, unreadCount, "exposure_time"sv, exposureTime);
    read_and_update_value(json, unreadCount, "brightness"sv, brightness);
    read_and_update_value(json, unreadCount, "contrast"sv, contrast);
    read_and_update_value(json, unreadCount, "saturation"sv, saturation);
    read_and_update_value(json, unreadCount, "sharpness"sv, sharpness);
    read_and_update_value(json, unreadCount, "gain"sv, gain);
    powerlineFrequency = static_cast<DCPowerlineFrequency>(read_and_return_value(json, unreadCount, "powerline_frequency"sv, static_cast<int>(powerlineFrequency)));
    read_and_update_value(json, unreadCount, "backlight_compensation"sv, backlightCompensation);
    read_and_update_value(json, unreadCount, "auto_exposure_time"sv, autoExposureTime);
    read_and_update_value(json, unreadCount, "auto_white_balance"sv, autoWhiteBalance);
    read_and_update_value(json, unreadCount, "hdr"sv, hdr);

    if(unreadCount != 0){
        tool::Log::warning(std::format("[DCColorSettings::init_from_json] [{}] values have not been initialized from json data.\n"sv, unreadCount));
    }
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

auto DCColorSettings::init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void {
    
    Settings::init_from_data(data, offset, sizeData);
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

    version = io::SettingsVersion::LastVersion;
}
