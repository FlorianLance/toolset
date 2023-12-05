
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

#include "k4sm_controller.hpp"

// base
#include "utility/benchmark.hpp"
#include "utility/logger.hpp"
#include "utility/paths.hpp"

// local
#include "data/k4sm_paths.hpp"
#include "k4sm_signals.hpp"

using namespace tool;

K4SMController::~K4SMController(){
    model.clean();
}

auto K4SMController::initialize() -> bool{

    // init main window
    // # screen
    graphics::Screen screen(1920, 1080, 0,0);
    // # gl context
    sf::ContextSettings context;
    context.depthBits         = 24;
    context.stencilBits       = 8;
    context.antialiasingLevel = 4;
    context.majorVersion      = 4;
    context.minorVersion      = 6;
    context.attributeFlags    = sf::ContextSettings::Attribute::Default;
    // # window
    const std::string numVersion = "1.6";
    m_glW = std::make_unique<graphics::K4SMGlWindow>(std::format("K4 scaner manager v{}", numVersion), screen,context);
    m_glW->init();

    // init paths
    K4SMPaths::initialize(tool::Paths::applicationDir, K4SMSettings::host_name());

    // init logger
    Logger::init(Paths::logsDir, K4SMPaths::logName);

    // setup benchmark
    Bench::disable_display();

    // connections
    set_connections();

    // init model
    if(!model.initialize()){
        Logger::error("Invalid initialization.\n");
        return false;
    }

    // setup ui with the current number of grabbers
    m_mainD.initialize(model.network.connections_nb());

    // update ui from settings
    model.trigger_settings();

    return true;
}

auto K4SMController::set_connections() -> void{

    // aliases
    // # drawers
    using LeftD   = graphics::K4SMLeftPanelChildDrawer;
    using DirectD = graphics::K4DirectDrawer;
    using RecD    = graphics::K4RecorderDrawer;
    using PlayerD = graphics::K4PlayerDrawer;
    using CalibD  = graphics::K4CalibratorDrawer;
    // # model
    using Rec     = camera::DCRecorder;
    using Player  = camera::DCPlayer;
    using Calib   = camera::DCCalibrator;
    using ServerD = camera::DCServerData;
    using ServerC = network::DCServerConnection;
    using ServerN = network::DCServerNetwork;
    using Settings= K4SMSettings;

    // pointers
    auto s              = K4SMSignals::get();
    // # drawers
    auto middleD        = &m_mainD.m_middlePanelD;
    auto leftD          = &m_mainD.m_leftPanelD;    

    auto directD        = &middleD->directD;
    auto recorderD      = &middleD->recorderD;
    auto playerD        = &middleD->playerD;
    auto calibrationD   = &middleD->calibratorD;
    // # model
    auto network        = &model.network;
    auto serverData     = &model.sData;
    auto settings       = &model.settings;
    auto recorder       = &model.recorder;
    auto player         = &model.player;
    auto calibration    = &model.calibration;

    // from logger
    Logger::get()->message_signal.connect([&](std::string message){
        m_mainD.m_leftPanelD.append_global_log(message);
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        m_mainD.m_leftPanelD.append_global_log(warning);
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        m_mainD.m_leftPanelD.append_global_log(error);
        std::cerr << error;
    });

    // to model
    ServerC::synchro_signal.connect(                        &K4SMModel::update_synchro,             &model);
    ServerC::feedback_signal.connect(                       &K4SMModel::add_feedback,               &model);
    s->ask_calibration_signal.connect(                      &K4SMModel::ask_calibration,            &model);
    s->update_filters_settings_signal.connect(              &K4SMModel::update_filters,             &model);
    s->update_calibration_filters_settings_signal.connect(  &K4SMModel::update_calibration_filters, &model);
    s->update_filtering_mode_signal.connect(                &K4SMModel::update_filtering_mode,      &model);
//    s->use_normal_filtering_for_all_signal.connect(     &K4SMModel::use_normal_filtering_for_all,      &model);
//    s->use_calibration_filtering_for_all_signal.connect(&K4SMModel::use_calibration_filtering_for_all, &model);

    // # network
    // ## init
    s->init_connection_signal.connect(                  &ServerN::init_connection,           network);
    // ## send data
    s->command_signal.connect(                          &ServerN::send_command,              network);
    s->update_device_settings_signal.connect(           &ServerN::send_device_settings,      network);
//    s->update_filters_settings_signal.connect(          &ServerN::send_filters,              network);
    s->update_color_settings_signal.connect(            &ServerN::send_color_settings,       network);
    s->update_delay_signal.connect(                     &ServerN::send_delay,                network);

    // # server data
    // ## update settings
    s->process_settings_action_signal.connect(          &Settings::process_settings_action,  settings);
    s->mouse_pressed_depth_direct_signal.connect(       &Settings::update_filters_depth_mask,settings);
    // ## update model
    s->update_model_settings_signal.connect(            &Settings::update_model,             settings);
    calibration->validated_calibration_signal.connect(  &Settings::update_model,             settings);
    // ## update states
    recorder->states_updated_signal.connect(            &Settings::update_recorder_states,   settings);
    player->states_updated_signal.connect(              &Settings::update_player_states,     settings);
    calibration->states_updated_signal.connect(         &Settings::update_calibrator_states, settings);    
    // ## update data
    ServerC::compressed_frame_signal.connect(           &ServerD::new_compressed_frame,      serverData);

//    s->calibrate_signal.connect(                        [&](){
//        std::vector<camera::K4Model> models;
//        for(const auto &grabberS : model.settings.grabbersSet){
//            models.push_back(grabberS.model);
//        }
//        std::cout << "CAL " << models.size() << "\n";
//        calibration->calibrate(models);
//    });

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
    m_glW->update_signal.connect(&K4SMController::update, this);
    m_glW->draw_imgui_signal.connect(&K4SMController::draw_ui, this);
}

auto K4SMController::start() -> void{
    m_glW->start();
    Logger::message("Exit K4 scaner manager.\n");
}

auto K4SMController::update() -> void{

    // aliases
    // # models
    auto player         = &model.player;
//    auto recorder       = &model.recorder;
//    auto calibration    = &model.calibration;
    // # drawers
    auto middleD        = &m_mainD.m_middlePanelD;    
    auto directD        = &middleD->directD;
    auto recorderD      = &middleD->recorderD;
    auto playerD        = &middleD->playerD;
    auto calibrationD   = &middleD->calibratorD;


    model.update();


//            recorder->add_compressed_frame(ii, lastCompressedFrame);
//            recorderD->update(&model.recorder);

//            // # to calibration
////            calibration->add_frame(ii, lastFrame);

//            // invalidate it
//            model.sData.invalid_last_frame(ii);
//        }
//    }

    // update

//    calibrationD->update(*calibration);
}

auto K4SMController::draw_ui(geo::Pt2f size) -> void{
    m_mainD.draw(size, model.settings);
}
