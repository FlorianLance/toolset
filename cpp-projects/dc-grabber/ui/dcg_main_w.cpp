
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
#include "utility/string.hpp"

// 3d-engine
#include "imgui-tb/imgui_convert.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"

using namespace tool;
using namespace tool::graphics;
using namespace tool::cam;
using namespace tool::net;

auto DCGMainW::initialize() -> bool{
    
    Log::log("DCGMainW::initialize\n");

    // init drawers
    deviceD.initialize(1);
    recorderD.initialize(1);

    return true;
}

auto draw_config_file_name(const std::string &filePath) -> void{

    if(filePath.empty()){
        ImGuiUiDrawer::text("No file loaded"sv, geo::Pt4f{1.f,0.f,0.f,1.f});
    }

    auto s = String::split_path_and_filename(filePath);
    if(s.second.contains("default")){
        ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{1.f,0.5f,0.15f,1.f});
    }else{
        ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{0,1.f,0,1.f});
    }
}

auto DCGMainW::draw(geo::Pt2f size, DCGModel *model) -> void{

    menuD.draw();

    geo::Pt2f settingsSize = {450.f, size.y()-40.f};
    geo::Pt2f displaySize  = {size.x()-800.f, size.y()-40.f};
    geo::Pt2f logsSize     = {320.f, size.y()-40.f};

    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    if (ImGui::Begin("###UiWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse)){

        // settings
        leftPanelD.draw(settingsSize, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize, model);

        ImGui::SameLine();

        // middle
        if(ImGui::BeginChild("Main###MainChild", to_iv2(displaySize), true, ImGuiWindowFlags_NoScrollWithMouse)){
            if(ImGui::BeginTabBar("Main###MainTabBar")){

                ImGui::BeginDisabled(true);

                ImGui::PushStyleColor(ImGuiCol_Tab,  ImVec4(0.0f, 0, 0.0f, 1));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1, 1.0f, 1));
                if (ImGui::BeginTabItem("[DATA VISUALIZATION]###settings_tabitem")){
                    ImGui::EndTabItem();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::EndDisabled();

                static bool forceSelectedOnce = true;
                ImGuiTabItemFlags flags = 0;
                if(forceSelectedOnce){
                    flags = ImGuiTabItemFlags_::ImGuiTabItemFlags_SetSelected;
                    forceSelectedOnce = false;
                }

                if(ImGui::BeginTabItem("Server device###server_device_tabitem", nullptr, flags)){
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

        if(ImGui::BeginChild("###right_child", to_iv2(logsSize), true)){
            if(ImGui::BeginTabBar("###right_tab_bar")){
                if (ImGui::BeginTabItem("Logs###logs_tab_item", nullptr)){
                    ImGui::Text("Global logs:");
                    logsD.draw("Logs###main_logs_imguilogs", to_iv2(logsSize + geo::Pt2f{0.f,-50.f}));
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Infos###infos_tab_item", nullptr)){

                    ImGuiUiDrawer::title("MONITORING");
                    ImGui::Spacing();

                    {
                        captureB.add_value(model->device->get_capture_duration_ms());
                        readB.add_value(model->device->get_duration_ms("READ_IMAGES"sv));
                        procB.add_value(model->device->get_processing_duration_ms());
                        convImageB.add_value(model->device->get_duration_ms("CONVERT_COLOR_IMAGE"sv));
                        resizeImageB.add_value(model->device->get_duration_ms("RESIZE_COLOR_IMAGE"sv));
                        filterB.add_value(model->device->get_duration_ms("FILTER"sv));
                        updateDataFrameB.add_value(model->device->get_duration_ms("UPDATE_DATA_FRAME"sv));
                        finalizeDataFrameB.add_value(model->device->get_duration_ms("FINALIZE_DATA_FRAME"sv));
                        updateFrameB.add_value(model->device->get_duration_ms("UPDATE_FRAME"sv));
                        finalizeFrameB.add_value(model->device->get_duration_ms("FINALIZE_FRAME"sv));

                        float framerate                 = model->device->get_average_framerate();
                        auto averageCapture             = captureB.get();
                        auto averageRead                = readB.get();
                        auto averageConvImage           = convImageB.get();
                        auto averageResizeImage         = resizeImageB.get();
                        auto averageFilter              = filterB.get();
                        auto averageProc                = procB.get();
                        auto averageUpdateDataFrameB    = updateDataFrameB.get();
                        auto averageFinalizeDataFrameB  = finalizeDataFrameB.get();
                        auto averageUpdateFrameB        = updateFrameB.get();
                        auto averageFinalizeFrameB      = finalizeFrameB.get();

                        elaspedBeforeSendingB.add_value(averageRead + averageConvImage + averageResizeImage + averageFilter +
                            averageUpdateDataFrameB + averageFinalizeDataFrameB);
                        {
                            std::unique_lock<std::mutex> lock(model->server.settings.cInfos.lock, std::try_to_lock);
                            if(lock.owns_lock()){
                                sendingB.add_value(model->server.settings.cInfos.lastFrameSentDurationMicroS*0.001);
                            }
                        }
                        ImGui::Text("Device thread usage(%%): ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:3.1f}"sv, model->device->get_proc_usage()*100.0), geo::Pt4f{0.,1.f,0.f,1.f});

                        ImGui::Text("Images/s: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:3.1f}"sv, framerate), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::Spacing();
                        ImGui::Text("Times (ms): Total: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:3.1f}"sv, averageCapture + averageRead + averageProc), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::Indent();
                        ImGui::Text("Cap: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:3.1f}"sv, averageCapture), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::SameLine();
                        ImGui::Text("Read: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:3.1f}"sv, averageRead), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::SameLine();
                        ImGui::Text("Proc: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:3.1f}"sv, averageProc), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::Unindent();

                        ImGui::Text("Proc details:");
                        ImGui::Indent();
                        ImGui::Text("Conv col: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,  averageConvImage), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::SameLine();
                        ImGui::Text("Res col: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, averageResizeImage), geo::Pt4f{0.,1.f,0.f,1.f});

                        ImGui::Text("Filt dep: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,  averageFilter), geo::Pt4f{0.,1.f,0.f,1.f});

                        ImGui::Text("Data frame: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, (averageUpdateDataFrameB + averageFinalizeDataFrameB)), geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::SameLine();
                        ImGui::Text("Display frame ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, (averageUpdateFrameB + averageFinalizeFrameB)), geo::Pt4f{0.,1.f,0.f,1.f});

                        ImGui::Text("Delay before sending: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, elaspedBeforeSendingB.get()),geo::Pt4f{0.,1.f,0.f,1.f});

                        ImGui::Text("Sending: ");
                        ImGui::SameLine();
                        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,sendingB.get()),geo::Pt4f{0.,1.f,0.f,1.f});
                        ImGui::Unindent();

                    }

                    // ...
                    ImGui::Separator();

                    ImGuiUiDrawer::title("SETTINGS FILES");
                    ImGui::Spacing();
                    ImGuiUiDrawer::text(std::format("Current local ID [{}]", model->server.settings.idG));
                    ImGuiUiDrawer::text_wrapped("Defined by the N value of the exe argument: \"-iN\", this id is used to determinewhich settings file to be loaded."sv, geo::Pt4f{0.5f,1.0f,0.5f,1.f});
                    ImGui::Separator();
                    ImGui::Spacing();

                    ImGui::Text("Config file loaded:");
                    ImGui::Indent();
                    draw_config_file_name(model->server.settings.globalFilePath);
                    ImGui::Unindent();


                    ImGui::EndTabItem();
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

auto DCGMainW::append_log(const std::string &log) -> void {
    logsD.add_log(log.c_str());
}
