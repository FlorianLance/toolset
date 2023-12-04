
#pragma once

/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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
#include "network/kinect4/k4_client_connection.hpp"
#include "camera/dc_device_manager.hpp"

// 3d-engine
#include "imgui-tb/imgui_k4_device_drawer.hpp"
#include "imgui-tb/imgui_k4_recorder_drawer.hpp"


// local
#include "k4sg_model.hpp"
#include "k4sg_left_panel_child_drawer.hpp"
#include "k4sg_main_menu_bar_drawer.hpp"

namespace tool::graphics {

class K4SGMainWindowDrawer{

public:

    auto initialize() -> bool;
    auto draw(geo::Pt2f size, K4SGModel &model) -> void;

    // drawers
    K4SGLeftPanelChildDrawer leftPanelD;
    K4DeviceDrawer deviceD;
    K4RecorderDrawer recorderD;

    K4SGMainMenuBarDrawer menuD;

};
}
