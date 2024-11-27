
/*******************************************************************************
** Toolset-dc-grabber                                                         **
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

// base
#include "depth-camera/network/dc_server.hpp"
#include "depth-camera/dc_device.hpp"
#include "depth-camera/dc_video_recorder.hpp"

namespace tool {

struct DCGUiSettings{
    // runtime
    bool focusWindow = false;
    int currentWindow = 3;
};

struct DCGModel{

    DCGModel();
    ~DCGModel();

    auto initialize() -> bool;
    auto update() -> void;
    auto clean() -> void;
    
    // save
    auto save_server_settings_file(const std::string &settingsFilePath) -> bool;

    // load
    auto load_server_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_default_device_settings_file() -> void;
    auto load_default_filters_settings_file() -> void;
    auto load_default_color_settings_file() -> void;
    auto load_default_model_settings_file() -> void;
    auto load_subpart_device_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_subpart_filters_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_subpart_color_settings_file(const std::string &settingsFilePath) -> bool;
    auto load_subpart_model_settings_file(const std::string &settingsFilePath) -> bool;

    // reset
    auto reset_device_settings() -> void;
    auto reset_filters_filters() -> void;
    auto reset_color_settings() -> void;
    auto reset_model_settings() -> void;

    // udpate
    auto update_color_settings_ui_only(const cam::DCColorSettings &colorS) -> void;

    // trigger
    auto trigger_settings() -> void;

    // ui
    DCGUiSettings uiSettings;

    cam::DCServer server;
    std::unique_ptr<cam::DCDevice> device = nullptr;
    cam::DCVideoRecorder recorder;

    tf::Executor executor;
    tf::Taskflow mainTF;
    std::atomic_bool doTask = true;
    tf::Future<void> resTask;
};
}
