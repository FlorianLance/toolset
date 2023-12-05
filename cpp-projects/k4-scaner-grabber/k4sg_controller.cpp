
/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

#include "k4sg_controller.hpp"

// base
#include "utility/benchmark.hpp"
#include "utility/logger.hpp"
#include "utility/paths.hpp"

// local
#include "data/k4sg_paths.hpp"
#include "k4sg_signals.hpp"

using namespace tool;


K4SGController::~K4SGController(){
    model.clean();
}

auto K4SGController::initialize() -> bool{

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
    m_glW = std::make_unique<graphics::K4SGGlWindow>(std::format("K4 scaner grabber v{}", numVersion), screen,context);
    m_glW->init();

    // init paths
    K4SGPaths::initialize(K4SGSettings::idLocalGrabber, tool::Paths::applicationDir, K4SGSettings::host_name());

    // init logger
    Logger::init(Paths::logsDir, K4SGPaths::logName);

    // setup benchmark
    Bench::disable_display();

    // connections
    set_connections();

    // init model
    if(!model.initialize()){
        Logger::error("Invalid initialization.\n");
        return false;
    }

    // setup ui
    m_mainD.initialize();

    // update ui from settings
    model.trigger_settings();

    return true;
}

auto K4SGController::set_connections() -> void{

    // aliases
    using Sett  = K4SGSettings;
    using CCo   = network::DCClientConnection;
    using Rec   = camera::DCRecorder;
    using RecD  = graphics::K4RecorderDrawer;
    using DevD  = graphics::K4DeviceDrawer;
    using DevM  = camera::DCDeviceManager;

    // pointers
    auto s    = K4SGSignals::get();
    auto sett = &model.settings;
    auto dev  = &model.device;
    auto rec  = &model.recorder;
    auto con  = &model.connection;
    auto devD = &m_mainD.deviceD;
    auto recD = &m_mainD.recorderD;

    // from logger
    Logger::get()->message_signal.connect([&](std::string message){
        m_mainD.leftPanelD.append_log(message);
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        m_mainD.leftPanelD.append_log(warning);
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        m_mainD.leftPanelD.append_log(error);
        std::cerr << error;
    });

    // to app
    CCo::shutdown_signal.connect(   [&](){system("c:\\windows\\system32\\shutdown /s");});
    CCo::quit_signal.connect(       [&](){m_glW->quit();});
    CCo::restart_signal.connect(    [&](){system("c:\\windows\\system32\\shutdown /R");});

    // to model
    // # network
    s->init_network_sending_settings_signal.connect(                 &CCo::init_sender,                                     con);
    s->ping_server_signal.connect(                                   &CCo::ping_server,                                     con);
    s->debug_device_send_signal.connect(                             &CCo::dummy_device_trigger,                            con);
    dev->new_compressed_frame_signal.connect(                        &CCo::send_frame,                                      con);
    // # settings
    CCo::receive_init_network_sending_settings_signal.connect(       &Sett::init_network_sending_settings,                  sett);
    CCo::receive_filters_signal.connect(                             &Sett::update_filters,                                 sett);
    CCo::receive_device_settings_signal.connect(                     &Sett::update_device_settings,                         sett);
    CCo::receive_color_settings_signal.connect(                      &Sett::update_color_settings,                          sett);
    CCo::receive_delay_signal.connect(                               &Sett::update_delay,                                   sett);
    CCo::disconnect_signal.connect(                                  &Sett::disconnect,                                     sett);
    dev->new_imu_sample_signal.connect(                              &Sett::update_imu_sample,                              sett);
    DevM::update_device_name_signal.connect(                         &Sett::update_device_name,                             sett);
    s->reset_device_settings_signal.connect(                         &Sett::reset_device_settings,                          sett);
    s->save_device_settings_to_default_file_signal.connect(          &Sett::save_device_settings_to_default_file,           sett);
    s->save_device_settings_to_current_hostname_file_signal.connect( &Sett::save_device_settings_to_current_hostname_file,  sett);
    s->load_default_device_settings_file_signal.connect(             &Sett::load_default_device_settings_file,              sett);
    s->load_current_hostname_device_settings_file_signal.connect(    &Sett::load_current_hostname_device_settings_file,     sett);
    s->mouse_pressed_depth_direct_signal.connect(                    &Sett::update_filters_depth_mask,                      sett);
    s->reset_filters_signal.connect(                                 &Sett::reset_filters,                                  sett);
    s->save_filters_to_default_file_signal.connect(                  &Sett::save_filters_to_default_file,                   sett);
    s->save_filters_to_current_hostname_file_signal.connect(         &Sett::save_filters_to_current_hostname_file,          sett);
    s->load_default_filters_file_signal.connect(                     &Sett::load_default_filters_file,                      sett);
    s->load_current_hostname_filters_file_signal.connect(            &Sett::load_current_hostname_filters_file,             sett);
    s->reset_color_settings_signal.connect(                          &Sett::reset_color_settings,                           sett);
    s->save_color_settings_to_default_file_signal.connect(           &Sett::save_color_settings_to_default_file,            sett);
    s->save_color_settings_to_current_hostname_file_signal.connect(  &Sett::save_color_settings_to_current_hostname_file,   sett);
    s->load_default_color_settings_file_signal.connect(              &Sett::load_default_settings_color_file,               sett);
    s->load_current_hostname_color_settings_file_signal.connect(     &Sett::load_current_hostname_color_settings_file,      sett);
    // # device
    CCo::update_device_list_signal.connect(                          &DevM::update_device_list,                             dev);
    s->update_device_settings_signal.connect(                        &DevM::update_settings,                                dev);
    s->update_filters_signal.connect(                                &DevM::update_filters,                                 dev);
    s->update_color_settings_signal.connect(                         &DevM::update_color_settings,                          dev);
    s->update_delay_signal.connect(                                  &DevM::update_delay,                                   dev);
    // # recorder
    s->save_recording_to_file_signal.connect(                        &Rec::save_to_file,                                    rec);
    s->update_recorder_settings_signal.connect(                      &Rec::update_settings,                                 rec);
    dev->new_compressed_frame_signal.connect(                        &Rec::add_compressed_frame_to_default_camera,          rec);

    // to drawers
    // # device
    dev->new_frame_signal.connect(                                   &DevD::update_frame,                                   devD);
    s->save_cloud_to_file_signal.connect(                            &DevD::save_cloud,                                     devD);
    s->update_scene_display_settings_signal.connect(                 &DevD::update_scene_display_settings,                  devD);
    s->update_cloud_display_settings_signal.connect(                 &DevD::update_cloud_display_settings,                  devD);
    s->update_model_signal.connect(                                  &graphics::K4CloudsSceneDrawer::update_model,          devD);

    s->update_filters_signal.connect(                                &DevD::update_filters,                                 devD);
    // # recorder
    s->update_scene_display_settings_signal.connect(                 &RecD::update_scene_display_settings,                  recD);
    s->update_cloud_display_settings_signal.connect(                 &RecD::update_cloud_display_settings,                  recD);

    // to controller
    m_glW->update_signal.connect(&K4SGController::update, this);
    m_glW->draw_imgui_signal.connect(&K4SGController::draw_ui, this);
}

auto K4SGController::start() -> void{
    m_glW->start();
    Logger::message("Exit K4 scaner grabber.\n");
}

auto K4SGController::update() -> void{

    model.connection.update();
    model.settings.network.lastFrameIdSent = model.connection.last_frame_id_sent();
    model.settings.network.lastFrameSentTS = model.connection.last_frame_sent_timestamp_nanosecond();

    m_mainD.deviceD.update();
//    m_mainD.recorderD.update(&model.recorder);
}

auto K4SGController::draw_ui(geo::Pt2f size) -> void{
    m_mainD.draw(size, model);
}
