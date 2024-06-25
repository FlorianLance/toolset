
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

// local
#include "dcm_signals.hpp"

using namespace tool::graphics;
using namespace tool::net;
using namespace tool::cam;

auto DCMMainWindowDrawer::initialize(size_t nbGrabbers) -> bool{

    m_nbGrabbers = nbGrabbers;
    m_leftPanelD.initialize(nbGrabbers);
    m_middlePanelD.initialize(nbGrabbers);

    return true;
}

auto DCMMainWindowDrawer::draw(geo::Pt2f size, DCMSettings &settings, DCMStates &states) -> void{

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

    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("###UiWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse)){

        // settings
        auto sw = size.x();
        auto sh = size.y();
        m_leftPanelD.draw({450.f,sh-50.f}, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize, settings, states);
        ImGui::SameLine();
        m_middlePanelD.draw({sw-450.f,sh-50.f}, settings.uiS);
    }
    ImGui::End();
}

auto DCMMainWindowDrawer::draw_menu() -> void{

    if (ImGui::BeginMainMenuBar()){

        if (ImGui::BeginMenu("Settings")){

            if (ImGui::BeginMenu("Network")){

                if(ImGui::MenuItem("Save current network file###save_current_network")){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Irrelevant, SType::Network, SFile::Irrelevant, 0);
                }
                ImGui::EndMenu();
            }

            draw_device_sub_menu();
            draw_color_sub_menu();
            draw_filters_sub_menu();
            draw_model_sub_menu();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Recordings")){
            if (ImGui::BeginMenu("All###recording_all")){
                if(ImGui::MenuItem("Save all current clouds files")){
                 Logger::message("Save all current clouds\n");
                    ImGuiFileDialog::Instance()->OpenDialog("Save all current clouds", "Choose base name file to save", ".obj", ".");
                }
                ImGui::EndMenu();
            }

            for(size_t ii = 0; ii < m_nbGrabbers; ++ii){
                if (ImGui::BeginMenu(std::format("Grabber{}###recording_grabber_{}", ii, ii).c_str())){
                    if(ImGui::MenuItem(std::format("Save current cloud###save_current_cloud_recording_{}", ii).c_str())){
                        ImGuiFileDialog::Instance()->OpenDialog(std::format("Save current cloud {}",ii), "Choose file to save", ".obj", ".");
                    }
                    ImGui::EndMenu();
                }
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
        ImGui::EndMainMenuBar();
    }

    ImGuiWindowFlags flags = 0;
    // dialogs
    if (ImGuiFileDialog::Instance()->Display("Save all current clouds", flags, ImVec2(500,200))) {

        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("Save all current cloudsl\n");
            DCMSignals::get()->save_all_current_clouds_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    for(size_t ii = 0; ii < m_nbGrabbers; ++ii){
        if (ImGuiFileDialog::Instance()->Display(std::format("Save current cloud {}",ii), flags,  ImVec2(500,200))) {
            if (ImGuiFileDialog::Instance()->IsOk()){
                Logger::message((std::format("Save current cloud {}\n",ii)));
                DCMSignals::get()->save_current_cloud_signal(ii, ImGuiFileDialog::Instance()->GetFilePathName());
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }
}

auto DCMMainWindowDrawer::draw_device_sub_menu() -> void{


    if (ImGui::BeginMenu("Device")){

        if (ImGui::BeginMenu("All###device_all")){

            if(ImGui::MenuItem("Reset settings###reset_device_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::All, SType::Device, SFile::Irrelevant, 0);
            }

            if(ImGui::MenuItem("Save settings to current hostname grabber file###save_hostname_grabber_device_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, SType::Device, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Save all settings to a all-in-one hostname file###save_all_grabber_device_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, SType::Device, SFile::HostAllInOne, 0);
            }

            if(ImGui::MenuItem("Load settings from default file###load_default_file_device_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Device, SFile::Default, 0);
            }

            if(ImGui::MenuItem("Load settings from current hostname grabber file###load_hostname_grabber_file_device_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Device, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Load all settings from a all-in-one hostname file###load_all_grabber_device_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Device, SFile::HostAllInOne, 0);
            }

            ImGui::EndMenu();
        }


        for(size_t ii = 0; ii < m_nbGrabbers; ++ii){
            if (ImGui::BeginMenu(std::format("Grabber{}###device_grabber_{}", ii, ii).c_str())){

                if(ImGui::MenuItem(std::format("Reset settings###reset_device_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::Individual, SType::Device, SFile::Irrelevant, ii);
                }

                if(ImGui::MenuItem(std::format("Save settings to default file###save_default_device_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Individual, SType::Device, SFile::Default, ii);
                }

                if(ImGui::MenuItem(std::format("Save settings to current hostname grabber file###save_hostname_grabber_device_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Individual, SType::Device, SFile::Host, ii);
                }

                if(ImGui::MenuItem(std::format("Load settings from default file###load_default_device_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::Individual, SType::Device, SFile::Default, ii);
                }

                if(ImGui::MenuItem(std::format("Load settings from current hostname grabber file###load_hostname_grabber_device_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::Individual, SType::Device, SFile::Host, ii);
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMenu();
    }


}

auto DCMMainWindowDrawer::draw_color_sub_menu() -> void{

    if (ImGui::BeginMenu("Color")){

        if (ImGui::BeginMenu("All###color_all")){

            if(ImGui::MenuItem("Reset settings###reset_color_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::All, SType::Color, SFile::Irrelevant, 0);
            }

            if(ImGui::MenuItem("Save settings to current hostname grabber file###save_hostname_grabber_color_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, SType::Color, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Save all settings to a all-in-one hostname file###save_all_grabber_color_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, SType::Color, SFile::HostAllInOne, 0);
            }

            if(ImGui::MenuItem("Load settings from default file###load_default_file_color_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Color, SFile::Default, 0);
            }

            if(ImGui::MenuItem("Load settings from current hostname grabber file###load_hostname_grabber_file_color_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Color, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Load all settings from a all-in-one hostname file###load_all_grabber_color_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Color, SFile::HostAllInOne, 0);
            }

            ImGui::EndMenu();
        }

        for(size_t ii = 0; ii < m_nbGrabbers; ++ii){
            if (ImGui::BeginMenu(std::format("Grabber{}###color_grabber_{}", ii, ii).c_str())){

                if(ImGui::MenuItem(std::format("Reset settings###reset_color_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::Individual, SType::Color, SFile::Irrelevant, ii);
                }

                if(ImGui::MenuItem(std::format("Save settings to default file###save_default_color_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Individual, SType::Color, SFile::Default, ii);
                }

                if(ImGui::MenuItem(std::format("Save settings to current hostname grabber file###save_hostname_grabber_color_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Individual, SType::Color, SFile::Host, ii);
                }

                if(ImGui::MenuItem(std::format("Load settings from default file###load_default_color_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::Individual, SType::Color, SFile::Default, ii);
                }

                if(ImGui::MenuItem(std::format("Load settings from current hostname grabber file###load_hostname_grabber_color_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::Individual, SType::Color, SFile::Host, ii);
                }

                ImGui::EndMenu();
            }
        }
        ImGui::EndMenu();
    }
}

auto DCMMainWindowDrawer::draw_filters_sub_menu() -> void {

    static auto filtersSubMenu = [](size_t nbG, bool normalFilters){

        auto type = normalFilters ? SType::Filters : SType::CalibrationFilters;

        if (ImGui::BeginMenu("All###filters_all")){

            if(ImGui::MenuItem("Reset settings###reset_filters_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::All, type, SFile::Irrelevant, 0);
            }

            if(ImGui::MenuItem("Save filters to current hostname grabber file###save_hostname_grabber_file_filters_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, type, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Save all settings to a all-in-one hostname file###save_all_grabber_file_filters_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, type, SFile::HostAllInOne, 0);
            }

            if(ImGui::MenuItem("Load filters from default file###load_default_file_filters_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, type, SFile::Default, 0);
            }

            if(ImGui::MenuItem("Load filters from current hostname grabber file###load_hostname_grabber_file_filters_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, type, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Load all settings from a all-in-one hostname file###load_all_grabber_file_filters_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, type, SFile::HostAllInOne, 0);
            }


            ImGui::EndMenu();
        }

        for(size_t ii = 0; ii < static_cast<size_t>(nbG); ++ii){
            if (ImGui::BeginMenu(std::format("Grabber{}###filters_grabber_{}", ii, ii).c_str())){

                if(ImGui::MenuItem(std::format("Reset filters###reset_filters_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::Individual, type, SFile::Irrelevant, ii);
                }

                if(ImGui::MenuItem(std::format("Save filters to default file###save_default_filters_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Individual, type, SFile::Default, ii);
                }

                if(ImGui::MenuItem(std::format("Save filters to current hostname grabber file###save_hostname_grabber_filters_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::Individual, type, SFile::Host, ii);
                }

                if(ImGui::MenuItem(std::format("Load filters from default file###load_default_filters_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::Individual, type, SFile::Default, ii);
                }

                if(ImGui::MenuItem(std::format("Load filters from current hostname grabber file###load_hostname_grabber_filters_{}", ii).c_str())){
                    DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::Individual, type, SFile::Host, ii);
                }

                ImGui::EndMenu();
            }
        }
    };


    if (ImGui::BeginMenu("Filters")){

        if (ImGui::BeginMenu("Normal")){
            filtersSubMenu(m_nbGrabbers, true);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Calibration")){
            filtersSubMenu(m_nbGrabbers, false);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

auto DCMMainWindowDrawer::draw_model_sub_menu() -> void {

    if (ImGui::BeginMenu("Model")){


        if (ImGui::BeginMenu("All###models_all")){

            if(ImGui::MenuItem("Reset models###reset_models_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Reset, STarget::All, SType::Model, SFile::Irrelevant, 0);
            }

            if(ImGui::MenuItem("Save models to current hostname grabber file###save_hostname_grabber_model_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, SType::Model, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Save all models to a all-in-one hostname file###save_all_grabber_filters_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Save, STarget::All, SType::Model, SFile::HostAllInOne, 0);
            }

            if(ImGui::MenuItem("Load models from current hostname grabber file###load_hostname_grabber_file_models_all")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Model, SFile::Host, 0);
            }

            if(ImGui::MenuItem("Load all models from a all-in-one hostname file###load_all_grabber_file_models_all_in_one")){
                DCMSignals::get()->process_settings_action_signal(SAction::Load, STarget::All, SType::Model, SFile::HostAllInOne, 0);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

