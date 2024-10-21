

/*******************************************************************************
** Toolset-3d-engine                                                          **
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

#include "imgui_dc_recorder_drawer.hpp"

// std
#include <format>

// base
#include "utility/logger.hpp"

// local
#include "imgui_ui_drawer.hpp"


using namespace tool::graphics;

auto DCRecorderDrawer::initialize(size_t nbGrabbers) -> void {
    
    auto lg = LogG("DCRecorderDrawer::initialize");
    DCCloudsSceneDrawer::initialize(nbGrabbers);
    m_currentFrames.resize(nbGrabbers);
    std::fill(std::begin(m_currentFrames), std::end(m_currentFrames), nullptr);
}

auto DCRecorderDrawer::set_frame(size_t idC, std::shared_ptr<cam::DCFrame> frame) -> void{

    if(idC >= m_currentFrames.size()){
        return;
    }

    m_currentFrames[idC] = frame;

    if(update_from_frame(idC, m_currentFrames[idC])){
        m_redrawClouds = true;
    }
}

auto DCRecorderDrawer::draw(bool focusWindow) -> void{

    // draw data
    static ImGuiID tabId = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_recorder_tabbar")){

        // all
        bool allTabOpened = false;
        if ((allTabOpened = ImGuiUiDrawer::begin_tab_item("All###display_recorder_all_tabitem"))){
            display.drawOnlyCloudId  = -1;

            if(m_redrawClouds || has_to_redraw_clouds()){
                draw_clouds_to_fbo();
            }
            m_redrawClouds = false;

            draw_all_clouds_drawers_in_one_tab();
            ImGui::EndTabItem();
        }

        // per client
        int previousCloudIdDisplayed = display.drawOnlyCloudId;
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            if (ImGuiUiDrawer::begin_tab_item(std::format("[{}]###display_recorder_per_client_tabitem_{}", ii, ii).c_str())){
                display.drawOnlyCloudId  = ii;

                if(m_redrawClouds || has_to_redraw_clouds()){
                    draw_clouds_to_fbo();
                }
                m_redrawClouds = false;

                draw_cloud_drawer_tab(ii, focusWindow, "display_recorder"sv);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();

        if((allTabOpened != m_allTabOpened) || (previousCloudIdDisplayed != display.drawOnlyCloudId)){
            m_redrawClouds = true;
            m_allTabOpened = allTabOpened;
        }
    }
}

