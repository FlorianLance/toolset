
/*******************************************************************************
** Toolset-k4-scaner-manager                                                  **
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

#include "k4sm_left_panel_child_drawer.hpp"

// base
#include "utility/logger.hpp"
#include "utility/format.hpp"
#include "camera/kinect4/k4_types.hpp"
#include "utility/string.hpp"

// 3d-engine
#include "imgui-tb/imgui_k4_ui_drawer.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "imgui/extra/ImGuiFileDialog.h"

// local
#include "k4sm_signals.hpp"

using namespace tool;
using namespace tool::camera;
using namespace tool::graphics;

auto K4SMLeftPanelChildDrawer::initialize(size_t nbGrabbers) -> void{

    if(fromFilters.size() != 2*nbGrabbers + 2){

        guiCurrentFromFiltersSelection = 0;
        guiCurrentFromColorsSelection = 0;
        guiCurrentTargetFiltersSelection = 0;
        guiCurrentTargetColorsSelection = 0;

        fromFilters.clear();
        targetsFilters.clear();

        fromColor.clear();
        targetsColor.clear();

        for(size_t ii = 0; ii < nbGrabbers; ++ii){
            fromFilters.push_back(fmt("[{}] normal", ii));
            fromColor.push_back(fmt("[{}]", ii));
        }
        for(size_t ii = 0; ii < nbGrabbers; ++ii){
            fromFilters.push_back(fmt("[{}] calib", ii));
        }

        targetsFilters.push_back("All");
        targetsFilters.push_back("All normal");
        targetsFilters.push_back("All calib");
        targetsColor.push_back("All");
        for(const auto &filter : fromFilters){
            targetsFilters.push_back(filter);            
        }        
        for(const auto &color : fromColor){
            targetsColor.push_back(color);
        }
    }

    feedbacksLogs.resize(nbGrabbers);
}

auto K4SMLeftPanelChildDrawer::draw(geo::Pt2f size, int windowFlags, K4SMSettings &settings) -> void {

    settings.globalSet.ui.settingsFiltersNormalSubPanelDisplayed      = false;
    settings.globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed = false;

    if(ImGui::BeginChild("###settings_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        auto region = ImGui::GetContentRegionAvail();
        auto region1 = region;
        auto region2 = region;
        region1.y *= 0.75f;
        if(ImGui::BeginChild("###settings_grabbers_child1", region1, true, windowFlags)){
            draw_grabbers_ui(settings);

            static ImGuiID tabId = 0;
            if(ImGuiUiDrawer::begin_tab_bar(&tabId, "###settings_tabbar1")){
                draw_commands_tab_item(settings.grabbersSet);
                draw_settings_tab_item(settings);
                draw_recorder_tab_item(settings.globalSta.recorder, settings.globalSet.recorder);
                draw_player_tab_item(settings.globalSta.player, settings.globalSet.player);
                draw_calibrator_tab_item(settings.globalSet.useNormalFilteringSettings, settings.globalSta.calibrator, settings.globalDrawerSet.calibrator, settings.globalSet.calibrator);
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();

        region2.y *= 0.25f;
        if(ImGui::BeginChild("###settings_grabbers_child2", region2, true, windowFlags)){
            if (ImGui::BeginTabBar("###settings_tabbar2")){
                draw_infos_tab_item(settings);
                draw_logs_tab_item();
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();
}


auto K4SMLeftPanelChildDrawer::append_global_log(const std::string &log) -> void {
    globalLogs.add_log(log.c_str());
}

auto K4SMLeftPanelChildDrawer::append_feedback_log(size_t idG, const std::string &log) -> void {
    if(idG < feedbacksLogs.size()){
        feedbacksLogs[idG].add_log(log.c_str());
    }
}

auto K4SMLeftPanelChildDrawer::draw_settings_tab_item(K4SMSettings &settings) -> void{
    if (ImGuiUiDrawer::begin_tab_item("Settings###settings_tabitem")){

        static ImGuiID tabId = 0;
        if(ImGuiUiDrawer::begin_tab_bar(&tabId, "###sub_settings_tabbar")){
            draw_device_tab_item(settings);
            draw_filters_tab_item(settings);
            draw_calibration_tab_item(settings.grabbersSet);
            draw_color_tab_item(settings.grabbersSet);
            draw_display_tab_item(settings.globalSet.sceneDisplay, settings.grabbersSet);
            draw_ui_tab_item(settings.globalSet.ui);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}


auto K4SMLeftPanelChildDrawer::draw_recorder_tab_item(camera::K4RecorderStates &rStates, camera::K4RecorderSettings &rSettings) -> void {

    if(K4UIDrawer::draw_recording_tab_item("Recorder###settings_recorder_tabitem", rStates, rSettings, autoUpdate)){
        K4SMSignals::get()->update_recorder_settings_signal(rSettings);
    }

    if(rStates.startRecording){
        K4SMSignals::get()->start_recorder_signal();
    }
    if(rStates.stopRecording){
        K4SMSignals::get()->stop_recorder_signal();
    }
    if(rStates.resetRecording){
        K4SMSignals::get()->reset_recorder_signal();
    }
    if(rStates.moveTime){
        K4SMSignals::get()->set_recorder_time_signal(rStates.currentTime);
    }

    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            K4SMSignals::get()->save_recorder_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    rStates.reset_actions();
}

auto K4SMLeftPanelChildDrawer::draw_player_tab_item(camera::K4PlayerStates &pStates, camera::K4PlayerSettings &pSettings) -> void{

    if(K4UIDrawer::draw_player_tab_item("Player###settings_player_tabitem", pStates, pSettings, autoUpdate)){
        K4SMSignals::get()->update_player_settings_signal(pSettings);
    }

    if(pStates.play){
        K4SMSignals::get()->start_player_signal();
    }
    if(pStates.pause){
        K4SMSignals::get()->pause_player_signal();
    }
    if(pStates.restart){
        K4SMSignals::get()->restart_player_signal();
    }
    if(pStates.removeUntil){
        K4SMSignals::get()->remove_until_current_frame_player_signal();
    }
    if(pStates.removeAfter){
        K4SMSignals::get()->remove_after_current_frame_player_signal();
    }
    if(pStates.merge){
        K4SMSignals::get()->merge_player_signal();
    }
    if(pStates.info){
        K4SMSignals::get()->info_player_signal();
    }
    if(pStates.moveTime){
        K4SMSignals::get()->set_player_time_signal(pStates.currentTime);
    }

    if (ImGuiFileDialog::Instance()->Display("Save cloud")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            K4SMSignals::get()->save_cloud_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Save video")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            K4SMSignals::get()->save_video_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Load video")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            K4SMSignals::get()->load_video_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    pStates.reset_actions();
}

auto K4SMLeftPanelChildDrawer::draw_calibrator_tab_item(bool useNormalFilteringSettings, camera::K4CalibratorStates &cStates, K4CalibratorDrawerSettings &cdSettings, camera::K4CalibratorSettings &cSettings) -> void{

    if(K4UIDrawer::draw_calibrator_tab_item("Calibrator###calibrator_tabitem",  useNormalFilteringSettings, cStates, cdSettings, cSettings, autoUpdate)){
        K4SMSignals::get()->update_calibration_settings_signal(cSettings);        
        K4SMSignals::get()->update_calibration_drawer_settings_signal(cdSettings);
    }

    if(cStates.recomputeRegisteringProcessing){
        K4SMSignals::get()->recompute_registering_processing_signal();
    }
    if(cStates.startCalibration){
        K4SMSignals::get()->start_calibration_registering_signal();
    }
    if(cStates.stopCalibration){
        K4SMSignals::get()->stop_calibration_registering_signal();
    }
    if(cStates.resetCalibration){
        K4SMSignals::get()->reset_calibration_registering_signal();
    }
    if(cStates.calibrate){
        K4SMSignals::get()->ask_calibration_signal();
    }
    if(cStates.validateCalibration){
        K4SMSignals::get()->validate_calibration_signal();
    }
    if(cStates.updateDisplaySettings){
        K4SMSignals::get()->update_calibration_display_signal();
    }
    if(cStates.updateFilteringMode){
        K4SMSignals::get()->update_filtering_mode_signal(cStates.filteringMode == 0);
    }

    cStates.reset_actions();
}

auto K4SMLeftPanelChildDrawer::draw_grabbers_ui(K4SMSettings &settings) -> void {

    ImGui::Text("Grabbers");

    for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){
        ImGuiUiDrawer::text_colored(settings.grabbersSet[ii].network.connected ? K4SMSettings::connectedSelectedC : K4SMSettings::selectedC, settings.grabbersSet[ii].network.name);
        if((ii % 4) != 0 || ii == 0){
            ImGui::SameLine();
        }
    }

    int nbConnected = 0;
    for(const auto &grabber : settings.grabbersSet){
        if(grabber.network.connected){
            ++nbConnected;
        }
    }
    ImGuiUiDrawer::text(fmt("Count: {}", settings.grabbersSet.size()));
    ImGui::SameLine();
    ImGuiUiDrawer::text(fmt("Connected: {}", nbConnected));
    ImGui::SameLine();

    if(settings.globalSta.recorder.isRecording){
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
        ImGui::Text("RECORDING...");
        ImGui::PopStyleColor();
    }else{
        ImGui::Text("Not recording.");
    }

    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
}




auto K4SMLeftPanelChildDrawer::draw_ui_tab_item(ui::K4SMUiSettings &ui) -> void {

    if (ImGuiUiDrawer::begin_tab_item("UI###settings_ui_tabitem")){
        ImGui::Checkbox("Focus window", &ui.focusWindow);
        ImGui::EndTabItem();
    }
}

auto K4SMLeftPanelChildDrawer::draw_commands_tab_item(std::vector<K4GrabberSettings> &grabbersS)  -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Commands###settings_commands_tabitem")){
        return;
    }


    static ImGuiID tabId = 0;
    if(!ImGuiUiDrawer::begin_tab_bar(&tabId, "###settings_commands_sub_tabbar")){
        return;
    }

    draw_all_commands_tab_item(grabbersS);

    for(auto &grabber : grabbersS){
        draw_individual_commands_tab_item(grabber);
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto K4SMLeftPanelChildDrawer::draw_all_commands_tab_item(std::vector<K4GrabberSettings> &grabbersS) -> void{

    if (!ImGuiUiDrawer::begin_tab_item("[all]###all_settings_commands_tabitem")){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Network:");
    ImGui::Indent();

    if(ImGui::Button("Connect all###settings_connect_all_button")){
        for(const auto &grabberS : grabbersS){
            K4SMSignals::get()->init_connection_signal(grabberS.id);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Disconnect all###settings_disconnect_all_button")){
        for(const auto &grabberS : grabbersS){
            K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Disconnect);
        }
    }
    ImGui::Unindent();

    ImGui::Text("Program:");
    ImGui::Indent();
    if(ImGui::Button("Quit all###settings_quit_all_button")){
        for(const auto &grabberS : grabbersS){
            K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Quit);
        }
    }
    ImGui::Unindent();

    ImGui::Text("Computer:");
    ImGui::Indent();

    if(ImGui::Button("Shutdown all###settings_shutdown_all_button")){
        for(const auto &grabberS : grabbersS){
            K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Shutdown);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart all###settings_restart_all_button")){
        for(const auto &grabberS : grabbersS){
            K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Restart);
        }
    }
    ImGui::Unindent();

    ImGui::Spacing();

    ImGui::Text("Synchronization:");
    ImGui::Indent();
    ImGui::Text("Average difference:");
    ImGui::Indent();

    using namespace  std::chrono;
    for(size_t ii = 0; ii < grabbersS.size(); ++ii){
        ImGuiUiDrawer::text(std::format("Grabber {} : {} µs",
            ii,
            duration_cast<microseconds>(nanoseconds(grabbersS[ii].synchroAverageDiff)).count())
        );
    }
    ImGui::Unindent();
    ImGui::Unindent();

    ImGui::EndTabItem();
}

auto K4SMLeftPanelChildDrawer::draw_individual_commands_tab_item(K4GrabberSettings &grabberS) -> void{

    if (!ImGuiUiDrawer::begin_tab_item(std::format("[{}]###{}_settings_commands_tabitem", grabberS.id, grabberS.id).c_str())){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Network:");
    ImGui::Indent();

    if(ImGui::Button("Connect###settings_connect_button")){
        Logger::message(fmt("connect {}\n", grabberS.id));
        K4SMSignals::get()->init_connection_signal(grabberS.id);
    }
    ImGui::SameLine();
    if(ImGui::Button("Disconnect###settings_disconnect_button")){
        K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Disconnect);
    }
    ImGui::Unindent();

    ImGui::Text("Program:");
    ImGui::Indent();
    if(ImGui::Button("Quit###settings_quit_button")){
         K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Quit);
    }
    ImGui::Unindent();

    ImGui::Text("Computer:");
    ImGui::Indent();

    if(ImGui::Button("Shutdown###settings_shutdown_button")){
        K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Shutdown);
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart###settings_restart_button")){
        K4SMSignals::get()->command_signal(grabberS.id, network::K4Command::Restart);
    }

    ImGui::Unindent();

    ImGui::Text("Delay:");
    int delay = static_cast<int>(grabberS.delay.delayMs);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Current delay (ms)", "delay_others", &delay, ImGuiIntS{0,0, 5000,1.f,100},ImGuiDragS())){
        grabberS.delay.delayMs = delay;
        K4SMSignals::get()->update_delay_signal(grabberS.id, grabberS.delay);
    }
    ImGui::Indent();

    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("UDP reading:");
    ImGui::Indent();
    ImGuiUiDrawer::text(fmt("IP address: {}", grabberS.network.readingAdress));
    ImGuiUiDrawer::text(fmt("Port: {}", grabberS.network.readingPort));
    ImGui::Unindent();

    ImGui::Text("UDP sending:");
    ImGui::Indent();
    ImGuiUiDrawer::text(fmt("IP address: {}", grabberS.network.sendingAdress));
    ImGuiUiDrawer::text(fmt("Port: {}", grabberS.network.sendingPort));
    ImGui::Unindent();

    ImGui::Text("State:");
    ImGui::Indent();
    ImGui::Text(grabberS.network.connected ? "Grabber connected to manager" : "Grabber not connected");
    ImGuiUiDrawer::text(fmt("Last frame id sent: {}", grabberS.network.lastFrameIdReceived));
    ImGui::Unindent();

    ImGui::Separator();
    // ImGui::Text(fmt("Data received (mB): {:.2f}", network::K4SMNetworkInstance::net.connections[ii]->totalReceivedBytes*0.000001, 2));
    ImGui::Separator();
    ImGui::Text("Feedbacks:");
    feedbacksLogs[grabberS.id].draw(fmt("grabber_{}_feedback", grabberS.id).c_str());

    ImGui::EndTabItem();
}


auto K4SMLeftPanelChildDrawer::draw_device_tab_item(K4SMSettings &settings) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Device###device_tabitem")){
        return;
    }

    ImGui::Spacing();
    if(ImGui::Button("Send all###device_send_all")){
        for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){
            K4SMSignals::get()->update_device_settings_signal(ii, settings.grabbersSet[ii].device);
        }
    }
    ImGui::Dummy(ImVec2{0,20});

    static ImGuiID tabId = 0;
    if(ImGuiUiDrawer::begin_tab_bar(&tabId, "Device_sub###device_sub_tabbar")){

        for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){

            bool updateDeviceList = false;
            auto update = K4UIDrawer::draw_device_settings_tab_item(fmt("[{}]###device_{}_tabitem", ii, ii),
                {"Cam 0","Cam 1","Cam 2","Cam 3"},
                settings.grabbersSet[ii].device,
                updateDeviceList,
                autoUpdate
            );

            if(updateDeviceList){
                // ...
            }

            if(update){
                K4SMSignals::get()->update_device_settings_signal(ii, settings.grabbersSet[ii].device);
            }
        }

        ImGui::EndTabBar();
    }

    ImGui::EndTabItem();
}

auto K4SMLeftPanelChildDrawer::draw_filters_tab_item(K4SMSettings &settings) -> void {

    static constexpr std::string_view base = "###settings_filters";
    if (!ImGuiUiDrawer::begin_tab_item(fmt("Filters{}_tabitem", base).c_str())){
        return;
    }

    int filteringMode = settings.globalSet.useNormalFilteringSettings ? 0 : 1;
    ImGui::Text("Use filtering mode:");
    ImGui::SameLine();
    if(ImGui::RadioButton("Normal###filters_normal_mode", &filteringMode, 0)){
        K4SMSignals::get()->update_filtering_mode_signal(true);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Calibration###filters_calibration_mode", &filteringMode, 1)){
        K4SMSignals::get()->update_filtering_mode_signal(false);
    }
    ImGui::Spacing();

    static ImGuiId tabId1 = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId1, fmt("{}_sub_tabbar", base).c_str())){

        if (ImGuiUiDrawer::begin_tab_item(fmt("Normal{}_normal_tabitem", base).c_str())){

            static ImGuiId tabId2 = 0;
            if (ImGuiUiDrawer::begin_tab_bar(&tabId2, fmt("{}_normal_sub_tabbar",base).c_str())){

                for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){

                    auto ret =  K4UIDrawer::draw_filters_tab_item(
                            fmt("[{}]{}_normal_{}_tabitem", ii, base, ii),
                            settings.grabbersSet[ii].device.configS.mode,
                            settings.grabbersSet[ii].filters,
                        autoUpdate);

                    settings.globalSet.ui.settingsFiltersNormalSubPanelDisplayed = std::get<0>(ret) || settings.globalSet.ui.settingsFiltersNormalSubPanelDisplayed;

                    if(std::get<1>(ret)){
                        K4SMSignals::get()->update_filters_settings_signal(ii, settings.grabbersSet[ii].filters);
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }

        if (ImGuiUiDrawer::begin_tab_item(fmt("Calibration{}_calibration_tabitem", base).c_str())){

            static ImGuiId tabId2 = 0;
            if (ImGuiUiDrawer::begin_tab_bar(&tabId2, fmt("{}_calibration_sub_tabbar",base).c_str())){

                for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){

                    auto ret = K4UIDrawer::draw_filters_tab_item(
                            fmt("[{}]{}_calibration_{}_tabitem", ii, base, ii),
                            settings.grabbersSet[ii].device.configS.mode,
                            settings.grabbersSet[ii].calibrationFilters,
                        autoUpdate);

                    settings.globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed = std::get<0>(ret) || settings.globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed;

                    if(std::get<1>(ret)){
                        K4SMSignals::get()->update_filters_settings_signal(ii, settings.grabbersSet[ii].calibrationFilters);
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::Spacing();
    ImGuiUiDrawer::title2("COPY");

    if(ImGui::Button(fmt("Copy{}_copy_filters", base).c_str())){

        K4Filters *fromFilters = nullptr;
        if(guiCurrentFromFiltersSelection < static_cast<int>(settings.grabbersSet.size())){
            int idGrabber = guiCurrentFromFiltersSelection;
            fromFilters = &settings.grabbersSet[idGrabber].filters;
        }else{
            int idGrabber = guiCurrentFromFiltersSelection - static_cast<int>(settings.grabbersSet.size());
            fromFilters = &settings.grabbersSet[idGrabber].calibrationFilters;
        }

        if(guiCurrentTargetFiltersSelection == 0){ // all
            for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){
                K4SMSignals::get()->update_filters_settings_signal(ii, *fromFilters);
                K4SMSignals::get()->update_calibration_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection == 1){ // all normal
            for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){
                K4SMSignals::get()->update_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection == 2){ // all calibration
            for(size_t ii = 0; ii < settings.grabbersSet.size(); ++ii){
                K4SMSignals::get()->update_calibration_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection < static_cast<int>(settings.grabbersSet.size() + 3)){ // normal grabbers
            int idGrabber = guiCurrentTargetFiltersSelection-3;
            K4SMSignals::get()->update_filters_settings_signal(idGrabber, *fromFilters);
        }else{ // calibration grabbers
            int idGrabber = guiCurrentTargetFiltersSelection-3 - static_cast<int>(settings.grabbersSet.size());
            K4SMSignals::get()->update_calibration_filters_settings_signal(idGrabber, *fromFilters);
        }
    }
    ImGui::SameLine();
    ImGui::Text(" from:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGuiUiDrawer::combo(fmt("{}_from_filter", base).c_str(), &guiCurrentFromFiltersSelection, fromFilters);
    ImGui::SameLine();
    ImGui::Text(" to:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGuiUiDrawer::combo(fmt("{}_target_filter", base).c_str(), &guiCurrentTargetFiltersSelection, targetsFilters);


    ImGui::EndTabItem();
}

auto K4SMLeftPanelChildDrawer::draw_display_tab_item(camera::K4SceneDisplaySettings &sceneDisplay, std::vector<K4GrabberSettings> &grabbers) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Display###display_settings_tabitem")){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("All grabbers");
    ImGui::Indent();
    if(ImGui::Button("Send###display_send_all")){
        K4SMSignals::get()->update_scene_display_settings_signal(sceneDisplay);
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Spacing();

    ImGui::Text("Color");
    ImGui::SameLine();
    if(ImGui::Button("Per camera###force_cloud_color_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.forceCloudColor = true;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Real###real_cloud_color_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.forceCloudColor = false;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    auto factorUnicolor = grabbers.front().cloudDisplay.factorUnicolor;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Color factor", &factorUnicolor, 0.01f, 0.f, 1.f)){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.factorUnicolor = factorUnicolor;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Spacing();    

    ImGui::Text("Render");
    ImGui::SameLine();
    if(ImGui::Button("Use voxels###use_voxels_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.useVoxels = true;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Use points###use_points_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.useVoxels = false;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    auto sizeVoxels = grabbers.front().cloudDisplay.sizeVoxels;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Size voxels###size_voxels_all", &sizeVoxels, 0.01f, 0.001f, 10.f, "%.3f")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.sizeVoxels = sizeVoxels;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    auto sizePoints = grabbers.front().cloudDisplay.sizePoints;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Size points###size_points_all", &sizePoints, 0.1f, 0.1f, 30.f, "%.1f")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.sizePoints = sizePoints;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Spacing();

    ImGui::Text("Visibility");
    ImGui::SameLine();
    if(ImGui::Button("On###set_cloud_visible_all")){

        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.cloudVisible = true;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Off###set_cloud_invisible_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.cloudVisible = false;
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Unindent();


    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_tabbar")){
        return;
    }

    if(K4UIDrawer::draw_scene_display_setings_tab_item("Scene###scene_display_tabitem", sceneDisplay, autoUpdate)){
        K4SMSignals::get()->update_scene_display_settings_signal(sceneDisplay);
    }

    for(size_t ii = 0; ii < grabbers.size(); ++ii){
        if(K4UIDrawer::draw_cloud_display_setings_tab_item(fmt("[{}]###{}_cloud_display_tabitem", ii, ii), grabbers[ii].cloudDisplay, autoUpdate)){
            K4SMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto graphics::K4SMLeftPanelChildDrawer::draw_calibration_tab_item(std::vector<K4GrabberSettings> &grabbers) -> void {


    if (!ImGuiUiDrawer::begin_tab_item("Calibration###calibration_settings_tabitem")){
        return;
    }

    ImGui::Spacing();
//    if(ImGui::Button("Send all###model_send_all")){
//        for(size_t ii = 0; ii < grabbers.size(); ++ii){
//            K4SMSignals::get()->update_model_settings_signal(ii, grabbers[ii].model);
//        }
//    }
//    ImGui::Spacing();

    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###calibration_settings_tabbar")){
        return;
    }

    for(size_t ii = 0; ii < grabbers.size(); ++ii){
        if(K4UIDrawer::draw_calibration_tab_item(fmt("[{}]###{}_cloud_calibration_tabitem", ii, ii), grabbers[ii].model, autoUpdate)){
            K4SMSignals::get()->update_model_settings_signal(ii, grabbers[ii].model);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

void K4SMLeftPanelChildDrawer::draw_color_tab_item(std::vector<K4GrabberSettings> &grabbers){

    static constexpr std::string_view base = "###settings_colors";
    if (!ImGuiUiDrawer::begin_tab_item(std::format("Color###{}_tabitem", base).c_str())){
        return;
    }

    static ImGuiID tabId = 0;
    if(!ImGuiUiDrawer::begin_tab_bar(&tabId, "###color_tabbar")){
        return;
    }
    for(size_t ii = 0; ii < grabbers.size(); ++ii){
        if(K4UIDrawer::draw_colors_settings_tab_item(fmt("[{}]###{}_color_settings_tabitem", ii, ii), grabbers[ii].color, autoUpdate)){
            K4SMSignals::get()->update_color_settings_signal(ii, grabbers[ii].color);
        }
    }
    ImGui::EndTabBar();

    ImGui::Spacing();
    ImGuiUiDrawer::title2("COPY");

    if(ImGui::Button(fmt("Copy{}_copy_colors", base).c_str())){
        K4ColorSettings *fromColor = &grabbers[guiCurrentFromColorsSelection].color;
        if(guiCurrentTargetColorsSelection == 0){ // all
            for(size_t ii = 0; ii < grabbers.size(); ++ii){
                K4SMSignals::get()->update_color_settings_signal(ii, *fromColor);
            }
        }else if(guiCurrentTargetColorsSelection < grabbers.size() + 1){
            int idGrabber = guiCurrentTargetColorsSelection-1;
            K4SMSignals::get()->update_color_settings_signal(idGrabber, *fromColor);
        }
    }
    ImGui::SameLine();
    ImGui::Text(" from:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGuiUiDrawer::combo(fmt("{}_from_color", base).c_str(), &guiCurrentFromColorsSelection, fromColor);
    ImGui::SameLine();
    ImGui::Text(" to:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGuiUiDrawer::combo(fmt("{}_target_color", base).c_str(), &guiCurrentTargetColorsSelection, targetsColor);
    ImGui::Dummy(ImVec2{0,20});


    ImGui::EndTabItem();
}

auto K4SMLeftPanelChildDrawer::draw_logs_tab_item() -> void {
    if (ImGui::BeginTabItem("Logs###settings_logs_tabitem")){
        ImGui::Text("Global logs:");
        globalLogs.draw("global_logs");
        ImGui::EndTabItem();
    }
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

auto K4SMLeftPanelChildDrawer::draw_infos_tab_item(const K4SMSettings &settings) -> void{
    if (ImGui::BeginTabItem("Infos###settings_infos_tabitem")){

        ImGui::Text("Config files loaded:");
        ImGui::Spacing();

        ImGui::Text("Global:");
        ImGui::Indent();
            ImGui::Text("Network:");
            ImGui::SameLine();
            draw_config_file_name(settings.globalSet.networkFilePath);
        ImGui::Unindent();

        for(const auto &grabber : settings.grabbersSet){
            ImGuiUiDrawer::text(std::format("Grabber {}:", grabber.id));
            ImGui::Indent();
            ImGui::Text("Device:");
            ImGui::SameLine();
            draw_config_file_name(grabber.deviceFilePath);
            ImGui::Text("Model:");
            ImGui::SameLine();
            draw_config_file_name(grabber.modelFilePath);
            ImGui::Text("Filters:");
            ImGui::SameLine();
            draw_config_file_name(grabber.filtersFilePath);
            ImGui::Text("Cal. filters:");
            ImGui::SameLine();
            draw_config_file_name(grabber.calibrationFiltersFilePath);
            ImGui::Unindent();
        }

        ImGui::EndTabItem();
    }
}


