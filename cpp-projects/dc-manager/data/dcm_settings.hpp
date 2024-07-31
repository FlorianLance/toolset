
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
#include "depth-camera/settings/dc_client_connection_settings.hpp"
#include "depth-camera/settings/dc_calibrator_settings.hpp"
#include "depth-camera/settings/dc_video_player_settings.hpp"
#include "depth-camera/settings/dc_video_recorder_settings.hpp"
#include "depth-camera/settings/dc_grabber_settings.hpp"

// local
#include "dcm_ui_settings.hpp"
#include "dcm_types.hpp"

// imgui
#include <imgui/imgui.h>

namespace tool {


struct DCMSettings{

    auto initialize() -> bool;
    auto add_default_device() -> void{}

    // getters
    static auto host_name() -> std::string;    

    // settings
    auto process_settings_action(SAction action, STarget target, SType type, SFile file, size_t id) -> void;
    auto update_model(size_t id, const cam::DCModelSettings &model) -> void;
    auto update_color_settings_from_device(size_t id, const cam::DCColorSettings &color) -> void;

    // trigger
    auto trigger_all_models() -> void;
    auto trigger_all_cloud_display() -> void;


    bool useNormalFilteringSettings = true;
    DCMUiSettings uiS;    
    // camera
    cam::DCSceneDisplaySettings sceneDisplayS;
    cam::DCVideoRecorderSettings recorderS;
    cam::DCVideoPlayerSettings playerS;
    cam::DCCalibratorSettings calibratorS;
    
    
    cam::DCClientConnectionSettings devicesConnectionsS;
    std::vector<cam::DCGrabberSettings> grabbersS;

    // graphics
    graphics::DCCalibratorDrawerSettings calibratorDrawerS;


    // # colors
    static const inline ImVec4 selectedC            = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static const inline ImVec4 connectedSelectedC   = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    static const inline ImVec4 unselectedC          = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    static const inline ImVec4 connectedUnselectedC = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);

private:

    std::vector<std::vector<geo::Pt2<int>>> pencils;


};
}



// tests
// auto update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<std::uint8_t> value) -> void;
