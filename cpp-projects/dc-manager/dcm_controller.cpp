
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

#include "dcm_controller.hpp"

// std
#include <iostream>

// base
#include "utility/logger.hpp"

// local
#include "dcm_signals.hpp"

using namespace tool;
using namespace cam;
using namespace graphics;

DCMController::~DCMController(){
    model->clean();
    model = nullptr;
    view  = nullptr;
}

auto DCMController::initialize() -> bool{

    auto lg = LogGuard("DCMController::initialize"sv);

    // create view
    view = std::make_unique<DCMView>();

    // create model
    model = std::make_unique<DCMModel>();

    // connections
    set_connections();

    // initialize model
    model->initialize();

    return true;
}

auto DCMController::set_connections() -> void{

    auto lg = LogGuard("DCMController::set_connections"sv);

    // pointers
    auto s              = DCMSignals::get();
    // # drawers
    auto middleD        = &view->mainW.m_middlePanelD;
    auto leftD          = &view->mainW.m_leftPanelD;
    auto directD        = &middleD->directD;
    auto recorderD      = &middleD->recorderD;
    auto playerD        = &middleD->playerD;
    auto calibratorD    = &middleD->calibratorD;
    // # model
    auto client         = &model->client;
    auto recorder       = &model->recorder;
    auto player         = &model->player;
    auto calibrator     = &model->calibrator;

    // from logger
    Logger::get()->message_signal.connect([&](std::string message){
        view->mainW.append_global_log(message);
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        view->mainW.append_global_log(warning);
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        view->mainW.append_global_log(error);
        std::cerr << error;
    });

    // ui
    // ## controller
    s->initialize_signal.connect([&](size_t nbDevices){
        view->initialize(model->client.devices_nb());
        model->trigger_settings();
    });
    view->gl()->update_signal.connect(                      &DCMController::update,                             this);
    view->gl()->draw_imgui_signal.connect(                  &DCMController::draw_ui,                            this);
    // ## calibrator
    s->update_calibration_display_signal.connect(           &DCCalibrator::trigger_data_updated,                calibrator);
    // ## calibrator drawer
    calibrator->data_updated_signal.connect(                &DCCalibratorDrawer::set_data,                      calibratorD);
    // ## left panel drawer
    client->feedback_received_signal.connect(               &DCMMainWindowDrawer::append_feedback_log,          &view->mainW);
    // ## player drawer
    player->initialize_signal.connect(                      &DCPlayerDrawer::initialize,                        playerD);

    // # actions
    s->add_device_signal.connect(                           &DCMModel::add_device,                              model.get());
    s->remove_last_device_signal.connect(                   &DCMModel::remove_last_device,                      model.get());
    s->remove_all_devices_signal.connect(                   &DCMModel::remove_all_devices,                      model.get());
    // ## model    
    s->process_settings_action_signal.connect(              &DCMModel::process_settings_action,                 model.get());
    s->ask_calibration_signal.connect(                      &DCMModel::ask_calibration,                         model.get());
    // ## client
    s->reset_remote_device_signal.connect(                  &DCClient::reset_remote_device,                     client);
    s->init_connection_signal.connect(                      &DCClient::init_connection_with_remote_device,      client);
    s->command_signal.connect(                              &DCClient::apply_command,                           client);
    s->reset_network_signal.connect(                        &DCClient::reset_network,                           client);
    // ## calibration
    s->start_calibration_registering_signal.connect(        &DCCalibrator::start_registering,                   calibrator);
    s->stop_calibration_registering_signal.connect(         &DCCalibrator::stop_registering,                    calibrator);
    s->reset_calibration_registering_signal.connect(        &DCCalibrator::reset_registering,                   calibrator);
    s->recompute_registering_processing_signal.connect(     &DCCalibrator::process_all_frames,                  calibrator);
    s->validate_calibration_signal.connect(                 &DCCalibrator::validate_calibration,                calibrator);
    s->calibrate_signal.connect(                            &DCCalibrator::calibrate,                           calibrator);
    // ## player
    s->start_player_signal.connect(                         &DCVideoPlayer::start_video,                        player);
    s->pause_player_signal.connect(                         &DCVideoPlayer::pause_video,                        player);
    s->restart_player_signal.connect(                       &DCVideoPlayer::go_to_start_time,                   player);
    s->set_player_time_signal.connect(                      &DCVideoPlayer::set_current_time,                   player);
    s->remove_until_current_frame_player_signal.connect(    &DCVideoPlayer::remove_until_current_frame,         player);
    s->remove_after_current_frame_player_signal.connect(    &DCVideoPlayer::remove_after_current_frame,         player);
    s->save_video_player_signal.connect(                    &DCVideoPlayer::save_to_file,                       player);
    s->load_video_player_signal.connect(                    &DCVideoPlayer::load_from_file,                     player);
    s->save_cloud_player_signal.connect(                    &DCVideoPlayer::save_cloud_to_file,                 player);
    s->merge_player_signal.connect(                         &DCVideoPlayer::merge,                              player);
    s->info_player_signal.connect(                          &DCVideoPlayer::display_infos,                      player);
    // ## recorder
    s->start_recorder_signal.connect(                       &DCVideoRecorder::start_recording,                  recorder);
    s->stop_recorder_signal.connect(                        &DCVideoRecorder::stop_recording,                   recorder);
    s->reset_recorder_signal.connect(                       &DCVideoRecorder::reset_recording,                  recorder);
    s->set_recorder_time_signal.connect(                    &DCVideoRecorder::set_time,                         recorder);
    s->save_recorder_signal.connect(                        &DCVideoRecorder::save_to_file,                     recorder);
    // ## direct drawer
    s->save_current_cloud_signal.connect(                   &DCDirectDrawer::save_current_cloud,                directD);

    // # settings
    // ## client
    s->update_model_settings_signal.connect(                &DCClient::update_model_settings,                   client);
    calibrator->validated_calibration_signal.connect(       &DCClient::update_model_settings,                   client);
    s->update_device_settings_signal.connect(               &DCClient::update_device_settings,                  client);
    s->update_filters_settings_signal.connect(              &DCClient::update_filters_settings,                 client);
    s->update_calibration_filters_settings_signal.connect(  &DCClient::update_calibration_filters_settings,     client);
    s->update_color_settings_signal.connect(                &DCClient::update_color_settings,                   client);
    s->update_delay_settings_signal.connect(                &DCClient::update_delay_settings,                   client);

    // ## calibration
    s->update_calibration_settings_signal.connect(          &DCCalibrator::update_settings,                     calibrator);
    // ## player
    s->update_player_settings_signal.connect(               &DCVideoPlayer::update_settings,                    player);
    // ## recorder
    s->update_model_settings_signal.connect(                &DCVideoRecorder::update_model,                     recorder);
    calibrator->validated_calibration_signal.connect(       &DCVideoRecorder::update_model,                     recorder);
    s->update_recorder_settings_signal.connect(             &DCVideoRecorder::update_settings,                  recorder);
    // ## direct drawer
    s->update_model_settings_signal.connect(                &DCDirectDrawer::update_model_settings,             directD);
    s->update_model_settings_ui_only_signal.connect(        &DCDirectDrawer::update_model_settings,             directD);
    calibrator->validated_calibration_signal.connect(       &DCDirectDrawer::update_model_settings,             directD);
    s->update_filters_settings_signal.connect(              &DCDirectDrawer::update_filters_settings,           directD);
    s->update_filters_settings_ui_only_signal.connect(      &DCDirectDrawer::update_filters_settings,           directD);
    s->update_scene_display_settings_signal.connect(        &DCDirectDrawer::update_scene_display_settings,     directD);
    s->update_cloud_display_settings_signal.connect(        &DCDirectDrawer::update_device_display_settings,    directD);
    // ## calibration drawer
    s->update_model_settings_signal.connect(                &DCCalibratorDrawer::update_client_model,          calibratorD);
    s->update_model_settings_ui_only_signal.connect(        &DCCalibratorDrawer::update_client_model,          calibratorD);

    calibrator->new_calibration_signal.connect(             &DCCalibratorDrawer::update_client_model,          calibratorD);
    calibrator->validated_calibration_signal.connect(       &DCCalibratorDrawer::update_client_model,          calibratorD);
    s->update_scene_display_settings_signal.connect(        &DCCalibratorDrawer::update_scene_display_settings, calibratorD);
    s->update_cloud_display_settings_signal.connect(        &DCCalibratorDrawer::update_client_cloud_display,  calibratorD);
    s->update_calibration_drawer_settings_signal.connect(   &DCCalibratorDrawer::update_settings,               calibratorD);
    // # player drawer
    s->update_scene_display_settings_signal.connect(        &DCPlayerDrawer::update_scene_display_settings,     playerD);
    s->update_cloud_display_settings_signal.connect(        &DCPlayerDrawer::update_device_display_settings,    playerD);
    // ## recorder drawer
    s->update_model_settings_signal.connect(                &DCRecorderDrawer::update_model_settings,           recorderD);
    s->update_model_settings_ui_only_signal.connect(        &DCRecorderDrawer::update_model_settings,           recorderD);
    calibrator->validated_calibration_signal.connect(       &DCRecorderDrawer::update_model_settings,           recorderD);
    s->update_scene_display_settings_signal.connect(        &DCRecorderDrawer::update_scene_display_settings,   recorderD);
    s->update_cloud_display_settings_signal.connect(        &DCRecorderDrawer::update_device_display_settings,  recorderD);

    // # frames
    // ## player
    player->new_frame_signal.connect(                       &DCPlayerDrawer::set_frame,                         playerD);
    // ## recorder
    client->new_data_frame_signal.connect(                  &DCVideoRecorder::add_data_frame,             recorder);
    client->new_frame_signal.connect(                       &DCVideoRecorder::add_frame,                        recorder);
    // ## calibrator
    client->new_frame_signal.connect(                       &DCCalibrator::add_frame,                           calibrator);
    // ## direct drawer
    client->new_frame_signal.connect(                       &DCDirectDrawer::update_frame,                         directD);
    // ## recorder drawer
    recorder->new_frame_signal.connect(                     &DCRecorderDrawer::set_frame,                       recorderD);
}

auto DCMController::start() -> void{
    auto lg = LogGuard("DCMController::start"sv);
    view->start();
}

auto DCMController::update() -> void{
    model->update();
    view->update();
}

auto DCMController::draw_ui(geo::Pt2f size) -> void{
    view->draw(size, model.get());
}
