
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



// imgui-opengl-engine
#include "imgui-tb/imgui_dc_device_drawer.hpp"
#include "imgui-tb/imgui_dc_recorder_drawer.hpp"

// local
#include "dcg_model.hpp"
#include "dcg_left_panel_child_drawer.hpp"
#include "dcg_main_menu_bar_drawer.hpp"

namespace tool::graphics {

class DCGMainW{

public:

    auto initialize() -> bool;
    auto draw(geo::Pt2f size, DCGModel *model) -> void;
    auto append_log(const std::string &log) -> void;

    // drawers
    DCGLeftPanelChildDrawer leftPanelD;
    DCDeviceDrawer deviceD;
    DCRecorderDrawer recorderD;
    DCGMainMenuBarDrawer menuD;
    ImguiLogs logsD;

    // monitoring
    AverageBuffer captureB;
    AverageBuffer readB;
    AverageBuffer procB;
    AverageBuffer convImageB;
    AverageBuffer resizeImageB;
    AverageBuffer filterB;
    AverageBuffer updateDataFrameB;
    AverageBuffer finalizeDataFrameB;
    AverageBuffer updateFrameB;
    AverageBuffer finalizeFrameB;
    AverageBuffer elaspedBeforeSendingB;
    AverageBuffer sendingB;
};
}
