
/*******************************************************************************
** Toolset-dc-manager                                                         **
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
#include "depth-camera/client/dc_client.hpp"
#include "depth-camera/dc_video_recorder.hpp"
#include "depth-camera/dc_video_player.hpp"
#include "depth-camera/dc_calibrator.hpp"

// local
#include "data/dcm_ui_settings.hpp"

namespace tool {

struct DCMModel{

    DCMModel();
    ~DCMModel();

    auto initialize() -> bool;
    auto update() -> void;
    auto clean() -> void;

    auto add_device(cam::DCClientType type) -> void;
    auto remove_last_device() -> void;

    auto process_settings_action(cam::SettingsAction sAction) -> void;
    // auto trigger_settings() -> void;
    auto ask_calibration() -> void;

    // ui
    DCMUiSettings uiSettings;

    cam::DCClient client;
    cam::DCVideoRecorder recorder;
    cam::DCVideoPlayer player;
    cam::DCCalibrator calibrator;
};
}
