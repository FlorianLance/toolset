
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


#include "dcm_main_window_drawer.hpp"

// base
#include "utility/logger.hpp"
#include "utility/io_file.hpp"
#include "utility/string.hpp"

// opengl-utility
#include "opengl/shader/shaders_manager.hpp"

// 3d-engine
#include "imgui/extra/ImGuiFileDialog.h"
#include "imgui-tb/imgui_convert.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"

// local
#include "dcm_signals.hpp"

using namespace tool::graphics;
using namespace tool::net;
using namespace tool::cam;

auto DCMMainWindowDrawer::initialize(size_t nbDevices) -> bool{

    m_nbDevices = nbDevices;
    m_leftPanelD.initialize(nbDevices);
    m_middlePanelD.initialize(nbDevices);
    feedbacksLogs.resize(nbDevices);

    return true;
}

auto DCMMainWindowDrawer::update() -> void{
    m_middlePanelD.update();
}

auto DCMMainWindowDrawer::draw(geo::Pt2f size, DCMModel *model) -> void{

    // menu
    draw_menu();

    // show demo
    if (m_showDemoWindow){
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

    // show metrics
    if (m_showMetricsWindow){
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowMetricsWindow(&m_showMetricsWindow);
    }

    ImGui::SetNextWindowPos(ImVec2(0, 20));
    if (ImGui::Begin("###UiWindow", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse)){

        geo::Pt2f settingsSize = {450.f, size.y()-40.f};
        geo::Pt2f displaySize  = {size.x()-800.f, size.y()-40.f};
        geo::Pt2f logsSize     = {320.f, size.y()-40.f};

        m_leftPanelD.draw(settingsSize, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize, model);
        ImGui::SameLine();
        m_middlePanelD.draw(displaySize, model->uiSettings);

        ImGui::SameLine();

        if(ImGui::BeginChild("Logs###logs_child", to_iv2(logsSize), true)){
            if(ImGui::BeginTabBar("Logs###logs_tab_bar")){ // , ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar

                if (ImGui::BeginTabItem("Logs###logs_tab_item", nullptr)){
                    ImGui::Text("Global logs:");
                    globalLogs.draw("global_logs", to_iv2(logsSize + geo::Pt2f{0.f,-50.f}));
                    ImGui::EndTabItem();
                }

                for(size_t idF = 0; idF < feedbacksLogs.size(); ++idF){
                    if (ImGui::BeginTabItem(std::format("F{}###feedback_logs_{}_tabitem", idF, idF).c_str())){
                        ImGuiUiDrawer::text(std::format("Feedback logs for device {}:", idF));
                        feedbacksLogs[idF].draw(std::format("device_{}_feedback", idF).c_str()), to_iv2(logsSize + geo::Pt2f{0.f,-50.f});
                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();


    }
    ImGui::End();
}

auto DCMMainWindowDrawer::draw_menu() -> void{

    bool openpopuptemp = false;
    if (ImGui::BeginMainMenuBar()){

        if (ImGui::BeginMenu("Settings")){

            if (ImGui::BeginMenu("Save")){
                if(ImGui::MenuItem("PC Host client JSON file###save_host_file")){
                    m_currentAction.update(SAction::Save, STarget::Irrelevant, SType::Global, SFile::Normal);
                }
                if(ImGui::MenuItem("Specific client JSON file###save_specific_global_file")){
                    m_currentAction.update(SAction::Save, STarget::Irrelevant, SType::Global, SFile::Specific);
                    m_currentAction.hasPath = true;
                    ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON file to save", ".json", ".");
                }
                if(ImGui::MenuItem("Default client JSON file###save_default_global_file")){
                    m_currentAction.update(SAction::Save, STarget::Irrelevant, SType::Global, SFile::Default);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Load")){

                if(ImGui::MenuItem("PC Host client JSON file###load_pc_host_file")){
                    m_currentAction.update(SAction::Load, STarget::Irrelevant, SType::Global, SFile::Normal);
                }

                if(ImGui::MenuItem("Specific client JSON file###load_specific_global_file")){
                    m_currentAction.update(SAction::Load, STarget::Irrelevant, SType::Global, SFile::Specific);
                    m_currentAction.hasPath = true;
                    ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Default client JSON file###load_default_global_file")){
                    m_currentAction.update(SAction::Load, STarget::Irrelevant, SType::Global, SFile::Default);
                }

                if (ImGui::BeginMenu("Only device")){

                    if(ImGui::MenuItem("All###load_all_device_file")){
                        m_currentAction.update(SAction::Load, STarget::All, SType::Device, SFile::Irrelevant);
                        m_currentAction.hasPath = true;
                        ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###load_device_file_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Load, STarget::Individual, SType::Device, SFile::Irrelevant, ii);
                            m_currentAction.hasPath = true;
                            ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Only filters")){

                    if(ImGui::MenuItem("All###load_all_filters_file")){
                        m_currentAction.update(SAction::Load, STarget::All, SType::Filters, SFile::Irrelevant);
                        m_currentAction.hasPath = true;
                        ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###load_filters_file_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Load, STarget::Individual, SType::Filters, SFile::Irrelevant, ii);
                            m_currentAction.hasPath = true;
                            ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Only calibration filters")){

                    if(ImGui::MenuItem("All###load_all_calibration_filters_file")){
                        m_currentAction.update(SAction::Load, STarget::All, SType::CalibrationFilters, SFile::Irrelevant);
                        m_currentAction.hasPath = true;
                        ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device filters [{}]###load_calibration_filters_file_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Load, STarget::Individual, SType::CalibrationFilters, SFile::Irrelevant, ii);
                            m_currentAction.hasPath = true;
                            ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Only color")){

                    if(ImGui::MenuItem("All###load_all_color_file")){
                        m_currentAction.update(SAction::Load, STarget::All, SType::Color, SFile::Irrelevant);
                        m_currentAction.hasPath = true;
                        ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###load_color_file_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Load, STarget::Individual, SType::Color, SFile::Irrelevant, ii);
                            m_currentAction.hasPath = true;
                            ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Only model")){

                    if(ImGui::MenuItem("All###load_all_model_file")){
                        m_currentAction.update(SAction::Load, STarget::All, SType::Model, SFile::Irrelevant);
                        m_currentAction.hasPath = true;
                        ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###load_model_file_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Load, STarget::Individual, SType::Model, SFile::Irrelevant, ii);
                            m_currentAction.hasPath = true;
                            ImGuiFileDialog::Instance()->OpenDialog("GetJSONPath", "Choose JSON to load", ".json", ".");
                        }
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Reset")){

                if (ImGui::BeginMenu("Device")){

                    if(ImGui::MenuItem("All###reset_all_device_settings")){
                        m_currentAction.update(SAction::Reset, STarget::All, SType::Device, SFile::Irrelevant);
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###reset_device_settings_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Reset, STarget::Individual, SType::Device, SFile::Irrelevant, ii);
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Filters")){

                    if(ImGui::MenuItem("All###reset_all_filters_settings")){
                        m_currentAction.update(SAction::Reset, STarget::All, SType::Filters, SFile::Irrelevant);
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###reset_filters_settings_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Reset, STarget::Individual, SType::Filters, SFile::Irrelevant, ii);
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Calibration filters")){

                    if(ImGui::MenuItem("All###reset_all_calibration_filters_settings")){
                        m_currentAction.update(SAction::Reset, STarget::All, SType::CalibrationFilters, SFile::Irrelevant);
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###reset_calibration_filters_settings_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Reset, STarget::Individual, SType::CalibrationFilters, SFile::Irrelevant, ii);
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Color")){

                    if(ImGui::MenuItem("All###reset_all_color_settings")){
                        m_currentAction.update(SAction::Reset, STarget::All, SType::Color, SFile::Irrelevant);
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###reset_color_settings_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Reset, STarget::Individual, SType::Color, SFile::Irrelevant, ii);
                        }
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Model")){

                    if(ImGui::MenuItem("All###reset_all_model_settings")){
                        m_currentAction.update(SAction::Reset, STarget::All, SType::Model, SFile::Irrelevant);
                    }

                    for(size_t ii = 0; ii < m_nbDevices; ++ii){
                        if(ImGui::MenuItem(std::format("Device [{}]###reset_model_settings_{}", ii, ii).c_str())){
                            m_currentAction.update(SAction::Reset, STarget::Individual, SType::Model, SFile::Irrelevant, ii);
                        }
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Recordings")){
            if (ImGui::BeginMenu("All###recording_all")){
                if(ImGui::MenuItem("Save all current clouds files")){
                    Log::message("Save all current clouds\n");
                    ImGuiFileDialog::Instance()->OpenDialog("Save all current clouds", "Choose base name file to save", ".obj", ".");
                }
                ImGui::EndMenu();
            }

            for(size_t ii = 0; ii < m_nbDevices; ++ii){
                if (ImGui::BeginMenu(std::format("Device [{}]###recording_device_{}", ii, ii).c_str())){
                    if(ImGui::MenuItem(std::format("Save current cloud###save_current_cloud_recording_{}", ii).c_str())){
                        ImGuiFileDialog::Instance()->OpenDialog(std::format("Save current cloud {}",ii), "Choose file to save", ".obj", ".");
                    }
                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")){
            if(ImGui::MenuItem("About")){
                openpopuptemp = true;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Developer")){

            if (ImGui::BeginMenu("Shaders")){
                for(std::string_view alias : ShadersManager::get_instance()->all_aliases()){
                    if (ImGui::BeginMenu(std::format("Shader {}", alias).c_str())){
                        for(const auto &path : ShadersManager::get_instance()->get_ptr(alias)->shaders_file_paths()){
                            if(ImGui::MenuItem(std::format("{}",String::split(path, ".").back()).c_str())){
                                File::execute_file(path);
                            }
                        }
                        ImGui::EndMenu();
                    }
                }

                if(ImGui::MenuItem("Reload shaders")){
                    ShadersManager::get_instance()->reload_all_shaders();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Windows")){
                ImGui::MenuItem("Demo window", nullptr, &m_showDemoWindow);
                ImGui::MenuItem("Metrics window", nullptr, &m_showMetricsWindow);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Debug")){
                if(ImGui::MenuItem("Make it crash")){
                    abort();
                }
                ImGui::EndMenu();
            }


            ImGui::EndMenu();
        }

        if (openpopuptemp == true) {
            ImGui::OpenPopup("About");
        }

        if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Project repository: https://github.com/FlorianLance/toolset/blob/main/LICENSE");
            ImGui::Spacing();
            ImGui::Text("Author: Florian Lance ");
            ImGui::Text("Github: https://github.com/FlorianLance");
            ImGui::Text("Linkedin: https://www.linkedin.com/in/florian-lance/");

            if (ImGui::Button("Close", ImVec2(60, 0))){
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


        ImGui::EndMainMenuBar();
    }

    ImGuiWindowFlags flags = 0;
    // dialogs
    if (ImGuiFileDialog::Instance()->Display("Save all current clouds", flags, ImVec2(500,200))) {

        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Save all current cloudsl\n");
            // DCMSignals::get()->save_all_current_clouds_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    for(size_t ii = 0; ii < m_nbDevices; ++ii){
        if (ImGuiFileDialog::Instance()->Display(std::format("Save current cloud {}",ii), flags,  ImVec2(500,200))) {
            if (ImGuiFileDialog::Instance()->IsOk()){
                Log::message((std::format("Save current cloud {}\n",ii)));
                DCMSignals::get()->save_current_cloud_signal(ii, ImGuiFileDialog::Instance()->GetFilePathName());
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    if(m_currentAction.hasPath && m_currentAction.valid){
        if (ImGuiFileDialog::Instance()->Display("GetJSONPath", flags,  ImVec2(500,200))) {
            if (ImGuiFileDialog::Instance()->IsOk()){
                m_currentAction.path = ImGuiFileDialog::Instance()->GetFilePathName();
                DCMSignals::get()->process_settings_action_signal(m_currentAction);
                m_currentAction.valid = false;
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }else if(m_currentAction.valid){
        DCMSignals::get()->process_settings_action_signal(m_currentAction);
        m_currentAction.valid = false;
    }
}



