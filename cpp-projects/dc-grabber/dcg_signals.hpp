
#pragma once

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


// base
#include "thirdparty/sigslot/signal.hpp"
#include "depth-camera/settings/dc_device_settings.hpp"
#include "depth-camera/settings/dc_display_settings.hpp"
#include "depth-camera/settings/dc_video_recorder_settings.hpp"
#include "depth-camera/settings/dc_misc_settings.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/settings/dc_filters_settings.hpp"
#include "network/settings/udp_server_settings.hpp"



namespace tool {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class DCGSignals{

public:
    static auto get() -> DCGSignals*;

    // actions
    // # network
    SSS<bool, int> sending_failure_signal;
    SSS<> reset_reading_network_signal;
    SSS<> disconnect_all_clients_signal;
    // # i/o
    SSS<std::string> save_cloud_to_file_signal;
    SSS<std::string> save_global_settings_file_signal;
    SSS<std::string> load_global_settings_file_signal;

    SSS<> reset_device_settings_signal;
    SSS<> reset_filters_settings_signal;
    SSS<> reset_color_settings_signal;
    SSS<> reset_model_settings_signal;
    SSS<> load_default_device_settings_file_signal;
    SSS<> load_default_filters_settings_file_signal;
    SSS<> load_default_color_settings_file_signal;
    SSS<> load_default_model_settings_file_signal;
    SSS<std::string> load_subpart_device_settings_file_signal;
    SSS<std::string> load_subpart_filters_settings_file_signal;
    SSS<std::string> load_subpart_color_settings_file_signal;
    SSS<std::string> load_subpart_model_settings_file_signal;
    // # recordings
    SSS<> start_recorder_signal;
    SSS<> stop_recorder_signal;
    SSS<> reset_recorder_signal;
    SSS<int> set_recorder_time_signal;
    SSS<std::string> save_recorder_signal;

    // settings
    SSS<const cam::DCDeviceSettings&> update_device_settings_signal;
    SSS<const cam::DCFiltersSettings&> update_filters_signal;
    SSS<const cam::DCFiltersSettings&> update_filters_ui_only_signal;
    SSS<const cam::DCColorSettings&> update_color_settings_signal;

    SSS<size_t, const cam::DCModelSettings&> update_model_settings_signal;
    SSS<cam::DCMiscSettings> update_misc_settings_signal;
    SSS<const cam::DCSceneDisplaySettings&> update_scene_display_settings_signal;
    SSS<size_t,const cam::DCDeviceDisplaySettings&> update_cloud_display_settings_signal;
    SSS<const cam::DCVideoRecorderSettings&> update_recorder_settings_signal;

    // input
    // SSS<size_t, size_t, geo::Pt2<int>, geo::Pt4<std::uint8_t>> mouse_pressed_color_direct_signal;
    // SSS<size_t, size_t, geo::Pt2<int>, geo::Pt3<std::uint8_t>> mouse_pressed_depth_direct_signal;
    // SSS<size_t, size_t, geo::Pt2<int>, geo::Pt3<std::uint8_t>> mouse_pressed_infra_direct_signal;

};

}
