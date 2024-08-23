
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

// base
#include "thirdparty/sigslot/signal.hpp"
#include "depth-camera/frame/dc_frame.hpp"
#include "depth-camera/frame/dc_compressed_frame.hpp"
#include "network/network_enums.hpp"

// local
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/settings/dc_calibrator_settings.hpp"
#include "depth-camera/settings/dc_video_recorder_settings.hpp"
#include "depth-camera/settings/dc_video_player_settings.hpp"
#include "depth-camera/settings/dc_display_settings.hpp"
#include "depth-camera/dc_types.hpp"

namespace tool {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class DCMSignals{

public:
    static auto get() -> DCMSignals*;

    // initialization
    SSS<size_t> initialize_signal;

    // actions
    // # network
    SSS<size_t> init_connection_signal;
    SSS<size_t, net::Command> command_signal;
    // # recording
    SSS<> start_recorder_signal;
    SSS<> stop_recorder_signal;
    SSS<> reset_recorder_signal;
    SSS<int> set_recorder_time_signal;
    SSS<std::string> save_recorder_signal;
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
    // # calibration
    SSS<> ask_calibration_signal;
    SSS<> reset_calibration_registering_signal;
    SSS<> start_calibration_registering_signal;
    SSS<> stop_calibration_registering_signal;
    SSS<> recompute_registering_processing_signal;
    SSS<std::vector<cam::DCModelSettings>> calibrate_signal;
    SSS<> validate_calibration_signal;
    SSS<> update_calibration_display_signal;
    // # i/o
    SSS<size_t, std::string> save_current_cloud_signal;
    // SSS<std::string> save_all_current_clouds_signal;

    // settings
    SSS<cam::SettingsAction> process_settings_action_signal;
    SSS<size_t, const cam::DCDeviceSettings &> update_device_settings_signal;
    SSS<size_t, const cam::DCFiltersSettings&> update_filters_settings_signal;
    SSS<size_t, const cam::DCFiltersSettings&> update_calibration_filters_settings_signal;
    SSS<size_t, const cam::DCColorSettings &> update_color_settings_signal;
    SSS<size_t, cam::DCDelaySettings> update_delay_settings_signal;
    SSS<size_t, const cam::DCModelSettings&> update_model_settings_signal;
    SSS<const cam::DCVideoRecorderSettings&> update_recorder_settings_signal;
    SSS<const cam::DCVideoPlayerSettings&> update_player_settings_signal;
    SSS<const cam::DCCalibratorSettings &> update_calibration_settings_signal;
    SSS<const cam::DCCalibratorDisplaySettings &> update_calibration_drawer_settings_signal;
    SSS<size_t, const cam::DCDeviceDisplaySettings&> update_cloud_display_settings_signal;
    SSS<const cam::DCSceneDisplaySettings&> update_scene_display_settings_signal;

    // // input
    // SSS<size_t, size_t, geo::Pt2<int>, geo::Pt4<std::uint8_t>> mouse_pressed_color_direct_signal;
    // SSS<size_t, size_t, geo::Pt2<int>, geo::Pt3<std::uint8_t>> mouse_pressed_depth_direct_signal;
    // SSS<size_t, size_t, geo::Pt2<int>, geo::Pt3<std::uint8_t>> mouse_pressed_infra_direct_signal;
};

}
