
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
#include "depth-camera/dc_enums.hpp"
#include "io/settings.hpp"

namespace tool::cam {

enum class ColorSettingsType : std::uint8_t{
    Exposure,
    Auto_exposure,
    White_balance,
    Auto_white_balance,
    Brightness,
    Contrast,
    Sharpness,
    Saturation,
    Gain,
    Power_line_frequency,
    Backlight_compensation,
    HDR,
    SizeEnum
};

using CSAvailable   = bool;
using CSMin         = std::int32_t;
using CSMax         = std::int32_t;
using CSStep        = std::int32_t;
using CSDefault     = std::int32_t;

using TDCColorSettingsTypes = std::tuple<
    ColorSettingsType,                          CSAvailable,    CSMin,  CSMax,  CSStep, CSDefault>;
using DCColorSettingsTypes = TupleArray<ColorSettingsType::SizeEnum, TDCColorSettingsTypes>;

static constexpr DCColorSettingsTypes foColorSettingsP = {{
    TDCColorSettingsTypes
    {ColorSettingsType::Exposure,               true,           1,      300,    1,      200},
    {ColorSettingsType::Auto_exposure,          true,           0,      1,      1,      1},
    {ColorSettingsType::White_balance,          true,           2000,   11000,  100,    6500},
    {ColorSettingsType::Auto_white_balance,     true,           0,      1,      1,      1},
    {ColorSettingsType::Brightness,             true,           1,      20,     1,      10},
    {ColorSettingsType::Contrast,               true,           1,      99,     1,      50},
    {ColorSettingsType::Sharpness,              true,           1,      40,     1,      24},
    {ColorSettingsType::Saturation,             true,           1,      255,    1,      64},
    {ColorSettingsType::Gain,                   true,           0,      80,     1,      0},
    {ColorSettingsType::Power_line_frequency,   true,           0,      2,      1,      2},
    {ColorSettingsType::Backlight_compensation, false,          0,      0,      0,      0},
    {ColorSettingsType::HDR,                    true,           0,      1,      1,      1},
}};
static constexpr DCColorSettingsTypes k4ColorSettingsP = {{
    TDCColorSettingsTypes
    {ColorSettingsType::Exposure,               true,           500,    133330, 1,      33330},
    {ColorSettingsType::Auto_exposure,          true,           0,      1,      1,      1},
    {ColorSettingsType::White_balance,          true,           2500,   12500,  10,     4500},
    {ColorSettingsType::Auto_white_balance,     true,           0,      1,      1,      1},
    {ColorSettingsType::Brightness,             true,           0,      255,    1,      128},
    {ColorSettingsType::Contrast,               true,           0,      10,     1,      5},
    {ColorSettingsType::Sharpness,              true,           0,      4,      1,      2},
    {ColorSettingsType::Saturation,             true,           0,      63,     1,      32},
    {ColorSettingsType::Gain,                   true,           0,      255,    1,      128},
    {ColorSettingsType::Power_line_frequency,   true,           1,      2,      1,      2},
    {ColorSettingsType::Backlight_compensation, true,           0,      1,      1,      0},
    {ColorSettingsType::HDR,                    false,          0,      0,      0,      0},
}};   

[[maybe_unused]] static constexpr auto type(const TDCColorSettingsTypes &tuple) -> ColorSettingsType{
    return std::get<0>(tuple);
}
[[maybe_unused]] static constexpr auto is_available(const TDCColorSettingsTypes &tuple) -> bool{
    return std::get<1>(tuple);
}
[[maybe_unused]] static constexpr auto min_value(const TDCColorSettingsTypes &tuple) -> std::int32_t{
    return std::get<2>(tuple);
}
[[maybe_unused]] static constexpr auto max_value(const TDCColorSettingsTypes &tuple) -> std::int32_t{
    return std::get<3>(tuple);
}
[[maybe_unused]] static constexpr auto step_value(const TDCColorSettingsTypes &tuple) -> std::int32_t{
    return std::get<4>(tuple);
}
[[maybe_unused]] static constexpr auto default_value(const TDCColorSettingsTypes &tuple) -> std::int32_t{
    return std::get<5>(tuple);
}

[[maybe_unused]] static constexpr auto is_available(ColorSettingsType sType, const DCColorSettingsTypes &types) -> bool{
    return types.at<0,1>(sType);
}
[[maybe_unused]] static constexpr auto min_value(ColorSettingsType sType, const DCColorSettingsTypes &types) -> std::int32_t{
    return types.at<0,2>(sType);
}
[[maybe_unused]] static constexpr auto max_value(ColorSettingsType sType, const DCColorSettingsTypes &types) -> std::int32_t{
    return types.at<0,3>(sType);
}
[[maybe_unused]] static constexpr auto step_value(ColorSettingsType sType, const DCColorSettingsTypes &types) -> std::int32_t{
    return types.at<0,4>(sType);
}
[[maybe_unused]] static constexpr auto default_value(ColorSettingsType sType, const DCColorSettingsTypes &types) -> std::int32_t{
    return types.at<0,5>(sType);
}

[[maybe_unused]] static constexpr auto is_available(ColorSettingsType sType, DCType type) -> bool{
    if(type == DCType::AzureKinect){
        return is_available(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMega)){
        return is_available(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto min_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return min_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMega)){
        return min_value(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto max_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return max_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMega)){
        return max_value(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto step_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return step_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMega)){
        return step_value(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto default_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return default_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMega)){
        return default_value(sType, foColorSettingsP);
    }
    return {};
}

struct DCColorSettings : io::Settings{

    std::uint16_t whiteBalance              = 4500;
    std::uint16_t exposureTime              = 0;

    std::uint8_t brightness                 = 128;
    std::uint8_t contrast                   = 5;    
    std::uint8_t saturation                 = 32;
    std::uint8_t sharpness                  = 4;

    std::uint8_t gain                       = 128;
    DCPowerlineFrequency powerlineFrequency = DCPowerlineFrequency::F50;
    bool backlightCompensation              = false;
    bool autoExposureTime                   = true;

    bool autoWhiteBalance                   = true;
    bool hdr                                = true;

    DCColorSettings(){
        sType   = io::SettingsType::Color;
        version = io::SettingsVersion::LastVersion;
    }
    DCColorSettings(std::span<const std::uint8_t> jsonBinary){
        DCColorSettings::init_from_json_binary(jsonBinary);
    }
    auto set_default_values(DCType type) -> void;
    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;


    // legacy
    auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void override;
};
}
