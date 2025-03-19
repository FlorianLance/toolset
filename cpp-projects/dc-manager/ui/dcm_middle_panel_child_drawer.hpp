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

// imgui-opengl-engine
#include "imgui-tb/imgui_dc_recorder_drawer.hpp"
#include "imgui-tb/imgui_dc_player_drawer.hpp"
#include "imgui-tb/imgui_dc_calibrator_drawer.hpp"
#include "imgui-tb/imgui_dc_direct_drawer.hpp"

// local
#include "data/dcm_ui_settings.hpp"

namespace tool::graphics {

class DCMMiddlePanelChildDrawer{

public:

    auto initialize(size_t nbGrabbers) -> void;
    auto update() -> void;
    auto draw(geo::Pt2f size, const DCMUiSettings &uiS) -> void;

    DCDirectDrawer directD;
    DCRecorderDrawer recorderD;
    DCPlayerDrawer playerD;
    DCCalibratorDrawer calibratorD;

private:


    bool m_directTabOpened = false;
    bool m_recordingTabOpened = false;
    bool m_playerTabOpened = false;
    bool m_calibrationTabOpened = false;

    bool m_newFrame = false;

};
}
