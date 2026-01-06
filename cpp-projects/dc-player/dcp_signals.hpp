
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

// std
#include <memory>

// sigslot
#include <sigslot/signal.hpp>

// base
#include "depth-camera/frame/dc_frame.hpp"
#include "depth-camera/frame/dc_data_frame.hpp"
#include "network/network_enums.hpp"

// local
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_misc_settings.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/settings/dc_calibrator_settings.hpp"
#include "depth-camera/settings/dc_video_recorder_settings.hpp"
#include "depth-camera/settings/dc_video_player_settings.hpp"
#include "depth-camera/settings/dc_display_settings.hpp"
#include "depth-camera/dc_types.hpp"
#include "depth-camera/dc_video.hpp"

namespace tool {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class DCPSignals{

public:
    static auto get() -> DCPSignals*;

    // initialization
    SSS<size_t> initialize_signal;


    // player
    SSS<> start_player_signal;
    SSS<> pause_player_signal;
    SSS<> restart_player_signal;
    SSS<> remove_until_current_frame_player_signal;
    SSS<> remove_after_current_frame_player_signal;
    SSS<> merge_player_signal;
    SSS<> info_player_signal;
    SSS<double> set_player_time_signal;
    SSS<std::string> save_cloud_player_signal;
    SSS<std::string> save_video_player_signal;
    SSS<std::string> load_video_player_signal;

    // # i/o
    SSS<size_t, std::string> save_current_cloud_signal;
    // SSS<std::string> save_all_current_clouds_signal;

    // settings
    SSS<cam::SettingsAction> process_settings_action_signal;
    SSS<size_t, const cam::DCDeviceSettings &> update_device_settings_signal;
    SSS<size_t, const cam::DCFiltersSettings&> update_filters_settings_signal;
    SSS<size_t, const cam::DCFiltersSettings&> update_filters_settings_ui_only_signal;
    SSS<size_t, const cam::DCFiltersSettings&> update_calibration_filters_settings_signal;
    SSS<size_t, const cam::DCColorSettings &> update_color_settings_signal;
    SSS<size_t, cam::DCMiscSettings> update_misc_settings_signal;
    SSS<size_t, const cam::DCModelSettings&> update_model_settings_signal;
    SSS<size_t, const cam::DCModelSettings&> update_model_settings_ui_only_signal;
    SSS<const cam::DCVideoRecorderSettings&> update_recorder_settings_signal;
    SSS<const cam::DCVideoPlayerSettings&> update_player_settings_signal;

    SSS<size_t, const cam::DCDeviceDisplaySettings&> update_cloud_display_settings_signal;
    SSS<const cam::DCSceneDisplaySettings&> update_scene_display_settings_signal;


};

}
