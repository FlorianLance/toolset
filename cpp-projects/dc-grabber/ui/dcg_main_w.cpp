
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

#include "dcg_main_w.hpp"

// base
#include "utility/logger.hpp"

// 3d-engine
#include "imgui-tb/imgui_convert.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"

using namespace tool::graphics;
using namespace tool::cam;
using namespace tool::net;

auto DCGMainW::initialize() -> bool{

    Logger::log("DCGMainW::initialize\n");

    // init drawers
    deviceD.initialize();
    recorderD.initialize(1);

    return true;
}

auto DCGMainW::draw(geo::Pt2f size, DCGModel *model) -> void{

    menuD.draw();

    geo::Pt2f settingsSize = {400.f, size.y()-50.f};
    geo::Pt2f displaySize  = {size.x()-800.f, size.y()-50.f};
    geo::Pt2f logsSize     = {400.f, size.y()-50.f};

    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("###UiWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse)){

        // settings
        leftPanelD.draw(settingsSize, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize, model);

        ImGui::SameLine();

        if(ImGui::BeginChild("Main###MainChild", to_iv2(displaySize), true, ImGuiWindowFlags_NoScrollWithMouse)){


            if(ImGui::BeginTabBar("Main###MainTabBar")){

                if(ImGui::BeginTabItem("Device###device_tabitem")){
                    deviceD.draw(model->uiSettings.focusWindow);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Recorder###recorder_tabitem")){;
                    recorderD.draw(model->uiSettings.focusWindow);
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        logsD.draw("Logs###main_logs_imguilogs", to_iv2(logsSize));
    }
    ImGui::End();
}

auto DCGMainW::append_log(const std::string &log) -> void {
    logsD.add_log(log.c_str());
}
