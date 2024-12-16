

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

#include "dcg_left_panel_child_drawer.hpp"

// base
#include "utility/string.hpp"
#include "utility/time.hpp"

// 3d-engine
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "imgui-tb/imgui_dc_ui_drawer.hpp"
#include "imgui/extra/ImGuiFileDialog.h"

// local
#include "dcg_signals.hpp"

using namespace tool;
using namespace tool::graphics;
using namespace tool::cam;

auto DCGLeftPanelChildDrawer::draw(geo::Pt2f size, int windowFlags, DCGModel *model) -> void {


    if(ImGui::BeginChild("###left_panel_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        if(ImGui::BeginChild("###settings_child", ImVec2(size.x(), size.y()-100), true, windowFlags)){


            if (ImGui::BeginTabBar("###main_tabbar")){

                ImGui::BeginDisabled(true);

                ImGui::PushStyleColor(ImGuiCol_Tab,  ImVec4(0.0f, 0, 0.0f, 1));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1, 1.0f, 1));
                if (ImGui::BeginTabItem("[SETTINGS]###settings_tabitem")){
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

                if (ImGui::BeginTabItem("Server device###server_device_tabitem", nullptr, flags)){

                    if(ImGui::BeginTabBar("###sub_server_device_tabbar")){
                        draw_network_tab_item(model);
                        draw_device_tab_item(model->server.settings.deviceS);
                        draw_filters_tab_item(model->server.settings.deviceS.configS, model->server.settings.filtersS);
                        draw_model_tab_item(model->server.settings.modelS);
                        draw_colors_settings_tab_item(model->server.settings.deviceS.configS.typeDevice, model->server.settings.colorS);
                        draw_display_tab_item(model->uiSettings, model->server.settings.sceneDisplayS,  model->server.settings.displayS);
                        draw_misc_tab_item(model->server.settings.miscS);
                        ImGui::EndTabBar();
                    }


                    ImGui::EndTabItem();
                }


                draw_recorder_tab_item(model->recorder.states, model->recorder.settings);
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

auto DCGLeftPanelChildDrawer::update_selected_color(const geo::Pt4f &color) -> void{
    secondLastClickedPixelColor = firstLastClickedPixelColor;
    firstLastClickedPixelColor = color;
}

auto DCGLeftPanelChildDrawer::draw_network_tab_item(DCGModel *model) -> void {

    // ImGui::PushStyleColor(ImGuiCol_Tab,         ImVec4(0, 1, 0, 1));
    // ImGui::PushStyleColor(ImGuiCol_TabSelected, ImVec4(0.5, 1, 0.5, 1));
    // ImGui::PushStyleColor(ImGuiCol_TabHovered,  ImVec4(0.2, 1, 0.2, 1));

    if (!ImGui::BeginTabItem("Network###settings_server_network_tabitem")){
        // ImGui::PopStyleColor();
        // ImGui::PopStyleColor();
        // ImGui::PopStyleColor();
        return;
    }
    // ImGui::PopStyleColor();
    // ImGui::PopStyleColor();
    // ImGui::PopStyleColor();

    DCServerSettings &settings = model->server.settings;

    ImGuiUiDrawer::title("CLIENT/SERVER CONNECTION");
    ImGui::Spacing();


    ImGui::Text("Protocol:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(75.f);
    int guiCurrentProtocolSelection = static_cast<int>(settings.udpServerS.protocol);
    if(ImGui::Combo("###settings_protocole_combo", &guiCurrentProtocolSelection, protocolItems, 2)){
        settings.udpServerS.protocol = static_cast<net::Protocol>(guiCurrentProtocolSelection);
        DCGSignals::get()->reset_reading_network_signal();
    }

    static ImGuiIntS idRIS = {0,0,10,0.1f,1};

    if(settings.udpServerS.protocol == net::Protocol::ipv4){
        ImGui::Text("Available IPV4 interfaces:");
        ImGui::Indent();
        for(auto &interface : settings.ipv4Interfaces){
            ImGuiUiDrawer::text(interface.ipAddress);
        }
        if(!settings.ipv4Interfaces.empty()){
            idRIS.max = settings.ipv4Interfaces.size()-1;
        }else{
            idRIS.max = 0;
        }
        ImGui::Unindent();
    }else if(settings.udpServerS.protocol == net::Protocol::ipv6){
        ImGui::Text("Available IPV6 interfaces:");
        ImGui::Indent();
        for(auto &interface : settings.ipv6Interfaces){
            ImGuiUiDrawer::text(interface.ipAddress);
        }
        if(!settings.ipv6Interfaces.empty()){
            idRIS.max = settings.ipv6Interfaces.size()-1;
        }else{
            idRIS.max = 0;
        }
        ImGui::Unindent();
    }

    if(ImGui::Checkbox("Use any interface", &settings.udpServerS.anyReadingInterface)){
        DCGSignals::get()->reset_reading_network_signal();
    }

    ImGui::BeginDisabled(settings.udpServerS.anyReadingInterface);

    static ImGuiDragS dsRIS = {50.f, true, true, true, true, true};
    int idRI = static_cast<int>(settings.udpServerS.readingInterfaceId);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("ID reading interface", "id_reading_interface", &idRI, idRIS, dsRIS)){
        settings.udpServerS.readingInterfaceId = idRI;
        DCGSignals::get()->reset_reading_network_signal();
    }

    ImGui::EndDisabled();

    static ImGuiIntS idRP = {8888,8888,9999,0.1f,1};
    static ImGuiDragS dsRP = {75.f, true, true, true, true, true};
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Reading port", "reading_port", &settings.udpServerS.readingPort, idRP, dsRP)){
        DCGSignals::get()->reset_reading_network_signal();
    }


    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Spacing();

    ImGuiUiDrawer::title("CURRENT STATE");

    ImGui::Text("Current UDP reading:");
    ImGui::Indent();
    ImGuiUiDrawer::text(std::format("IP address: [{}]", settings.udpReadingInterface.ipAddress));
    ImGuiUiDrawer::text(std::format("Port: {}", settings.udpServerS.readingPort));
    ImGui::Unindent();

    ImGui::Text("Clients:");
    ImGui::Indent();
    {
        std::unique_lock lg(settings.cInfos.lock);
        ImGuiUiDrawer::text(std::format("Nb connected: [{}]", settings.cInfos.clientsConnected.size()));
        auto cTime = Time::nanoseconds_since_epoch();
        for(const auto &id : settings.cInfos.clientsConnected){
            ImGuiUiDrawer::text(std::format("name: [{}], last ping: [{}]ms"sv, id.first, Time::difference_ms(id.second, cTime).count()));
        }

        ImGui::Text("Last frame sent:");
        ImGui::Indent();

        auto lastFrameSentTS = settings.cInfos.lastFrameSentTS;
        if(lastFrameSentTS.count() != 0){
            ImGuiUiDrawer::text(std::format("ID: {}", settings.cInfos.lastFrameIdSent));
            ImGuiUiDrawer::text(std::format("Time (ms): {}", std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - lastFrameSentTS).count()));
        }else{
            ImGuiUiDrawer::text(std::format("ID: ..."));
            ImGuiUiDrawer::text(std::format("Time (ms): ..."));
        }
        ImGuiUiDrawer::text(std::format("Data frame size (M/bytes): {}", settings.cInfos.sizeDataFrame/10000000.0));
        ImGuiUiDrawer::text(std::format("Nb 9K packets (approximate): {}", settings.cInfos.sizeDataFrame/9000));

        ImGui::Unindent();
    }
    ImGui::Unindent();
    ImGui::Spacing();
    if(ImGui::Button("Disconnect all clients")){
        DCGSignals::get()->disconnect_all_clients_signal();
    }

    ImGui::EndTabItem();
}

auto DCGLeftPanelChildDrawer::draw_device_tab_item(cam::DCDeviceSettings &device) -> void {
    if(std::get<1>(DCUIDrawer::draw_dc_device_settings_tab_item("Device###device_tabitem",device, std::nullopt))){
        DCGSignals::get()->update_device_settings_signal(device);
    }
}

auto DCGLeftPanelChildDrawer::draw_filters_tab_item(const cam::DCConfigSettings &config, cam::DCFiltersSettings &filters) -> void {
    if(std::get<1>(DCUIDrawer::draw_dc_filters_settings_tab_item("Filters###filters_tabitem", config.mode, filters))){
        DCGSignals::get()->update_filters_signal(filters);
    }
}

auto DCGLeftPanelChildDrawer::draw_colors_settings_tab_item(cam::DCType type, cam::DCColorSettings &colors) -> void{
    if(std::get<1>(DCUIDrawer::draw_dc_colors_settings_tab_item("Colors###colors_tabitem", type, colors))){
        DCGSignals::get()->update_color_settings_signal(colors);
    }
}

auto DCGLeftPanelChildDrawer::draw_display_tab_item(DCGUiSettings &uiS, DCSceneDisplaySettings &sceneDisplayS, DCDeviceDisplaySettings &displayS) -> void {

    if (!ImGui::BeginTabItem("Display###display_settings_tabitem")){
        return;
    }

    ImGuiUiDrawer::title("UI");

    if(ImGui::Checkbox("Focus window###settings_focus_window", &uiS.focusWindow)){
    }

    ImGuiUiDrawer::title("3D VIEW");

    if (ImGui::BeginTabBar("###display_tabbar")){
        if(DCUIDrawer::draw_dc_scene_display_setings_tab_item("Scene###scene_display_tabitem", sceneDisplayS)){
            DCGSignals::get()->update_scene_display_settings_signal(sceneDisplayS);
        }
        if(DCUIDrawer::draw_dc_cloud_display_setings_tab_item("Device###device_display_tabitem", displayS)){
            DCGSignals::get()->update_cloud_display_settings_signal(0, displayS);
        }
        ImGui::EndTabBar();
    }
    ImGui::EndTabItem();
}

auto DCGLeftPanelChildDrawer::draw_recorder_tab_item(cam::DCVideoRecorderStates &recStates, cam::DCVideoRecorderSettings &recSetings) -> void {

    if(DCUIDrawer::draw_dc_recorder_tab_item("Recorder###settings_recorder_tabitem", recStates, recSetings, true)){
        DCGSignals::get()->update_recorder_settings_signal(recSetings);
    }

    if(recStates.startRecording){
        DCGSignals::get()->start_recorder_signal();
    }
    if(recStates.stopRecording){
        DCGSignals::get()->stop_recorder_signal();
    }
    if(recStates.resetRecording){
        DCGSignals::get()->reset_recorder_signal();
    }
    if(recStates.moveTime){
        DCGSignals::get()->set_recorder_time_signal(recStates.currentTime);
    }
    recStates.reset_actions();

    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCGSignals::get()->save_recorder_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

auto DCGLeftPanelChildDrawer::draw_model_tab_item(cam::DCModelSettings &modelS) -> void {

    if(DCUIDrawer::draw_dc_model_tab_item("Model###model_tabitem", modelS)){
        DCGSignals::get()->update_model_settings_signal(0, modelS);
    }
}

auto DCGLeftPanelChildDrawer::draw_misc_tab_item(cam::DCMiscSettings &miscS) -> void{
    if(DCUIDrawer::draw_dc_misc_tab_item("Misc###misc_tabitem", miscS)){
        DCGSignals::get()->update_misc_settings_signal(miscS);
    }
}


auto DCGLeftPanelChildDrawer::draw_audio_tab_item() -> void {
    if (!ImGui::BeginTabItem("Audio###settings_audio_tabitem")){
        return;
    }

    ImGui::EndTabItem();
    //                ImPlot::SetNextPlotLimitsX(0., 50000.);
    //                ImPlot::SetNextPlotLimitsY(-1., 1.);
    //                if (ImPlot::BeginPlot("Stairstep Plot", "x", "f(x)")) {
    //                    if(channelsData2[0].size() > 0){
    //                        ImPlot::PlotLine("Signal 1", channelsData2[0].data(), channelsData2[0].size(), 0.01f);
    //                        auto sum_of_elems = std::accumulate(channelsData2[0].begin(), channelsData2[0].end(), 0.f);
    //                        Log::message(std::format("sum_of_elems: {}\n", sum_of_elems));
    //                    }


    //                    ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 2.0f);
    ////                            ImPlot::PlotStairs("Signal 2", ys2, 101, 0.01f);
    //                    ImPlot::EndPlot();
    //                }


    //                ImPlot::PushStyleVar(ImPlotStyleVar_PlotMinSize,{0,0});
    //                if (ImPlot::BeginPlot("##Plot2",ImVec2(-1,-1),ImPlotFlags_CanvasOnly)) {
    ////                    ImPlot::SetupAxes(NULL,NULL,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
    ////                    ImPlot::SetupAxesLimits(0,1,-1,1,ImGuiCond_Always);
    //                    int idx = (int)((m_samples.size() - N_FFT) * (m_time/m_duration));
    //                    idx -= idx % N_FFT;
    //                    kiss_fftr(m_fft, &m_samples[idx], reinterpret_cast<kiss_fft_cpx*>(m_fft_out));
    //                    auto getter1 = [](void* data, int i) {
    //                        ImSpectrogram& spec = *(ImSpectrogram*)(data);
    //                        double db = 20*log10(std::abs(spec.m_fft_out[i]));
    //                        double x = remap01((double)i,0.0,(double)(N_FRQ-1));
    //                        double y = remap(db,spec.m_min_db,spec.m_max_db,-1.0,1.0);
    //                        return ImPlotPoint(x,y);
    //                    };
    //                    auto getter2 = [](void*, int i) {
    //                        double x = remap01((double)i,0.0,(double)(N_FRQ-1));
    //                        return ImPlotPoint(x,-1.0);
    //                    };
    //                    ImPlot::SetNextFillStyle({1,1,1,0.1f});
    //                    ImPlot::PlotShadedG("##FreqDomain",getter1,this,getter2,nullptr,N_FRQ);
    //                    ImPlot::SetNextLineStyle(ImPlot::SampleColormap(0.8f));
    //                    ImPlot::PlotLine("##TimeDomain",&m_samples[idx],N_FFT,1.0/(N_FFT-1));
    //                    ImPlot::EndPlot();
    //                }
    //                ImPlot::PopStyleVar();
    //                ImGui::End();
    //            };


}

