
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

// base
#include "utility/benchmark.hpp"
#include "utility/logger.hpp"
#include "utility/paths.hpp"

// local
#include "data/dcm_paths.hpp"
#include "dcm_signals.hpp"

using namespace tool;

DCMController::~DCMController(){
    model->clean();
}

auto DCMController::initialize() -> bool{

    // init paths
    DCMPaths::initialize(tool::Paths::applicationDir, DCMSettings::host_name());

    // init logger
    Logger::init(Paths::logsDir, DCMPaths::logName);

    // setup benchmark
    Bench::disable_display();

    view  = std::make_unique<DCMView>();
    model = std::make_unique<DCMModel>();

    // connections
    set_connections();

    // initialize
    // # model
    if(!model->initialize()){
        Logger::error("Invalid initialization.\n");
        return false;
    }
    // # view
    view->initialize(model->sNetwork.devices_nb());

    model->trigger_settings();

    return true;
}

auto DCMController::set_connections() -> void{

    // aliases
    // # drawers
    using LeftD         = graphics::DCMLeftPanelChildDrawer;
    using DirectD       = graphics::DCDirectDrawer;
    using RecD          = graphics::DCRecorderDrawer;
    using PlayerD       = graphics::DCPlayerDrawer;
    using CalibD        = graphics::DCCalibratorDrawer;
    // # model
    using Rec           = camera::DCRecorder;
    using Player        = camera::DCPlayer;
    using Calib         = camera::DCCalibrator;
    using ServerData    = camera::DCServerData;
    using ServerNet     = network::DCServerNetwork;
    using Settings      = DCMSettings;

    // pointers
    auto s              = DCMSignals::get();
    // # drawers
    auto middleD        = &view->mainW.m_middlePanelD;
    auto leftD          = &view->mainW.m_leftPanelD;

    auto directD        = &middleD->directD;
    auto recorderD      = &middleD->recorderD;
    auto playerD        = &middleD->playerD;
    auto calibrationD   = &middleD->calibratorD;
    // # model
    auto serverNet      = &model->sNetwork;
    auto serverData     = &model->sData;
    auto settings       = &model->settings;
    auto recorder       = &model->recorder;
    auto player         = &model->player;
    auto calibration    = &model->calibration;

    // from logger
    Logger::get()->message_signal.connect([&](std::string message){
        view->mainW.m_leftPanelD.append_global_log(message);
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        view->mainW.m_leftPanelD.append_global_log(warning);
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        view->mainW.m_leftPanelD.append_global_log(error);
        std::cerr << error;
    });

    // to model

    serverNet->remote_synchro_signal.connect(               &DCMModel::update_synchro,             model.get());
    serverNet->remote_feedback_signal.connect(              &DCMModel::add_feedback,               model.get());
    s->ask_calibration_signal.connect(                      &DCMModel::ask_calibration,            model.get());
    s->update_filters_settings_signal.connect(              &DCMModel::update_filters,             model.get());
    s->update_calibration_filters_settings_signal.connect(  &DCMModel::update_calibration_filters, model.get());
    s->update_filtering_mode_signal.connect(                &DCMModel::update_filtering_mode,      model.get());
//    s->use_normal_filtering_for_all_signal.connect(     &K4SMModel::use_normal_filtering_for_all,      &model);
//    s->use_calibration_filtering_for_all_signal.connect(&K4SMModel::use_calibration_filtering_for_all, &model);

    // # network
    // ## init
    s->init_connection_signal.connect(                  &ServerNet::init_connection,           serverNet);
    // ## send data
    s->command_signal.connect(                          &ServerNet::apply_command,              serverNet);
    s->update_device_settings_signal.connect(           &ServerNet::update_device_settings,      serverNet);
//    s->update_filters_settings_signal.connect(          &ServerN::send_filters,              network);
    s->update_color_settings_signal.connect(            &ServerNet::update_color_settings,       serverNet);
    s->update_delay_signal.connect(                     &ServerNet::update_delay_settings,                serverNet);

    // # server data
    // ## update settings
    s->process_settings_action_signal.connect(          &Settings::process_settings_action,  settings);
    // s->mouse_pressed_depth_direct_signal.connect(       &Settings::update_filters_depth_mask,settings);
    // ## update model
    s->update_model_settings_signal.connect(            &Settings::update_model,             settings);
    calibration->validated_calibration_signal.connect(  &Settings::update_model,             settings);
    // ## update states
    recorder->states_updated_signal.connect(            &Settings::update_recorder_states,   settings);
    player->states_updated_signal.connect(              &Settings::update_player_states,     settings);
    calibration->states_updated_signal.connect(         &Settings::update_calibrator_states, settings);    
    // ## update data    
    serverNet->remote_frame_signal.connect(             &ServerData::new_compressed_frame,      serverData);
    serverNet->local_frame_signal.connect(              &ServerData::new_frame,                 serverData);

//    s->calibrate_signal.connect(                        [&](){
//        std::vector<camera::K4Model> models;
//        for(const auto &grabberS : model.settings.grabbersSet){
//            models.push_back(grabberS.model);
//        }
//        std::cout << "CAL " << models.size() << "\n";
//        calibration->calibrate(models);
//    });

    s->color_settings_reset_signal.connect(             &Settings::update_color_settings_from_device, settings);

    // # recorder
    // ## update settings
    s->update_recorder_settings_signal.connect(         &Rec::update_settings,               recorder);
    // ## update model
    s->update_model_settings_signal.connect(            &Rec::update_model,                  recorder);
    calibration->validated_calibration_signal.connect(  &Rec::update_model,                  recorder);
    // ## update data
    s->new_compressed_frame_signal.connect(             &Rec::add_compressed_frame,          recorder);
    // ## do actions
    s->start_recorder_signal.connect(                   &Rec::start_recording,               recorder);
    s->stop_recorder_signal.connect(                    &Rec::stop_recording,                recorder);
    s->reset_recorder_signal.connect(                   &Rec::reset_recording,               recorder);
    s->set_recorder_time_signal.connect(                &Rec::set_time,                      recorder);
    s->save_recorder_signal.connect(                    &Rec::save_to_file,                  recorder);

    // # player
    // ## update settings
    s->update_player_settings_signal.connect(           &Player::update_settings,            player);
    // ## do actions
    s->start_player_signal.connect(                     &Player::start_playing,              player);
    s->pause_player_signal.connect(                     &Player::stop_playing,               player);
    s->restart_player_signal.connect(                   &Player::restart,                    player);
    s->set_player_time_signal.connect(                  &Player::set_current_time,           player);
    s->remove_until_current_frame_player_signal.connect(&Player::remove_until_current_frame, player);
    s->remove_after_current_frame_player_signal.connect(&Player::remove_after_current_frame, player);
    s->save_video_player_signal.connect(                &Player::save_to_file,               player);
    s->load_video_player_signal.connect(                &Player::load_from_file,             player);
    s->save_cloud_player_signal.connect(                &Player::save_cloud_to_file,         player);
    s->merge_player_signal.connect(                     &Player::merge,                      player);
    s->info_player_signal.connect(                      &Player::display_infos,              player);

    // # calibration
    // ## update settings
    s->update_calibration_settings_signal.connect(      &Calib::update_settings,             calibration);
    // ## update data
    s->new_frame_signal.connect(                        &Calib::add_frame,                   calibration);
    // ## do actions
    s->start_calibration_registering_signal.connect(    &Calib::start_registering,           calibration);
    s->stop_calibration_registering_signal.connect(     &Calib::stop_registering,            calibration);
    s->reset_calibration_registering_signal.connect(    &Calib::reset_registering,           calibration);
    s->recompute_registering_processing_signal.connect( &Calib::process_all_frames,          calibration);
    s->update_calibration_display_signal.connect(       &Calib::send_data_updated_signal,    calibration);
    s->validate_calibration_signal.connect(             &Calib::validate_calibration,        calibration);
    s->calibrate_signal.connect(                        &Calib::calibrate,                   calibration);

    // to drawers
    // # left panel
    s->feedback_received_signal.connect(                 &LeftD::append_feedback_log,                leftD);
    // # middle panel
    // ## direct
    s->update_scene_display_settings_signal.connect(     &DirectD::update_scene_display_settings,    directD);
    s->update_cloud_display_settings_signal.connect(     &DirectD::update_cloud_display_settings,    directD);
    s->update_model_settings_signal.connect(             &DirectD::update_model,                     directD);
    calibration->validated_calibration_signal.connect(   &DirectD::update_model,                     directD);
    s->new_frame_signal.connect(                         &DirectD::set_frame,                        directD);
    // ## recorder
    s->update_scene_display_settings_signal.connect(     &RecD::update_scene_display_settings,       recorderD);
    s->update_cloud_display_settings_signal.connect(     &RecD::update_cloud_display_settings,       recorderD);
    s->update_model_settings_signal.connect(             &RecD::update_model,                        recorderD);
    calibration->validated_calibration_signal.connect(   &RecD::update_model,                        recorderD);
    recorder->new_frame_signal.connect(                  &RecD::set_frame,                           recorderD);
    // ## player
    s->update_scene_display_settings_signal.connect(     &PlayerD::update_scene_display_settings,    playerD);
    s->update_cloud_display_settings_signal.connect(     &PlayerD::update_cloud_display_settings,    playerD);
    player->initialize_signal.connect(                   &PlayerD::initialize,                       playerD);
    player->new_frame_signal.connect(                    &PlayerD::set_frame,                        playerD);

    // ## calibration
    s->update_scene_display_settings_signal.connect(     &CalibD::update_scene_display_settings,     calibrationD);
    s->update_cloud_display_settings_signal.connect(     &CalibD::update_grabber_cloud_display,      calibrationD);
    s->update_model_settings_signal.connect(             &CalibD::update_grabber_model,              calibrationD);
    s->update_calibration_drawer_settings_signal.connect(&CalibD::update_settings,                   calibrationD);
    calibration->data_updated_signal.connect(            &CalibD::set_data,                          calibrationD);
    calibration->new_calibration_signal.connect(         &CalibD::update_grabber_model,              calibrationD);
    calibration->validated_calibration_signal.connect(   &CalibD::update_grabber_model,              calibrationD);

    // to controller
    view->gl()->update_signal.connect(&DCMController::update, this);
    view->gl()->draw_imgui_signal.connect(&DCMController::draw_ui, this);
}

auto DCMController::start() -> void{
    view->start();
}

auto DCMController::update() -> void{
    model->update();
}

auto DCMController::draw_ui(geo::Pt2f size) -> void{
    view->draw(size, model.get());
}
