
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

#include "dc_settings_paths.hpp"

// local
#include "utility/logger.hpp"

using namespace tool;

auto DCSettingsPaths::initialize(const std::filesystem::path &base_, const std::string &hostName_) -> void{

    base                            = base_;
    hostName                        = hostName_;
    configPath                      = base       / "config/";
    settingsPath                    = configPath / "settings/";
    networkPath                     = configPath / "network/";
    calibrationPath                 = configPath / "calibration/";

    outputPath                      = base       / "output/";

    defaultNetwork                  = networkPath / "network_default.config";
    hostNetwork                     = networkPath / std::format("network_{}.config", hostName);

    defaultFilters                  = settingsPath / "filters_default.config";
    allGrabbersFilters              = settingsPath / std::format("filters_{}_all.config", hostName);

    defaultCalibrationFilters       = settingsPath / "calibration_filters_default.config";
    allGrabbersCalibrationFilters   = settingsPath / std::format("calibration_filters_{}_all.config", hostName);

    defaultDevice                   = settingsPath / "device_default.config";
    allGrabbersDevice               = settingsPath / std::format("device_{}_all.config", hostName);

    defaultColor                    = settingsPath / "color_default.config";
    allGrabbersColor                = settingsPath / std::format("color_{}_all.config", hostName);

    allGrabbersModel                = calibrationPath / std::format("model_{}_all.config", hostName);

    logName                         = std::format("k4sm_manager_{}.html", hostName);
}

auto DCSettingsPaths::initialize_grabbers(size_t nbConnections) -> void{

    grabbersFilters.clear();
    grabbersCalibrationFilters.clear();
    grabbersDevice.clear();
    grabbersColor.clear();
    grabbersModel.clear();

    for(size_t ii = 0; ii < nbConnections; ++ii){
        grabbersFilters.push_back(              settingsPath    / std::format("filters_{}_G{}.config", hostName, ii));
        grabbersCalibrationFilters.push_back(   settingsPath    / std::format("calibration_filters_{}_G{}.config", hostName, ii));
        grabbersDevice.push_back(               settingsPath    / std::format("device_{}_G{}.config", hostName, ii));
        grabbersColor.push_back(                settingsPath    / std::format("color_{}_G{}.config", hostName, ii));
        grabbersModel.push_back(                calibrationPath / std::format("model_{}_G{}.config", hostName, ii));
    }
}

auto DCSettingsPaths::get_network_settings_file_path() -> std::string{

    if(std::filesystem::exists(hostNetwork)){
        return hostNetwork.string();
    }else if(std::filesystem::exists(defaultNetwork)){
        Logger::warning("Cannot find current host network settings file, use default instead.\n");
        return defaultNetwork.string();
    }else{
        Logger::warning("Cannot find default network setings file.\n");
    }
    return {};
}

auto DCSettingsPaths::get_filters_settings_file_path(size_t idG) -> std::string{

    if(idG < grabbersFilters.size()){
        if(std::filesystem::exists(grabbersFilters[idG])){
            return grabbersFilters[idG].string();
        }
    }else{
        Logger::warning(std::format("Invalid grabber id [{}] for filters settings file.\n", idG));
    }
    if(std::filesystem::exists(defaultFilters)){
        Logger::warning(std::format("Cannot find filters settings file for grabber [{}], use default instead.\n", idG));
        return defaultFilters.string();
    }else{
        Logger::warning("Cannot find default filters setings file.\n");
    }
    return {};
}

auto DCSettingsPaths::get_calibration_filters_settings_file_path(size_t idG) -> std::string{

    if(idG < grabbersCalibrationFilters.size()){
        if(std::filesystem::exists(grabbersCalibrationFilters[idG])){
            return grabbersCalibrationFilters[idG].string();
        }
    }else{
        Logger::warning(std::format("Invalid grabber id [{}] for calibration filters settings file.\n", idG));
    }
    if(std::filesystem::exists(defaultCalibrationFilters)){
        Logger::warning(std::format("Cannot find calibration filters settings file for grabber [{}], use default instead.\n", idG));
        return defaultCalibrationFilters.string();
    }else{
        Logger::warning("Cannot find default calibration filters setings file.\n");
    }
    return {};
}

auto DCSettingsPaths::get_device_settings_file_path(size_t idG) -> std::string{

    if(idG < grabbersDevice.size()){
        if(std::filesystem::exists(grabbersDevice[idG])){
            return grabbersDevice[idG].string();
        }
    }else{
        Logger::warning(std::format("Invalid grabber id [{}] for device settings file.\n", idG));
    }
    if(std::filesystem::exists(defaultDevice)){
        Logger::warning(std::format("Cannot find device settings file for grabber [{}], use default instead.\n", idG));
        return defaultDevice.string();
    }else{
        Logger::warning("Cannot find default device setings file.\n");
    }
    return {};
}

auto DCSettingsPaths::get_color_settings_file_path(size_t idG) -> std::string{

    if(idG < grabbersColor.size()){
        if(std::filesystem::exists(grabbersColor[idG])){
            return grabbersColor[idG].string();
        }
    }else{
        Logger::warning(std::format("Invalid grabber id [{}] for color settings file.\n", idG));
    }
    if(std::filesystem::exists(defaultColor)){
        Logger::warning(std::format("Cannot find color settings file for grabber [{}], use default instead.\n", idG));
        return defaultColor.string();
    }else{
        Logger::warning("Cannot find default color setings file.\n");
    }
    return {};
}

auto DCSettingsPaths::get_model_settings_file_path(size_t idG) -> std::string{

    if(idG < grabbersModel.size()){
        if(std::filesystem::exists(grabbersModel[idG])){
            return grabbersModel[idG].string();
        }
    }else{
        Logger::warning(std::format("nvalid grabber id [{}] for model settings file.\n", idG));
    }
    return {};
}

