
#pragma once

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

// local
#include "depth-camera/dc_enums.hpp"

namespace tool {

struct DCGrabberPaths{
    std::filesystem::path device;
    std::filesystem::path filters;
    std::filesystem::path calibrationFilters;
    std::filesystem::path model;
    std::filesystem::path color;
};

struct DCSettingsPaths{

    static auto get() -> DCSettingsPaths*;
    auto initialize(char *argv[], cam::DCApplicationType type, size_t id) -> void;

    auto client_settings_file() -> std::string;
    auto server_settings_file() -> std::string;
    // legacy
    auto network_settings_file() -> std::string;
    // # client
    auto client_filters_settings_file(size_t idG) -> std::string;
    auto client_calibration_filters_settings_file(size_t idG) -> std::string;
    auto client_device_settings_file(size_t idG) -> std::string;
    auto client_color_settings_file(size_t idG) -> std::string;
    auto client_model_settings_file(size_t idG) -> std::string;
    // # server
    auto server_filters_settings_file() -> std::string;
    auto server_calibration_filters_settings_file() -> std::string;
    auto server_device_settings_file() -> std::string;
    auto server_color_settings_file() -> std::string;
    auto server_model_settings_file() -> std::string;

    // files path
    std::string logName;
    std::filesystem::path defaultClient;
    std::filesystem::path client;
    std::filesystem::path defaultServer;
    std::filesystem::path server;

    // legacy
    // # common
    // ## network
    std::filesystem::path defaultJsonNetwork;
    std::filesystem::path jsonNetwork;
    std::filesystem::path configNetwork;
    // ## default
    DCGrabberPaths defaultJson;
    DCGrabberPaths defaultConfig;
    // # server
    DCGrabberPaths serverJson;
    DCGrabberPaths serverConfig;
    // # client
    DCGrabberPaths allJsonClients;
    DCGrabberPaths allConfigClients;
    std::vector<DCGrabberPaths> clientsJson;
    std::vector<DCGrabberPaths> clientsConfig;

};
}
