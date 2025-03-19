
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

#include "dcp_left_panel_child_drawer.hpp"

// std
#include <format>

// base
#include "utility/logger.hpp"
#include "utility/string.hpp"
#include "utility/time.hpp"

// imgui-opengl-engine
#include "imgui-tb/imgui_dc_ui_drawer.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "imgui/extra/ImGuiFileDialog.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

// local
#include "dcp_signals.hpp"


using namespace tool;
using namespace tool::cam;
using namespace tool::graphics;
using namespace tool::net;
using namespace std::chrono;


auto DCPLeftPanelChildDrawer::initialize(size_t nbGrabbers) -> void{

    guiCurrentFromFiltersSelection = 0;
    guiCurrentFromFiltersSelection = 0;
    guiCurrentFromColorsSelection = 0;
    guiCurrentTargetFiltersSelection = 0;
    guiCurrentTargetColorsSelection = 0;

    fromFilters.clear();
    targetsFilters.clear();

    fromColor.clear();
    targetsColor.clear();

    for(size_t ii = 0; ii < nbGrabbers; ++ii){
        fromFilters.push_back(std::format("[{}] normal", ii));
        fromColor.push_back(std::format("[{}]", ii));
    }
    for(size_t ii = 0; ii < nbGrabbers; ++ii){
        fromFilters.push_back(std::format("[{}] calib", ii));
    }

    targetsFilters.push_back("All");
    targetsFilters.push_back("All normal");
    targetsFilters.push_back("All calib");
    targetsColor.push_back("All");
    for(const auto &filter : fromFilters){
        targetsFilters.push_back(filter);
    }
    for(const auto &color : fromColor){
        targetsColor.push_back(color);
    }
}

auto DCPLeftPanelChildDrawer::draw(geo::Pt2f size, int windowFlags, DCPModel *model) -> void {

    if(ImGui::BeginChild("###left_panel_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        if(ImGui::BeginChild("###settings_child", ImVec2(size.x(), size.y()-100), true, windowFlags)){

            if(ImGui::BeginTabBar("###settings_tabbar1")){
                draw_player_tab_item(model->player);
                ImGui::EndTabBar();
            }

        }
        ImGui::EndChild();

        if(ImGui::BeginChild("###tools_child", ImVec2(size.x(), 80), true, windowFlags)){
            ImGui::Text("Tools:");
            ImGui::Indent();
            // ImGui::ColorPicker4()
            ImGuiColorEditFlags flags = ImGuiColorEditFlags_DisplayHex;// | ImGuiColorEditFlags_NoAlpha;
            ImGui::ColorEdit4("First last pixel color", firstLastClickedPixelColor.array.data(),flags);
            ImGui::ColorEdit4("Second last pixel color", secondLastClickedPixelColor.array.data(),flags);
            ImGui::Unindent();
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

}

auto DCPLeftPanelChildDrawer::update_selected_color(const geo::Pt4f &color) -> void{
    secondLastClickedPixelColor = firstLastClickedPixelColor;
    firstLastClickedPixelColor = color;
}





auto DCPLeftPanelChildDrawer::draw_player_tab_item(cam::DCVideoPlayer &player) -> void{

    if(DCUIDrawer::draw_dc_player_tab_item("Player###settings_player_tabitem", player.states, player.settings)){
        DCPSignals::get()->update_player_settings_signal(player.settings);
    }

    if(player.states.play){
        DCPSignals::get()->start_player_signal();
    }
    if(player.states.pause){
        DCPSignals::get()->pause_player_signal();
    }
    if(player.states.restart){
        DCPSignals::get()->restart_player_signal();
    }
    if(player.states.removeUntil){
        DCPSignals::get()->remove_until_current_frame_player_signal();
    }
    if(player.states.removeAfter){
        DCPSignals::get()->remove_after_current_frame_player_signal();
    }
    if(player.states.merge){
        DCPSignals::get()->merge_player_signal();
    }
    if(player.states.info){
        DCPSignals::get()->info_player_signal();
    }
    if(player.states.moveTime){
        DCPSignals::get()->set_player_time_signal(player.states.currentTime);
    }

    if (ImGuiFileDialog::Instance()->Display("Save cloud")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCPSignals::get()->save_cloud_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Save video")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCPSignals::get()->save_video_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Load video")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCPSignals::get()->load_video_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    player.states.reset_actions();
}

auto to_imvec4(const geo::Pt4f &pt) -> ImVec4{
    return ImVec4(pt.x(),pt.y(),pt.z(),pt.w());
}

auto DCPLeftPanelChildDrawer::draw_ui_tab_item(DCPUiSettings &ui) -> void {

    if (ImGuiUiDrawer::begin_tab_item("UI###settings_ui_tabitem")){
        ImGui::Checkbox("Focus window", &ui.focusWindow);
        ImGui::EndTabItem();
    }
}

auto draw_config_file_name(const std::optional<std::string> &filePath) -> void{
    if(filePath.has_value()){
        auto s = String::split_path_and_filename(filePath.value());
        if(s.second.contains("default")){
            ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{1.f,0.5f,0.15f,1.f});
            return;
        }else if(!filePath.value().empty()){
            ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{0,1.f,0,1.f});
            return;
        }
    }
    ImGuiUiDrawer::text("No file loaded."sv, geo::Pt4f{1.f,0.0f,0.0f,1.f});
}


auto DCPLeftPanelChildDrawer::draw_display_tab_item(DCClient &client) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Display###display_settings_tabitem")){
        return;
    }

    // ImGui::Spacing();
    // ImGui::Text("All grabbers");
    // ImGui::SameLine();
    // if(ImGui::Button("Send###display_send_all")){
    //     DCPSignals::get()->update_scene_display_settings_signal(client.settings.sceneDisplayS);
    //     for(auto &clientDeviceS : client.settings.devicesS){
    //         DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
    //     }
    // }

    // ImGui::Indent();

    // ImGui::Spacing();

    ImGuiUiDrawer::title("APPLY TO ALL"sv);
    ImGuiUiDrawer::title("COLORS"sv, {0.5f,1.f,0.5f,1.f});
    ImGui::Indent();

    if(ImGui::Button("Per camera###force_cloud_color_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.forceColor = true;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Real###real_cloud_color_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.forceColor = false;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    auto factorUnicolor = client.settings.devicesS.front().displayS.factorUnicolor;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Color factor", &factorUnicolor, 0.01f, 0.f, 1.f)){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.factorUnicolor = factorUnicolor;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::Unindent();
    ImGui::Spacing();

    ImGuiUiDrawer::title("GEOMETRY"sv, {0.5f,1.f,0.5f,1.f});
    ImGui::Indent();

    if(ImGui::Button("Use backface culling###use_backfaceculling_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.backFaceCulling = true;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Disable backface culling###disable_backfaceculling_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.backFaceCulling = false;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    if(ImGui::Button("Use circles###use_circles_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.circles = true;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Use points###use_points_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.circles = false;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::SetNextItemWidth(100.f);
    {
        ImGuiDragS  pointSizeD = {100.f, true, true, true, true, false};
        ImGuiFloatS pointSizeFS = {5.f, 0.1f, 30.0f, 0.1f, 0.1f};
        auto pointSizeV = client.settings.devicesS.front().displayS.pointSize;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Point size","display_size_points_all", &pointSizeV, pointSizeFS, pointSizeD)){
            for(auto &clientDeviceS : client.settings.devicesS){
                clientDeviceS.displayS.pointSize = pointSizeV;
                DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
            }
        }
    }

    {
        ImGuiDragS  squareSizeD = {100.f, true, true, true, true, false};
        ImGuiFloatS squareSizeFS = {0.005f, 0.001f, 0.05f, 0.0001f, 0.001f};
        float squareSizeV = client.settings.devicesS.front().displayS.squareSize;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Square size","display_square_size_all", &squareSizeV, squareSizeFS, squareSizeD)){
            for(auto &clientDeviceS : client.settings.devicesS){
                clientDeviceS.displayS.squareSize = squareSizeV;
                DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
            }
        }
    }

    {
        ImGuiDragS  circleRadiusD = {100.f, true, true, true, true, false};
        ImGuiFloatS circleRadiusFS = {0.25f, 0.00f, 0.5f, 0.001f, 0.001f};
        float circleRadiusV = client.settings.devicesS.front().displayS.circleRadius;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Circle radius","display_circle_radius_all", &circleRadiusV, circleRadiusFS, circleRadiusD)){
            for(auto &clientDeviceS : client.settings.devicesS){
                clientDeviceS.displayS.circleRadius = circleRadiusV;
                DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
            }
        }
    }

    ImGui::Unindent();
    ImGui::Spacing();

    ImGuiUiDrawer::title("VISIBILITY"sv, {0.5f,1.f,0.5f,1.f});
    ImGui::Indent();

    ImGui::Text("Clouds:");
    ImGui::SameLine();
    if(ImGui::Button("Show all###show_clouds_alls")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCapture = true;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Hide all###hide_clouds_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCapture = false;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Text("Cameras frustums:");
    ImGui::SameLine();
    if(ImGui::Button("Show all###show_frustums_alls")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCameraFrustum = true;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Hide all###hide_frustums_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCameraFrustum = false;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Text("Filterings gizmos:");
    ImGui::SameLine();
    if(ImGui::Button("Show all###show_filtering_gizmos_alls")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showFilteringGizmos = true;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Hide all###hide_filtering_gizmos_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showFilteringGizmos = false;
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Unindent();


    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_tabbar")){
        return;
    }

    if(DCUIDrawer::draw_dc_scene_display_setings_tab_item("Scene###scene_display_tabitem", client.settings.sceneDisplayS)){
        DCPSignals::get()->update_scene_display_settings_signal(client.settings.sceneDisplayS);
    }

    for(auto &clientDeviceS : client.settings.devicesS){
        if(DCUIDrawer::draw_dc_cloud_display_setings_tab_item(std::format("[{}]###{}_cloud_display_tabitem", clientDeviceS.id, clientDeviceS.id), clientDeviceS.displayS)){
            DCPSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto graphics::DCPLeftPanelChildDrawer::draw_model_tab_item(cam::DCClient &client) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Model###model_settings_tabitem")){
        return;
    }

    ImGui::Spacing();

    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###model_settings_tabbar")){
        return;
    }

    for(auto &clientDeviceS : client.settings.devicesS){
        if(DCUIDrawer::draw_dc_model_tab_item(std::format("[{}]###{}_cloud_calibration_tabitem", clientDeviceS.id, clientDeviceS.id), clientDeviceS.modelS)){
            DCPSignals::get()->update_model_settings_signal(clientDeviceS.id, clientDeviceS.modelS);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}




