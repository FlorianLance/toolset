
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
#include "data/dcm_settings.hpp"
#include "data/dcm_states.hpp"
#include "data/dcm_types.hpp"
#include "data/dcm_ui_settings.hpp"

namespace tool {

struct DCMModel{

    DCMModel();
    ~DCMModel();

    static auto host_name() -> std::string;

    auto initialize() -> bool;
    auto clean() -> void;
    auto update() -> void;

    auto process_settings_action(SettingsAction sAction) -> void;

    // calibration
    auto ask_calibration() -> void;

    // settings
    // # update
    auto update_filters(size_t idC, const cam::DCFiltersSettings& filters) -> void;
    auto update_calibration_filters(size_t idC, const cam::DCFiltersSettings& calibrationFilters) -> void;
    auto update_device_settings(size_t idC, const cam::DCDeviceSettings &deviceS) -> void;
    auto update_color_settings(size_t idC, const cam::DCColorSettings &colorS) -> void;
    auto update_delay_settings(size_t idC, const cam::DCDelaySettings &delayS) -> void;
    // # triggers
    auto trigger_settings() -> void;

    // ui
    DCMUiSettings uiSettings;

    cam::DCClient client;
    cam::DCVideoRecorder recorder;
    cam::DCVideoPlayer player;
    cam::DCCalibrator calibrator;
};
}
