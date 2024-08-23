
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

// local
#include "dcg_signals.hpp"

using namespace tool;
using namespace cam;
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

        if(std::string legacySettingsFile = DCSettingsPaths::get()->server_device_settings_file(); !legacySettingsFile.empty()){
            if(server.settings.deviceS.load_from_file(legacySettingsFile)){
                server.settings.deviceFilePath = std::move(legacySettingsFile);
            }
        }
        if(std::string legacySettingsFile = DCSettingsPaths::get()->server_filters_settings_file(); !legacySettingsFile.empty()){
            if(server.settings.filtersS.load_from_file(legacySettingsFile)){
                server.settings.filtersFilePath = std::move(legacySettingsFile);
            }
        }
        if(std::string legacySettingsFile = DCSettingsPaths::get()->server_color_settings_file(); !legacySettingsFile.empty()){
            if(server.settings.colorS.load_from_file(legacySettingsFile)){
                server.settings.colorFilePath = std::move(legacySettingsFile);
            }
        }
        if(std::string legacySettingsFile = DCSettingsPaths::get()->server_model_settings_file(); !legacySettingsFile.empty()){
            if(server.settings.modelS.load_from_file(legacySettingsFile)){
                server.settings.modelFilePath = std::move(legacySettingsFile);
            }
        }
    }

    recorder.initialize(1);

    device->update_device_settings(server.settings.deviceS);

    return true;
}

auto DCGModel::update() -> void{
    server.update();    
    recorder.update();
}

auto DCGModel::clean() -> void{
    auto lg = LogGuard("DCGModel::clean"sv);
    server.clean();
}


auto DCGModel::trigger_settings() -> void{
    auto lg = LogGuard("DCGModel::trigger_settings"sv);
    // device settings: device
    // DCGSignals::get()->update_device_settings_signal(server.settings.deviceS);
    // color settings: device
    DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
    // filters settings: device drawer / device
    DCGSignals::get()->update_filters_signal(server.settings.filtersS);
    // model settings: device drawer / recorder drawer / recorder
    DCGSignals::get()->update_model_settings_signal(0, server.settings.modelS);
    // delay setings: device
    DCGSignals::get()->update_delay_settings_signal(server.settings.delayS);
    // display settings: device drawer / recorder drawer
    DCGSignals::get()->update_cloud_display_settings_signal(0, server.settings.displayS);
    // scene settings: device drawer / recorder drawer
    DCGSignals::get()->update_scene_display_settings_signal(server.settings.sceneDisplayS);
}

auto DCGModel::reset_device_settings() -> void{
    server.settings.deviceS = cam::DCDeviceSettings();
    DCGSignals::get()->update_device_settings_signal(server.settings.deviceS);
}

auto DCGModel::reset_filters_filters() -> void{
    server.settings.filtersS = cam::DCFiltersSettings();
    DCGSignals::get()->update_filters_signal(server.settings.filtersS);
}

auto DCGModel::reset_color_settings() -> void{
    server.settings.colorS = cam::DCColorSettings();
    DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
}

auto DCGModel::reset_model_settings() -> void{
    server.settings.modelS = cam::DCModelSettings();
    DCGSignals::get()->update_model_settings_signal(0, server.settings.modelS);
}

auto DCGModel::load_default_device_settings_file() -> void{
    DCServerSettings settings;
    if(settings.load_from_file(DCSettingsPaths::get()->defaultServer.string())){
        server.settings.deviceS = std::move(settings.deviceS);
        DCGSignals::get()->update_device_settings_signal(server.settings.deviceS);
    }
}

auto DCGModel::load_default_filters_settings_file() -> void{
    DCServerSettings settings;
    if(settings.load_from_file(DCSettingsPaths::get()->defaultServer.string())){
        server.settings.filtersS = std::move(settings.filtersS);
        DCGSignals::get()->update_filters_signal(server.settings.filtersS);
    }
}

auto DCGModel::load_default_color_settings_file() -> void{
    DCServerSettings settings;
    if(settings.load_from_file(DCSettingsPaths::get()->defaultServer.string())){
        server.settings.colorS = std::move(settings.colorS);
        DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
    }
}

auto DCGModel::load_default_model_settings_file() -> void{
    DCServerSettings settings;
    if(settings.load_from_file(DCSettingsPaths::get()->defaultServer.string())){
        server.settings.modelS = std::move(settings.modelS);
        DCGSignals::get()->update_model_settings_signal(0, server.settings.modelS);
    }
}

auto DCGModel::load_subpart_device_settings_file(const std::string &settingsFilePath) -> bool{
    DCServerSettings settings;
    if(settings.load_from_file(settingsFilePath)){
        server.settings.deviceS = std::move(settings.deviceS);
        DCGSignals::get()->update_device_settings_signal(server.settings.deviceS);
        return true;
    }
    return false;
}

auto DCGModel::load_subpart_filters_settings_file(const std::string &settingsFilePath) -> bool{
    DCServerSettings settings;
    if(settings.load_from_file(settingsFilePath)){
        server.settings.filtersS = std::move(settings.filtersS);
        DCGSignals::get()->update_filters_signal(server.settings.filtersS);
        return true;
    }
    return false;
}

auto DCGModel::load_subpart_color_settings_file(const std::string &settingsFilePath) -> bool{
    DCServerSettings settings;
    if(settings.load_from_file(settingsFilePath)){
        server.settings.colorS = std::move(settings.colorS);
        DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
        return true;
    }
    return false;
}

auto DCGModel::load_subpart_model_settings_file(const std::string &settingsFilePath) -> bool{
    DCServerSettings settings;
    if(settings.load_from_file(settingsFilePath)){
        server.settings.modelS = std::move(settings.modelS);
        DCGSignals::get()->update_model_settings_signal(0, server.settings.modelS);
        return true;
    }
    return false;
}

auto DCGModel::save_global_settings(const std::string &settingsFilePath) -> bool{

    if(server.settings.save_to_json_str_file(settingsFilePath)){
        Logger::message(std::format("Global settings file saved at [{}].\n", settingsFilePath));
        return true;
    }
    return false;
}



