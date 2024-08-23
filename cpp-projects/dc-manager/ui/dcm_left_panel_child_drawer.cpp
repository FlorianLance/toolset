
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
#include "imgui/misc/cpp/imgui_stdlib.h"

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

auto DCMLeftPanelChildDrawer::draw(geo::Pt2f size, int windowFlags, DCMModel *model) -> void {

    if(ImGui::BeginChild("###settings_child", ImVec2(size.x(), size.y()), true, windowFlags)){

        auto region = ImGui::GetContentRegionAvail();
        auto region1 = region;
        auto region2 = region;
        region1.y *= 0.75f;
        if(ImGui::BeginChild("###settings_grabbers_child1", region1, true, windowFlags)){

            draw_grabbers_ui(model);

            if(ImGui::BeginTabBar("###settings_tabbar1")){
                draw_commands_tab_item(model);
                draw_settings_tab_item(model);
                draw_recorder_tab_item(model->recorder);
                draw_player_tab_item(model->player);
                draw_calibrator_tab_item(model->client.settings.useNormalFilteringSettings,  model->calibrator.states, model->uiSettings.calibratorDisplayS, model->calibrator.settings);
                ImGui::EndTabBar();
            }
        }
        ImGui::EndChild();

        region2.y *= 0.25f;
        if(ImGui::BeginChild("###settings_grabbers_child2", region2, true, windowFlags)){
            if (ImGui::BeginTabBar("###settings_tabbar2")){
                draw_infos_tab_item(model->client);
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

auto DCMLeftPanelChildDrawer::draw_settings_tab_item(DCMModel *model) -> void{
    if (ImGuiUiDrawer::begin_tab_item("Settings###settings_tabitem")){

        static ImGuiID tabId = 0;
        if(ImGuiUiDrawer::begin_tab_bar(&tabId, "###sub_settings_tabbar")){
            draw_device_tab_item(model->client);
            draw_filters_tab_item(model->client);
            draw_calibration_tab_item(model->client);
            draw_color_tab_item(model->client);
            draw_display_tab_item(model->client);
            draw_ui_tab_item(model->uiSettings);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

auto DCMLeftPanelChildDrawer::draw_recorder_tab_item(cam::DCVideoRecorder &recorder) -> void {

    if(DCUIDrawer::draw_dc_recorder_tab_item("Recorder###settings_recorder_tabitem", recorder.states, recorder.settings)){
        DCMSignals::get()->update_recorder_settings_signal(recorder.settings);
    }

    if(recorder.states.startRecording){
        DCMSignals::get()->start_recorder_signal();
    }
    if(recorder.states.stopRecording){
        DCMSignals::get()->stop_recorder_signal();
    }
    if(recorder.states.resetRecording){
        DCMSignals::get()->reset_recorder_signal();
    }
    if(recorder.states.moveTime){
        DCMSignals::get()->set_recorder_time_signal(recorder.states.currentTime);
    }

    if (ImGuiFileDialog::Instance()->Display("Save recording")) {
        if (ImGuiFileDialog::Instance()->IsOk()){
            DCMSignals::get()->save_recorder_signal(ImGuiFileDialog::Instance()->GetFilePathName());
        }
        ImGuiFileDialog::Instance()->Close();
    }

    recorder.states.reset_actions();
}

auto DCMLeftPanelChildDrawer::draw_player_tab_item(cam::DCVideoPlayer &player) -> void{

    if(DCUIDrawer::draw_dc_player_tab_item("Player###settings_player_tabitem", player.states, player.settings)){
        DCMSignals::get()->update_player_settings_signal(player.settings);
    }

    if(player.states.play){
        DCMSignals::get()->start_player_signal();
    }
    if(player.states.pause){
        DCMSignals::get()->pause_player_signal();
    }
    if(player.states.restart){
        DCMSignals::get()->restart_player_signal();
    }
    if(player.states.removeUntil){
        DCMSignals::get()->remove_until_current_frame_player_signal();
    }
    if(player.states.removeAfter){
        DCMSignals::get()->remove_after_current_frame_player_signal();
    }
    if(player.states.merge){
        DCMSignals::get()->merge_player_signal();
    }
    if(player.states.info){
        DCMSignals::get()->info_player_signal();
    }
    if(player.states.moveTime){
        DCMSignals::get()->set_player_time_signal(player.states.currentTime);
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
    player.states.reset_actions();
}

auto DCMLeftPanelChildDrawer::draw_calibrator_tab_item(bool useNormalFilteringSettings, cam::DCCalibratorStates &cStates, DCCalibratorDisplaySettings &cdSettings, cam::DCCalibratorSettings &cSettings) -> void{

    if(DCUIDrawer::draw_dc_calibrator_tab_item("Calibrator###calibrator_tabitem",  useNormalFilteringSettings, cStates, cdSettings, cSettings)){
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
        // DCMSignals::get()->update_filtering_mode_signal(cStates.filteringMode == 0);
    }

    cStates.reset_actions();
}

auto to_imvec4(const geo::Pt4f &pt) -> ImVec4{
    return ImVec4(pt.x(),pt.y(),pt.z(),pt.w());
}

auto DCMLeftPanelChildDrawer::draw_grabbers_ui(DCMModel *model) -> void {

    ImGui::Text("Servers");

    for(const auto &cDeviceS : model->client.settings.devicesS){
        ImGuiUiDrawer::text_colored(to_imvec4(cDeviceS.connected ? model->uiSettings.connectedSelectedC : model->uiSettings.selectedC), cDeviceS.name);
        if((cDeviceS.id % 4) != 0 || cDeviceS.id == 0){
            ImGui::SameLine();
        }
    }

    int nbConnected = 0;
    for(const auto &cDeviceS : model->client.settings.devicesS){
        if(cDeviceS.connected){
            ++nbConnected;
        }
    }
    ImGuiUiDrawer::text(std::format("Count: {}", model->client.devices_nb()));
    ImGui::SameLine();
    ImGuiUiDrawer::text(std::format("Connected: {}", nbConnected));
    ImGui::SameLine();

    if(model->recorder.states.isRecording){
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

auto DCMLeftPanelChildDrawer::draw_commands_tab_item(DCMModel *model)  -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Commands###settings_commands_tabitem")){
        return;
    }

    static ImGuiID tabId = 0;
    if(!ImGuiUiDrawer::begin_tab_bar(&tabId, "###settings_commands_sub_tabbar")){
        return;
    }

    draw_all_commands_tab_item(model);
    
    for(size_t idC = 0; idC < model->client.settings.devicesS.size(); ++idC){
        draw_individual_commands_tab_item(model->client.settings.devicesS[idC]);
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_all_commands_tab_item(DCMModel *model) -> void{

    using namespace  std::chrono;

    if (!ImGuiUiDrawer::begin_tab_item("[all]###all_settings_commands_tabitem")){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Network:");
    ImGui::Indent();

    // if(ImGui::Button("Reset###settings_reset_network")){
    //     DCMSignals::get()->reset_network_signal();
    // }
    // ImGui::SameLine();
    if(ImGui::Button("Connect all###settings_connect_all_button")){                
        for(const auto &deviceS : model->client.settings.devicesS){
            DCMSignals::get()->init_connection_signal(deviceS.id);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Disconnect all###settings_disconnect_all_button")){
        for(const auto &deviceS : model->client.settings.devicesS){
            DCMSignals::get()->command_signal(deviceS.id, net::Command::disconnect);
        }
    }
    ImGui::Unindent();

    ImGui::Text("Program:");
    ImGui::Indent();
    if(ImGui::Button("Quit all###settings_quit_all_button")){
        for(const auto &deviceS : model->client.settings.devicesS){
            DCMSignals::get()->command_signal(deviceS.id, net::Command::quit);
        }
    }
    ImGui::Unindent();

    ImGui::Text("Computer:");
    ImGui::Indent();

    if(ImGui::Button("Shutdown all###settings_shutdown_all_button")){
        for(const auto &deviceS : model->client.settings.devicesS){
            DCMSignals::get()->command_signal(deviceS.id, net::Command::shutdown);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart all###settings_restart_all_button")){
        for(const auto &deviceS : model->client.settings.devicesS){
            DCMSignals::get()->command_signal(deviceS.id, net::Command::restart);
        }
    }
    ImGui::Unindent();

    ImGui::Spacing();

    ImGui::Text("Status:");
    ImGui::Indent();
    {
        ImGui::Text("Network");
        ImGui::Text("# Percentage success (/100):");
        ImGui::Indent();
        for(const auto &deviceS : model->client.settings.devicesS){
            ImGuiUiDrawer::text(std::format("Grabber {} : {} ", deviceS.id, deviceS.receivedNetworkStatus.percentageSuccess));
        }
        ImGui::Unindent();

        ImGui::Text("# Bandwidth (MB/s)):");
        ImGui::Indent();
        for(const auto &deviceS : model->client.settings.devicesS){
            ImGuiUiDrawer::text(std::format("Grabber {} : {}", deviceS.id, deviceS.receivedNetworkStatus.bandwidthBytes/1048576));
        }
        ImGui::Unindent();

        ImGui::Text("Data");
        ImGui::Text("# Framerate (fps):");
        ImGui::Indent();
        for(const auto &deviceS : model->client.settings.devicesS){
            ImGuiUiDrawer::text(std::format("Grabber {} : {}", deviceS.id, deviceS.receivedDataStatus.framerate));
        }
        ImGui::Unindent();


        ImGui::Text("Total latency (from capture to reception) (µs):");
        ImGui::Indent();
        for(const auto &deviceS : model->client.settings.devicesS){
            ImGuiUiDrawer::text(std::format("Grabber {} : {}", deviceS.id, deviceS.receivedDataStatus.latency));
        }
        ImGui::Unindent();
    }
    ImGui::Unindent();

    ImGui::Text("Synchronization:");
    ImGui::Indent();
    {
        ImGui::Text("Average difference (µs):");
        ImGui::Indent();
        for(const auto &deviceS : model->client.settings.devicesS){
            ImGuiUiDrawer::text(std::format("Grabber {} : {} ",
                deviceS.id,
                duration_cast<microseconds>(nanoseconds(deviceS.synchroAverageDiff)).count())
            );
        }
        ImGui::Unindent();
    }
    ImGui::Unindent();

    ImGui::Text("Network interfaces:");
    ImGui::Spacing();
    ImGui::Text("IPV4:");

    for(size_t id = 0; id < model->client.settings.ipv4Interfaces.size(); ++id){
        ImGuiUiDrawer::text(std::format("[Id]: {}, [A]: {}",
            id,
            model->client.settings.ipv4Interfaces[id].ipAddress)
        );
    }
    ImGui::Spacing();
    ImGui::Text("IPV6:");
    for(size_t id = 0; id < model->client.settings.ipv6Interfaces.size(); ++id){
        auto address = model->client.settings.ipv6Interfaces[id].ipAddress;
        String::replace_all(address, "%", "[P]");
        ImGuiUiDrawer::text(std::format("[Id]: {}, [A]: {}",
            id,
            address
        ));
    }

    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_individual_commands_tab_item(DCClientDeviceSettings &clientDeviceS) -> void{

    if (!ImGuiUiDrawer::begin_tab_item(std::format("[{}]###{}_settings_commands_tabitem", clientDeviceS.id, clientDeviceS.id).c_str())){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("Network:");
    ImGui::Indent();

    if(ImGui::Button("Connect###settings_connect_button")){
        DCMSignals::get()->init_connection_signal(clientDeviceS.id);
    }
    ImGui::SameLine();
    if(ImGui::Button("Disconnect###settings_disconnect_button")){
        DCMSignals::get()->command_signal(clientDeviceS.id, net::Command::disconnect);
    }
    ImGui::Unindent();

    ImGui::Text("Program:");
    ImGui::Indent();
    if(ImGui::Button("Quit###settings_quit_button")){
        DCMSignals::get()->command_signal(clientDeviceS.id, net::Command::quit);
    }
    ImGui::Unindent();

    ImGui::Text("Computer:");
    ImGui::Indent();

    if(ImGui::Button("Shutdown###settings_shutdown_button")){
        DCMSignals::get()->command_signal(clientDeviceS.id, net::Command::shutdown);
    }
    ImGui::SameLine();
    if(ImGui::Button("Restart###settings_restart_button")){
        DCMSignals::get()->command_signal(clientDeviceS.id, net::Command::restart);
    }

    ImGui::Unindent();

    ImGui::Text("Delay:");
    int delay = static_cast<int>(clientDeviceS.delayS.delayMs);
    if(ImGuiUiDrawer::draw_drag_int_with_buttons("Current delay (ms)", "delay_others", &delay, ImGuiIntS{0,0, 5000,1.f,100},ImGuiDragS())){
        clientDeviceS.delayS.delayMs = delay;
        DCMSignals::get()->update_delay_settings_signal(clientDeviceS.id, clientDeviceS.delayS);
    }
    ImGui::Indent();
    ImGui::Unindent();

    ImGui::Spacing();
    ImGui::Separator();


    if(clientDeviceS.connectionS.connectionType == DCDeviceConnectionType::Remote){
        ImGui::Text("[REMOTE DEVICE]");

        ImGui::Text("UDP reading:");
        ImGui::Indent();
        ImGuiUiDrawer::text(std::format("IP address: {}", clientDeviceS.connectionS.readingAddress));
        ImGuiUiDrawer::text(std::format("Port: {}", clientDeviceS.connectionS.readingPort));
        ImGui::Unindent();

        if(clientDeviceS.connectionS.protocol == net::Protocol::ipv6){
            ImGui::Text("PROTOCOL: IPV6");
        }else{
            ImGui::Text("PROTOCOL: IPV4");
        }
        ImGui::Spacing();

        ImGui::Text("UDP sending:");
        ImGui::Indent();
        ImGuiUiDrawer::text(std::format("IP address: {}", clientDeviceS.connectionS.sendingAddress));
        ImGuiUiDrawer::text(std::format("Port: {}", clientDeviceS.connectionS.sendingPort));
        ImGui::Unindent();

    }else if(clientDeviceS.connectionS.connectionType == DCDeviceConnectionType::Local){
        ImGui::Text("[LOCAL DEVICE]");
    }

    ImGui::Text("State:");
    ImGui::Indent();
    ImGui::Text(clientDeviceS.connected ? "Client connected to server" : "Client not connected");
    ImGuiUiDrawer::text(std::format("Last frame id sent: {}", clientDeviceS.lastFrameIdReceived));
    ImGui::Unindent();

    ImGui::Separator();
    // ImGui::Text(std::format("Data received (mB): {:.2f}", net::K4SMNetworkInstance::net.connections[ii]->totalReceivedBytes*0.000001, 2));
    ImGui::Separator();
    ImGui::Text("Feedbacks:");
    feedbacksLogs[clientDeviceS.id].draw(std::format("grabber_{}_feedback", clientDeviceS.id).c_str());

    ImGui::EndTabItem();
}


auto DCMLeftPanelChildDrawer::draw_device_tab_item(DCClient &client) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Device###device_tabitem")){
        return;
    }

    bool updateCurrent = false;
    ImGui::Spacing();
    ImGui::Text("Send to grabber:");
    ImGui::SameLine();
    if(ImGui::Button("Current###device_send_current")){
        updateCurrent = true;
    }        
    ImGui::SameLine();
    if(ImGui::Button("All###device_send_all")){
        for(const auto &clientDeviceS : client.settings.devicesS){
            DCMSignals::get()->update_device_settings_signal(clientDeviceS.id, clientDeviceS.deviceS);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    ImGui::Separator();

    ImGui::Text("Apply profiles:");
    ImGui::SameLine();
    if(ImGui::Button("Remote grabber###remote_grabber_profile_device")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.deviceS.apply_remote_profile();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        for(auto &clientDeviceS : client.settings.devicesS){
            DCMSignals::get()->update_device_settings_signal(clientDeviceS.id, clientDeviceS.deviceS);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Only manager###only_manager_profile_device")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.deviceS.apply_local_profile();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        for(auto &clientDeviceS : client.settings.devicesS){
            DCMSignals::get()->update_device_settings_signal(clientDeviceS.id, clientDeviceS.deviceS);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    ImGui::Separator();


    static std::string order = "";
    if(order.empty()){
        for(auto &clientDeviceS : client.settings.devicesS){
            if(clientDeviceS.id != 0){
                order += ";";
            }
            order += std::to_string(clientDeviceS.id);
        }
    }
    // = "0;1;2;3;4;5;6;7;8;9";
    if(ImGui::Button("Apply settings for camera order")){
        auto values = String::split(order, ';');
        if(values.size() == client.settings.devicesS.size()){
            std::set<int> allIds;
            std::vector<size_t> ids;
            for(const auto &value : values){
                int id = std::stoi(value);
                if(id >= 0 && !allIds.contains(id)){
                    allIds.insert(id);
                    ids.push_back(static_cast<size_t>(id));
                }else{
                    Logger::error("Invalid order cameras ids.");
                    ids.clear();
                    break;
                }
            }

            if(!ids.empty()){
                for(auto &clientDeviceS : client.settings.devicesS){
                    clientDeviceS.deviceS.update_with_device_id(clientDeviceS.id, ids[clientDeviceS.id]);
                }
                for(auto &clientDeviceS : client.settings.devicesS){
                    DCMSignals::get()->update_device_settings_signal(clientDeviceS.id, clientDeviceS.deviceS);
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }else{
            Logger::error("Invalid order cameras count.");
        }
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150.f);
    ImGui::InputText("###cameras order", &order);


    ImGui::Dummy(ImVec2{0,10});

    static ImGuiID tabId = 0;
    if(ImGuiUiDrawer::begin_tab_bar(&tabId, "Device_sub###device_sub_tabbar")){

        for(auto &clientDeviceS : client.settings.devicesS){

            auto currentDeviceType = clientDeviceS.deviceS.configS.typeDevice;
            auto ret = DCUIDrawer::draw_dc_device_settings_tab_item(
                std::format("[{}]###device_{}_tabitem",clientDeviceS.id, clientDeviceS.id),
                clientDeviceS.deviceS
            );

            if(currentDeviceType != clientDeviceS.deviceS.configS.typeDevice){
                tool::Logger::message("DEVICE CHANGED COLOR SETTINGS DEFAULTED\n");
                clientDeviceS.colorS.set_default_values(clientDeviceS.deviceS.configS.typeDevice);
                // DCMSignals::get()->color_settings_reset_signal(clientDeviceS.id, clientDeviceS.colorS);
                DCMSignals::get()->update_color_settings_signal(clientDeviceS.id, clientDeviceS.colorS);
            }
            if(std::get<1>(ret) || (updateCurrent && std::get<0>(ret))){
                DCMSignals::get()->update_device_settings_signal(clientDeviceS.id, clientDeviceS.deviceS);
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_filters_tab_item(DCClient &client) -> void {

    static constexpr std::string_view base = "###settings_filters";
    if (!ImGuiUiDrawer::begin_tab_item(std::format("Filters{}_tabitem", base).c_str())){
        return;
    }

    ImGui::Spacing();

    bool updateCurrent = false;
    bool updateAll = false;
    ImGui::Text("Send to grabber:");
    ImGui::SameLine();
    if(ImGui::Button("Current###filters_send_current")){
        updateCurrent = true;
    }
    ImGui::SameLine();
    if(ImGui::Button("All###filters_send_all")){
        updateAll = true;
    }

    int filteringMode = client.settings.useNormalFilteringSettings ? 0 : 1;
    ImGui::Text("Filtering mode:");
    ImGui::SameLine();
    if(ImGui::RadioButton("Normal###filters_normal_mode", &filteringMode, 0)){
        client.settings.useNormalFilteringSettings = true;
        updateAll = true;
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Calibration###filters_calibration_mode", &filteringMode, 1)){
        client.settings.useNormalFilteringSettings = false;
        updateAll = true;
    }
    ImGui::Spacing();

    if(updateAll){
        for(auto &clientDeviceS : client.settings.devicesS){
            if(client.settings.useNormalFilteringSettings){
                DCMSignals::get()->update_filters_settings_signal(clientDeviceS.id, clientDeviceS.filtersS);
            }else{
                DCMSignals::get()->update_calibration_filters_settings_signal(clientDeviceS.id, clientDeviceS.calibrationFiltersS);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        updateCurrent = false;
    }

    ImGui::Separator();

    static ImGuiId tabId1 = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId1, std::format("{}_sub_tabbar", base).c_str())){

        if (ImGuiUiDrawer::begin_tab_item(std::format("Normal{}_normal_tabitem", base).c_str())){

            static ImGuiId tabId2 = 0;
            if (ImGuiUiDrawer::begin_tab_bar(&tabId2, std::format("{}_normal_sub_tabbar",base).c_str())){

                for(auto &clientDeviceS : client.settings.devicesS){

                    auto ret = DCUIDrawer::draw_dc_filters_settings_tab_item(
                        std::format("[{}]{}_normal_{}_tabitem", clientDeviceS.id, base, clientDeviceS.id),
                        clientDeviceS.deviceS.configS.mode,
                        clientDeviceS.filtersS
                    );

                    if(std::get<1>(ret) || (updateCurrent && std::get<0>(ret))){
                        DCMSignals::get()->update_filters_settings_signal(clientDeviceS.id, clientDeviceS.filtersS);
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }

        if (ImGuiUiDrawer::begin_tab_item(std::format("Calibration{}_calibration_tabitem", base).c_str())){

            static ImGuiId tabId2 = 0;
            if (ImGuiUiDrawer::begin_tab_bar(&tabId2, std::format("{}_calibration_sub_tabbar",base).c_str())){

                for(auto &clientDeviceS : client.settings.devicesS){

                    auto ret = DCUIDrawer::draw_dc_filters_settings_tab_item(
                        std::format("[{}]{}_calibration_{}_tabitem", clientDeviceS.id, base, clientDeviceS.id),
                        clientDeviceS.deviceS.configS.mode,
                        clientDeviceS.calibrationFiltersS
                    );

                    if(std::get<1>(ret) || (updateCurrent && std::get<0>(ret))){
                        DCMSignals::get()->update_calibration_filters_settings_signal(clientDeviceS.id, clientDeviceS.calibrationFiltersS);
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
        if(guiCurrentFromFiltersSelection < static_cast<int>(client.settings.devicesS.size())){
            int idGrabber = guiCurrentFromFiltersSelection;
            fromFilters = &client.settings.devicesS[idGrabber].filtersS;
        }else{
            int idGrabber = guiCurrentFromFiltersSelection - static_cast<int>(client.settings.devicesS.size());
            fromFilters = &client.settings.devicesS[idGrabber].calibrationFiltersS;
        }

        if(guiCurrentTargetFiltersSelection == 0){ // all
            for(size_t ii = 0; ii < client.settings.devicesS.size(); ++ii){
                DCMSignals::get()->update_filters_settings_signal(ii, *fromFilters);
                DCMSignals::get()->update_calibration_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection == 1){ // all normal
            for(size_t ii = 0; ii < client.settings.devicesS.size(); ++ii){
                DCMSignals::get()->update_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection == 2){ // all calibration
            for(size_t ii = 0; ii < client.settings.devicesS.size(); ++ii){
                DCMSignals::get()->update_calibration_filters_settings_signal(ii, *fromFilters);
            }
        }else if(guiCurrentTargetFiltersSelection < static_cast<int>(client.settings.devicesS.size() + 3)){ // normal grabbers
            int idGrabber = guiCurrentTargetFiltersSelection-3;
            DCMSignals::get()->update_filters_settings_signal(idGrabber, *fromFilters);
        }else{ // calibration grabbers
            int idGrabber = guiCurrentTargetFiltersSelection-3 - static_cast<int>(client.settings.devicesS.size());
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

auto DCMLeftPanelChildDrawer::draw_display_tab_item(DCClient &client) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Display###display_settings_tabitem")){
        return;
    }

    ImGui::Spacing();
    ImGui::Text("All grabbers");
    ImGui::SameLine();
    if(ImGui::Button("Send###display_send_all")){
        DCMSignals::get()->update_scene_display_settings_signal(client.settings.sceneDisplayS);
        for(auto &clientDeviceS : client.settings.devicesS){
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Indent();

    ImGui::Spacing();

    ImGui::Text("Colors:");
    ImGui::Indent();

    if(ImGui::Button("Per camera###force_cloud_color_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.forceColor = true;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Real###real_cloud_color_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.forceColor = false;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    auto factorUnicolor = client.settings.devicesS.front().displayS.factorUnicolor;
    ImGui::SetNextItemWidth(100.f);
    if(ImGui::DragFloat("Color factor", &factorUnicolor, 0.01f, 0.f, 1.f)){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.factorUnicolor = factorUnicolor;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::Unindent();
    ImGui::Spacing();

    ImGui::Text("Geometry:");
    ImGui::Indent();

    if(ImGui::Button("Use backface culling###use_backfaceculling_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.backFaceCulling = true;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Disable backface culling###disable_backfaceculling_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.backFaceCulling = false;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    if(ImGui::Button("Use circles###use_circles_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.circles = true;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Use points###use_points_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.circles = false;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::SetNextItemWidth(100.f);
    {
        ImGuiDragS  pointSizeD = {100.f, true, true, true, true, false};
        ImGuiFloatS pointSizeFS = {5.f, 0.1f, 30.0f, 0.1f, 0.1f};
        auto pointSizeV = client.settings.devicesS.front().displayS.pointSize;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Point size","display_size_points_all", &pointSizeV, pointSizeFS, pointSizeD)){
            for(auto &clientDeviceS : client.settings.devicesS){
                clientDeviceS.displayS.pointSize = pointSizeV;
                DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
            }
        }
    }

    {
        ImGuiDragS  squareSizeD = {100.f, true, true, true, true, false};
        ImGuiFloatS squareSizeFS = {0.005f, 0.001f, 0.05f, 0.0001f, 0.001f};
        float squareSizeV = client.settings.devicesS.front().displayS.squareSize;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Square size","display_square_size_all", &squareSizeV, squareSizeFS, squareSizeD)){
            for(auto &clientDeviceS : client.settings.devicesS){
                clientDeviceS.displayS.squareSize = squareSizeV;
                DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
            }
        }
    }

    {
        ImGuiDragS  circleRadiusD = {100.f, true, true, true, true, false};
        ImGuiFloatS circleRadiusFS = {0.25f, 0.00f, 0.5f, 0.001f, 0.001f};
        float circleRadiusV = client.settings.devicesS.front().displayS.circleRadius;
        if(ImGuiUiDrawer::draw_drag_float_with_buttons("Circle radius","display_circle_radius_all", &circleRadiusV, circleRadiusFS, circleRadiusD)){
            for(auto &clientDeviceS : client.settings.devicesS){
                clientDeviceS.displayS.circleRadius = circleRadiusV;
                DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
            }
        }
    }

    ImGui::Unindent();
    ImGui::Spacing();

    ImGui::Text("Visibility:");
    ImGui::Indent();

    ImGui::Text("Clouds:");
    ImGui::SameLine();
    if(ImGui::Button("Show all###show_clouds_alls")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCapture = true;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Hide all###hide_clouds_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCapture = false;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Text("Cameras frustums:");
    ImGui::SameLine();
    if(ImGui::Button("Show all###show_frustums_alls")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCameraFrustum = true;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Hide all###hide_frustums_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showCameraFrustum = false;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Text("Filterings gizmos:");
    ImGui::SameLine();
    if(ImGui::Button("Show all###show_filtering_gizmos_alls")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showFilteringGizmos = true;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Hide all###hide_filtering_gizmos_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            clientDeviceS.displayS.showFilteringGizmos = false;
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::Unindent();
    ImGui::Unindent();


    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_tabbar")){
        return;
    }

    if(DCUIDrawer::draw_dc_scene_display_setings_tab_item("Scene###scene_display_tabitem", client.settings.sceneDisplayS)){
        DCMSignals::get()->update_scene_display_settings_signal(client.settings.sceneDisplayS);
    }

    for(auto &clientDeviceS : client.settings.devicesS){
        if(DCUIDrawer::draw_dc_cloud_display_setings_tab_item(std::format("[{}]###{}_cloud_display_tabitem", clientDeviceS.id, clientDeviceS.id), clientDeviceS.displayS)){
            DCMSignals::get()->update_cloud_display_settings_signal(clientDeviceS.id, clientDeviceS.displayS);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto graphics::DCMLeftPanelChildDrawer::draw_calibration_tab_item(cam::DCClient &client) -> void {

    if (!ImGuiUiDrawer::begin_tab_item("Calibration###calibration_settings_tabitem")){
        return;
    }

    ImGui::Spacing();

    static ImGuiID tabId = 0;
    if (!ImGuiUiDrawer::begin_tab_bar(&tabId, "###calibration_settings_tabbar")){
        return;
    }

    for(auto &clientDeviceS : client.settings.devicesS){
        if(DCUIDrawer::draw_dc_model_tab_item(std::format("[{}]###{}_cloud_calibration_tabitem", clientDeviceS.id, clientDeviceS.id), clientDeviceS.modelS)){
            DCMSignals::get()->update_model_settings_signal(clientDeviceS.id, clientDeviceS.modelS);
        }
    }

    ImGui::EndTabBar();
    ImGui::EndTabItem();
}

auto DCMLeftPanelChildDrawer::draw_color_tab_item(cam::DCClient &client) -> void{

    static constexpr std::string_view base = "###settings_colors";
    if (!ImGuiUiDrawer::begin_tab_item(std::format("Color###{}_tabitem", base).c_str())){
        return;
    }

    bool updateCurrent = false;
    ImGui::Spacing();
    ImGui::Text("Send to grabber:");
    ImGui::SameLine();
    if(ImGui::Button("Current###color_send_current")){
        updateCurrent = true;
    }
    ImGui::SameLine();
    if(ImGui::Button("All###color_send_all")){
        for(auto &clientDeviceS : client.settings.devicesS){
            DCMSignals::get()->update_color_settings_signal(clientDeviceS.id, clientDeviceS.colorS);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
    ImGui::Separator();


    static ImGuiID tabId = 0;
    if(!ImGuiUiDrawer::begin_tab_bar(&tabId, "###color_tabbar")){
        return;
    }

    for(auto &clientDeviceS : client.settings.devicesS){
        auto ret = DCUIDrawer::draw_dc_colors_settings_tab_item(std::format("[{}]###{}_color_settings_tabitem", clientDeviceS.id, clientDeviceS.id), clientDeviceS.deviceS.configS.typeDevice, clientDeviceS.colorS);
        if(std::get<1>(ret) || (updateCurrent && std::get<0>(ret))){
            DCMSignals::get()->update_color_settings_signal(clientDeviceS.id, clientDeviceS.colorS);
        }
    }
    ImGui::EndTabBar();

    ImGui::Spacing();
    ImGuiUiDrawer::title2("COPY");

    if(ImGui::Button(std::format("Copy{}_copy_colors", base).c_str())){
        DCColorSettings *fromColor = &client.settings.devicesS[guiCurrentFromColorsSelection].colorS;
        if(guiCurrentTargetColorsSelection == 0){ // all
            for(size_t ii = 0; ii < client.settings.devicesS.size(); ++ii){
                DCMSignals::get()->update_color_settings_signal(ii, *fromColor);
            }
        }else if(guiCurrentTargetColorsSelection < client.settings.devicesS.size() + 1){
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
            return;
        }else if(!filePath.value().empty()){
            ImGuiUiDrawer::text(std::get<1>(s), geo::Pt4f{0,1.f,0,1.f});
            return;
        }
    }
    ImGuiUiDrawer::text("No file loaded."sv, geo::Pt4f{1.f,0.0f,0.0f,1.f});
}

auto DCMLeftPanelChildDrawer::draw_infos_tab_item(cam::DCClient &client) -> void{
    if (ImGui::BeginTabItem("Infos###settings_infos_tabitem")){

        ImGui::Text("Config files loaded:");
        ImGui::Spacing();

        ImGui::Text("Global:");
        ImGui::Indent();
            ImGui::Text("Network:");
            ImGui::SameLine();
            draw_config_file_name(client.settings.filePath);
        ImGui::Unindent();

        for(const auto &clientDeviceS : client.settings.devicesS){
            ImGuiUiDrawer::text(std::format("Grabber {}:", clientDeviceS.id));
            ImGui::Indent();
            ImGui::Text("Device:");
            ImGui::SameLine();
            draw_config_file_name(clientDeviceS.deviceFilePath);
            ImGui::Text("Model:");
            ImGui::SameLine();
            draw_config_file_name(clientDeviceS.modelFilePath);
            ImGui::Text("Color:");
            ImGui::SameLine();
            draw_config_file_name(clientDeviceS.colorFilePath);
            ImGui::Text("Filters:");
            ImGui::SameLine();
            draw_config_file_name(clientDeviceS.filtersFilePath);
            ImGui::Text("Cal. filters:");
            ImGui::SameLine();
            draw_config_file_name(clientDeviceS.calibrationFiltersFilePath);
            ImGui::Unindent();
        }

        ImGui::EndTabItem();
    }
}


