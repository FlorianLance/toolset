
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

// std
#include <iostream>

// base
#include "utility/logger.hpp"

// 3d-engine
#include "imgui-tb/imgui_dc_ui_drawer.hpp"

// local
#include "dcg_signals.hpp"

using namespace tool;
using namespace cam;
using namespace graphics;
using namespace std::string_view_literals;

DCGController::~DCGController(){
    model->clean();
}

auto DCGController::initialize(size_t id) -> bool{

    auto lg = LogGuard("DCGController::initialize"sv);

    // init view
    view  = std::make_unique<DCGView>(id);

    // init model
    model = std::make_unique<DCGModel>();
    model->server.settings.idG = id;

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

    auto lg = LogGuard("DCGController::set_connections"sv);

    // pointers
    auto s         = DCGSignals::get();
    // # drawers
    auto deviceD   = &view->mainW.deviceD;
    auto recorderD = &view->mainW.recorderD;
    // # model
    auto device    = model->device.get();
    auto recorder  = &model->recorder;
    auto server    = &model->server;

    // from logger
    Logger::get()->message_signal.connect([&](std::string message){
        view->mainW.append_log(message);
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        view->mainW.append_log(warning);
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        view->mainW.append_log(error);
        std::cerr << error;
    });

    device->update_device_name_signal.connect( [&](int deviceId, std::string deviceName){
        tool::graphics::DCUIDrawer::udpate_device_name(deviceId, deviceName); // TODO: move to settings/states ?
    });


    // # ui
    // ## controller
    view->gl()->update_signal.connect(&DCGController::update, this);
    view->gl()->draw_imgui_signal.connect(&DCGController::draw_ui, this);

    // # actions
    // ## app
    server->shutdown_signal.connect([&](){system("c:\\windows\\system32\\shutdown /s");});
    server->quit_signal.connect(&DCGView::exit, view.get());
    server->restart_signal.connect([&](){system("c:\\windows\\system32\\shutdown /R");});
    // ## model
    s->save_global_settings_file_signal.connect(                &DCGModel::save_global_settings,                           model.get());
    s->reset_device_settings_signal.connect(                    &DCGModel::reset_device_settings,                          model.get());
    s->reset_filters_settings_signal.connect(                   &DCGModel::reset_filters_filters,                          model.get());
    s->reset_color_settings_signal.connect(                     &DCGModel::reset_color_settings,                           model.get());
    s->reset_model_settings_signal.connect(                     &DCGModel::reset_model_settings,                           model.get());
    s->load_default_device_settings_file_signal.connect(        &DCGModel::load_default_device_settings_file,              model.get());
    s->load_default_filters_settings_file_signal.connect(       &DCGModel::load_default_filters_settings_file,             model.get());
    s->load_default_color_settings_file_signal.connect(         &DCGModel::load_default_color_settings_file,               model.get());
    s->load_default_model_settings_file_signal.connect(         &DCGModel::load_default_model_settings_file,               model.get());
    s->load_subpart_device_settings_file_signal.connect(        &DCGModel::load_subpart_device_settings_file,              model.get());
    s->load_subpart_filters_settings_file_signal.connect(       &DCGModel::load_subpart_filters_settings_file,             model.get());
    s->load_subpart_color_settings_file_signal.connect(         &DCGModel::load_subpart_color_settings_file,               model.get());
    s->load_subpart_model_settings_file_signal.connect(         &DCGModel::load_subpart_model_settings_file,               model.get());
    device->color_settings_reset_signal.connect(                &DCGModel::update_color_settings_ui_only,                  model.get());
    // ## server
    s->sending_failure_signal.connect(                          &DCServer::simulate_sending_failure,                       server);
    s->reset_reading_network_signal.connect(                    &DCServer::reset_network,                                  server);
    s->disconnect_all_clients_signal.connect(                   &DCServer::disconnect_clients,                             server);
    // ## device drawer
    s->save_cloud_to_file_signal.connect(                       &DCDeviceDrawer::save_current_cloud,                       deviceD);
    // ## recorder
    s->start_recorder_signal.connect(                           &DCVideoRecorder::start_recording,                         recorder);
    s->stop_recorder_signal.connect(                            &DCVideoRecorder::stop_recording,                          recorder);
    s->reset_recorder_signal.connect(                           &DCVideoRecorder::reset_recording,                         recorder);
    s->set_recorder_time_signal.connect(                        &DCVideoRecorder::set_time,                                recorder);
    s->save_recorder_signal.connect(                            &DCVideoRecorder::save_to_file,                            recorder);

    // # settings
    // ## device
    s->update_device_settings_signal.connect(                   &DCDevice::update_device_settings,                         device);
    s->update_filters_signal.connect(                           &DCDevice::update_filters_settings,                        device);        
    s->update_color_settings_signal.connect(                    &DCDevice::update_color_settings,                          device);
    s->update_delay_settings_signal.connect(                    &DCDevice::update_delay_settings,                          device);
    // ## recorder
    s->update_model_settings_signal.connect(                    &DCVideoRecorder::update_model,                            recorder);
    s->update_recorder_settings_signal.connect(                 &DCVideoRecorder::update_settings,                         recorder);
    // ## device drawer
    s->update_filters_signal.connect(                           &DCDeviceDrawer::update_filters_settings,                  deviceD);
    s->update_filters_ui_only_signal.connect(                   &DCDeviceDrawer::update_filters_settings,                  deviceD);
    s->update_model_settings_signal.connect(                    &graphics::DCCloudsSceneDrawer::update_model_settings,     deviceD);
    s->update_scene_display_settings_signal.connect(            &DCDeviceDrawer::update_scene_display_settings,            deviceD);
    s->update_cloud_display_settings_signal.connect(            &DCDeviceDrawer::update_device_display_settings,           deviceD);
    // ## recorder drawer
    s->update_model_settings_signal.connect(                    &DCRecorderDrawer::update_model_settings,                  recorderD);
    s->update_scene_display_settings_signal.connect(            &DCRecorderDrawer::update_scene_display_settings,          recorderD);
    s->update_cloud_display_settings_signal.connect(            &DCRecorderDrawer::update_device_display_settings,         recorderD);

    // # frames
    // ## server
    device->new_compressed_frame_signal.connect(                &DCServer::send_frame,                                     server);
    // ## recorder
    device->new_compressed_frame_signal.connect(                &DCVideoRecorder::add_compressed_frame_to_default_device,  recorder);
    // ## device drawer
    device->new_frame_signal.connect(                           &DCDeviceDrawer::update_frame,                             deviceD);
    // ## recorder drawer
    recorder->new_frame_signal.connect(                         &DCRecorderDrawer::set_frame,                              recorderD);
}

auto DCGController::start() -> void{
    auto lg = LogGuard("DCGController::start"sv);
    view->start();
}

auto DCGController::update() -> void{
    model->update();
    view->update();
}

auto DCGController::draw_ui(geo::Pt2f size) -> void{
    view->draw(size, model.get());
}
