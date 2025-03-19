
/*******************************************************************************
** Toolset-dc-player                                                          **
** MIT License                                                                **
** Copyright (c) [2025] [Florian Lance]                                       **
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


#include "dcp_controller.hpp"

// std
#include <iostream>

// base
#include "utility/logger.hpp"

// local
#include "dcp_signals.hpp"

using namespace tool;
using namespace cam;
using namespace graphics;

DCPController::~DCPController(){
    model->clean();
    model = nullptr;
    view  = nullptr;
}

auto DCPController::initialize(size_t id) -> bool{
    
    auto lg = LogG("DCPController::initialize"sv);

    // create view
    view = std::make_unique<DCPView>(id);

    // create model
    model = std::make_unique<DCPModel>();

    // connections
    set_connections();

    // initialize model
    model->initialize();

    return true;
}

auto DCPController::set_connections() -> void{
    
    auto lg = LogG("DCPController::set_connections"sv);

    // pointers
    auto s              = DCPSignals::get();
    // # drawers
    auto middleD        = &view->mainW.m_middlePanelD;
    auto leftD          = &view->mainW.m_leftPanelD;
    auto playerD        = &middleD->playerD;
    // # model
    auto player         = &model->player;

    // from logger
    auto logger = Logger::get_instance();
    logger->message_signal.connect([&](std::string message){
        view->mainW.append_global_log(message);
        // std::cout << message;
    });
    logger->warning_signal.connect([&](std::string warning){
        view->mainW.append_global_log(warning);
        // std::cerr << warning;
    });
    logger->error_signal.connect([&](std::string error){
        view->mainW.append_global_log(error);
        // std::cerr << error;
    });

    // ui
    // ## controller
    s->initialize_signal.connect([&](size_t nbDevices){
        view->initialize(nbDevices);
        model->trigger_settings();
    });
    view->gl()->update_signal.connect(                      &DCPController::update,                             this);
    view->gl()->draw_imgui_signal.connect(                  &DCPController::draw_ui,                            this);

    // ## player drawer
    player->initialize_signal.connect(                      &DCPlayerDrawer::initialize,                        playerD);

    // # actions   
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

    // ## player
    s->update_player_settings_signal.connect(               &DCVideoPlayer::update_settings,                    player);
    s->update_scene_display_settings_signal.connect(        &DCPlayerDrawer::update_scene_display_settings,     playerD);
    s->update_cloud_display_settings_signal.connect(        &DCPlayerDrawer::update_device_display_settings,    playerD);

    // # frames
    // ## player
    player->new_frame_signal.connect(                       &DCPlayerDrawer::set_frame,                         playerD);
}

auto DCPController::start() -> void{
    auto lg = LogG("DCPController::start"sv);
    view->start();
}

// #include "utility/time.hpp"
auto DCPController::update() -> void{
    // auto t1 = Time::nanoseconds_since_epoch();
    model->update();
    view->update();
    // Log::fmessage("update {}\n", Time::now_difference_micro_s(t1));
}

auto DCPController::draw_ui(geo::Pt2f size) -> void{
    // auto t1 = Time::nanoseconds_since_epoch();
    view->draw(size, model.get());
    // Log::fmessage("draw_ui {}\n", Time::now_difference_micro_s(t1));
}
