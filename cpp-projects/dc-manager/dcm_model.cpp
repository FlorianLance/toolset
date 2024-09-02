
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
    auto lg = LogGuard("DCMModel::clean"sv);
    client.clean();
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
            if(!deviceClientS.filtersS.load_from_file(deviceClientS.filtersFilePath = paths->client_filters_settings_file(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No filters settings file found for grabber with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read calibration filters settings file
            if(!deviceClientS.calibrationFiltersS.load_from_file(deviceClientS.calibrationFiltersFilePath =  paths->client_calibration_filters_settings_file(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No calibration settings file found for grabber with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read device settings file
            if(!deviceClientS.deviceS.load_from_file(deviceClientS.deviceFilePath = paths->client_device_settings_file(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No device file found for grabber with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read color settings file
            if(!deviceClientS.colorS.load_from_file(deviceClientS.colorFilePath = paths->client_color_settings_file(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No color file found for grabber with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
            // read model settings file
            if(!deviceClientS.modelS.load_from_file(deviceClientS.modelFilePath =  paths->client_model_settings_file(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No model file found for grabber with id [{}], default parameters used instead.\n", deviceClientS.id));
            }
        }
    }

    size_t nbDevices = client.devices_nb();
    recorder.initialize(nbDevices);
    calibrator.initialize(nbDevices);
    DCMSignals::get()->initialize_signal(nbDevices);

    return true;
}


auto DCMModel::ask_calibration() -> void{
    std::vector<cam::DCModelSettings> models;    
    for(const auto &devS : client.settings.devicesS){
        models.push_back(devS.modelS);
    }
    DCMSignals::get()->calibrate_signal(std::move(models));
}


auto DCMModel::update() -> void{
    client.update();
    calibrator.update();
    player.update();
    recorder.update();
}

auto DCMModel::process_settings_action(SettingsAction sAction) -> void{

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
                        Logger::error(std::format("Cannot load sub settings from file [{}] for all devices, incompatible number of devices, current: [{}], from file: [{}]\n", path, cSettings.devicesS.size(), client.devices_nb()));
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
                        Logger::error(std::format("Cannot load sub settings with id [{}] from file [{}], not enough devices from avaiable: [{}]\n", sAction.id, path, cSettings.devicesS.size()));
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

auto DCMModel::trigger_settings() -> void{
    auto lg = LogGuard("DCMModel::trigger_settings"sv);
    // color settings: device
    // DCGSignals::get()->update_color_settings_signal(server.settings.colorS);
}

