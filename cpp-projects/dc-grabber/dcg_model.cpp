
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

#include "dcg_model.hpp"

// base
#include "depth-camera/settings/dc_settings_paths.hpp"
#include "utility/logger.hpp"

using namespace tool;
using namespace std::string_view_literals;

DCGModel::DCGModel(){
    auto lg = LogGuard("DCGModel::DCGModel"sv);
    device = std::make_unique<cam::DCDevice>();
    device->start_thread();
}

DCGModel::~DCGModel(){
    auto lg = LogGuard("~DCGModel::DCGModel"sv);
    device->stop_thread();
    device = nullptr;
}

auto DCGModel::initialize() -> bool{

    auto lg = LogGuard("DCGModel::initialize"sv);
    
    // check if path exist
    if(auto filePath = DCSettingsPaths::get()->server_settings_file(); !filePath.empty()){
        if(!server.initialize(filePath)){
            return false;
        }
    }else {

        // legacy
        if(auto filePath = DCSettingsPaths::get()->network_settings_file(); !filePath.empty()){
            if(!server.legacy_initialize(filePath)){
                return false;
            }
        }else{
            return false;
        }

        server.load_device_settings_file(DCSettingsPaths::get()->server_device_settings_file());
        server.load_filters_settings_file(DCSettingsPaths::get()->server_filters_settings_file());
        server.load_calibration_filters_settings_file(DCSettingsPaths::get()->server_calibration_filters_settings_file());
        server.load_color_settings_file(DCSettingsPaths::get()->server_color_settings_file());
        server.load_model_settings_file(DCSettingsPaths::get()->server_model_settings_file());
    }

    recorder.initialize(1);

    return true;
}

auto DCGModel::update() -> void{
    server.update();    
    uiSettings.lastFrameIdSent = server.last_frame_id_sent();
    uiSettings.lastFrameSentTS = server.last_frame_sent_timestamp_nanosecond();
    recorder.update();
}

auto DCGModel::clean() -> void{
    auto lg = LogGuard("DCGModel::clean"sv);
    server.clean();
}

// auto DCGModel::trigger_settings() -> void{

//     auto lg = LogGuard("DCGModel::trigger_settings"sv);
//     // triggers data to init ui
//     Logger::log("### filters\n");
//     settings.triggers_filters_settings();
//     Logger::log("### display\n");
//     settings.triggers_display_settings();
//     Logger::log("### color\n");
//     settings.triggers_color_settings();
//     Logger::log("### delay\n");
//     settings.triggers_delay_settings();
//     Logger::log("### model\n");
//     settings.triggers_model();
//     Logger::log("### device\n");
//     settings.triggers_device_settings();
// }

