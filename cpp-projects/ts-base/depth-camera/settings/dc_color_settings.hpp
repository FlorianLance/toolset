
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
#include "utility/settings.hpp"
#include "geometry/point3.hpp"
#include "image/color_utility.hpp"
#include "utility/stop_watch.hpp"
#include "depth-camera/dc_enums.hpp"

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
using CSCalibStart  = std::int32_t;
using CSCalibInc    = std::int32_t;
using CSCalibEnd    = std::int32_t;

using TDCColorSettingsTypes = std::tuple<
    ColorSettingsType,                          CSAvailable,    CSMin,  CSMax,  CSStep, CSDefault>;
using DCColorSettingsTypes = TupleArray<ColorSettingsType::SizeEnum, TDCColorSettingsTypes>;

static constexpr DCColorSettingsTypes foColorSettingsP = {{
    TDCColorSettingsTypes
    {ColorSettingsType::Exposure,               true,           1,      2000,   1,      200},
    {ColorSettingsType::Auto_exposure,          true,           0,      1,      1,      1},
    {ColorSettingsType::White_balance,          true,           2000,   11000,  100,    6500},
    {ColorSettingsType::Auto_white_balance,     true,           0,      1,      1,      1},
    {ColorSettingsType::Brightness,             true,           0,      128,    1,      0},
    {ColorSettingsType::Contrast,               true,           1,      60,     1,      32},
    {ColorSettingsType::Sharpness,              true,           1,      15,     1,      6},
    {ColorSettingsType::Saturation,             true,           1,      80,     1,      54},
    {ColorSettingsType::Gain,                   true,           1,      240,    1,      60},
    {ColorSettingsType::Power_line_frequency,   true,           0,      2,      1,      1},
    {ColorSettingsType::Backlight_compensation, false,          0,      0,      0,      0},
    {ColorSettingsType::HDR,                    true,           0,      1,      1,      1},
}};
static constexpr DCColorSettingsTypes k4ColorSettingsP = {{
    TDCColorSettingsTypes
    {ColorSettingsType::Exposure,               true,           500,    33330, 1,       33330},
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

// TODO: add gemini

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
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMegaEthernet) || (type == DCType::FemtoMegaUSB)){
        return is_available(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto min_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return min_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMegaEthernet) || (type == DCType::FemtoMegaUSB)){
        return min_value(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto max_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return max_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMegaEthernet) || (type == DCType::FemtoMegaUSB)){
        return max_value(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto step_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return step_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMegaEthernet) || (type == DCType::FemtoMegaUSB)){
        return step_value(sType, foColorSettingsP);
    }
    return {};
}

[[maybe_unused]] static constexpr auto default_value(ColorSettingsType sType, DCType type) -> std::int32_t{
    if(type == DCType::AzureKinect){
        return default_value(sType, k4ColorSettingsP);
    }else if((type == DCType::FemtoBolt) || (type == DCType::FemtoMegaEthernet) || (type == DCType::FemtoMegaUSB)){
        return default_value(sType, foColorSettingsP);
    }
    return {};
}

struct DCColorSettings : Settings{

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

    // runtime
    int currentColorCheckedId = -1;
    std::array<std::tuple<geo::Pt2f, img::ColorRGBA8, double>, 24> checkerPositions;
    static constexpr std::array<std::tuple<img::ColorRGB8, std::string_view>, 24> spyderChecker24Colors ={
        std::make_tuple(img::ColorRGB8(249,242,238), std::string_view("cardWhite1E")),
        {{0,127,159},   std::string_view("primaryCyan1F")},
        {{222,118,32},  std::string_view("primaryOrange1G")},
        {{98,187,166},  std::string_view("aqua1H")},
        {{202,198,195}, std::string_view("gray20p2E")},
        {{192,75,145},  std::string_view("primaryMagenta2F")},
        {{58,88,159},   std::string_view("bluePrint2G")},
        {{126,125,174}, std::string_view("lavender2H")},
        {{161,157,154}, std::string_view("gray40p3E")},
        {{245,205,0},   std::string_view("primaryYellow3F")},
        {{195,79,95},   std::string_view("pink3G")},
        {{82,106,60},   std::string_view("evergreen3H")},
        {{122,118,116}, std::string_view("gray60p4E")},
        {{186,26,51},   std::string_view("primaryRed4F")},
        {{83,58,106},   std::string_view("violet4G")},
        {{87,120,155},  std::string_view("steelBlue4H")},
        {{80,80,78},    std::string_view("gray80p5E")},
        {{57,146,64},   std::string_view("primaryGreen5F")},
        {{157,188,54},  std::string_view("appleGreen5G")},
        {{197,145,125}, std::string_view("classicLightSkin5H")},
        {{43,41,43},    std::string_view("cardBlack6E")},
        {{25,55,135},   std::string_view("primaryBlue6F")},
        {{238,158,25},  std::string_view("sunFlower6G")},
        {{112,76,60},   std::string_view("classicDarkSkin6H")}
    };
    bool autoCalibration = false;
    bool displayColors = true;
    size_t idAutoCalibration = 0;
    StopWatch calibrationW;
    std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t,std::int32_t,std::int32_t>> calibrationValues;
    bool calibrationUpdated = false;
    bool storeScore = false;
    std::vector<double> calibrationScore;
    double currentScore = 0.f;

    enum class AutoCalibrationMode : std::int8_t{
        GaBrWb = 0,
        Gain,
        Brightness,
        WhiteBalance,
        Saturation,
        Contrast,
        Special
    };
    struct CalibValuesSettings{
        std::int32_t gainStart;
        std::int32_t gainEnd;
        std::int32_t gainInc;

        std::int32_t brightnessStart;
        std::int32_t brightnessEnd;
        std::int32_t brightnessInc;

        std::int32_t whiteBalanceStart;
        std::int32_t whiteBalanceEnd;
        std::int32_t whiteBalanceInc;

        std::int32_t saturationStart;
        std::int32_t saturationEnd;
        std::int32_t saturationInc;

        std::int32_t contrastStart;
        std::int32_t contrastEnd;
        std::int32_t contrastInc;
    };

    AutoCalibrationMode acMode = AutoCalibrationMode::GaBrWb;

    // bool acGain = true;
    // std::uint8_t acMinGain=0, acMaxGain=0, acIncGain=1;
    // bool acBrightness = true;
    // size_t nbAcParameters = 0;



    DCColorSettings(){
        sType   = SettingsType::Color;
        version = SettingsVersion::LastVersion;
        checkerPositions.fill({{-1.f,-1.f}, {0,0,0}, 0.});
    }
    DCColorSettings(std::span<const std::uint8_t> jsonBinary){
        DCColorSettings::init_from_json_binary(jsonBinary);
        checkerPositions.fill({{-1.f,-1.f}, {0,0,0}, 0.});
    }
    auto set_default_values(DCType type) -> void;
    auto init_from_json(const nlohmann::json &json) -> void override;
    auto convert_to_json() const -> nlohmann::json override;


    auto update_auto_calibration(DCType type) -> bool;    
    auto apply_current_calibration_settings(DCType type, size_t idCalibration) -> void;
    auto store_score(double score) -> void;

    // legacy
    auto init_from_data(std::byte const * const data, size_t &offset, size_t sizeData) -> void override;

private:

    auto add_calib_values(const CalibValuesSettings &cvS) -> void;
};
}
