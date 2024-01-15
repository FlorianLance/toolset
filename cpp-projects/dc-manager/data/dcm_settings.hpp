
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
#include "network/udp_server_network_settings.hpp"
#include "camera/settings/dc_device_settings.hpp"
#include "camera/settings/dc_calibrator_settings.hpp"
#include "camera/settings/dc_player_settings.hpp"
#include "camera/settings/dc_display_settings.hpp"
#include "camera/settings/dc_recorder_settings.hpp"
#include "camera/settings/dc_color_settings.hpp"
#include "camera/settings/dc_delay_settings.hpp"
#include "camera/settings/dc_model_settings.hpp"
#include "camera/settings/dc_filters_settings.hpp"

// 3d-engine
#include "imgui-tb/imgui_ui_drawer.hpp"

// local
#include "dcm_ui_settings.hpp"

#include "dcm_types.hpp"

namespace tool {

struct DCMGrabberNetworkSettings{

    bool connected = false;
    std::string name = "0 xxx.xxx.xxx";

    std::string readingAdress;
    int readingPort;
    std::string sendingAdress;
    int sendingPort;
    network::Protocol protocol;

    size_t lastFrameIdReceived = 0;
};

struct DCMGrabberSettings{

    size_t id = 0;
    DCMGrabberNetworkSettings network;

    camera::DCFiltersSettings filters;
    std::string filtersFilePath;
    camera::DCFiltersSettings calibrationFilters = camera::DCFiltersSettings::default_init_for_calibration();
    std::string calibrationFiltersFilePath;
    camera::DCDeviceSettings device;
    std::string deviceFilePath;
    camera::DCColorSettings color;
    std::string colorFilePath;
    camera::DCModelSettings model;
    std::string modelFilePath;
    camera::DCDelaySettings delay;

    camera::DCCloudDisplaySettings cloudDisplay;    

    std::int64_t synchroAverageDiff = 0;
};

struct DCMGlobalStates{
    camera::DCRecorderStates recorder;
    camera::DCPlayerStates player;
    camera::DCCalibratorStates calibrator;
};

struct DCMGlobalDrawerSettings{
    graphics::DCCalibratorDrawerSettings calibrator;
};

struct DCMGlobalSettings{

    bool useNormalFilteringSettings = true;

    UdpServerNetworkSettings network;
    std::string networkFilePath;
    ui::DCMUiSettings ui;
    camera::DCSceneDisplaySettings sceneDisplay;

    camera::DCRecorderSettings recorder;
    camera::DCPlayerSettings player;
    camera::DCCalibratorSettings calibrator;
};

struct DCMSettings{

    auto initialize() -> bool;

    // getters
    static auto host_name() -> std::string;    

    // settings
    auto process_settings_action(SAction action, STarget target, SType type, SFile file, size_t id) -> void;
    auto update_model(size_t id, const camera::DCModelSettings &model) -> void;

    // states
    auto update_recorder_states(camera::DCRecorderStates recorderStates) -> void;
    auto update_player_states(camera::DCPlayerStates playerStates) -> void;
    auto update_calibrator_states(camera::DCCalibratorStates calibratorStates) -> void;

    auto update_color_settings_from_device(size_t id, const camera::DCColorSettings &color) -> void;

    // trigger
    auto trigger_all_models() -> void;
    auto trigger_all_cloud_display() -> void;

    // tests
    // auto update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<std::uint8_t> value) -> void;

    DCMGlobalSettings globalSet;
    DCMGlobalStates globalSta;
    DCMGlobalDrawerSettings globalDrawerSet;
    std::vector<DCMGrabberSettings> grabbersSet;

    // # colors
    static const inline ImVec4 selectedC            = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static const inline ImVec4 connectedSelectedC   = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    static const inline ImVec4 unselectedC          = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    static const inline ImVec4 connectedUnselectedC = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);

private:

    std::vector<std::vector<geo::Pt2<int>>> pencils;

    static const inline std::vector<geo::Pt4f> cloudsColors = {
        {1.0f,0.0f,0.0f, 1.0f},
        {0.0f,1.0f,0.0f, 1.0f},
        {0.0f,0.0f,1.0f, 1.0f},
        {1.0f,1.0f,0.0f, 1.0f},
        {0.0f,1.0f,1.0f, 1.0f},
        {1.0f,0.0f,1.0f, 1.0f},
        {0.5f,0.5f,0.0f, 1.0f},
        {0.0f,0.5f,0.5f, 1.0f},
        {0.5f,0.0f,0.5f, 1.0f},
        {1.0f,0.5f,0.0f, 1.0f},
        {0.0f,0.5f,1.0f, 1.0f},
        {1.0f,0.0f,0.5f, 1.0f},
    };
};
}
