
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
#include "geometry/point2.hpp"

// 3d-engine
#include "imgui-tb/imgui_logs.hpp"

// local
#include "dcm_model.hpp"

namespace tool::graphics {

[[maybe_unused]] static const char* filtersTypes[] = {
    "Normal", "Calibration"
};

class DCMLeftPanelChildDrawer{

public:

    auto initialize(size_t nbGrabbers) -> void;
    auto draw(geo::Pt2f size, int windowFlags, DCMModel *model) -> void;

    double uiFramerateMS = 0.0;

private:

    auto draw_clients_ui(DCMModel *model) -> void;
    // tab items
    auto draw_global_tab_item(DCMModel *model) -> void;
    auto draw_clients_tab_item(DCMModel *model) -> void;
    auto draw_ui_tab_item(DCMUiSettings &ui) -> void;

    // # settings
    auto draw_type_tab_item(cam::DCClient &client) -> void;
    auto draw_device_tab_item(cam::DCClient &client) -> void;
    auto draw_filters_tab_item(cam::DCClient &client) -> void;
    auto draw_display_tab_item(cam::DCClient &client) -> void;
    auto draw_model_tab_item(cam::DCClient &client) -> void;
    auto draw_color_tab_item(cam::DCClient &client) -> void;
    auto draw_recorder_tab_item(cam::DCVideoRecorder &recorder) -> void;
    auto draw_player_tab_item(cam::DCVideoPlayer &player) -> void;
    auto draw_calibrator_tab_item(cam::DCClient &client, cam::DCCalibratorStates &cStates, cam::DCCalibratorDisplaySettings &cdSettings, cam::DCCalibratorSettings &cSettings) -> void;

    // ui
    int guiCurrentFiltersTypeSelection = 0;
    int guiCurrentFromFiltersSelection = 0;
    int guiCurrentTargetFiltersSelection = 0;

    int guiCurrentFromColorsSelection = 0;
    int guiCurrentTargetColorsSelection = 0;



    std::vector<std::string> fromFilters;
    std::vector<std::string> targetsFilters;

    std::vector<std::string> fromColor;
    std::vector<std::string> targetsColor;


};
}
