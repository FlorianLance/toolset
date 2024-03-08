
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
    geo::Pt2f displaySize  = {size.x()-400.f, size.y()-50.f};

    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("UI###UiWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)){

        // settings
        leftPanelD.draw(settingsSize, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize, model->settings, model->states);

        ImGui::SameLine();

        if(ImGui::BeginChild("Main###MainChild", to_iv2(displaySize), true, ImGuiWindowFlags_NoScrollWithMouse)){

            std::int64_t captD          = model->device->get_capture_duration_ms();
            std::int64_t procD          = model->device->get_processing_duration_ms();
            std::int64_t convertImageD  = model->device->get_duration_micro_s("CONVERT_COLOR_IMAGE"sv);
            std::int64_t resizeImageD   = model->device->get_duration_micro_s("RESIZE_COLOR_IMAGE"sv);
            std::int64_t filterDepthD   = model->device->get_duration_micro_s("FILTER_DEPTH"sv);
            std::int64_t totalD         = captD+procD;

            auto framesS = 1000.f/totalD;
            if(framesS > 100){
                framesS = 0;
            }
            ImGuiUiDrawer::text(std::format("Images/s: {:5.2} ", framesS));

            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("Times (ms): Total: {:3} ", captD+procD));
            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("Capture: {:3}", captD));
            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("Processing: {:3}", procD));
            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("[Sub] Convert color: {:5.3}", convertImageD*0.001f));
            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("[Sub] Resize color: {:5.3}", resizeImageD*0.001f));
            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("[Sub] Filter depth: {:5.3}", filterDepthD*0.001f));
            ImGui::SameLine();
            ImGuiUiDrawer::text(std::format("Sending: {:5.3f}",(0.001*model->connection.last_frame_sending_duration_micros_s())));

            if(ImGui::BeginTabBar("Main###MainTabBar")){

                if(ImGui::BeginTabItem("Device###device_tabitem")){
                    deviceD.draw(model->settings.displayS.focusWindow);
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Recorder###recorder_tabitem")){;
                    recorderD.draw();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

