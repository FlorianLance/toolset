
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

#include "dcg_controller.hpp"

// base
#include "utility/benchmark.hpp"
#include "utility/logger.hpp"
#include "utility/paths.hpp"

// 3d-engine
#include "imgui-tb/imgui_dc_ui_drawer.hpp"

// local
#include "dcg_signals.hpp"
#include "data/dcg_paths.hpp"

using namespace tool;


DCGController::~DCGController(){
    model->clean();
}

auto DCGController::initialize(size_t idLocalGrabber) -> bool{

    // init paths
    DCGPaths::initialize(idLocalGrabber, tool::Paths::applicationDir, DCGSettings::host_name());
    // init logger
    Logger::init(Paths::logsDir, DCGPaths::logName);

    // setup benchmark
    Bench::disable_display();

    view  = std::make_unique<DCGView>();
    model = std::make_unique<DCGModel>();
    model->settings.idLocalGrabber = idLocalGrabber;

    // connections
    set_connections();

    // initialize
    // # model
    if(!model->initialize()){
        Logger::error("Invalid initialization.\n");
        return false;
    }
    // # view
    view->initialize();

    model->trigger_settings();

    return true;
}

auto DCGController::set_connections() -> void{

    // aliases
    using Sett  = DCGSettings;
    using CCo   = net::DCClientConnection;
    using Rec   = cam::DCRecorder;
    using RecD  = graphics::DCRecorderDrawer;
    using DevD  = graphics::DCDeviceDrawer;
    using DevM  = cam::DCDeviceManager;

    // pointers
    auto s    = DCGSignals::get();
    auto sett = &model->settings;
    auto dev  = &model->device;
    auto rec  = &model->recorder;
    auto con  = &model->connection;
    auto devD = &view->mainW.deviceD;
    auto recD = &view->mainW.recorderD;

    // from logger
    Logger::get()->message_signal.connect([&](std::string message){
        view->mainW.leftPanelD.append_log(message);
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        view->mainW.leftPanelD.append_log(warning);
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        view->mainW.leftPanelD.append_log(error);
        std::cerr << error;
    });

    // to app
    CCo::shutdown_signal.connect([&](){system("c:\\windows\\system32\\shutdown /s");});
    CCo::quit_signal.connect(&DCGView::exit, view.get());
    CCo::restart_signal.connect([&](){system("c:\\windows\\system32\\shutdown /R");});

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
    DevM::update_device_name_signal.connect(                         [&](int deviceId, std::string deviceName){
        tool::graphics::DCUIDrawer::udpate_device_name(deviceId, deviceName);
    });
    s->reset_device_settings_signal.connect(                         &Sett::reset_device_settings,                          sett);
    s->save_device_settings_to_default_file_signal.connect(          &Sett::save_device_settings_to_default_file,           sett);
    s->save_device_settings_to_current_hostname_file_signal.connect( &Sett::save_device_settings_to_current_hostname_file,  sett);
    s->load_default_device_settings_file_signal.connect(             &Sett::load_default_device_settings_file,              sett);
    s->load_current_hostname_device_settings_file_signal.connect(    &Sett::load_current_hostname_device_settings_file,     sett);
    // s->mouse_pressed_depth_direct_signal.connect(                    &Sett::update_filters_depth_mask,                      sett);
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
    dev->color_settings_reset_signal.connect(                        &Sett::update_color_settings_from_device_manager,      sett);
    // # device
    // CCo::update_device_list_signal.connect(                          &DevM::update_device_list,                             dev);
    s->update_device_settings_signal.connect(                        &DevM::update_device_settings,                         dev);
    s->update_filters_signal.connect(                                &DevM::update_filters_settings,                        dev);
    s->update_color_settings_signal.connect(                         &DevM::update_color_settings,                          dev);
    s->update_delay_signal.connect(                                  &DevM::update_delay_settings,                          dev);
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
    s->update_model_signal.connect(                                  &graphics::DCCloudsSceneDrawer::update_model,          devD);
    s->update_filters_signal.connect(                                &DevD::update_filters_settings,                        devD);
    // # recorder
    s->update_scene_display_settings_signal.connect(                 &RecD::update_scene_display_settings,                  recD);
    s->update_cloud_display_settings_signal.connect(                 &RecD::update_cloud_display_settings,                  recD);

    // to controller
    view->gl()->update_signal.connect(&DCGController::update, this);
    view->gl()->draw_imgui_signal.connect(&DCGController::draw_ui, this);
}

auto DCGController::start() -> void{
    view->start();
}

auto DCGController::update() -> void{
    model->update();
    view->update();
}

auto DCGController::draw_ui(geo::Pt2f size) -> void{
    view->draw(size, model.get());
}
