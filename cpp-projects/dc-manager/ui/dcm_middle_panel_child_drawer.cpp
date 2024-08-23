
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

#include "dcm_middle_panel_child_drawer.hpp"

// 3d-engine
#include "imgui-tb/imgui_helper.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"

using namespace tool;
using namespace tool::graphics;
using namespace tool::cam;

auto DCMMiddlePanelChildDrawer::initialize(size_t nbGrabbers) -> void {
    directD.initialize(nbGrabbers);
    recorderD.initialize(nbGrabbers);
    calibratorD.initialize(nbGrabbers);
}

auto DCMMiddlePanelChildDrawer::update() -> void{
    directD.update();
}

auto DCMMiddlePanelChildDrawer::draw(geo::Pt2f size, const DCMUiSettings &uiS) -> void {

    auto windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
    if(ImGui::BeginChild("###display_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        static ImGuiID tabId = 0;
        if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_tabbar")){

            if((m_directTabOpened = ImGuiUiDrawer::begin_tab_item("Direct###display_direct_tabitem"))){
                directD.draw(uiS.focusWindow);
                ImGui::EndTabItem();
            }

            if((m_recordingTabOpened = ImGuiUiDrawer::begin_tab_item("Recorder###display_recorder_tabitem"))){
                recorderD.draw(uiS.focusWindow);
                ImGui::EndTabItem();
            }
            if((m_playerTabOpened = ImGuiUiDrawer::begin_tab_item("Player###display_player_tabitem"))){
                playerD.draw(uiS.focusWindow);
                ImGui::EndTabItem();
            }
            if((m_calibrationTabOpened = ImGuiUiDrawer::begin_tab_item("Calibrator###calibrator_direct_tabitem"))){

                auto sizeW = content_region_size_available();
                if(ImGui::BeginChild("###calibrator_child1", ImVec2(sizeW.x()*0.6, sizeW.y()*0.9), true, windowFlags)){
                    calibratorD.draw();
                }
                ImGui::EndChild();

                ImGui::SameLine();

                if(ImGui::BeginChild("###calibrator_child2", ImVec2(size.x()*0.4, size.y()*0.9), true, windowFlags)){
                    directD.draw_only_clouds();
                }
                ImGui::EndChild();                
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}



