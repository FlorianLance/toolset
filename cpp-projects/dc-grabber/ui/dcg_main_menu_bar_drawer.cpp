
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

// 3d-engine
#include "imgui/extra/ImGuiFileDialog.h"

// local
#include "dcg_signals.hpp"

using namespace tool;
using namespace tool::graphics;

auto DCGMainMenuBarDrawer::draw() -> void{

    // draw menu
    if (ImGui::BeginMainMenuBar()){

        if (ImGui::BeginMenu("Settings")){

            if (ImGui::BeginMenu("Global")){
                if(ImGui::MenuItem("Save to specific file###save_global_settings_to_specific_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Global settings specific file", "Choose file to save", ".json", ".");
                }
                if(ImGui::MenuItem("Save to hostname file###save_global_settings_to_hostname_file")){
                    Logger::message("Save to default file\n");
                    DCGSignals::get()->save_global_settings_file_signal(DCSettingsPaths::get()->server.string());
                }
                if(ImGui::MenuItem("Save to default file###save_global_settings_to_default_file")){
                    Logger::message("Save to default file\n");
                    DCGSignals::get()->save_global_settings_file_signal(DCSettingsPaths::get()->defaultServer.string());
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Device")){

                if(ImGui::MenuItem("Reset###reset_device_settings")){
                    DCGSignals::get()->reset_device_settings_signal();
                }

                if(ImGui::MenuItem("Load device subpart from specific global file###load_device_subpart_from_global_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load device subpart from specific global file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Load from default global file###load_device_subpart_from_default_global_file")){
                    DCGSignals::get()->load_default_device_settings_file_signal();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Filters")){

                if(ImGui::MenuItem("Reset###reset_filters_settings")){
                    DCGSignals::get()->reset_filters_settings_signal();
                }

                if(ImGui::MenuItem("Load subpart from specific global file###load_filters_subpart_from_global_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load filters subpart from specific global file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Load from default global file###load_filters_subpart_from_default_global_file")){
                    DCGSignals::get()->load_default_filters_settings_file_signal();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Color")){

                if(ImGui::MenuItem("Reset###reset_color_settings")){
                    DCGSignals::get()->reset_color_settings_signal();
                }

                if(ImGui::MenuItem("Load subpart from specific global file###load_color_subpart_from_global_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load color subpart from specific global file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Load from default global file###load_color_subpart_from_default_global_file")){
                    DCGSignals::get()->load_default_color_settings_file_signal();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Model")){

                if(ImGui::MenuItem("Reset###reset_model_settings")){
                    DCGSignals::get()->reset_model_settings_signal();
                }

                if(ImGui::MenuItem("Load subpart from specific global file###load_model_subpart_from_global_file")){
                    ImGuiFileDialog::Instance()->OpenDialog("Load model subpart from specific global file", "Choose file to load", ".json", ".");
                }

                if(ImGui::MenuItem("Load from default global file###load_model_subpart_from_default_global_file")){
                    DCGSignals::get()->load_default_model_settings_file_signal();
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
        ImGui::EndMainMenuBar();
    }


    // dialogs
    ImGuiWindowFlags flags = 0;

    // # save global settings file
    if (ImGuiFileDialog::Instance()->Display("Global settings specific file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("save global settings file\n");
            DCGSignals::get()->save_global_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // # save current recorder cloud
    if (ImGuiFileDialog::Instance()->Display("Save cloud", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("save cloud to file\n");
            DCGSignals::get()->save_cloud_to_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // # load
    if (ImGuiFileDialog::Instance()->Display("Load device subpart from specific global file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("Load device subpart from specific global file\n");
            DCGSignals::get()->load_subpart_device_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load filters subpart from specific global file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("Load filters subpart from specific global file\n");
            DCGSignals::get()->load_subpart_filters_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load color subpart from specific global file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("Load color subpart from specific global file\n");
            DCGSignals::get()->load_subpart_color_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("Load model subpart from specific global file", flags, ImVec2(500,200))) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("Load model subpart from specific global file\n");
            DCGSignals::get()->load_subpart_model_settings_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
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
