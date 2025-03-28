
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

#include "dcg_main_menu_bar_drawer.hpp"

// base
#include "utility/string.hpp"
#include "utility/io_file.hpp"
#include "utility/logger.hpp"
#include "depth-camera/settings/dc_settings_paths.hpp"

// opengl-utility
#include "opengl/shader/shaders_manager.hpp"

// imgui-opengl-engine
#include "imgui/extra/ImGuiFileDialog.h"

// local
#include "dcg_signals.hpp"

using namespace tool;
using namespace tool::graphics;

auto DCGMainMenuBarDrawer::draw() -> void{

    // draw menu
    bool openpopuptemp = false;
    if (ImGui::BeginMainMenuBar()){

        if (ImGui::BeginMenu("Settings")){

            if (ImGui::BeginMenu("Save")){
                if(ImGui::MenuItem("PC Host server JSON file###save_pc_host_server_json_file")){
                    Log::message("Save to PC Host server JSON file\n");
                    DCGSignals::get()->save_global_settings_file_signal(DCSettingsPaths::get()->server.string());
                }
                if(ImGui::MenuItem("Specific server JSON file###save_specific_server_json_file")){
                    Log::message("Save to specific server JSON file\n");
                    ImGuiFileDialog::Instance()->OpenDialog("Save PC Host server JSON file", "Choose file to save", ".json", ".");
                }
                if(ImGui::MenuItem("Default server JSON FILE###save_default_server_json_file")){
                    Log::message("Save to default server JSON file\n");
                    DCGSignals::get()->save_global_settings_file_signal(DCSettingsPaths::get()->defaultServer.string());
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Load")){
                if(ImGui::MenuItem("PC Host server JSON file###load_pc_host_server_json_file")){
                    Log::message("Load PC Host server JSON file\n");
                    DCGSignals::get()->load_global_settings_file_signal(DCSettingsPaths::get()->server.string());
                }
                if(ImGui::MenuItem("Specific server JSON file###load_specific_server_json_file")){
                    Log::message("Load specific server JSON file\n");
                    ImGuiFileDialog::Instance()->OpenDialog("Load PC Host server JSON file", "Choose file to load", ".json", ".");
                }
                if(ImGui::MenuItem("Default server JSON file###load_default_server_json_file")){
                    Log::message("Load default server JSON file\n");
                    DCGSignals::get()->load_global_settings_file_signal(DCSettingsPaths::get()->defaultServer.string());
                }

                if(ImGui::MenuItem("Device subpart from specific JSON file###load_device_subpart_specific_server_json_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load device subpart from specific JSON file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Filters subpart from specific JSON file###load_filters_subpart_specific_server_json_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load filters subpart from specific JSON file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Color subpart from specific JSON file###load_color_subpart_specific_server_json_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load color subpart from specific JSON file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Model subpart from specific JSON file###load_model_subpart_specific_server_json_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load model subpart from specific JSON file", "Choose file to load", ".json", ".");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Reset")){

                if(ImGui::MenuItem("Device###reset_device_settings")){
                    Log::message("Reset device settings\n");
                    DCGSignals::get()->reset_device_settings_signal();
                }
                if(ImGui::MenuItem("Filters###reset_filters_settings")){
                    Log::message("Reset filters settings\n");
                    DCGSignals::get()->reset_filters_settings_signal();
                }
                if(ImGui::MenuItem("Color###reset_color_settings")){
                    Log::message("Reset color settings\n");
                    DCGSignals::get()->reset_color_settings_signal();
                }
                if(ImGui::MenuItem("Model###reset_model_settings")){
                    Log::message("Reset model settings\n");
                    DCGSignals::get()->reset_model_settings_signal();
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Recorder")){
            if(ImGui::MenuItem("Save current cloud file")){
                ImGuiFileDialog::Instance()->OpenDialog("Save cloud", "Choose file to save", ".obj", ".");
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

                if(ImGui::MenuItem("Disable sending failure")){
                    DCGSignals::get()->sending_failure_signal(false, 0);
                }

                if(ImGui::MenuItem("Enable sending failure 1%")){
                    DCGSignals::get()->sending_failure_signal(true, 1);
                }

                if(ImGui::MenuItem("Enable sending failure 10%")){
                    DCGSignals::get()->sending_failure_signal(true, 10);
                }

                if(ImGui::MenuItem("Enable sending failure 50%")){
                    DCGSignals::get()->sending_failure_signal(true, 50);
                }

                if(ImGui::MenuItem("Enable sending failure 90%")){
                    DCGSignals::get()->sending_failure_signal(true, 90);
                }

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


    // dialogs
    ImGuiWindowFlags flags = 0;


    // # save global settings file
    if (ImGuiFileDialog::Instance()->Display("Save PC Host server JSON file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Save PC Host server JSON file\n");
            DCGSignals::get()->save_global_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load PC Host server JSON file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Load PC Host server JSON file\n");
            DCGSignals::get()->load_global_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // # load
    if (ImGuiFileDialog::Instance()->Display("Load device subpart from specific JSON file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Load device subpart from specific JSON file\n");
            DCGSignals::get()->load_subpart_device_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load filters subpart from specific JSON file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Load filters subpart from specific JSON file\n");
            DCGSignals::get()->load_subpart_filters_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load color subpart from specific JSON file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Load color subpart from specific JSON file\n");
            DCGSignals::get()->load_subpart_color_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load model subpart from specific JSON file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("Load model subpart from specific JSON file\n");
            DCGSignals::get()->load_subpart_model_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // # save current recorder cloud
    if (ImGuiFileDialog::Instance()->Display("Save cloud", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Log::message("save cloud to file\n");
            DCGSignals::get()->save_cloud_to_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }


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
}
