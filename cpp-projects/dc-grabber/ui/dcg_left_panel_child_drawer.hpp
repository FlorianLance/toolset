
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
#include "geometry/point2.hpp"
#include "utility/monitoring.hpp"

// 3d-engine
#include "imgui-tb/imgui_logs.hpp"

// local
#include "dcg_model.hpp"

namespace tool::graphics {

class DCGLeftPanelChildDrawer{

public:

    auto draw(geo::Pt2f size, int windowFlags, DCGModel *model) -> void;

    bool settingsPaneDisplayed = false;
    bool filtersPanelDisplayed = false;


    auto update_selected_color(const geo::Pt4f& color) -> void;

private:

    auto draw_network_tab_item(DCGModel *model) -> void;
    auto draw_device_tab_item(cam::DCDeviceSettings &device) -> void;
    auto draw_filters_tab_item(const cam::DCConfigSettings &config, cam::DCFiltersSettings &filters) -> void;

    auto draw_colors_settings_tab_item(
        cam::DCType type,
        cam::DCColorSettings &colors) -> void;
    
    auto draw_display_tab_item(DCGUiSettings &uiS, cam::DCSceneDisplaySettings &sceneDisplayS, cam::DCDeviceDisplaySettings &displayS)-> void;
    auto draw_audio_tab_item()-> void;
    auto draw_recorder_tab_item(cam::DCVideoRecorderStates &recStates, cam::DCVideoRecorderSettings &recSetings) -> void;
    auto draw_model_tab_item(cam::DCModelSettings &modelS)-> void;
    auto draw_misc_tab_item(cam::DCMiscSettings &miscS) -> void;

    geo::Pt4f firstLastClickedPixelColor;
    geo::Pt4f secondLastClickedPixelColor;


};
}
