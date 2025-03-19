
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


// local
#include "dcp_left_panel_child_drawer.hpp"
#include "dcp_middle_panel_child_drawer.hpp"
#include "dcp_model.hpp"

namespace tool::graphics {

class DCPMainWindowDrawer{

public:

    auto initialize(size_t nbGrabbers) -> bool;
    auto update() -> void;
    auto draw(geo::Pt2f size, DCPModel *model) -> void;

    auto append_global_log(const std::string &log) -> void {
        globalLogs.add_log(log.c_str());
    }

    auto append_feedback_log(size_t idG, net::Feedback feedback) -> void {
        if(idG < feedbacksLogs.size()){
            feedbacksLogs[idG].add_log(std::format("Valid [{}] received\n", to_string(static_cast<net::DCMessageType>(feedback.receivedMessageType))).c_str());
        }
    }


private:

    auto draw_menu()-> void;
    // auto draw_device_sub_menu() -> void;
    // auto draw_color_sub_menu() -> void;
    // auto draw_filters_sub_menu() -> void;
    // auto draw_model_sub_menu() -> void;

    // ui
    bool m_showDemoWindow = false;
    bool m_showMetricsWindow = false;
    size_t m_nbDevices = 0;
    cam::SettingsAction m_currentAction;

public:

    // drawers
    DCPLeftPanelChildDrawer m_leftPanelD;
    DCPMiddlePanelChildDrawer m_middlePanelD;
    ImguiLogs globalLogs;
    std::vector<ImguiLogs> feedbacksLogs;
};
}
