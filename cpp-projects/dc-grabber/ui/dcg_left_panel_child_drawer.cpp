

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

    if(ImGui::BeginChild("###settings_child2", ImVec2(size.x(), size.y()), true, windowFlags)){
        if (ImGui::BeginTabBar("###settings_tabbar")){
            draw_server_info_tab_item(model);

            if (ImGui::BeginTabItem("Settings###settings_tabitem")){
                if(ImGui::BeginTabBar("###sub_settings_tabbar")){
                    draw_device_tab_item(model->server.settings.deviceS);
                    draw_filters_tab_item(model->server.settings.deviceS.configS, model->server.settings.filtersS);
                    draw_model_tab_item(model->server.settings.modelS);
                    draw_colors_settings_tab_item(model->server.settings.deviceS.configS.typeDevice, model->server.settings.colorS);
                    draw_display_tab_item(model->uiSettings, model->server.settings.sceneDisplayS,  model->server.settings.displayS);
                    draw_delay_tab_item(model->server.settings.delayS);
                    ImGui::EndTabBar();
                }
                ImGui::EndTabItem();
            }

            draw_recording_tab_item(model->recorder.states, model->recorder.settings);
            // draw_audio_tab_item();
            draw_logs_tab_item();
            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();
}

auto DCGLeftPanelChildDrawer::append_log(const std::string &log) -> void {
    m_logs.add_log(log.c_str());
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

#include "utility/logger.hpp"

auto DCGLeftPanelChildDrawer::draw_server_info_tab_item(DCGModel *model) -> void {

    if (!ImGui::BeginTabItem("Server info###settings_server_info_tabitem")){
        return;
    }

    DCServerSettings &settings = model->server.settings;

    ImGuiUiDrawer::title2("NETWORK");
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

    static ImGuiDragS dsRIS = {50.f, true, true, true, true, true};
    int idRI = static_cast<int>(settings.udpServerS.udpReadingInterfaceId);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("ID reading interface", "id_reading_interface", &idRI, idRIS, dsRIS)){
        settings.udpServerS.udpReadingInterfaceId = idRI;
        DCGSignals::get()->reset_reading_network_signal();
    }

    static ImGuiIntS idRP = {8888,8888,9999,0.1f,1};
    static ImGuiDragS dsRP = {75.f, true, true, true, true, true};
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Reading port", "reading_port", &settings.udpServerS.udpReadingPort, idRP, dsRP)){
        DCGSignals::get()->reset_reading_network_signal();
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Spacing();

    ImGui::Text("Current UDP reading:");
    ImGui::Indent();
    ImGuiUiDrawer::text(std::format("IP address: [{}]", settings.udpReadingInterface.ipAddress));
    ImGuiUiDrawer::text(std::format("Port: {}", settings.udpServerS.udpReadingPort));
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

        ImGui::Unindent();
    }
    ImGui::Unindent();
    ImGui::Spacing();
    if(ImGui::Button("Disconnect all clients")){
        DCGSignals::get()->disconnect_all_clients_signal();
    }
    ImGui::Separator();

    ImGuiUiDrawer::title2("MONITORING");
    ImGui::Spacing();

    {
        captureB.add_value(model->device->get_capture_duration_ms());
        readB.add_value(model->device->get_duration_ms("READ_IMAGES"sv));
        procB.add_value(model->device->get_processing_duration_ms());
        convImageB.add_value(model->device->get_duration_ms("CONVERT_COLOR_IMAGE"sv));
        resizeImageB.add_value(model->device->get_duration_ms("RESIZE_COLOR_IMAGE"sv));
        filterDepthB.add_value(model->device->get_duration_ms("FILTER_DEPTH"sv));
        updateCompFrameB.add_value(model->device->get_duration_ms("UPDATE_COMPRESSED_FRAME"sv));
        finalizeCompFrameB.add_value(model->device->get_duration_ms("FINALIZE_COMPRESSED_FRAME"sv));
        updateFrameB.add_value(model->device->get_duration_ms("UPDATE_FRAME"sv));
        finalizeFrameB.add_value(model->device->get_duration_ms("FINALIZE_FRAME"sv));

        float framerate                 = model->device->get_average_framerate();
        auto averageCapture             = captureB.get();
        auto averageRead                = readB.get();
        auto averageProc                = procB.get();
        auto averageUpdateCompFrameB    = updateCompFrameB.get();
        auto averageFinalizeCompFrameB  = finalizeCompFrameB.get();
        auto averageUpdateFrameB        = updateFrameB.get();
        auto averageFinalizeFrameB      = finalizeFrameB.get();

        elaspedBeforeSendingB.add_value(averageRead + averageProc + averageUpdateCompFrameB + averageFinalizeCompFrameB);
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
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,  convImageB.get()), geo::Pt4f{0.,1.f,0.f,1.f});
        ImGui::SameLine();
        ImGui::Text("Res col: ");
        ImGui::SameLine();
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,  resizeImageB.get()), geo::Pt4f{0.,1.f,0.f,1.f});
        ImGui::SameLine();
        ImGui::Text("Filt dep: ");
        ImGui::SameLine();
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,  filterDepthB.get()), geo::Pt4f{0.,1.f,0.f,1.f});

        ImGui::Text("GCFrame: ");
        ImGui::SameLine();
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, (averageUpdateCompFrameB + averageFinalizeCompFrameB)), geo::Pt4f{0.,1.f,0.f,1.f});
        ImGui::SameLine();
        ImGui::Text("GFrame ");
        ImGui::SameLine();
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, (averageUpdateFrameB + averageFinalizeFrameB)), geo::Pt4f{0.,1.f,0.f,1.f});


        ImGui::Text("Delay before sending: ");
        ImGui::SameLine();
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv, elaspedBeforeSendingB.get()),geo::Pt4f{0.,1.f,0.f,1.f});
        ImGui::SameLine();

        ImGui::Text("Sending: ");
        ImGui::SameLine();
        ImGuiUiDrawer::text(std::format("{:4.2f}"sv,sendingB.get()),geo::Pt4f{0.,1.f,0.f,1.f});
        ImGui::Unindent();

    }

    // ...

    ImGui::Separator();

    ImGuiUiDrawer::title2("SETTINGS FILES");
    ImGui::Spacing();
    ImGuiUiDrawer::text(std::format("Current local ID [{}]", settings.idG));
    ImGui::Text("Defined by the N value of the exe argument: \"-iN\"");
    ImGui::Text("This id is used to determinewhich settings file\n to be loaded");
    ImGui::Separator();
    ImGui::Spacing();

    if(!settings.globalFilePath.empty()){
        ImGui::Text("Global:");
        ImGui::Indent();
        draw_config_file_name(settings.globalFilePath);
        ImGui::Unindent();
    }else{
        ImGui::Text("[No global file found, use legacy files instead]");

        ImGui::Text("Device:");
        ImGui::Indent();
        draw_config_file_name(settings.deviceFilePath);
        ImGui::Unindent();

        ImGui::Text("Filters:");
        ImGui::Indent();
        draw_config_file_name(settings.filtersFilePath);
        ImGui::Unindent();

        ImGui::Text("Color:");
        ImGui::Indent();
        draw_config_file_name(settings.colorFilePath);
        ImGui::Unindent();

        ImGui::Text("Model:");
        ImGui::Indent();
        draw_config_file_name(settings.modelFilePath);
        ImGui::Unindent();
    }

    ImGui::EndTabItem();
}

auto DCGLeftPanelChildDrawer::draw_device_tab_item(cam::DCDeviceSettings &device) -> void {
    if(std::get<1>(DCUIDrawer::draw_dc_device_settings_tab_item("Device###device_tabitem",device))){
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
    bool update = false;

    if(ImGui::Checkbox("Focus window###settings_focus_window", &uiS.focusWindow)){
        update = true;
    }

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

auto DCGLeftPanelChildDrawer::draw_recording_tab_item(cam::DCVideoRecorderStates &recStates, cam::DCVideoRecorderSettings &recSetings) -> void {

    if(DCUIDrawer::draw_dc_recorder_tab_item("Recording###settings_recording_tabitem", recStates, recSetings)){
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

auto DCGLeftPanelChildDrawer::draw_model_tab_item(cam::DCModelSettings &model) -> void {

    if(DCUIDrawer::draw_dc_model_tab_item("Model###model_tabitem", model)){
        DCGSignals::get()->update_model_settings_signal(0, model);
    }
}

auto DCGLeftPanelChildDrawer::draw_delay_tab_item(cam::DCDelaySettings &delayS) -> void{
    if(DCUIDrawer::draw_dc_delay_tab_item("Delay###delay_tabitem", delayS)){
        DCGSignals::get()->update_delay_settings_signal(delayS);
    }
}

auto DCGLeftPanelChildDrawer::draw_logs_tab_item() -> void {
    if (!ImGui::BeginTabItem("Logs###settings_logs_tabitem")){
        return;
    }

    m_logs.draw("Logs###settings_logs_imguilogs");
    ImGui::EndTabItem();
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
    //                        Logger::message(std::format("sum_of_elems: {}\n", sum_of_elems));
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

