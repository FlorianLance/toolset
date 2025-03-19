
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

#include "dcp_middle_panel_child_drawer.hpp"

// imgui-opengl-engine
#include "imgui-tb/imgui_helper.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"

using namespace tool;
using namespace tool::graphics;
using namespace tool::cam;

auto DCPMiddlePanelChildDrawer::initialize(size_t nbGrabbers) -> void {

}

auto DCPMiddlePanelChildDrawer::update() -> void{

}

auto DCPMiddlePanelChildDrawer::draw(geo::Pt2f size, const DCPUiSettings &uiS) -> void {

    auto windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
    if(ImGui::BeginChild("###display_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        static ImGuiID tabId = 0;
        if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_tabbar")){

            if((m_playerTabOpened = ImGuiUiDrawer::begin_tab_item("Player###player_tabitem"))){
                playerD.draw(uiS.focusWindow);
                ImGui::EndTabItem();
            }


            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}



