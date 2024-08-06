
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

// 3d-engine
#include "imgui/extra/ImGuiFileDialog.h"


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
    client.clean();
}

auto DCMModel::initialize() -> bool{

    // check if path exist
    if(std::filesystem::exists(DCSettingsPaths::client)){
        if(!client.initialize(DCSettingsPaths::client.string())){
            return false;
        }
    }else if(std::filesystem::exists(DCSettingsPaths::defaultClient)){
        if(!client.initialize(DCSettingsPaths::defaultClient.string())){
            return false;
        }
    }else{

        // legacy
        std::string networkFilePath;
        if(std::filesystem::exists(DCSettingsPaths::hostNetwork)){
            networkFilePath = DCSettingsPaths::hostNetwork.string();
        }else if(std::filesystem::exists(DCSettingsPaths::hostNetworkLegacy)){
            networkFilePath = DCSettingsPaths::hostNetworkLegacy.string();
        }else if(std::filesystem::exists(DCSettingsPaths::defaultNetwork)){
            networkFilePath = DCSettingsPaths::defaultNetwork.string();
        }else{
            Logger::error("[DCMModel::initialize] No client or network file path found\n");
            return false;
        }

        if(!client.load_network_settings_file(networkFilePath)){
            return false;
        }

        DCSettingsPaths::initialize_grabbers(client.devices_nb());

        for(auto &deviceClientS : client.settings.devicesS){
            // read filters settings file
            if(!deviceClientS.filtersS.load_from_file(deviceClientS.filtersFilePath = DCSettingsPaths::get_filters_settings_file_path(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No filters settings file found for grabber with id [{}], default parameters used instead [{}].\n", deviceClientS.id, deviceClientS.filtersFilePath));
            }
            // read calibration filters settings file
            if(!deviceClientS.calibrationFiltersS.load_from_file(deviceClientS.calibrationFiltersFilePath =  DCSettingsPaths::get_calibration_filters_settings_file_path(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No calibration settings file found for grabber with id [{}], default parameters used instead [{}].\n", deviceClientS.id, deviceClientS.calibrationFiltersFilePath));
            }
            // read device settings file
            if(!deviceClientS.deviceS.load_from_file(deviceClientS.deviceFilePath = DCSettingsPaths::get_device_settings_file_path(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No device file found for grabber with id [{}], default parameters used instead [{}].\n", deviceClientS.id, deviceClientS.deviceFilePath));
            }
            // read color settings file
            if(!deviceClientS.colorS.load_from_file(deviceClientS.colorFilePath = DCSettingsPaths::get_color_settings_file_path(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No color file found for grabber with id [{}], default parameters used instead [{}].\n", deviceClientS.id, deviceClientS.colorFilePath));
            }
            // read model settings file
            if(!deviceClientS.modelS.load_from_file(deviceClientS.modelFilePath =  DCSettingsPaths::get_model_settings_file_path(deviceClientS.id))){
                Logger::error(std::format("[DCMModel] No model file found for grabber with id [{}], default parameters used instead [{}].\n", deviceClientS.id, deviceClientS.modelFilePath));
            }
        }
    }

    size_t nbConnections = client.devices_nb();
    recorder.initialize(nbConnections);
    calibrator.initialize(nbConnections);

    return true;
}

auto DCMModel::trigger_settings() -> void{
    client.trigger_all_models_settings();
    client.trigger_all_device_display_settings();
}

auto DCMModel::ask_calibration() -> void{
    std::vector<cam::DCModelSettings> models;    
    for(const auto &devS : client.settings.devicesS){
        models.push_back(devS.modelS);
    }
    DCMSignals::get()->calibrate_signal(std::move(models));
}

auto DCMModel::update_filters(size_t idC, const cam::DCFiltersSettings &filters) -> void {
    client.update_filters(idC, filters);
}

auto DCMModel::update_calibration_filters(size_t idC, const cam::DCFiltersSettings &calibrationFilters) -> void{
    client.update_calibration_filters(idC, calibrationFilters);
}

auto DCMModel::update_device_settings(size_t idC, const cam::DCDeviceSettings &deviceS) -> void {
    client.update_device_settings(idC, deviceS);
}

auto DCMModel::update_color_settings(size_t idC, const cam::DCColorSettings &colorS) -> void{
    client.update_color_settings(idC, colorS);
}

auto DCMModel::update_delay_settings(size_t idC, const cam::DCDelaySettings &delayS) -> void{
    client.update_delay_settings(idC, delayS);
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
                path = DCSettingsPaths::client.string();
            }else if(sAction.file == SFile::Default){
                path = DCSettingsPaths::defaultClient.string();
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
                if(std::filesystem::exists(DCSettingsPaths::client)){
                    path = DCSettingsPaths::client.string();
                }
            }else if(sAction.file == SFile::Default){
                if(std::filesystem::exists(DCSettingsPaths::defaultClient)){
                    path = DCSettingsPaths::defaultClient.string();
                }
            }else if(sAction.file == SFile::Specific){
                path = sAction.path;
            }

            if(!path.empty()){
                clean();
                if(client.initialize(path)){
                    size_t nbConnections = client.devices_nb();
                    recorder.initialize(nbConnections);
                    calibrator.initialize(nbConnections);
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
                                client.update_filters(id, cSettings.devicesS[id].filtersS);
                            }else if(sAction.type == SType::CalibrationFilters){
                                client.update_calibration_filters(id, cSettings.devicesS[id].calibrationFiltersS);
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
                            client.update_filters(sAction.id, cSettings.devicesS[sAction.id].filtersS);
                        }else if(sAction.type == SType::CalibrationFilters){
                            client.update_calibration_filters(sAction.id, cSettings.devicesS[sAction.id].calibrationFiltersS);
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
                    client.update_filters(id, DCFiltersSettings());
                }else if(sAction.type == SType::CalibrationFilters){
                    client.update_calibration_filters(id, DCFiltersSettings::default_init_for_calibration());
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
                client.update_filters(sAction.id, DCFiltersSettings());
            }else if(sAction.type == SType::CalibrationFilters){
                client.update_calibration_filters(sAction.id, DCFiltersSettings::default_init_for_calibration());
            }else if(sAction.type == SType::Color){
                client.update_color_settings(sAction.id, DCColorSettings());
            }else if(sAction.type == SType::Model){
                client.update_model_settings(sAction.id, DCModelSettings());
            }
        }
    }
}

auto DCMModel::host_name() -> std::string{
    return Host::get_name();
}
