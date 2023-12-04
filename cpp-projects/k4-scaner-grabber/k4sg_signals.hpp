
#pragma once

/*******************************************************************************
** Toolbox-k4-scaner-grabber                                                  **
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
#include <memory>

// base
#include "thirdparty/sigslot/signal.hpp"
#include "camera/settings/dc_device_settings.hpp"
#include "camera/settings/dc_display_settings.hpp"
#include "camera/settings/dc_recorder_settings.hpp"
#include "camera/dc_model.hpp"
#include "camera/dc_filters.hpp"
#include "network/kinect4/k4_client_network_settings.hpp"

// local
#include "data/k4sg_ui_settings.hpp"

namespace tool {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class K4SGSignals{

public:
    static auto get() -> K4SGSignals*;

    // input
    SSS<size_t, size_t, geo::Pt2<int>, geo::Pt4<std::uint8_t>> mouse_pressed_color_direct_signal;
    SSS<size_t, size_t, geo::Pt2<int>, geo::Pt3<std::uint8_t>> mouse_pressed_depth_direct_signal;
    SSS<size_t, size_t, geo::Pt2<int>, geo::Pt3<std::uint8_t>> mouse_pressed_infra_direct_signal;

    // config
    SSS<network::K4ClientNetworkSettings*> init_network_sending_settings_signal;
    SSS<size_t,const camera::DCCloudDisplaySettings&> update_cloud_display_settings_signal;
    SSS<const camera::DCSceneDisplaySettings&> update_scene_display_settings_signal;
    SSS<const camera::DCFilters&> update_filters_signal;
    SSS<const camera::DCColorSettings&> update_color_settings_signal;
    SSS<const camera::DCDeviceSettings&> update_device_settings_signal;
    SSS<camera::DCDelaySettings> update_delay_signal;
    SSS<> update_device_list_signal;
    SSS<size_t, const camera::DCModel&> update_model_signal;
    SSS<ui::K4SGUiSettings*> update_ui_settings_signal;

    // recording
    SSS<> start_recorder_signal;
    SSS<> stop_recorder_signal;
    SSS<> reset_recorder_signal;
    SSS<int> set_recorder_time_signal;
    SSS<std::string> save_recorder_signal;
    SSS<camera::DCRecorderSettings> update_recorder_settings_signal;


    // i/o
    // # data
    SSS<std::string> save_recording_to_file_signal;
    SSS<std::string> save_cloud_to_file_signal;
    // # device
    SSS<> reset_device_settings_signal;
    SSS<> save_device_settings_to_default_file_signal;
    SSS<> save_device_settings_to_current_hostname_file_signal;
    SSS<> load_default_device_settings_file_signal;
    SSS<> load_current_hostname_device_settings_file_signal;
    // # filters
    SSS<> reset_filters_signal;
    SSS<> save_filters_to_default_file_signal;
    SSS<> save_filters_to_current_hostname_file_signal;
    SSS<> load_default_filters_file_signal;
    SSS<> load_current_hostname_filters_file_signal;
    // # color
    SSS<> reset_color_settings_signal;
    SSS<> save_color_settings_to_default_file_signal;
    SSS<> save_color_settings_to_current_hostname_file_signal;
    SSS<> load_default_color_settings_file_signal;
    SSS<> load_current_hostname_color_settings_file_signal;
    // # other
    SSS<> ping_server_signal;
    SSS<> debug_device_send_signal;

};

}
