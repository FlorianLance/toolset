
/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

#include "k4sg_main_menu_bar_drawer.hpp"

// base
#include "utility/string.hpp"
#include "utility/io_file.hpp"
#include "utility/logger.hpp"

// 3d-engine
#include "engine/shaders_manager.hpp"
#include "imgui/extra/ImGuiFileDialog.h"

// local
#include "k4sg_signals.hpp"

using namespace tool::graphics;

auto K4SGMainMenuBarDrawer::draw() -> void{

    // draw menu
    if (ImGui::BeginMainMenuBar()){

        if (ImGui::BeginMenu("Settings")){
            if (ImGui::BeginMenu("Device")){
                if(ImGui::MenuItem("Reset device settings")){
                    K4SGSignals::get()->reset_device_settings_signal();
                }
                if(ImGui::MenuItem("Save device settings to default file")){
                    K4SGSignals::get()->save_device_settings_to_default_file_signal();
                }
                if(ImGui::MenuItem("Save device settings to current hostname file")){
                    K4SGSignals::get()->save_device_settings_to_current_hostname_file_signal();
                }

                if(ImGui::MenuItem("Load default device settings file")){
                    K4SGSignals::get()->load_default_device_settings_file_signal();
                }
                if(ImGui::MenuItem("Load current hostname device settings file")){
                    K4SGSignals::get()->load_current_hostname_device_settings_file_signal();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Filters")){
                if(ImGui::MenuItem("Reset filters")){
                    K4SGSignals::get()->reset_filters_signal();
                }
                if(ImGui::MenuItem("Save filters to default file")){
                    K4SGSignals::get()->save_filters_to_default_file_signal();
                }
                if(ImGui::MenuItem("Save filters to current hostname file")){
                    K4SGSignals::get()->save_filters_to_current_hostname_file_signal();
                }
                if(ImGui::MenuItem("Load default filters file")){
                    K4SGSignals::get()->load_default_filters_file_signal();
                }
                if(ImGui::MenuItem("Load current hostname filters file")){
                    K4SGSignals::get()->load_current_hostname_filters_file_signal();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Color")){
                if(ImGui::MenuItem("Reset color settings")){
                    K4SGSignals::get()->reset_color_settings_signal();
                }
                if(ImGui::MenuItem("Save color settings to default file")){
                    K4SGSignals::get()->save_color_settings_to_default_file_signal();
                }
                if(ImGui::MenuItem("Save color settings to current hostname file")){
                    K4SGSignals::get()->save_color_settings_to_current_hostname_file_signal();
                }
                if(ImGui::MenuItem("Load default color settings file")){
                    K4SGSignals::get()->load_default_color_settings_file_signal();
                }
                if(ImGui::MenuItem("Load current hostname color settings file")){
                    K4SGSignals::get()->load_current_hostname_color_settings_file_signal();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Recordings")){

            if(ImGui::MenuItem("Save recording to file")){
                ImGuiFileDialog::Instance()->OpenDialog("Save recording", "Choose file to save", ".kvid", ".");
            }

            if(ImGui::MenuItem("Save current cloud file")){
                ImGuiFileDialog::Instance()->OpenDialog("Save cloud", "Choose file to save", ".obj", ".");
            }

            ImGui::EndMenu();
        }


        if (ImGui::BeginMenu("Developer")){

            if (ImGui::BeginMenu("Shaders")){
                for(std::string_view alias : ShadersManager::get_instance()->get_all_aliases()){
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
                if(ImGui::MenuItem("Trigger debug_device_send_signal")){
                    K4SGSignals::get()->debug_device_send_signal();
                }
                if(ImGui::MenuItem("Ping server")){
                    K4SGSignals::get()->ping_server_signal();
                }
                ImGui::EndMenu();                
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }


    // dialogs
    // # save
    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            K4SGSignals::get()->save_recording_to_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Save cloud")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            Logger::message("save_cloud_to_file_signal\n");
            K4SGSignals::get()->save_cloud_to_file_signal(ImGuiFileDialog::Instance()->GetFilePathName());
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
