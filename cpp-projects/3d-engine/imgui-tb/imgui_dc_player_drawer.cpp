

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

#include "imgui_dc_player_drawer.hpp"

// std
#include <format>

// local
#include "imgui_ui_drawer.hpp"

using namespace tool::graphics;

auto DCPlayerDrawer::initialize(const std::vector<cam::DCModelSettings> &models) -> void {

    DCCloudsSceneDrawer::initialize(models.size());
    for(size_t ii = 0; ii < models.size(); ++ii){
        update_model_settings(ii, models[ii]);
        std::cout << models[ii].transformation << "\n";
    }

    m_currentFrames.resize(models.size());
    std::fill(std::begin(m_currentFrames), std::end(m_currentFrames), nullptr);        
}

auto DCPlayerDrawer::set_frame(size_t idC, std::shared_ptr<cam::DCFrame> frame) -> void{

    if(idC >= m_currentFrames.size()){
        return;
    }

    m_currentFrames[idC] = frame;

    if(update_from_frame(idC, m_currentFrames[idC])){
        m_redrawClouds = true;
    }
}

auto DCPlayerDrawer::draw() -> void{

    if(m_redrawClouds || has_to_redraw_clouds()){
        draw_clouds_to_fbo();
    }
    m_redrawClouds = false;

    bool focusWindow = false;

    // draw data
    static ImGuiID tabId = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_recorder_tabbar")){

        // all
        bool allTabOpened = false;
        if ((allTabOpened = ImGuiUiDrawer::begin_tab_item("All###display_player_all_tabitem"))){
            display.drawOnlyCloudId  = -1;
            draw_all_clouds_drawers_in_one_tab();
            ImGui::EndTabItem();
        }

        // per grabber
        int previousCloudIdDisplayed = display.drawOnlyCloudId;
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            if (ImGuiUiDrawer::begin_tab_item(std::format("[{}]###display_player_per_grabber_tabitem_{}", ii, ii).c_str())){
                display.drawOnlyCloudId  = ii;
                draw_cloud_drawer_tab(ii, focusWindow, "display_player"sv);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();

        if((allTabOpened != m_allTabOpened) || (previousCloudIdDisplayed != display.drawOnlyCloudId)){
            m_redrawClouds = true;
            m_allTabOpened = allTabOpened;
        }
    }


//    if(ImGui::Button("Test merge")){

//        player->merge_cameras(0.01f, geo::Pt3f{-20.f,-20.f,-20.f},geo::Pt3f{20.f,20.f,20.f});
//        initialize(1);

//        m_currentFrames.clear();
//        m_currentFrames.reserve(1);

//        cam::K4Model model;
//        model.transformation = player->video()->get_camera_data(0)->transform.conv<float>();
//        update_model(0, std::move(model));
//        m_currentFrames.push_back(std::make_shared<cam::K4Frame>());

//        m_redrawClouds = true;
//    }


//    if (ImGuiFileDialog::Instance()->Display("Load recording")) {
//        if (ImGuiFileDialog::Instance()->IsOk()){

//            player->load_from_file(ImGuiFileDialog::Instance()->GetFilePathName());
//            initialize(player->video()->nb_cameras());

//            m_currentFrames.clear();
//            m_currentFrames.reserve(player->video()->nb_cameras());
//            for(size_t ii = 0; ii < player->video()->nb_cameras(); ++ii){
//                cam::K4Model model;
//                model.transformation = player->video()->get_camera_data(ii)->transform.conv<float>();
//                update_model(ii, std::move(model));
//                m_currentFrames.push_back(std::make_shared<cam::K4Frame>());
//            }
//            m_redrawClouds = true;
//        }
//        ImGuiFileDialog::Instance()->Close();
//    }

//    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
//        if (ImGuiFileDialog::Instance()->IsOk()){
//            Logger::message(std::format("Save recording to {}\n", ImGuiFileDialog::Instance()->GetFilePathName()));
//            player->save_to_file(ImGuiFileDialog::Instance()->GetFilePathName());
//        }
//        ImGuiFileDialog::Instance()->Close();
//    }

//    if (ImGuiFileDialog::Instance()->Display("Save cloud")) {
//        if (ImGuiFileDialog::Instance()->IsOk()){
//            Logger::message(std::format("Save cloud to {}\n", ImGuiFileDialog::Instance()->GetFilePathName()));
//            cam::K4Frame f;
//            player->uncompress_frame(0, f);
//            files::CloudIO::save_cloud(ImGuiFileDialog::Instance()->GetFilePathName(), f.cloud);
//        }
//        ImGuiFileDialog::Instance()->Close();
//    }
}

