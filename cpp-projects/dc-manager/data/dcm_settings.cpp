
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

#include "dcm_settings.hpp"

// std
#include <format>
#include <filesystem>
#include <fstream>

// base
#include "utility/logger.hpp"
#include "utility/string.hpp"
#include "utility/format.hpp"

// local
#include "dcm_signals.hpp"
#include "data/dcm_paths.hpp"

using namespace tool;
using namespace tool::network;


auto DCMSettings::initialize() -> bool{

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
    if(std::filesystem::exists(DCMPaths::hostNetwork)){
        globalSet.networkFilePath = DCMPaths::hostNetwork.string();
    }else if(std::filesystem::exists(DCMPaths::defaultNetwork)){
        Logger::warning("Cannot find current host network file, use default instead.\n");
        globalSet.networkFilePath = DCMPaths::defaultNetwork.string();
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
    DCMPaths::initialize_grabbers(globalSet.network.clientsInfo.size());

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
        if(std::filesystem::exists(DCMPaths::grabbersFilters[idC])){
            grabbersSet[idC].filtersFilePath = DCMPaths::grabbersFilters[idC].string();
        }else if(std::filesystem::exists(DCMPaths::defaultFilters)){
            grabbersSet[idC].filtersFilePath = DCMPaths::defaultFilters.string();
        }
        if(grabbersSet[idC].filtersFilePath.length() > 0){
            grabbersSet[idC].filters.init_from_file(grabbersSet[idC].filtersFilePath);
        }else{
            Logger::error("No filters file found.\n");
        }

        // read calibration filters settings file
        if(std::filesystem::exists(DCMPaths::grabbersCalibrationFilters[idC])){
            grabbersSet[idC].calibrationFiltersFilePath = DCMPaths::grabbersCalibrationFilters[idC].string();
        }else if(std::filesystem::exists(DCMPaths::defaultCalibrationFilters)){
            grabbersSet[idC].calibrationFiltersFilePath = DCMPaths::defaultCalibrationFilters.string();
        }
        if(grabbersSet[idC].calibrationFiltersFilePath.length() > 0){
            grabbersSet[idC].calibrationFilters.init_from_file(grabbersSet[idC].calibrationFiltersFilePath);
        }else{
            Logger::error("No calibration filters file found.\n");
        }

        // read device settings file
        if(std::filesystem::exists(DCMPaths::grabbersDevice[idC])){
            grabbersSet[idC].deviceFilePath = DCMPaths::grabbersDevice[idC].string();
        }else if(std::filesystem::exists(DCMPaths::defaultDevice)){
            grabbersSet[idC].deviceFilePath = DCMPaths::defaultDevice.string();
        }
        if(grabbersSet[idC].deviceFilePath.length() > 0){
            grabbersSet[idC].device.init_from_file(grabbersSet[idC].deviceFilePath);
        }else{
            Logger::error("No device settings file found.\n");
        }

        // read color settings file
        if(std::filesystem::exists(DCMPaths::grabbersColor[idC])){
            grabbersSet[idC].colorFilePath = DCMPaths::grabbersColor[idC].string();
        }else if(std::filesystem::exists(DCMPaths::defaultColor)){
            grabbersSet[idC].colorFilePath = DCMPaths::defaultColor.string();
        }
        if(grabbersSet[idC].colorFilePath.length() > 0){
            grabbersSet[idC].color.init_from_file(grabbersSet[idC].colorFilePath);
        }else{
            Logger::error("No color settings file found.\n");
        }

        // read model file
        if(std::filesystem::exists(DCMPaths::grabbersModel[idC])){
            grabbersSet[idC].modelFilePath = DCMPaths::grabbersModel[idC].string();
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

auto DCMSettings::update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<uint8_t> value) -> void{

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
        DCMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].filters);
    }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
        DCMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].calibrationFilters);
    }
}

auto DCMSettings::host_name() -> std::string{
    return Host::get_name();
}

auto DCMSettings::process_settings_action(SAction action, STarget target, SType type, SFile file, size_t id) -> void{

    std::vector<DCMGrabberSettings*> tGrabbers;
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
                grabber->filters = camera::DCFiltersSettings();
            }
            break;
        case SType::CalibrationFilters:
            for(auto grabber : tGrabbers){
                grabber->calibrationFilters = camera::DCFiltersSettings::default_init_for_calibration();
            }
            break;
        case SType::Color:
            for(auto grabber : tGrabbers){
                grabber->color = camera::DCColorSettings();
            }
            break;
        case SType::Model:
            for(auto grabber : tGrabbers){
                grabber->model = camera::DCModelSettings();
            }
            break;
        }
        break;
    case SAction::Load:
        switch(type){
        case SType::Device:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->device.init_from_file((file == SFile::Default ? DCMPaths::defaultDevice : DCMPaths::grabbersDevice[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allDevicesS;
                for(auto grabber : tGrabbers){
                    allDevicesS.push_back(&grabber->device);
                }
                io::BinaryFileSettings::init_from_file(allDevicesS, DCMPaths::allGrabbersDevice.string());
            }
            break;
        case SType::Filters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->filters.init_from_file((file == SFile::Default ? DCMPaths::defaultFilters : DCMPaths::grabbersFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allFilters;
                for(auto grabber : tGrabbers){
                    allFilters.push_back(&grabber->filters);
                }
                io::BinaryFileSettings::init_from_file(allFilters, DCMPaths::allGrabbersFilters.string());
            }
            break;
        case SType::CalibrationFilters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->calibrationFilters.init_from_file((file == SFile::Default ? DCMPaths::defaultCalibrationFilters : DCMPaths::grabbersCalibrationFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allCalibrationFilters;
                for(auto grabber : tGrabbers){
                    allCalibrationFilters.push_back(&grabber->calibrationFilters);
                }
                io::BinaryFileSettings::init_from_file(allCalibrationFilters, DCMPaths::allGrabbersCalibrationFilters.string());
            }
            break;
        case SType::Color:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->color.init_from_file((file == SFile::Default ? DCMPaths::defaultColor : DCMPaths::grabbersColor[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allColors;
                for(auto grabber : tGrabbers){
                    allColors.push_back(&grabber->color);
                }
                io::BinaryFileSettings::init_from_file(allColors, DCMPaths::allGrabbersColor.string());
            }
            break;
        case SType::Model:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    if(file == SFile::Default){
                        grabber->model = camera::DCModelSettings();
                    }else{
                        grabber->model.init_from_file(DCMPaths::grabbersModel[grabber->id].string());
                    }
                }
            }else{
                std::vector<io::TextSettings*> allModels;
                for(auto grabber : tGrabbers){
                    allModels.push_back(&grabber->model);
                }
                io::TextSettings::init_from_file(allModels, DCMPaths::allGrabbersModel.string());
            }
            break;
        }
        break;
    case SAction::Save:
        switch(type){
        case SType::Device:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->device.save_to_file((file == SFile::Default ? DCMPaths::defaultDevice : DCMPaths::grabbersDevice[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allDevicesS;
                for(auto grabber : tGrabbers){
                    allDevicesS.push_back(&grabber->device);
                }
                io::BinaryFileSettings::save_to_file(allDevicesS, DCMPaths::allGrabbersDevice.string());

            }
            break;
        case SType::Filters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->filters.save_to_file((file == SFile::Default ? DCMPaths::defaultFilters : DCMPaths::grabbersFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allFilters;
                for(auto grabber : tGrabbers){
                    allFilters.push_back(&grabber->filters);
                }
                io::BinaryFileSettings::save_to_file(allFilters, DCMPaths::allGrabbersFilters.string());
            }
            break;
        case SType::CalibrationFilters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->calibrationFilters.save_to_file((file == SFile::Default ? DCMPaths::defaultCalibrationFilters : DCMPaths::grabbersCalibrationFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allCalibrationFilters;
                for(auto grabber : tGrabbers){
                    allCalibrationFilters.push_back(&grabber->calibrationFilters);
                }
                io::BinaryFileSettings::save_to_file(allCalibrationFilters, DCMPaths::allGrabbersCalibrationFilters.string());
            }
            break;
        case SType::Color:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->color.save_to_file((file == SFile::Default ? DCMPaths::defaultColor : DCMPaths::grabbersColor[grabber->id]).string());
                }
            }else{
                std::vector<io::BinaryFileSettings*> allColors;
                for(auto grabber : tGrabbers){
                    allColors.push_back(&grabber->color);
                }
                io::BinaryFileSettings::save_to_file(allColors, DCMPaths::allGrabbersColor.string());
            }
            break;
        case SType::Model:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    if(file == SFile::Host){
                        grabber->model.save_to_file(DCMPaths::grabbersModel[grabber->id].string());
                    }
                }
            }else{
                std::vector<io::TextSettings*> allModels;
                for(auto grabber : tGrabbers){
                    allModels.push_back(&grabber->model);
                }
                io::TextSettings::save_to_file(allModels, DCMPaths::allGrabbersModel.string());
            }
            break;
        }
        break;
    }
}

auto DCMSettings::update_model(size_t id, const camera::DCModelSettings &model) -> void {
    grabbersSet[id].model = model;
}

auto DCMSettings::update_recorder_states(camera::DCRecorderStates recorderStates) -> void{
    globalSta.recorder = recorderStates;
}

auto DCMSettings::update_player_states(camera::DCPlayerStates playerStates) -> void{
    globalSta.player = playerStates;
}

auto DCMSettings::update_calibrator_states(camera::DCCalibratorStates calibratorStates) -> void{
    globalSta.calibrator = calibratorStates;
}

auto DCMSettings::trigger_all_models() -> void {
    for(const auto &grabber : grabbersSet){
        tool::DCMSignals::get()->update_model_settings_signal(grabber.id, grabber.model);
    }       
}

auto DCMSettings::trigger_all_cloud_display() -> void {
    for(const auto &grabber : grabbersSet){
        tool::DCMSignals::get()->update_cloud_display_settings_signal(grabber.id, grabber.cloudDisplay);
    }
}
