


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

#include "dc_grabber_settings.hpp"

// std
#include <format>

// local
#include "dc_settings_paths.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;

auto DCMGrabberSettings::initialize(size_t idG, const net::ReadSendNetworkInfos &grabberInfos) -> void{

    this->id = idG;

    // init network
    network.name          = std::format("G{}", idG);
    network.sendingAdress = grabberInfos.sendingAdress;
    network.readingAdress = grabberInfos.readingAdress;
    network.sendingPort   = grabberInfos.sendingPort;
    network.readingPort   = grabberInfos.readingPort;
    network.protocol      = grabberInfos.protocol;

    // read filters settings file
    if(!filters.init_from_file(filtersFilePath =  DCSettingsPaths::get_filters_settings_file_path(idG))){
        Logger::error(std::format("[DCMGrabberSettings] No filters settings file found for grabber n°[{}]], default parameters used instead.\n", idG));
    }

    // read calibration filters settings file
    if(!calibrationFilters.init_from_file(calibrationFiltersFilePath =  DCSettingsPaths::get_calibration_filters_settings_file_path(idG))){
        Logger::error(std::format("[DCMGrabberSettings] No calibration filters settings file found for grabber n°[{}]], default parameters used instead.\n", idG));
    }

    // read calibration filters settings file
    if(!device.init_from_file(deviceFilePath = DCSettingsPaths::get_device_settings_file_path(idG))){
        Logger::error(std::format("[DCMGrabberSettings] No device settings file found for grabber n°[{}]], default parameters used instead.\n", idG));
    }

    // read color settings file
    if(!color.init_from_file(colorFilePath = DCSettingsPaths::get_color_settings_file_path(idG))){
        Logger::error(std::format("[DCMGrabberSettings] No color settings file found for grabber n°[{}]], default parameters used instead.\n", idG));
    }

    // read model settings file
    if(!model.init_from_file(modelFilePath =  DCSettingsPaths::get_model_settings_file_path(idG))){
        Logger::error(std::format("[DCMGrabberSettings] No model settings file found for grabber n°[{}]], default parameters used instead.\n", idG));
    }

    // display
    cloudDisplay.cloudColor = cloudsColors[idG];
}
