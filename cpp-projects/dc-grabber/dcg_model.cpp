
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

    auto lg = LogG("DCGModel::DCGModel"sv);
    device = std::make_unique<cam::DCDevice>();
    Log::message("start thread\n");
    device->start_thread();

    server.device_settings_received_signal.connect([&](){
        DCGSignals::get()->update_device_settings_signal(server.settings.deviceS);
    });
    server.color_settings_received_signal.connect([&](){
        DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
    });
    server.filters_settings_received_signal.connect([&](){
        DCGSignals::get()->update_filters_signal(server.settings.filtersS);
    });
    server.delay_settings_received_signal.connect([&](){
        DCGSignals::get()->update_misc_settings_signal(server.settings.miscS);
    });

    Log::message("end model\n");
}

DCGModel::~DCGModel(){

    auto lg = LogG("~DCGModel::DCGModel"sv);

    // task
    // doTask = false;
    // mainTF.dump(std::cout);
    // resTask.wait();


    // thread
    device->stop_thread();

    // all
    device = nullptr;    

}

auto DCGModel::initialize() -> bool{
    
    auto lg = LogG("DCGModel::initialize"sv);
    
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

    // device
    device->update_device_settings(server.settings.deviceS);

    // color
    device->update_color_settings(server.settings.colorS);

    // filters
    device->update_filters_settings(server.settings.filtersS);

    // delay
    device->update_misc_settings(server.settings.miscS);

    // {
    //     tf::Task startLoopT = mainTF.emplace([&](){
    //         Log::message("start loop\n");
    //     }).name("startLoop");

    //     tf::Task checkLoopT = mainTF.emplace([&](){
    //         return doTask ? 0 : 1;
    //     }).name("checkLoop");

    //     auto deviceReadFramesT = mainTF.composed_of(*device->read_frames_task()).name("deviceReadFramesModule");


    //     tf::Task backToCheckT = mainTF.emplace([&](){

    //     }).name("backToCheck");

    //     tf::Task endLoopT = mainTF.emplace([&](){

    //     }).name("endLoop");
    //     resTask = executor.run(mainTF);

    //     startLoopT.precede(checkLoopT);
    //     checkLoopT.precede(deviceReadFramesT, endLoopT);
    //     deviceReadFramesT.precede(backToCheckT);
    //     backToCheckT.precede(checkLoopT);
    // }




    return true;
}

auto DCGModel::update() -> void{

    server.update();    
    recorder.update();

    if(server.settings.colorS.update_auto_calibration(server.settings.deviceS.configS.typeDevice)){
        DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
    }
}

auto DCGModel::clean() -> void{
    auto lg = LogG("DCGModel::clean"sv);
    server.clean();
}

auto DCGModel::trigger_settings() -> void{
    // filters
    DCGSignals::get()->update_filters_ui_only_signal(server.settings.filtersS);
    // model
    DCGSignals::get()->update_model_settings_signal(0, server.settings.modelS);
    // cloud display
    DCGSignals::get()->update_cloud_display_settings_signal(0, server.settings.displayS);
    // // scene display
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

auto DCGModel::update_color_settings_ui_only(const cam::DCColorSettings &colorS) -> void{
    server.settings.colorS = colorS;
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

auto DCGModel::save_server_settings_file(const std::string &settingsFilePath) -> bool{

    if(server.settings.save_to_json_str_file(settingsFilePath)){
        Log::message(std::format("Server settings file saved at [{}].\n", settingsFilePath));
        return true;
    }
    return false;
}

auto DCGModel::load_server_settings_file(const std::string &settingsFilePath) -> bool{
    if(server.settings.load_from_file(settingsFilePath)){
        Log::message(std::format("Server settings file loaded from [{}].\n", settingsFilePath));
        return true;
    }
    return false;
}



