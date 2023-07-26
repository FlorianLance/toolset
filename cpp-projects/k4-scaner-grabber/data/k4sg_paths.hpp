
#pragma once

/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

struct K4SGPaths{

    static auto initialize(size_t idLocalGrabber, const std::filesystem::path &base, const std::string &hostName) -> void{

        logName         = std::format("k4sg_grabber_{}_{}.html", hostName, idLocalGrabber);

        configDir       = base       / "config/";
        settingsDir     = configDir / "settings/";
        networkDir      = configDir / "network/";
        calibrationDir  = configDir / "calibration/";

        defaultNetwork  = networkDir / "network_default.config";
        hostNetwork     = networkDir / std::format("network_{}_G{}.config", hostName, idLocalGrabber);

        defaultModel    = calibrationDir / "model_default.config";
        hostModel       = calibrationDir / std::format("model_{}_G{}.config", hostName, idLocalGrabber);

        defaultFilters  = settingsDir / "filters_default.config";
        hostFilters     = settingsDir / std::format("filters_{}_G{}.config", hostName, idLocalGrabber);

        defaultDevice   = settingsDir / "device_default.config";
        hostDevice      = settingsDir / std::format("device_{}_G{}.config", hostName, idLocalGrabber);

        defaultColor    = settingsDir / "color_default.config";
        hostColor       = settingsDir / std::format("color_{}_G{}.config", hostName, idLocalGrabber);
    }

    static inline std::filesystem::path configDir;
    static inline std::filesystem::path settingsDir;
    static inline std::filesystem::path networkDir;
    static inline std::filesystem::path calibrationDir;

    static inline std::string logName;
    static inline std::filesystem::path defaultNetwork;
    static inline std::filesystem::path hostNetwork;
    static inline std::filesystem::path defaultFilters;
    static inline std::filesystem::path hostFilters;
    static inline std::filesystem::path defaultDevice;
    static inline std::filesystem::path hostDevice;
    static inline std::filesystem::path defaultColor;
    static inline std::filesystem::path hostColor;
    static inline std::filesystem::path defaultModel;
    static inline std::filesystem::path hostModel;
};
}
