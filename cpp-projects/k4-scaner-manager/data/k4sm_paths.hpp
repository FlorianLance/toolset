
#pragma once

/*******************************************************************************
** Toolset-k4-scaner-manager                                                  **
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

// std
#include <filesystem>
#include <format>

namespace tool {

struct K4SMPaths{

    static auto initialize(const std::filesystem::path &base_, const std::string &hostName_) -> void{

        base = base_;
        hostName = hostName_;
        configPath      = base       / "config/";
        settingsPath    = configPath / "settings/";
        networkPath     = configPath / "network/";
        calibrationPath = configPath / "calibration/";

        outputPath      = base       / "output/";

        defaultNetwork  = networkPath / "network_default.config";
        hostNetwork     = networkPath / std::format("network_{}.config", hostName);

        defaultFilters                  = settingsPath / "filters_default.config";
        allGrabbersFilters              = settingsPath / std::format("filters_{}_all.config", hostName);

        defaultCalibrationFilters       = settingsPath / "calibration_filters_default.config";
        allGrabbersCalibrationFilters   = settingsPath / std::format("calibration_filters_{}_all.config", hostName);

        defaultDevice                   = settingsPath / "device_default.config";
        allGrabbersDevice               = settingsPath / std::format("device_{}_all.config", hostName);

        defaultColor                    = settingsPath / "color_default.config";
        allGrabbersColor                = settingsPath / std::format("color_{}_all.config", hostName);

        allGrabbersModel                = calibrationPath / std::format("model_{}_all.config", hostName);

        logName = std::format("k4sg_manager_{}.html", hostName);
    }

    static auto initialize_grabbers(size_t nbConnections) -> void{

        for(size_t ii = 0; ii < nbConnections; ++ii){
            grabbersFilters.push_back(settingsPath / std::format("filters_{}_G{}.config", hostName, ii));
            grabbersCalibrationFilters.push_back(settingsPath / std::format("calibration_filters_{}_G{}.config", hostName, ii));
            grabbersDevice.push_back(settingsPath / std::format("device_{}_G{}.config", hostName, ii));
            grabbersColor.push_back(settingsPath / std::format("color_{}_G{}.config", hostName, ii));
            grabbersModel.push_back(calibrationPath / std::format("model_{}_G{}.config", hostName, ii));
        }
    }

    static inline std::filesystem::path base;
    static inline std::filesystem::path configPath;
    static inline std::filesystem::path settingsPath;
    static inline std::filesystem::path networkPath;
    static inline std::filesystem::path calibrationPath;
    static inline std::filesystem::path outputPath;

    static inline std::string hostName;
    static inline std::string logName;

    static inline std::filesystem::path defaultNetwork;
    static inline std::filesystem::path hostNetwork;

    static inline std::filesystem::path defaultDevice;
    static inline std::filesystem::path allGrabbersDevice;
    static inline std::vector<std::filesystem::path> grabbersDevice;

    static inline std::filesystem::path defaultFilters;
    static inline std::filesystem::path allGrabbersFilters;
    static inline std::vector<std::filesystem::path> grabbersFilters;

    static inline std::filesystem::path defaultCalibrationFilters;
    static inline std::filesystem::path allGrabbersCalibrationFilters;
    static inline std::vector<std::filesystem::path> grabbersCalibrationFilters;

    static inline std::filesystem::path defaultColor;
    static inline std::filesystem::path allGrabbersColor;
    static inline std::vector<std::filesystem::path> grabbersColor;

    static inline std::filesystem::path allGrabbersModel;
    static inline std::vector<std::filesystem::path> grabbersModel;

};
}
