
/*******************************************************************************
** Toolset-k4-scaner-manager                                                  **
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

#include "k4sm_settings.hpp"

// std
#include <format>
#include <filesystem>
#include <fstream>

// base
#include "utility/logger.hpp"
#include "utility/string.hpp"
#include "utility/format.hpp"

// local
#include "k4sm_signals.hpp"
#include "data/k4sm_paths.hpp"

using namespace tool;
using namespace tool::network;


auto K4SMSettings::initialize() -> bool{

    // init pencils
    pencils.resize(3);
    int ray = 10;
    for(int offR = -ray; offR < ray; ++offR){
        for(int offC = -ray; offC < ray; ++offC){
            if(offR*offR + offC*offC < ray*ray){
                pencils[0].emplace_back(offR, offC);
            }
        }
    }
    ray = 40;
    for(int offR = -ray; offR < ray; ++offR){
        for(int offC = -ray; offC < ray; ++offC){
            if(offR*offR + offC*offC < ray*ray){
                pencils[1].emplace_back(offR, offC);
            }
        }
    }
    ray = 100;
    for(int offR = -ray; offR < ray; ++offR){
        for(int offC = -ray; offC < ray; ++offC){
            if(offR*offR + offC*offC < ray*ray){
                pencils[2].emplace_back(offR, offC);
            }
        }
    }

    // init network
    if(!globalSet.network.initialize()){
        return false;
    }
    // read network config file
    if(std::filesystem::exists(K4SMPaths::hostNetwork)){
        globalSet.networkFilePath = K4SMPaths::hostNetwork.string();
    }else if(std::filesystem::exists(K4SMPaths::defaultNetwork)){
        Logger::warning("Cannot find current host network file, use default instead.\n");
        globalSet.networkFilePath = K4SMPaths::defaultNetwork.string();
    }
    if(globalSet.networkFilePath.length() > 0){
        if(!globalSet.network.init_from_file(globalSet.networkFilePath)){
            return false;
        }
    }else{
        Logger::error("No network file found.\n");
        return false;
    }

    // init grabbers paths
    K4SMPaths::initialize_grabbers(globalSet.network.clientsInfo.size());

    // init grabbers settings
    size_t idC = 0;
    grabbersSet.resize(globalSet.network.clientsInfo.size());
    for(const auto &grabber : globalSet.network.clientsInfo){

        grabbersSet[idC].id = idC;
        grabbersSet[idC].network.name          = std::format("G{}", idC);
        grabbersSet[idC].network.sendingAdress = grabber.sendingAdress;
        grabbersSet[idC].network.readingAdress = grabber.readingAdress;
        grabbersSet[idC].network.sendingPort   = grabber.sendingPort;
        grabbersSet[idC].network.readingPort   = grabber.readingPort;

        // read filters settings file
        if(std::filesystem::exists(K4SMPaths::grabbersFilters[idC])){
            grabbersSet[idC].filtersFilePath = K4SMPaths::grabbersFilters[idC].string();
        }else if(std::filesystem::exists(K4SMPaths::defaultFilters)){
            grabbersSet[idC].filtersFilePath = K4SMPaths::defaultFilters.string();
        }
        if(grabbersSet[idC].filtersFilePath.length() > 0){
            grabbersSet[idC].filters.init_from_file(grabbersSet[idC].filtersFilePath);
        }else{
            Logger::error("No filters file found.\n");
        }

        // read calibration filters settings file
        if(std::filesystem::exists(K4SMPaths::grabbersCalibrationFilters[idC])){
            grabbersSet[idC].calibrationFiltersFilePath = K4SMPaths::grabbersCalibrationFilters[idC].string();
        }else if(std::filesystem::exists(K4SMPaths::defaultCalibrationFilters)){
            grabbersSet[idC].calibrationFiltersFilePath = K4SMPaths::defaultCalibrationFilters.string();
        }
        if(grabbersSet[idC].calibrationFiltersFilePath.length() > 0){
            grabbersSet[idC].calibrationFilters.init_from_file(grabbersSet[idC].calibrationFiltersFilePath);
        }else{
            Logger::error("No calibration filters file found.\n");
        }

        // read device settings file
        if(std::filesystem::exists(K4SMPaths::grabbersDevice[idC])){
            grabbersSet[idC].deviceFilePath = K4SMPaths::grabbersDevice[idC].string();
        }else if(std::filesystem::exists(K4SMPaths::defaultDevice)){
            grabbersSet[idC].deviceFilePath = K4SMPaths::defaultDevice.string();
        }
        if(grabbersSet[idC].deviceFilePath.length() > 0){
            grabbersSet[idC].device.init_from_file(grabbersSet[idC].deviceFilePath);
        }else{
            Logger::error("No device settings file found.\n");
        }

        // read color settings file
        if(std::filesystem::exists(K4SMPaths::grabbersColor[idC])){
            grabbersSet[idC].colorFilePath = K4SMPaths::grabbersColor[idC].string();
        }else if(std::filesystem::exists(K4SMPaths::defaultColor)){
            grabbersSet[idC].colorFilePath = K4SMPaths::defaultColor.string();
        }
        if(grabbersSet[idC].colorFilePath.length() > 0){
            grabbersSet[idC].color.init_from_file(grabbersSet[idC].colorFilePath);
        }else{
            Logger::error("No color settings file found.\n");
        }

        // read model file
        if(std::filesystem::exists(K4SMPaths::grabbersModel[idC])){
            grabbersSet[idC].modelFilePath = K4SMPaths::grabbersModel[idC].string();
        }
        if(grabbersSet[idC].modelFilePath.length() > 0){
            grabbersSet[idC].model.init_from_file(grabbersSet[idC].modelFilePath);
        }else{
            Logger::error("No model calibration file found.\n");
        }

        // display (TODO)
        grabbersSet[idC].cloudDisplay.cloudColor = cloudsColors[idC];

        ++idC;
    }

    // calibration
    for(size_t ii = 0; ii < globalSet.network.clientsInfo.size(); ++ii){
        globalSet.calibrator.models.push_back(fmt("[{}]", ii));
        globalSet.calibrator.sources.push_back(fmt("[{}]", ii));

    }
    if(globalSet.network.clientsInfo.size() > 1){
        globalSet.calibrator.sourceSelectionId = 1;
    }

    globalSet.calibrator.sources.push_back("All");

    return true;
}


auto K4SMSettings::update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<uint8_t> value) -> void{

    static_cast<void>(value);
    if(idB != 0 && idB != 1){
        return;
    }

    auto dRes = depth_resolution(grabbersSet[idC].device.configS.mode);
    for(const auto &idP : pencils[grabbersSet[idC].filters.idPencil]){
        auto pt = idP + pixel;
        if(pt.x() >= 0 && pt.x() < dRes.x() && pt.y() >= 0 && pt.y() < dRes.y()){

            auto id = pt.y()*dRes.x() + pt.x();
            if(globalSet.ui.settingsFiltersNormalSubPanelDisplayed){
                grabbersSet[idC].filters.depthMask.set(id, idB == 0);
            }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
                grabbersSet[idC].calibrationFilters.depthMask.set(id, idB == 0);
            }
        }
    }

    if(globalSet.ui.settingsFiltersNormalSubPanelDisplayed){
        K4SMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].filters);
    }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
        K4SMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].calibrationFilters);
    }
}

auto K4SMSettings::host_name() -> std::string{
    return Host::get_name();
}

auto K4SMSettings::process_settings_action(SAction action, STarget target, SType type, SFile file, size_t id) -> void{

    std::vector<K4GrabberSettings*> tGrabbers;
    if(target == STarget::All){
        for(auto &grabber : grabbersSet){
            tGrabbers.push_back(&grabber);
        }
    }else{
        tGrabbers.push_back(&grabbersSet[id]);
    }

    switch(action){
    case SAction::Reset:
        switch(type){
        case SType::Device:
            for(auto grabber : tGrabbers){
                grabber->device = camera::DCDeviceSettings();
            }
            break;
        case SType::Filters:
            for(auto grabber : tGrabbers){
                grabber->filters = camera::DCFilters();
            }
            break;
        case SType::CalibrationFilters:
            for(auto grabber : tGrabbers){
                grabber->calibrationFilters = camera::DCFilters::default_init_for_calibration();
            }
            break;
        case SType::Color:
            for(auto grabber : tGrabbers){
                grabber->color = camera::DCColorSettings();
            }
            break;
        case SType::Model:
            for(auto grabber : tGrabbers){
                grabber->model = camera::DCModel();
            }
            break;
        }
        break;
    case SAction::Load:
        switch(type){
        case SType::Device:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->device.init_from_file((file == SFile::Default ? K4SMPaths::defaultDevice : K4SMPaths::grabbersDevice[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allDevicesS;
                for(auto grabber : tGrabbers){
                    allDevicesS.push_back(&grabber->device);
                }
                files::BinaryFileSettings::init_from_file(allDevicesS, K4SMPaths::allGrabbersDevice.string());
            }
            break;
        case SType::Filters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->filters.init_from_file((file == SFile::Default ? K4SMPaths::defaultFilters : K4SMPaths::grabbersFilters[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allFilters;
                for(auto grabber : tGrabbers){
                    allFilters.push_back(&grabber->filters);
                }
                files::BinaryFileSettings::init_from_file(allFilters, K4SMPaths::allGrabbersFilters.string());
            }
            break;
        case SType::CalibrationFilters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->calibrationFilters.init_from_file((file == SFile::Default ? K4SMPaths::defaultCalibrationFilters : K4SMPaths::grabbersCalibrationFilters[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allCalibrationFilters;
                for(auto grabber : tGrabbers){
                    allCalibrationFilters.push_back(&grabber->calibrationFilters);
                }
                files::BinaryFileSettings::init_from_file(allCalibrationFilters, K4SMPaths::allGrabbersCalibrationFilters.string());
            }
            break;
        case SType::Color:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->color.init_from_file((file == SFile::Default ? K4SMPaths::defaultColor : K4SMPaths::grabbersColor[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allColors;
                for(auto grabber : tGrabbers){
                    allColors.push_back(&grabber->color);
                }
                files::BinaryFileSettings::init_from_file(allColors, K4SMPaths::allGrabbersColor.string());
            }
            break;
        case SType::Model:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    if(file == SFile::Default){
                        grabber->model = camera::DCModel();
                    }else{
                        grabber->model.init_from_file(K4SMPaths::grabbersModel[grabber->id].string());
                    }
                }
            }else{
                std::vector<files::TextSettings*> allModels;
                for(auto grabber : tGrabbers){
                    allModels.push_back(&grabber->model);
                }
                files::TextSettings::init_from_file(allModels, K4SMPaths::allGrabbersModel.string());
            }
            break;
        }
        break;
    case SAction::Save:
        switch(type){
        case SType::Device:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->device.save_to_file((file == SFile::Default ? K4SMPaths::defaultDevice : K4SMPaths::grabbersDevice[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allDevicesS;
                for(auto grabber : tGrabbers){
                    allDevicesS.push_back(&grabber->device);
                }
                files::BinaryFileSettings::save_to_file(allDevicesS, K4SMPaths::allGrabbersDevice.string());

            }
            break;
        case SType::Filters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->filters.save_to_file((file == SFile::Default ? K4SMPaths::defaultFilters : K4SMPaths::grabbersFilters[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allFilters;
                for(auto grabber : tGrabbers){
                    allFilters.push_back(&grabber->filters);
                }
                files::BinaryFileSettings::save_to_file(allFilters, K4SMPaths::allGrabbersFilters.string());
            }
            break;
        case SType::CalibrationFilters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->calibrationFilters.save_to_file((file == SFile::Default ? K4SMPaths::defaultCalibrationFilters : K4SMPaths::grabbersCalibrationFilters[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allCalibrationFilters;
                for(auto grabber : tGrabbers){
                    allCalibrationFilters.push_back(&grabber->calibrationFilters);
                }
                files::BinaryFileSettings::save_to_file(allCalibrationFilters, K4SMPaths::allGrabbersCalibrationFilters.string());
            }
            break;
        case SType::Color:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->color.save_to_file((file == SFile::Default ? K4SMPaths::defaultColor : K4SMPaths::grabbersColor[grabber->id]).string());
                }
            }else{
                std::vector<files::BinaryFileSettings*> allColors;
                for(auto grabber : tGrabbers){
                    allColors.push_back(&grabber->color);
                }
                files::BinaryFileSettings::save_to_file(allColors, K4SMPaths::allGrabbersColor.string());
            }
            break;
        case SType::Model:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    if(file == SFile::Host){
                        grabber->model.save_to_file(K4SMPaths::grabbersModel[grabber->id].string());
                    }
                }
            }else{
                std::vector<files::TextSettings*> allModels;
                for(auto grabber : tGrabbers){
                    allModels.push_back(&grabber->model);
                }
                files::TextSettings::save_to_file(allModels, K4SMPaths::allGrabbersModel.string());
            }
            break;
        }
        break;
    }
}

auto K4SMSettings::update_model(size_t id, const camera::DCModel &model) -> void {
    grabbersSet[id].model = model;
}

auto K4SMSettings::update_recorder_states(camera::DCRecorderStates recorderStates) -> void{
    globalSta.recorder = recorderStates;
}

auto K4SMSettings::update_player_states(camera::DCPlayerStates playerStates) -> void{
    globalSta.player = playerStates;
}

auto K4SMSettings::update_calibrator_states(camera::DCCalibratorStates calibratorStates) -> void{
    globalSta.calibrator = calibratorStates;
}

auto K4SMSettings::trigger_all_models() -> void {
    for(const auto &grabber : grabbersSet){
        tool::K4SMSignals::get()->update_model_settings_signal(grabber.id, grabber.model);
    }       
}

auto K4SMSettings::trigger_all_cloud_display() -> void {
    for(const auto &grabber : grabbersSet){
        tool::K4SMSignals::get()->update_cloud_display_settings_signal(grabber.id, grabber.cloudDisplay);
    }
}
