

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
#include "utility/format.hpp"
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

auto DCGLeftPanelChildDrawer::draw(geo::Pt2f size, int windowFlags, DCGSettings &settings, DCGStates &states) -> void {

    settings.displayS.settingsFiltersSubPanelDisplayed = false;

    if(ImGui::BeginChild("###settings_child2", ImVec2(size.x(), size.y()), true, windowFlags)){
        if (ImGui::BeginTabBar("###settings_tabbar")){
            draw_client_info_tab_item(settings);

            if (ImGui::BeginTabItem("Settings###settings_tabitem")){
                if(ImGui::BeginTabBar("###sub_settings_tabbar")){
                    draw_device_tab_item(settings.deviceS);
                    draw_filters_tab_item(settings.displayS, settings.deviceS.configS, settings.filtersS);
                    draw_model_tab_item(settings.modelS);
                    draw_colors_settings_tab_item(settings.deviceS.configS.typeDevice, settings.colorS);
                    draw_display_tab_item(settings.displayS);

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            draw_recording_tab_item(states.recorder, settings.recorderS);
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


auto draw_config_file_name(const std::optional<std::string> &filePath) -> void{
    if(filePath.has_value()){
        auto s = String::split_path_and_filename(filePath.value());        
        if(s.second.contains("default")){
            ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{1.f,0.5f,0.15f,1.f});
        }else{
            ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{0,1.f,0,1.f});
        }
    }else{
        ImGui::Text("No file loaded.");
    }
}

auto DCGLeftPanelChildDrawer::draw_client_info_tab_item(DCGSettings &settings) -> void {

    if (!ImGui::BeginTabItem("Client info###settings_client_info_tabitem")){
        return;
    }



    ImGuiUiDrawer::title2("NETWORK");
    ImGui::Spacing();   

    ImGuiUiDrawer::text(fmt("Id grabber: {}", settings.idLocalGrabber));
    ImGui::Spacing();

    ImGui::Text("Connection state:");
    ImGui::SameLine();
    if(settings.networkS.is_connected_to_manager()){
        ImGuiUiDrawer::text("Connected to manager"sv, geo::Pt4f{0,1.f,0,1.f});
    }else{
        ImGuiUiDrawer::text("Not connected"sv, geo::Pt4f{1.f,0.5f,0.15f,1.f});
    }

    if(settings.networkS.protocol == net::Protocol::ipv6){
        ImGui::Text("PROTOCOL: IPV6");
    }else{
        ImGui::Text("PROTOCOL: IPV4");
    }
    ImGui::Spacing();

    ImGui::Text("UDP reading:");
    ImGui::Indent();
    ImGuiUiDrawer::text(fmt("IP address: {}", settings.networkS.udpReadingInterface.ipAddress));
    ImGuiUiDrawer::text(fmt("Port: {}", settings.networkS.udpReadingPort));
    ImGui::Unindent();

    ImGui::Text("UDP sending:");
    ImGui::Indent();
    ImGuiUiDrawer::text(fmt("IP address: {}", settings.networkS.udpSendingAdress));
    ImGuiUiDrawer::text(fmt("Port: {}", settings.networkS.udpSendingPort));
    ImGui::Unindent();   

    ImGui::Separator();

    ImGui::Text("Last frame sent:");
    ImGui::Indent();
    ImGuiUiDrawer::text(fmt("ID: {}", settings.networkS.lastFrameIdSent));
    ImGuiUiDrawer::text(fmt("Time (ms): {}", std::chrono::duration_cast<std::chrono::milliseconds>(Time::nanoseconds_since_epoch() - settings.networkS.lastFrameSentTS).count()));
    ImGui::Unindent();

    ImGui::Separator();

    if(settings.imuSample.has_value()){
        ImGui::Text("IMU last sample");
        ImGui::Indent();
        ImGuiUiDrawer::text(std::format("{:17.2f} : temperature(°)", settings.imuSample.value().temperature));
        const auto &a = settings.imuSample.value().acc;
        ImGuiUiDrawer::text(std::format("{:5.2f} {:5.2f} {:5.2f} : accelerometer(m/s²)", a.x(), a.y(), a.z()));
        const auto &g = settings.imuSample.value().gyr;
        ImGuiUiDrawer::text(std::format("{:5.2f} {:5.2f} {:5.2f} : gyroscope(rad/s)", g.x(), g.y(), g.z()));
        ImGui::Unindent();
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Text("Other:");
    ImGui::Indent();

    int delay = settings.delayS.delayMs;
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Current delay", "delay_others", &delay, ImGuiIntS{0,0, 5000,1.f,100},ImGuiDragS())){
        settings.delayS.delayMs = delay;
        DCGSignals::get()->update_delay_settings_signal(settings.delayS);
    }
    ImGui::Unindent();
    ImGui::Spacing();

    ImGuiUiDrawer::title2("CONFIG FILES");
    ImGui::Spacing();

    ImGui::Text("Network:");
    ImGui::Indent();
    draw_config_file_name(settings.netWorkFilePath);
    ImGui::Unindent();

    ImGui::Text("Color:");
    ImGui::Indent();
    draw_config_file_name(settings.colorFilePath);
    ImGui::Unindent();

    ImGui::Text("Filters:");
    ImGui::Indent();
    draw_config_file_name(settings.filtersFilePath);
    ImGui::Unindent();

    ImGui::Text("Device:");
    ImGui::Indent();
    draw_config_file_name(settings.deviceFilePath);
    ImGui::Unindent();

    ImGui::Text("Model:");
    ImGui::Indent();
    draw_config_file_name(settings.modelFilePath);
    ImGui::Unindent();

    ImGui::EndTabItem();
}

auto DCGLeftPanelChildDrawer::draw_device_tab_item(
    cam::DCDeviceSettings &device) -> void {

    auto signals = DCGSignals::get();

    bool updateDeviceList = false;

    auto update = DCUIDrawer::draw_dc_device_settings_tab_item("Device###device_tabitem",
        device,
        updateDeviceList,
        m_autoUpdate
    );


    if(update){
        signals->update_device_settings_signal(device);
    }
}

auto DCGLeftPanelChildDrawer::draw_filters_tab_item(
    ui::DCGDisplaySettingsSettings &ui,
    const cam::DCConfigSettings &config,
    cam::DCFiltersSettings &filters) -> void {

    auto ret = DCUIDrawer::draw_dc_filters_settings_tab_item("Filters###filters_tabitem", config.mode, filters, m_autoUpdate);
    ui.settingsFiltersSubPanelDisplayed  = std::get<0>(ret);
    if(std::get<1>(ret)){
        DCGSignals::get()->update_filters_signal(filters);
    }
}

auto DCGLeftPanelChildDrawer::draw_colors_settings_tab_item(cam::DCType type, cam::DCColorSettings &colors) -> void{

    if(DCUIDrawer::draw_dc_colors_settings_tab_item("Colors###colors_tabitem", type, colors, m_autoUpdate)){
        DCGSignals::get()->update_color_settings_signal(colors);
    }
}

auto DCGLeftPanelChildDrawer::draw_display_tab_item(ui::DCGDisplaySettingsSettings &dcDisplaySettings) -> void {

    if (!ImGui::BeginTabItem("Display###display_settings_tabitem")){
        return;
    }
    bool update = false;

    if(ImGui::Checkbox("Focus window###settings_focus_window", &dcDisplaySettings.focusWindow)){
        update = true;
    }

    if (ImGui::BeginTabBar("###display_tabbar")){
        if(DCUIDrawer::draw_dc_scene_display_setings_tab_item("Scene###scene_display_tabitem", dcDisplaySettings.sceneDisplay, m_autoUpdate)){
            DCGSignals::get()->update_scene_display_settings_signal(dcDisplaySettings.sceneDisplay);
        }
        if(DCUIDrawer::draw_dc_cloud_display_setings_tab_item("Cloud###cloud_display_tabitem", dcDisplaySettings.cloudDisplay, m_autoUpdate)){
            DCGSignals::get()->update_cloud_display_settings_signal(0, dcDisplaySettings.cloudDisplay);
        }
        ImGui::EndTabBar();
    }

    if(update){
        DCGSignals::get()->update_grabber_display_settings_signal(&dcDisplaySettings);
    }

    ImGui::EndTabItem();
}

auto DCGLeftPanelChildDrawer::draw_recording_tab_item(cam::DCVideoRecorderStates &recStates, cam::DCVideoRecorderSettings &recSetings) -> void {
    if(DCUIDrawer::draw_dc_recorder_tab_item("Recording###settings_recording_tabitem", recStates, recSetings, m_autoUpdate)){
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

    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCGSignals::get()->save_recorder_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

auto DCGLeftPanelChildDrawer::draw_model_tab_item(cam::DCModelSettings &model) -> void {

    if(DCUIDrawer::draw_dc_model_tab_item("Model###model_tabitem", model, m_autoUpdate)){
        DCGSignals::get()->update_model_settings_signal(0, model);
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

