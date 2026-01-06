

/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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

// base
#include "utility/io_data.hpp"
#include "utility/logger.hpp"
#include "utility/json_utility.hpp"

using namespace tool::cam;
using json = nlohmann::json;

auto DCColorSettings::convert_to_json() const -> nlohmann::json{

    json json;
    // base
    add_value(json, "base"sv, Settings::convert_to_json());
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

auto DCColorSettings::update_auto_calibration(DCType type) -> bool{

    if(autoCalibration){

        // start calibrtation timer
        if(!calibrationW.is_started()){

            Log::message("RESET VALUES\n");

            // build values
            calibrationValues.clear();

            CalibValuesSettings cvS;

            cvS.gainStart          = gain;
            cvS.gainEnd            = gain;
            cvS.gainInc            = step_value(ColorSettingsType::Gain, type);

            cvS.brightnessStart    = brightness;
            cvS.brightnessEnd      = brightness;
            cvS.brightnessInc      = step_value(ColorSettingsType::Brightness, type);

            cvS.whiteBalanceStart  = whiteBalance;
            cvS.whiteBalanceEnd    = whiteBalance;
            cvS.whiteBalanceInc    = step_value(ColorSettingsType::White_balance, type);

            cvS.saturationStart    = saturation;
            cvS.saturationEnd      = saturation;
            cvS.saturationInc      = step_value(ColorSettingsType::Saturation, type);

            cvS.contrastStart    = contrast;
            cvS.contrastEnd      = contrast;
            cvS.contrastInc      = step_value(ColorSettingsType::Contrast, type);

            if(acMode == AutoCalibrationMode::GaBrWb){
                if(type == DCType::AzureKinect){

                    cvS.brightnessStart = 100;
                    cvS.brightnessEnd   = 150;
                    cvS.brightnessInc   = 10;

                    cvS.gainStart = 60;
                    cvS.gainEnd   = 140;
                    cvS.gainInc   = 10;

                    cvS.whiteBalanceStart = 2500;
                    cvS.whiteBalanceEnd   = 4500;
                    cvS.whiteBalanceInc   = 40;

                    add_calib_values(cvS);

                }else if ((type == DCType::FemtoBolt) || (type == DCType::FemtoMegaUSB) || (type == DCType::FemtoMegaEthernet)){

                    cvS.brightnessStart = 40;
                    cvS.brightnessEnd   = 60;
                    cvS.brightnessInc   = 5;

                    cvS.gainStart = 30;
                    cvS.gainEnd   = 90;
                    cvS.gainInc   = 10;

                    cvS.whiteBalanceStart = 2000;
                    cvS.whiteBalanceEnd   = 6000;
                    cvS.whiteBalanceInc   = 100;

                    add_calib_values(cvS);
                }
            }

            switch (acMode) {
            case AutoCalibrationMode::Gain:
                cvS.gainStart           = min_value(ColorSettingsType::Gain, type);
                cvS.gainEnd             = max_value(ColorSettingsType::Gain, type);
                cvS.gainInc             = step_value(ColorSettingsType::Gain, type);
                add_calib_values(cvS);
                break;
            case AutoCalibrationMode::Brightness:
                cvS.brightnessStart     = min_value(ColorSettingsType::Brightness, type);
                cvS.brightnessEnd       = max_value(ColorSettingsType::Brightness, type);
                cvS.brightnessInc       = step_value(ColorSettingsType::Brightness, type);
                add_calib_values(cvS);
                break;
            case AutoCalibrationMode::WhiteBalance:
                cvS.whiteBalanceStart   = min_value(ColorSettingsType::White_balance, type);
                cvS.whiteBalanceEnd     = max_value(ColorSettingsType::White_balance, type);
                if(type == DCType::AzureKinect){
                    cvS.whiteBalanceInc = 5*step_value(ColorSettingsType::White_balance, type);
                }else{
                    cvS.whiteBalanceInc = step_value(ColorSettingsType::White_balance, type);
                }
                add_calib_values(cvS);
                break;
            case AutoCalibrationMode::Saturation:
                cvS.saturationStart   = min_value(ColorSettingsType::Saturation, type);
                cvS.saturationEnd     = max_value(ColorSettingsType::Saturation, type);
                cvS.saturationInc     = step_value(ColorSettingsType::Saturation, type);
                add_calib_values(cvS);
                break;
            case AutoCalibrationMode::Contrast:
                cvS.contrastStart   = min_value(ColorSettingsType::Contrast, type);
                cvS.contrastEnd     = max_value(ColorSettingsType::Contrast, type);
                cvS.contrastInc     = step_value(ColorSettingsType::Contrast, type);
                add_calib_values(cvS);
                break;
            case AutoCalibrationMode::Special:

                cvS.contrastStart   = min_value(ColorSettingsType::Contrast, DCType::FemtoBolt);
                cvS.contrastEnd     = max_value(ColorSettingsType::Contrast, DCType::FemtoBolt);
                cvS.contrastInc     = step_value(ColorSettingsType::Contrast, DCType::FemtoBolt);
                add_calib_values(cvS);
                cvS.contrastStart    = contrast;
                cvS.contrastEnd      = contrast;
                cvS.contrastInc      = step_value(ColorSettingsType::Contrast, type);

                cvS.saturationStart   = min_value(ColorSettingsType::Saturation, DCType::FemtoBolt);
                cvS.saturationEnd     = max_value(ColorSettingsType::Saturation, DCType::FemtoBolt);
                cvS.saturationInc     = step_value(ColorSettingsType::Saturation, DCType::FemtoBolt);
                add_calib_values(cvS);
                cvS.saturationStart    = saturation;
                cvS.saturationEnd      = saturation;
                cvS.saturationInc      = step_value(ColorSettingsType::Saturation, type);

                cvS.whiteBalanceStart   = min_value(ColorSettingsType::White_balance, DCType::FemtoBolt);
                cvS.whiteBalanceEnd     = max_value(ColorSettingsType::White_balance, DCType::FemtoBolt);
                cvS.whiteBalanceInc     = step_value(ColorSettingsType::White_balance, DCType::FemtoBolt);
                add_calib_values(cvS);
                cvS.whiteBalanceStart  = whiteBalance;
                cvS.whiteBalanceEnd    = whiteBalance;
                cvS.whiteBalanceInc    = step_value(ColorSettingsType::White_balance, type);

                cvS.brightnessStart     = min_value(ColorSettingsType::Brightness, DCType::FemtoBolt);
                cvS.brightnessEnd       = max_value(ColorSettingsType::Brightness, DCType::FemtoBolt);
                cvS.brightnessInc       = step_value(ColorSettingsType::Brightness, DCType::FemtoBolt);
                add_calib_values(cvS);
                cvS.brightnessStart    = brightness;
                cvS.brightnessEnd      = brightness;
                cvS.brightnessInc      = step_value(ColorSettingsType::Brightness, type);

                cvS.gainStart           = min_value(ColorSettingsType::Gain, DCType::FemtoBolt);
                cvS.gainEnd             = max_value(ColorSettingsType::Gain, DCType::FemtoBolt);
                cvS.gainInc             = step_value(ColorSettingsType::Gain, DCType::FemtoBolt);
                add_calib_values(cvS);
                cvS.gainStart          = gain;
                cvS.gainEnd            = gain;
                cvS.gainInc            = step_value(ColorSettingsType::Gain, type);

                break;
            default:
                break;
            }

            calibrationScore.resize(calibrationValues.size());
            std::fill(calibrationScore.begin(), calibrationScore.end(), 1000.0);

            calibrationW.start();
            idAutoCalibration = 0;
            calibrationUpdated = false;
            storeScore = false;
            Log::message("START\n");
        }

        // end condition
        if(idAutoCalibration >= calibrationValues.size()){

            Log::message("END\n");
            // compute final score
            for(auto &cv : calibrationScore){
                Log::fmessage(" {} | ", cv);
            }
            Log::message("\n");

            auto minV = std::min_element(calibrationScore.cbegin(), calibrationScore.cend());
            size_t idMinScore = std::distance(calibrationScore.cbegin(), minV);
            Log::fmessage("Best score {} id {}\n", calibrationScore[idMinScore], idMinScore);

            // apply best calibration
            const auto &bestValues = calibrationValues[idMinScore];            
            gain            = std::get<0>(bestValues);
            brightness      = std::get<1>(bestValues);
            contrast        = std::get<2>(bestValues);
            saturation      = std::get<3>(bestValues);
            whiteBalance    = std::get<4>(bestValues);

            autoCalibration = false;
            idAutoCalibration = 0;
            calibrationW.stop();
            calibrationUpdated = false;
            storeScore = false;
            return true;
        }

        // once timer ellapsed, ask to compute score
        if(!calibrationUpdated){
            // Log::message("APPLY\n");
            apply_current_calibration_settings(type, idAutoCalibration);
            calibrationUpdated = true;
            return true;
        }else if(calibrationW.ellapsed_milli_s() > 125 && !storeScore){
            // Log::message("ASK TO STORE\n");
            storeScore = true;
        }

    }else{
        idAutoCalibration = 0;
        calibrationW.stop();
        calibrationUpdated = false;
        storeScore = false;
    }

    return false;
}


auto DCColorSettings::add_calib_values(const CalibValuesSettings &cvS) -> void{
    for(std::int32_t cGain = cvS.gainStart; cGain <= cvS.gainEnd; cGain += cvS.gainInc){
        for(std::int32_t cBrightness = cvS.brightnessStart; cBrightness <= cvS.brightnessEnd; cBrightness += cvS.brightnessInc){
            for(std::int32_t cContrast = cvS.contrastStart; cContrast <= cvS.contrastEnd; cContrast += cvS.contrastInc){
                for(std::int32_t cSaturation = cvS.saturationStart; cSaturation <= cvS.saturationEnd; cSaturation += cvS.saturationInc){
                    for(std::int32_t cWhiteBalance = cvS.whiteBalanceStart; cWhiteBalance <= cvS.whiteBalanceEnd; cWhiteBalance += cvS.whiteBalanceInc){
                        calibrationValues.push_back({cGain,cBrightness,cContrast,cSaturation,cWhiteBalance});
                    }
                }
            }
        }
    }
}


auto DCColorSettings::apply_current_calibration_settings(DCType type, size_t idCalibration) -> void{
    autoExposureTime = false;
    autoWhiteBalance = false;
    gain            = std::get<0>(calibrationValues[idCalibration]);
    brightness      = std::get<1>(calibrationValues[idCalibration]);
    contrast        = std::get<2>(calibrationValues[idCalibration]);
    saturation      = std::get<3>(calibrationValues[idCalibration]);
    whiteBalance    = std::get<4>(calibrationValues[idCalibration]);

    // if set value
    // modify default values
}

auto DCColorSettings::store_score(double score) -> void{

    currentScore = score;
    if(!storeScore){
        return;
    }
    Log::message("STORE SCORE\n");
    calibrationScore[idAutoCalibration] = score;
    Log::fmessage("SCORE STORED {} : {} \n", idAutoCalibration, score);
    storeScore = false;
    calibrationUpdated = false;
    ++idAutoCalibration;
    calibrationW.reset();
    calibrationW.start();
}

auto DCColorSettings::init_from_json(const nlohmann::json &json) -> void{

    size_t unreadCount = 0;
    // base
    Settings::init_from_json(read_and_return_object(json, unreadCount, "base"sv));
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

    version = SettingsVersion::LastVersion;
}
