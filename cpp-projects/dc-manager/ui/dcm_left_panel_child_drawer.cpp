
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

#include "dcm_left_panel_child_drawer.hpp"

// std
#include <format>

// base
#include "utility/logger.hpp"
#include "utility/string.hpp"

// 3d-engine
#include "imgui-tb/imgui_dc_ui_drawer.hpp"
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "imgui/extra/ImGuiFileDialog.h"

// local
#include "dcm_signals.hpp"

using namespace tool;
using namespace tool::cam;
using namespace tool::graphics;
using namespace tool::net;

auto DCMLeftPanelChildDrawer::initialize(size_t nbGrabbers) -> void{

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
            fromFilters.push_back(std::format("[{}] normal", ii));
            fromColor.push_back(std::format("[{}]", ii));
        }
        for(size_t ii = 0; ii < nbGrabbers; ++ii){
            fromFilters.push_back(std::format("[{}] calib", ii));
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

auto DCMLeftPanelChildDrawer::draw(geo::Pt2f size, int windowFlags, DCMSettings &settings, DCMStates &states) -> void {

    settings.uiS.settingsFiltersNormalSubPanelDisplayed      = false;
    settings.uiS.settingsFiltersCalibrationSubPanelDisplayed = false;

    if(ImGui::BeginChild("###settings_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        auto region = ImGui::GetContentRegionAvail();
        auto region1 = region;
        auto region2 = region;
        region1.y *= 0.75f;
        if(ImGui::BeginChild("###settings_grabbers_child1", region1, true, windowFlags)){
            draw_grabbers_ui(settings, states);

            static ImGuiID tabId = 0;
            if(ImGuiUiDrawer::begin_tab_bar(&tabId, "###settings_tabbar1")){
                draw_commands_tab_item(settings.networkS, settings.grabbersS);
                draw_settings_tab_item(settings);
                draw_recorder_tab_item(states.recorder, settings.recorderS);
                draw_player_tab_item(states.player, settings.playerS);
                draw_calibrator_tab_item(settings.useNormalFilteringSettings, states.calibrator, settings.calibratorDrawerS, settings.calibratorS);
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


auto DCMLeftPanelChildDrawer::append_global_log(const std::string &log) -> void {
    globalLogs.add_log(log.c_str());
}

auto DCMLeftPanelChildDrawer::append_feedback_log(size_t idG, const std::string &log) -> void {
    if(idG < feedbacksLogs.size()){
        feedbacksLogs[idG].add_log(log.c_str());
    }
}

auto DCMLeftPanelChildDrawer::draw_settings_tab_item(DCMSettings &settings) -> void{
    if (ImGuiUiDrawer::begin_tab_item("Settings###settings_tabitem")){

        static ImGuiID tabId = 0;
        if(ImGuiUiDrawer::begin_tab_bar(&tabId, "###sub_settings_tabbar")){
            draw_device_tab_item(settings);
            draw_filters_tab_item(settings);
            draw_calibration_tab_item(settings.grabbersS);
            draw_color_tab_item(settings.grabbersS);
            draw_display_tab_item(settings.sceneDisplayS, settings.grabbersS);
            draw_ui_tab_item(settings.uiS);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}


auto DCMLeftPanelChildDrawer::draw_recorder_tab_item(cam::DCVideoRecorderStates &rStates, cam::DCVideoRecorderSettings &rSettings) -> void {

    if(DCUIDrawer::draw_dc_recorder_tab_item("Recorder###settings_recorder_tabitem", rStates, rSettings, autoUpdate)){
        DCMSignals::get()->update_recorder_settings_signal(rSettings);
    }

    if(rStates.startRecording){
        DCMSignals::get()->start_recorder_signal();
    }
    if(rStates.stopRecording){
        DCMSignals::get()->stop_recorder_signal();
    }
    if(rStates.resetRecording){
        DCMSignals::get()->reset_recorder_signal();
    }
    if(rStates.moveTime){
        DCMSignals::get()->set_recorder_time_signal(rStates.currentTime);
    }

    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCMSignals::get()->save_recorder_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    rStates.reset_actions();
}

auto DCMLeftPanelChildDrawer::draw_player_tab_item(cam::DCVideoPlayerStates &pStates, cam::DCVideoPlayerSettings &pSettings) -> void{

    if(DCUIDrawer::draw_dc_player_tab_item("Player###settings_player_tabitem", pStates, pSettings, autoUpdate)){
        DCMSignals::get()->update_player_settings_signal(pSettings);
    }

    if(pStates.play){
        DCMSignals::get()->start_player_signal();
    }
    if(pStates.pause){
        DCMSignals::get()->pause_player_signal();
    }
    if(pStates.restart){
        DCMSignals::get()->restart_player_signal();
    }
    if(pStates.removeUntil){
        DCMSignals::get()->remove_until_current_frame_player_signal();
    }
    if(pStates.removeAfter){
        DCMSignals::get()->remove_after_current_frame_player_signal();
    }
    if(pStates.merge){
        DCMSignals::get()->merge_player_signal();
    }
    if(pStates.info){
        DCMSignals::get()->info_player_signal();
    }
    if(pStates.moveTime){
        DCMSignals::get()->set_player_time_signal(pStates.currentTime);
    }

    if (ImGuiFileDialog::Instance()->Display("Save cloud")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCMSignals::get()->save_cloud_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Save video")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCMSignals::get()->save_video_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("Load video")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCMSignals::get()->load_video_player_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }
    pStates.reset_actions();
}

auto DCMLeftPanelChildDrawer::draw_calibrator_tab_item(bool useNormalFilteringSettings, cam::DCCalibratorStates &cStates, DCCalibratorDrawerSettings &cdSettings, cam::DCCalibratorSettings &cSettings) -> void{

    if(DCUIDrawer::draw_dc_calibrator_tab_item("Calibrator###calibrator_tabitem",  useNormalFilteringSettings, cStates, cdSettings, cSettings, autoUpdate)){
        DCMSignals::get()->update_calibration_settings_signal(cSettings);
        DCMSignals::get()->update_calibration_drawer_settings_signal(cdSettings);
    }

    if(cStates.recomputeRegisteringProcessing){
        DCMSignals::get()->recompute_registering_processing_signal();
    }
    if(cStates.startCalibration){
        DCMSignals::get()->start_calibration_registering_signal();
    }
    if(cStates.stopCalibration){
        DCMSignals::get()->stop_calibration_registering_signal();
    }
    if(cStates.resetCalibration){
        DCMSignals::get()->reset_calibration_registering_signal();
    }
    if(cStates.calibrate){
        DCMSignals::get()->ask_calibration_signal();
    }
    if(cStates.validateCalibration){
        DCMSignals::get()->validate_calibration_signal();
    }
    if(cStates.updateDisplaySettings){
        DCMSignals::get()->update_calibration_display_signal();
    }
    if(cStates.updateFilteringMode){
        DCMSignals::get()->update_filtering_mode_signal(cStates.filteringMode == 0);
    }

    cStates.reset_actions();
}

auto DCMLeftPanelChildDrawer::draw_grabbers_ui(DCMSettings &settings, DCMStates &states) -> void {

    ImGui::Text("Grabbers");

    for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){
        ImGuiUiDrawer::text_colored(settings.grabbersS[ii].network.connected ? DCMSettings::connectedSelectedC : DCMSettings::selectedC, settings.grabbersS[ii].network.name);
        if((ii % 4) != 0 || ii == 0){
            ImGui::SameLine();
        }
    }

    int nbConnected = 0;
    for(const auto &grabber : settings.grabbersS){
        if(grabber.network.connected){
            ++nbConnected;
        }
    }
    ImGuiUiDrawer::text(std::format("Count: {}", settings.grabbersS.size()));
    ImGui::SameLine();
    ImGuiUiDrawer::text(std::format("Connected: {}", nbConnected));
    ImGui::SameLine();

    if(states.recorder.isRecording){
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




auto DCMLeftPanelChildDrawer::draw_ui_tab_item(DCMUiSettings &ui) -> void {

    if (ImGuiUiDrawer::begin_tab_item("UI###settings_ui_tabitem")){
        ImGui::Checkbox("Focus window", &ui.focusWindow);
        ImGui::EndTabItem();
    }
}

auto DCMLeftPanelChildDrawer::draw_commands_tab_item(const UdpServerNetworkSettings &networkS, std::vector<DCMGrabberSettings> &grabbersS)  -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Commands###settings_commands_tabitem")){
        return;
    }


    static ImGuiID tabId = 0;
    if(!ImGuiUiDrawer::begin_tab_bar(&tabId, "###settings_commands_sub_tabbar")){
        return;
    }

    draw_all_commands_tab_item(networkS, grabbersS);

    for(auto &grabber : grabbersS){
        draw_individual_commands_tab_item(grabber);
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_all_commands_tab_item(const UdpServerNetworkSettings &networkS, std::vector<DCMGrabberSettings> &grabbersS) -> void{

    if (!ImGuiUiDrawer::begin_tab_item("[all]###all_settings_commands_tabitem")){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Network:");
    ImGui::Indent();

    if(ImGui::Button("Reset###settings_reset_network")){
        DCMSignals::get()->reset_network_signal();
    }
    ImGui::SameLine();
    if(ImGui::Button("Connect all###settings_connect_all_button")){
        for(const auto &grabberS : grabbersS){
            DCMSignals::get()->init_connection_signal(grabberS.id);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Disconnect all###settings_disconnect_all_button")){
        for(const auto &grabberS : grabbersS){
            DCMSignals::get()->command_signal(grabberS.id, net::Command::Disconnect);
        }
    }
    ImGui::Unindent();

    ImGui::Text("Program:");
    ImGui::Indent();
    if(ImGui::Button("Quit all###settings_quit_all_button")){
        for(const auto &grabberS : grabbersS){
            DCMSignals::get()->command_signal(grabberS.id, net::Command::Quit);
        }
    }
    ImGui::Unindent();

    ImGui::Text("Computer:");
    ImGui::Indent();

    if(ImGui::Button("Shutdown all###settings_shutdown_all_button")){
        for(const auto &grabberS : grabbersS){
            DCMSignals::get()->command_signal(grabberS.id, net::Command::Shutdown);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart all###settings_restart_all_button")){
        for(const auto &grabberS : grabbersS){
            DCMSignals::get()->command_signal(grabberS.id, net::Command::Restart);
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

    ImGui::Text("Network interfaces:");
    ImGui::Spacing();
    ImGui::Text("IPV4:");
    for(size_t id = 0; id < networkS.ipv4Interfaces.size(); ++id){
        ImGuiUiDrawer::text(std::format("[Id]: {}, [A]: {}",
            id,
            networkS.ipv4Interfaces[id].ipAddress)
        );
    }
    ImGui::Spacing();
    ImGui::Text("IPV6:");
    for(size_t id = 0; id < networkS.ipv6Interfaces.size(); ++id){
        ImGuiUiDrawer::text(std::format("[Id]: {}, [A]: {}",
            id,
            networkS.ipv6Interfaces[id].ipAddress)
        );
    }

    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_individual_commands_tab_item(DCMGrabberSettings &grabberS) -> void{

    if (!ImGuiUiDrawer::begin_tab_item(std::format("[{}]###{}_settings_commands_tabitem", grabberS.id, grabberS.id).c_str())){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Network:");
    ImGui::Indent();

    if(ImGui::Button("Connect###settings_connect_button")){
        DCMSignals::get()->init_connection_signal(grabberS.id);
    }
    ImGui::SameLine();
    if(ImGui::Button("Disconnect###settings_disconnect_button")){
        DCMSignals::get()->command_signal(grabberS.id, net::Command::Disconnect);
    }
    ImGui::Unindent();

    ImGui::Text("Program:");
    ImGui::Indent();
    if(ImGui::Button("Quit###settings_quit_button")){
        DCMSignals::get()->command_signal(grabberS.id, net::Command::Quit);
    }
    ImGui::Unindent();

    ImGui::Text("Computer:");
    ImGui::Indent();

    if(ImGui::Button("Shutdown###settings_shutdown_button")){
        DCMSignals::get()->command_signal(grabberS.id, net::Command::Shutdown);
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart###settings_restart_button")){
        DCMSignals::get()->command_signal(grabberS.id, net::Command::Restart);
    }

    ImGui::Unindent();

    ImGui::Text("Delay:");
    int delay = static_cast<int>(grabberS.delay.delayMs);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Current delay (ms)", "delay_others", &delay, ImGuiIntS{0,0, 5000,1.f,100},ImGuiDragS())){
        grabberS.delay.delayMs = delay;
        DCMSignals::get()->update_delay_settings_signal(grabberS.id, grabberS.delay);
    }
    ImGui::Indent();

    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("UDP reading:");
    ImGui::Indent();
    ImGuiUiDrawer::text(std::format("IP address: {}", grabberS.network.readingAdress));
    ImGuiUiDrawer::text(std::format("Port: {}", grabberS.network.readingPort));
    ImGui::Unindent();

    if(grabberS.network.protocol == net::Protocol::ipv6){
        ImGui::Text("PROTOCOL: IPV6");
    }else{
        ImGui::Text("PROTOCOL: IPV4");
    }
    ImGui::Spacing();

    ImGui::Text("UDP sending:");
    ImGui::Indent();
    ImGuiUiDrawer::text(std::format("IP address: {}", grabberS.network.sendingAdress));
    ImGuiUiDrawer::text(std::format("Port: {}", grabberS.network.sendingPort));
    ImGui::Unindent();

    ImGui::Text("State:");
    ImGui::Indent();
    ImGui::Text(grabberS.network.connected ? "Grabber connected to manager" : "Grabber not connected");
    ImGuiUiDrawer::text(std::format("Last frame id sent: {}", grabberS.network.lastFrameIdReceived));
    ImGui::Unindent();

    ImGui::Separator();
    // ImGui::Text(std::format("Data received (mB): {:.2f}", net::K4SMNetworkInstance::net.connections[ii]->totalReceivedBytes*0.000001, 2));
    ImGui::Separator();
    ImGui::Text("Feedbacks:");
    feedbacksLogs[grabberS.id].draw(std::format("grabber_{}_feedback", grabberS.id).c_str());

    ImGui::EndTabItem();
}


auto DCMLeftPanelChildDrawer::draw_device_tab_item(DCMSettings &settings) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Device###device_tabitem")){
        return;
    }

    ImGui::Spacing();
    if(ImGui::Button("Send all###device_send_all")){
        for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){
            DCMSignals::get()->update_device_settings_signal(ii, settings.grabbersS[ii].device);
        }
    }
    ImGui::Dummy(ImVec2{0,20});

    static ImGuiID tabId = 0;
    if(ImGuiUiDrawer::begin_tab_bar(&tabId, "Device_sub###device_sub_tabbar")){

        for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){

            bool updateDeviceList = false;

            auto currentDeviceType = settings.grabbersS[ii].device.configS.typeDevice;
            auto update = DCUIDrawer::draw_dc_device_settings_tab_item(std::format("[{}]###device_{}_tabitem", ii, ii),
                                                                       settings.grabbersS[ii].device,
                updateDeviceList,
                autoUpdate
            );

            if(currentDeviceType != settings.grabbersS[ii].device.configS.typeDevice){
                tool::Logger::message("DEVICE CHANGED COLOR SETTINGS DEFAULTED\n");
                settings.grabbersS[ii].color.set_default_values(settings.grabbersS[ii].device.configS.typeDevice);
                DCMSignals::get()->color_settings_reset_signal(ii, settings.grabbersS[ii].color);
            }

            if(update){
                DCMSignals::get()->update_device_settings_signal(ii, settings.grabbersS[ii].device);
            }
        }

        ImGui::EndTabBar();
    }

    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_filters_tab_item(DCMSettings &settings) -> void {

    static constexpr std::string_view base = "###settings_filters";
    if (!ImGuiUiDrawer::begin_tab_item(std::format("Filters{}_tabitem", base).c_str())){
        return;
    }

    int filteringMode = settings.useNormalFilteringSettings ? 0 : 1;
    ImGui::Text("Use filtering mode:");
    ImGui::SameLine();
    if(ImGui::RadioButton("Normal###filters_normal_mode", &filteringMode, 0)){
        DCMSignals::get()->update_filtering_mode_signal(true);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Calibration###filters_calibration_mode", &filteringMode, 1)){
        DCMSignals::get()->update_filtering_mode_signal(false);
    }
    ImGui::Spacing();

    static ImGuiId tabId1 = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId1, std::format("{}_sub_tabbar", base).c_str())){

        if (ImGuiUiDrawer::begin_tab_item(std::format("Normal{}_normal_tabitem", base).c_str())){

            static ImGuiId tabId2 = 0;
            if (ImGuiUiDrawer::begin_tab_bar(&tabId2, std::format("{}_normal_sub_tabbar",base).c_str())){

                for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){

                    auto ret =  DCUIDrawer::draw_dc_filters_settings_tab_item(
                            std::format("[{}]{}_normal_{}_tabitem", ii, base, ii),
                        settings.grabbersS[ii].device.configS.mode,
                        settings.grabbersS[ii].filters,
                        autoUpdate);

                    settings.uiS.settingsFiltersNormalSubPanelDisplayed = std::get<0>(ret) || settings.uiS.settingsFiltersNormalSubPanelDisplayed;

                    if(std::get<1>(ret)){
                        DCMSignals::get()->update_filters_settings_signal(ii, settings.grabbersS[ii].filters);
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }

        if (ImGuiUiDrawer::begin_tab_item(std::format("Calibration{}_calibration_tabitem", base).c_str())){

            static ImGuiId tabId2 = 0;
            if (ImGuiUiDrawer::begin_tab_bar(&tabId2, std::format("{}_calibration_sub_tabbar",base).c_str())){

                for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){

                    auto ret = DCUIDrawer::draw_dc_filters_settings_tab_item(
                            std::format("[{}]{}_calibration_{}_tabitem", ii, base, ii),
                        settings.grabbersS[ii].device.configS.mode,
                        settings.grabbersS[ii].calibrationFilters,
                        autoUpdate);

                    settings.uiS.settingsFiltersCalibrationSubPanelDisplayed = std::get<0>(ret) || settings.uiS.settingsFiltersCalibrationSubPanelDisplayed;

                    if(std::get<1>(ret)){
                        DCMSignals::get()->update_calibration_filters_settings_signal(ii, settings.grabbersS[ii].calibrationFilters);
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

    if(ImGui::Button(std::format("Copy{}_copy_filters", base).c_str())){

        DCFiltersSettings *fromFilters = nullptr;
        if(guiCurrentFromFiltersSelection < static_cast<int>(settings.grabbersS.size())){
            int idGrabber = guiCurrentFromFiltersSelection;
            fromFilters = &settings.grabbersS[idGrabber].filters;
        }else{
            int idGrabber = guiCurrentFromFiltersSelection - static_cast<int>(settings.grabbersS.size());
            fromFilters = &settings.grabbersS[idGrabber].calibrationFilters;
        }

        if(guiCurrentTargetFiltersSelection == 0){ // all
            for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){
                DCMSignals::get()->update_filters_settings_signal(ii, *fromFilters);
                DCMSignals::get()->update_calibration_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection == 1){ // all normal
            for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){
                DCMSignals::get()->update_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection == 2){ // all calibration
            for(size_t ii = 0; ii < settings.grabbersS.size(); ++ii){
                DCMSignals::get()->update_calibration_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection < static_cast<int>(settings.grabbersS.size() + 3)){ // normal grabbers
            int idGrabber = guiCurrentTargetFiltersSelection-3;
            DCMSignals::get()->update_filters_settings_signal(idGrabber, *fromFilters);
        }else{ // calibration grabbers
            int idGrabber = guiCurrentTargetFiltersSelection-3 - static_cast<int>(settings.grabbersS.size());
            DCMSignals::get()->update_calibration_filters_settings_signal(idGrabber, *fromFilters);
        }
    }
    ImGui::SameLine();
    ImGui::Text(" from:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGuiUiDrawer::combo(std::format("{}_from_filter", base).c_str(), &guiCurrentFromFiltersSelection, fromFilters);
    ImGui::SameLine();
    ImGui::Text(" to:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGuiUiDrawer::combo(std::format("{}_target_filter", base).c_str(), &guiCurrentTargetFiltersSelection, targetsFilters);


    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_display_tab_item(DCSceneDisplaySettings &sceneDisplay, std::vector<DCMGrabberSettings> &grabbers) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Display###display_settings_tabitem")){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("All grabbers");
    ImGui::Indent();
    if(ImGui::Button("Send###display_send_all")){
        DCMSignals::get()->update_scene_display_settings_signal(sceneDisplay);
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Spacing();

    ImGui::Text("Color");
    ImGui::SameLine();
    if(ImGui::Button("Per camera###force_cloud_color_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.forceCloudColor = true;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Real###real_cloud_color_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.forceCloudColor = false;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    auto factorUnicolor = grabbers.front().cloudDisplay.factorUnicolor;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Color factor", &factorUnicolor, 0.01f, 0.f, 1.f)){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.factorUnicolor = factorUnicolor;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Spacing();    

    ImGui::Text("Render");
    ImGui::SameLine();
    if(ImGui::Button("Use voxels###use_voxels_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.useVoxels = true;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Use points###use_points_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.useVoxels = false;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    auto sizeVoxels = grabbers.front().cloudDisplay.sizeVoxels;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Size voxels###size_voxels_all", &sizeVoxels, 0.01f, 0.001f, 10.f, "%.3f")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.sizeVoxels = sizeVoxels;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    auto sizePoints = grabbers.front().cloudDisplay.sizePoints;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Size points###size_points_all", &sizePoints, 0.1f, 0.1f, 30.f, "%.1f")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.sizePoints = sizePoints;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Spacing();

    ImGui::Text("Visibility");
    ImGui::SameLine();
    if(ImGui::Button("On###set_cloud_visible_all")){

        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.cloudVisible = true;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Off###set_cloud_invisible_all")){
        for(size_t ii = 0; ii < grabbers.size(); ++ii){
            grabbers[ii].cloudDisplay.cloudVisible = false;
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }
    ImGui::Unindent();


    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_tabbar")){
        return;
    }

    if(DCUIDrawer::draw_dc_scene_display_setings_tab_item("Scene###scene_display_tabitem", sceneDisplay, autoUpdate)){
        DCMSignals::get()->update_scene_display_settings_signal(sceneDisplay);
    }

    for(size_t ii = 0; ii < grabbers.size(); ++ii){
        if(DCUIDrawer::draw_dc_cloud_display_setings_tab_item(std::format("[{}]###{}_cloud_display_tabitem", ii, ii), grabbers[ii].cloudDisplay, autoUpdate)){
            DCMSignals::get()->update_cloud_display_settings_signal(ii, grabbers[ii].cloudDisplay);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto graphics::DCMLeftPanelChildDrawer::draw_calibration_tab_item(std::vector<DCMGrabberSettings> &grabbers) -> void {


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
        if(DCUIDrawer::draw_dc_model_tab_item(std::format("[{}]###{}_cloud_calibration_tabitem", ii, ii), grabbers[ii].model, autoUpdate)){
            DCMSignals::get()->update_model_settings_signal(ii, grabbers[ii].model);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_color_tab_item(std::vector<DCMGrabberSettings> &grabbers) -> void{

    static constexpr std::string_view base = "###settings_colors";
    if (!ImGuiUiDrawer::begin_tab_item(std::format("Color###{}_tabitem", base).c_str())){
        return;
    }

    static ImGuiID tabId = 0;
    if(!ImGuiUiDrawer::begin_tab_bar(&tabId, "###color_tabbar")){
        return;
    }

    for(size_t ii = 0; ii < grabbers.size(); ++ii){        
        if(DCUIDrawer::draw_dc_colors_settings_tab_item(std::format("[{}]###{}_color_settings_tabitem", ii, ii), grabbers[ii].device.configS.typeDevice, grabbers[ii].color, autoUpdate)){
            DCMSignals::get()->update_color_settings_signal(ii, grabbers[ii].color);
        }
    }
    ImGui::EndTabBar();

    ImGui::Spacing();
    ImGuiUiDrawer::title2("COPY");

    if(ImGui::Button(std::format("Copy{}_copy_colors", base).c_str())){
        DCColorSettings *fromColor = &grabbers[guiCurrentFromColorsSelection].color;
        if(guiCurrentTargetColorsSelection == 0){ // all
            for(size_t ii = 0; ii < grabbers.size(); ++ii){
                DCMSignals::get()->update_color_settings_signal(ii, *fromColor);
            }
        }else if(guiCurrentTargetColorsSelection < grabbers.size() + 1){
            int idGrabber = guiCurrentTargetColorsSelection-1;
            DCMSignals::get()->update_color_settings_signal(idGrabber, *fromColor);
        }
    }
    ImGui::SameLine();
    ImGui::Text(" from:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGuiUiDrawer::combo(std::format("{}_from_color", base).c_str(), &guiCurrentFromColorsSelection, fromColor);
    ImGui::SameLine();
    ImGui::Text(" to:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    ImGuiUiDrawer::combo(std::format("{}_target_color", base).c_str(), &guiCurrentTargetColorsSelection, targetsColor);
    ImGui::Dummy(ImVec2{0,20});


    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_logs_tab_item() -> void {
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

auto DCMLeftPanelChildDrawer::draw_infos_tab_item(const DCMSettings &settings) -> void{
    if (ImGui::BeginTabItem("Infos###settings_infos_tabitem")){

        ImGui::Text("Config files loaded:");
        ImGui::Spacing();

        ImGui::Text("Global:");
        ImGui::Indent();
            ImGui::Text("Network:");
            ImGui::SameLine();
            draw_config_file_name(settings.networkS.filePath);
        ImGui::Unindent();

            for(const auto &grabber : settings.grabbersS){
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


