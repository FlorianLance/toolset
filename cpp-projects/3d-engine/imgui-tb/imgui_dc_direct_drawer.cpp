
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

#include "imgui_dc_direct_drawer.hpp"

// std
#include <format>

// base
#include "io/cloud_io.hpp"

// local
#include "imgui_ui_drawer.hpp"

using namespace tool::graphics;

auto DCDirectDrawer::initialize(size_t nbGrabbers) -> void{
    DCCloudsSceneDrawer::initialize(nbGrabbers);
}

auto DCDirectDrawer::set_frame(size_t idGrabber, std::shared_ptr<cam::DCFrame> frame) -> void{
    if(update_from_frame(idGrabber, frame)){
        m_redrawClouds = true;
    }
}

auto DCDirectDrawer::redraw_clouds_to_fbo() -> void{

    if(m_redrawClouds || has_to_redraw_clouds()){

        // store visibility
        std::vector<int> cloudsVisibility;
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            cloudsVisibility.push_back(cloudsD[ii]->display.showCloud ? 1 : 0);
        }

        // display only current grabber id cloud if per tab
        if(!m_allTabOpened){
            for(size_t ii = 0; ii < cloudsD.size(); ++ii){
                if(cloudsD[ii]->display.showCloud){
                    cloudsD[ii]->display.showCloud = ii == m_currentTabOpened;
                }
            }
        }
        draw_clouds_to_fbo();

        // restore visibility
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            cloudsD[ii]->display.showCloud = cloudsVisibility[ii] == 1;
        }
    }
    m_redrawClouds = false;
}


auto DCDirectDrawer::draw(bool focus) -> void{

    redraw_clouds_to_fbo();

    static ImGuiID tabId = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_direct_tabbar")){

        // all
        if ((m_allTabOpened = ImGuiUiDrawer::begin_tab_item("All###display_direct_all_tabitem"))){
            draw_all_clouds_drawers_in_one_tab();
            ImGui::EndTabItem();
        }

        // per grabber
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            if (ImGuiUiDrawer::begin_tab_item(std::format("[{}]###display_direct_per_grabber_tabitem_{}", ii, ii).c_str())){
                m_currentTabOpened = ii;
                draw_cloud_drawer_tab(ii, focus, "display_direct"sv);
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }
}

auto DCDirectDrawer::draw_only_clouds() -> void{

    redraw_clouds_to_fbo();

    static ImGuiID tabId = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_direct_tabbar")){

        // all
        if (ImGuiUiDrawer::begin_tab_item("All###display_direct_all_tabitem")){
            draw_all_clouds_drawers_in_one_tab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

auto DCDirectDrawer::save_current_cloud(size_t idC, const std::string &path) -> void{
    io::CloudIO::save_cloud(path, cloudsD[idC]->lastFrame->cloud);
}

