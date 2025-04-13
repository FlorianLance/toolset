
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


#include "dcm_model.hpp"

// base
#include "depth-camera/settings/dc_settings_paths.hpp"
#include "utility/logger.hpp"

// local
#include "dcm_signals.hpp"

using namespace tool;
using namespace cam;
using namespace tool::net;

DCMModel::DCMModel(){
}

DCMModel::~DCMModel(){
    clean();
}

auto DCMModel::clean() -> void {
    auto lg = LogG("DCMModel::clean"sv);
    client.clean();
}

auto DCMModel::add_device(cam::DCClientType type) -> void{
    addDeviceEvent = type;
}

auto DCMModel::remove_last_device() -> void{
    m_removeLastDeviceEvent = true;
}

auto DCMModel::remove_all_devices() -> void{
    while(client.devices_nb() > 0){
        client.remove_last_device();
        recorder.remove_last_device();
    }
    calibrator.initialize(0);
    DCMSignals::get()->initialize_signal(0);
}

auto DCMModel::initialize() -> bool{

    auto paths = DCSettingsPaths::get();

    // check if path exist
    if(auto filePath = paths->client_settings_file(); !filePath.empty()){
        if(!client.initialize(filePath)){
            return false;
        }
    }else{

        // legacy
        if(auto filePath = paths->network_settings_file(); !filePath.empty()){
            if(!client.legacy_initialize(filePath)){
                return false;
            }
        }else{
            return false;
        }

        for(auto &deviceClientS : client.settings.devicesS){

            // read filters settings file
            if(!deviceClientS.filtersS.load_from_file(paths->client_filters_settings_file(deviceClientS.id))){
                Log::error(std::format("[DCMModel] No filters settings file found for client with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read calibration filters settings file
            if(!deviceClientS.calibrationFiltersS.load_from_file(paths->client_calibration_filters_settings_file(deviceClientS.id))){
                Log::error(std::format("[DCMModel] No calibration settings file found for client with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read device settings file
            if(!deviceClientS.deviceS.load_from_file(paths->client_device_settings_file(deviceClientS.id))){
                Log::error(std::format("[DCMModel] No device file found for client with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read color settings file
            if(!deviceClientS.colorS.load_from_file(paths->client_color_settings_file(deviceClientS.id))){
                Log::error(std::format("[DCMModel] No color file found for client with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read model settings file
            if(!deviceClientS.modelS.load_from_file(paths->client_model_settings_file(deviceClientS.id))){
                Log::error(std::format("[DCMModel] No model file found for client with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
        }
    }

    size_t nbDevices = client.devices_nb();
    recorder.initialize(nbDevices);        
    calibrator.initialize(nbDevices);

    // devices
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){        
        client.apply_device_settings(idC);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // color
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){
        client.apply_color_settings(idC);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // filters / delay
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){
        client.apply_filters_settings(idC);
        client.apply_misc_settings(idC);
    }

    // models
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){
        recorder.update_model(idC, client.settings.devicesS[idC].modelS);
    }

    DCMSignals::get()->initialize_signal(nbDevices);

    return true;
}

auto DCMModel::trigger_settings() -> void{
    // filters
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){
        DCMSignals::get()->update_filters_settings_ui_only_signal(idC, client.settings.devicesS[idC].filtersS);
    }
    // models
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){
        DCMSignals::get()->update_model_settings_ui_only_signal(idC, client.settings.devicesS[idC].modelS);
    }
    // cloud display
    for(size_t idC = 0; idC < client.devices_nb(); ++idC){
        DCMSignals::get()->update_cloud_display_settings_signal(idC, client.settings.devicesS[idC].displayS);
    }
    // scene display
    DCMSignals::get()->update_scene_display_settings_signal(client.settings.sceneDisplayS);
}


auto DCMModel::ask_calibration() -> void{
    std::vector<cam::DCModelSettings> models;    
    for(const auto &devS : client.settings.devicesS){
        models.push_back(devS.modelS);
    }
    DCMSignals::get()->calibrate_signal(std::move(models));
}


auto DCMModel::update() -> void{

    // events
    if(addDeviceEvent.has_value()){
        
        auto lg = LogG("DCMModel::update::addDeviceEvent"sv);

        DCClientDeviceSettings ds;
        ds.connectionS.connectionType = addDeviceEvent.value();
        ds.connectionS.startReadingThread = true;

        client.add_device(ds);
        client.apply_device_settings(client.devices_nb()-1);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        client.apply_color_settings(client.devices_nb()-1);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        client.apply_filters_settings(client.devices_nb()-1);
        client.apply_misc_settings(client.devices_nb()-1);

        recorder.add_device();
        recorder.update_model(client.devices_nb()-1, client.settings.devicesS.back().modelS);

        calibrator.initialize(client.devices_nb());
        DCMSignals::get()->initialize_signal(client.devices_nb());

        addDeviceEvent = std::nullopt;
    }

    if(m_removeLastDeviceEvent){

        if(client.devices_nb() > 0){
            auto lg = LogG("DCMModel::update::removeLastDeviceEvent"sv);
            int newDeviceNb = client.devices_nb() -1;
            client.remove_last_device();
            recorder.remove_last_device();
            calibrator.initialize(newDeviceNb);
            DCMSignals::get()->initialize_signal(newDeviceNb);
        }

        m_removeLastDeviceEvent = false;
    }

    client.update();
    calibrator.update();
    player.update();
    recorder.update();

    for(size_t idC = 0; idC < client.settings.devicesS.size(); ++idC){
        if(client.settings.devicesS[idC].colorS.update_auto_calibration(client.settings.devicesS[idC].deviceS.configS.typeDevice)){
            DCMSignals::get()->update_color_settings_signal(idC, client.settings.devicesS[idC].colorS);
        }
    }

}

auto DCMModel::process_settings_action(SettingsAction sAction) -> void{

    Log::fmessage("process_settings_action {} {}\n", (int)sAction.action, (int)sAction.file);
    std::string path;

    if(sAction.action == SAction::Save){

        if(sAction.type == SType::Global){
            if(sAction.file == SFile::Normal){                
                path = DCSettingsPaths::get()->client.string();
            }else if(sAction.file == SFile::Default){
                path = DCSettingsPaths::get()->defaultClient.string();
            }else if(sAction.file == SFile::Specific){
                path = sAction.path;
            }
        }

        Log::fmessage("PROCESS PATH {}\n", path);

        if(!path.empty()){
            client.settings.save_to_json_str_file(path);
        }

    }else if(sAction.action == SAction::Load){

        if(sAction.type == SType::Global){

            if(sAction.file == SFile::Normal){
                if(std::filesystem::exists(DCSettingsPaths::get()->client)){
                    path = DCSettingsPaths::get()->client.string();
                }
            }else if(sAction.file == SFile::Default){
                if(std::filesystem::exists(DCSettingsPaths::get()->defaultClient)){
                    path = DCSettingsPaths::get()->defaultClient.string();
                }
            }else if(sAction.file == SFile::Specific){
                path = sAction.path;
            }

            if(!path.empty()){

                clean();
                if(client.initialize(path)){
                    size_t nbDevices = client.devices_nb();
                    recorder.initialize(nbDevices);
                    calibrator.initialize(nbDevices);
                    DCMSignals::get()->initialize_signal(nbDevices);
                }
            }
        }else {

            path = sAction.path;

            DCClientSettings cSettings;
            if(cSettings.load_from_file(path)){

                if(sAction.target == STarget::All){
                    if(cSettings.devicesS.size() == client.devices_nb()) {
                        for(size_t id = 0; id < client.devices_nb(); ++id){
                            if(sAction.type == SType::Device){
                                client.update_device_settings(id, cSettings.devicesS[id].deviceS);
                            }else if(sAction.type == SType::Filters){
                                client.update_filters_settings(id, cSettings.devicesS[id].filtersS);
                            }else if(sAction.type == SType::CalibrationFilters){
                                client.update_calibration_filters_settings(id, cSettings.devicesS[id].calibrationFiltersS);
                            }else if(sAction.type == SType::Color){
                                client.update_color_settings(id, cSettings.devicesS[id].colorS);
                            }else if(sAction.type == SType::Model){
                                client.update_model_settings(id, cSettings.devicesS[id].modelS);
                            }
                        }
                    }else{
                        Log::error(std::format("Cannot load sub settings from file [{}] for all devices, incompatible number of devices, current: [{}], from file: [{}]\n", path, cSettings.devicesS.size(), client.devices_nb()));
                    }

                }else if(sAction.target == STarget::Individual){
                    if(sAction.id < cSettings.devicesS.size()){
                        if(sAction.type == SType::Device){
                            client.update_device_settings(sAction.id, cSettings.devicesS[sAction.id].deviceS);
                        }else if(sAction.type == SType::Filters){
                            client.update_filters_settings(sAction.id, cSettings.devicesS[sAction.id].filtersS);
                        }else if(sAction.type == SType::CalibrationFilters){
                            client.update_calibration_filters_settings(sAction.id, cSettings.devicesS[sAction.id].calibrationFiltersS);
                        }else if(sAction.type == SType::Color){
                            client.update_color_settings(sAction.id, cSettings.devicesS[sAction.id].colorS);
                        }else if(sAction.type == SType::Model){
                            client.update_model_settings(sAction.id, cSettings.devicesS[sAction.id].modelS);
                        }

                    }else{
                        Log::error(std::format("Cannot load sub settings with id [{}] from file [{}], not enough devices from avaiable: [{}]\n", sAction.id, path, cSettings.devicesS.size()));
                    }
                }
            }else{
                // ...
            }
        }
        // ...
    }else if(sAction.action == SAction::Reset){

        if(sAction.target == STarget::All){

            for(size_t id = 0; id < client.devices_nb(); ++id){
                if(sAction.type == SType::Device){
                    client.update_device_settings(id, DCDeviceSettings());
                }else if(sAction.type == SType::Filters){
                    client.update_filters_settings(id, DCFiltersSettings());
                }else if(sAction.type == SType::CalibrationFilters){
                    client.update_calibration_filters_settings(id, DCFiltersSettings::default_init_for_calibration());
                }else if(sAction.type == SType::Color){
                    client.update_color_settings(id, DCColorSettings());
                }else if(sAction.type == SType::Model){
                    client.update_model_settings(id, DCModelSettings());
                }
            }

        }else if(sAction.target == STarget::Individual){
            if(sAction.type == SType::Device){
                client.update_device_settings(sAction.id, DCDeviceSettings());
            }else if(sAction.type == SType::Filters){
                client.update_filters_settings(sAction.id, DCFiltersSettings());
            }else if(sAction.type == SType::CalibrationFilters){
                client.update_calibration_filters_settings(sAction.id, DCFiltersSettings::default_init_for_calibration());
            }else if(sAction.type == SType::Color){
                client.update_color_settings(sAction.id, DCColorSettings());
            }else if(sAction.type == SType::Model){
                client.update_model_settings(sAction.id, DCModelSettings());
            }
        }
    }
}

// auto DCMModel::trigger_settings() -> void{
//     auto lg = LogGuard("DCMModel::trigger_settings"sv);
//     // color settings: device
//     // DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
// }

