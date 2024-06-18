
#pragma once

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

// std
#include <filesystem>

namespace tool {

struct DCSettingsPaths{

    static auto initialize(const std::filesystem::path &base_, const std::string &hostName_) -> void;
    static auto initialize_grabbers(size_t nbConnections) -> void;

    static auto get_network_settings_file_path() -> std::string;
    static auto get_filters_settings_file_path(size_t idG) -> std::string;
    static auto get_calibration_filters_settings_file_path(size_t idG) -> std::string;
    static auto get_device_settings_file_path(size_t idG) -> std::string;
    static auto get_color_settings_file_path(size_t idG) -> std::string;
    static auto get_model_settings_file_path(size_t idG) -> std::string;

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
    static inline std::filesystem::path hostNetworkLegacy;

    static inline std::filesystem::path defaultDevice;
    static inline std::filesystem::path allGrabbersDevice;
    static inline std::filesystem::path allGrabbersDeviceLegacy;
    static inline std::vector<std::filesystem::path> grabbersDevice;
    static inline std::vector<std::filesystem::path> grabbersDeviceLegacy;

    static inline std::filesystem::path defaultFilters;
    static inline std::filesystem::path allGrabbersFilters;
    static inline std::filesystem::path allGrabbersFiltersLegacy;
    static inline std::vector<std::filesystem::path> grabbersFilters;
    static inline std::vector<std::filesystem::path> grabbersFiltersLegacy;

    static inline std::filesystem::path defaultCalibrationFilters;
    static inline std::filesystem::path allGrabbersCalibrationFilters;
    static inline std::filesystem::path allGrabbersCalibrationFiltersLegacy;
    static inline std::vector<std::filesystem::path> grabbersCalibrationFilters;
    static inline std::vector<std::filesystem::path> grabbersCalibrationFiltersLegacy;

    static inline std::filesystem::path defaultColor;
    static inline std::filesystem::path allGrabbersColor;
    static inline std::filesystem::path allGrabbersColorLegacy;
    static inline std::vector<std::filesystem::path> grabbersColor;
    static inline std::vector<std::filesystem::path> grabbersColorLegacy;

    static inline std::filesystem::path allGrabbersModel;
    static inline std::filesystem::path allGrabbersModelLegacy;
    static inline std::vector<std::filesystem::path> grabbersModel;
    static inline std::vector<std::filesystem::path> grabbersModelLegacy;

};
}
