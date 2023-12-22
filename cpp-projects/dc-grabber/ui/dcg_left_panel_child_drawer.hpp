
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

// 3d-engine
#include "imgui-tb/imgui_logs.hpp"

// local
#include "data/dcg_settings.hpp"

namespace tool::graphics {

class DCGLeftPanelChildDrawer{

public:

    auto draw(geo::Pt2f size, int windowFlags, DCGSettings &settings) -> void;
    auto append_log(const std::string &log) -> void;

    bool settingsPaneDisplayed = false;
    bool filtersPanelDisplayed = false;

private:

    auto draw_client_info_tab_item(DCGSettings &settings) -> void;

    auto draw_device_tab_item(
        camera::DCDeviceSettings &device) -> void;

    auto draw_filters_tab_item(
        ui::DCGDisplaySettingsSettings &ui,
        const camera::DCConfigSettings &config,
        camera::DCFiltersSettings &filters) -> void;

    auto draw_colors_settings_tab_item(
        camera::DCType type,
        camera::DCColorSettings &colors) -> void;

    auto draw_display_tab_item(ui::DCGDisplaySettingsSettings &ui)-> void;
    auto draw_logs_tab_item()-> void;
    auto draw_audio_tab_item()-> void;
    auto draw_recording_tab_item(camera::DCRecorderStates &recStates, camera::DCRecorderSettings &recSetings) -> void;
    auto draw_model_tab_item(camera::DCModelSettings &model)-> void;

    ImguiLogs m_logs;
    bool m_autoUpdate = true;

    static const inline std::vector<std::string> windows = {
        "Color###focus_windows_color",
        "Depth###focus_windows_depth",
        "Infrared###focus_windows_infrared",
        "Cloud###focus_windows_cloud"
    };
};
}
