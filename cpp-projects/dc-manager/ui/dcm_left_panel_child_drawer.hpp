
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

// 3d-engine
#include "imgui-tb/imgui_logs.hpp"

// local
#include "data/dcm_settings.hpp"

namespace tool::graphics {

[[maybe_unused]] static const char* filtersTypes[] = {
    "Normal", "Calibration"
};


class DCMLeftPanelChildDrawer{

public:

    auto initialize(size_t nbGrabbers) -> void;
    auto draw(geo::Pt2f size, int windowFlags, DCMSettings &settings) -> void;
    auto append_global_log(const std::string &log) -> void;
    auto append_feedback_log(size_t idG, const std::string &log) -> void;

private:

    auto draw_grabbers_ui(DCMSettings &settings) -> void;
    // tab items
    auto draw_commands_tab_item(const UdpServerNetworkSettings &networkS, std::vector<DCMGrabberSettings> &grabbersS) -> void;
    auto draw_settings_tab_item(DCMSettings &settings) -> void;
    auto draw_ui_tab_item(ui::DCMUiSettings &ui) -> void;
    auto draw_logs_tab_item() -> void;
    auto draw_infos_tab_item(const DCMSettings &settings) -> void;
    // sub tab items
    // # command
    auto draw_all_commands_tab_item(const UdpServerNetworkSettings &networkS, std::vector<DCMGrabberSettings> &grabbersS) -> void;
    auto draw_individual_commands_tab_item(DCMGrabberSettings &grabberS) -> void;
    // # settings
    auto draw_device_tab_item(DCMSettings &settings) -> void;
    auto draw_filters_tab_item(DCMSettings &settings) -> void;
    auto draw_display_tab_item(camera::DCSceneDisplaySettings &sceneDisplay, std::vector<DCMGrabberSettings> &grabbers) -> void;
    auto draw_calibration_tab_item( std::vector<DCMGrabberSettings> &grabbers) -> void;
    auto draw_color_tab_item(std::vector<DCMGrabberSettings> &grabbers) -> void;
    auto draw_recorder_tab_item(camera::DCRecorderStates &rStates, camera::DCRecorderSettings &rSettings) -> void;
    auto draw_player_tab_item(camera::DCPlayerStates &pStates, camera::DCPlayerSettings &pSettings) -> void;
    auto draw_calibrator_tab_item(bool useNormalFilteringSettings, camera::DCCalibratorStates &cStates, DCCalibratorDrawerSettings &cdSettings, camera::DCCalibratorSettings &cSettings) -> void;

    // ui
    bool autoUpdate = true;
    int guiCurrentFiltersTypeSelection = 0;
    int guiCurrentFromFiltersSelection = 0;
    int guiCurrentTargetFiltersSelection = 0;

    int guiCurrentFromColorsSelection = 0;
    int guiCurrentTargetColorsSelection = 0;


    // logs
    ImguiLogs globalLogs;
    std::vector<ImguiLogs> feedbacksLogs;
    std::vector<std::string> fromFilters;
    std::vector<std::string> targetsFilters;

    std::vector<std::string> fromColor;
    std::vector<std::string> targetsColor;
};
}
