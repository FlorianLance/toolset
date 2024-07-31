
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
#include "depth-camera/settings/dc_settings_paths.hpp"

// local
#include "dcm_signals.hpp"

using namespace tool;
using namespace tool::net;
using namespace tool::cam;


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
    if(!devicesConnectionsS.initialize(DCSettingsPaths::get_network_settings_file_path())){
        devicesConnectionsS.default_initialize();
    }

    // init grabbers paths
    DCSettingsPaths::initialize_grabbers(devicesConnectionsS.connectionsS.size());

    // init grabbers settings
    grabbersS.resize(devicesConnectionsS.connectionsS.size());
    for(size_t idC = 0; idC < devicesConnectionsS.connectionsS.size(); ++idC){
        grabbersS[idC].initialize(idC);
    }

    // calibration
    calibratorS.initialize(devicesConnectionsS.connectionsS.size());

    return true;
}


auto DCMSettings::host_name() -> std::string{
    return Host::get_name();
}

auto DCMSettings::process_settings_action(SAction action, STarget target, SType type, SFile file, size_t id) -> void{
    
    std::vector<DCGrabberSettings*> tGrabbers;
    if(target == STarget::All){
        for(auto &grabber : grabbersS){
            tGrabbers.push_back(&grabber);
        }
    }else if (target == STarget::Individual){
        tGrabbers.push_back(&grabbersS[id]);
    }

    switch(action){
    case SAction::Reset:
        switch(type){
        case SType::Device:
            for(auto grabber : tGrabbers){
                grabber->device = DCDeviceSettings();
            }
            break;
        case SType::Filters:
            for(auto grabber : tGrabbers){
                grabber->filters = DCFiltersSettings();
            }
            break;
        case SType::CalibrationFilters:
            for(auto grabber : tGrabbers){
                grabber->calibrationFilters = DCFiltersSettings::default_init_for_calibration();
            }
            break;
        case SType::Color:
            for(auto grabber : tGrabbers){
                grabber->color = DCColorSettings();
            }
            break;
        case SType::Model:
            for(auto grabber : tGrabbers){
                grabber->model = DCModelSettings();
            }
            break;
        case SType::Network:
            break;
        }                
        break;
    case SAction::Load:
        switch(type){
        case SType::Device:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->device.load_from_file((file == SFile::Default ? DCSettingsPaths::defaultDevice : DCSettingsPaths::grabbersDevice[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allDevicesS;
                for(auto grabber : tGrabbers){
                    allDevicesS.push_back(&grabber->device);
                }
                io::Settings::load_multi_from_file(allDevicesS, DCSettingsPaths::allGrabbersDevice.string());
            }
            break;
        case SType::Filters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->filters.load_from_file((file == SFile::Default ? DCSettingsPaths::defaultFilters : DCSettingsPaths::grabbersFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allFilters;
                for(auto grabber : tGrabbers){
                    allFilters.push_back(&grabber->filters);
                }
                io::Settings::load_multi_from_file(allFilters, DCSettingsPaths::allGrabbersFilters.string());
            }
            break;
        case SType::CalibrationFilters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->calibrationFilters.load_from_file((file == SFile::Default ? DCSettingsPaths::defaultCalibrationFilters : DCSettingsPaths::grabbersCalibrationFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allCalibrationFilters;
                for(auto grabber : tGrabbers){
                    allCalibrationFilters.push_back(&grabber->calibrationFilters);
                }
                io::Settings::load_multi_from_file(allCalibrationFilters, DCSettingsPaths::allGrabbersCalibrationFilters.string());
            }
            break;
        case SType::Color:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->color.load_from_file((file == SFile::Default ? DCSettingsPaths::defaultColor : DCSettingsPaths::grabbersColor[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allColors;
                for(auto grabber : tGrabbers){
                    allColors.push_back(&grabber->color);
                }
                io::Settings::load_multi_from_file(allColors, DCSettingsPaths::allGrabbersColor.string());
            }
            break;
        case SType::Model:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    if(file == SFile::Default){
                        grabber->model = DCModelSettings();
                    }else{
                        grabber->model.load_from_file(DCSettingsPaths::grabbersModel[grabber->id].string());
                    }
                }
            }else{
                std::vector<io::Settings*> allModels;
                for(auto grabber : tGrabbers){
                    allModels.push_back(&grabber->model);
                }
                io::Settings::load_multi_from_file(allModels, DCSettingsPaths::allGrabbersModel.string());
            }
            break;
        case SType::Network:
            break;
        }
        break;
    case SAction::Save:
        switch(type){
        case SType::Device:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->device.save_to_json_str_file((file == SFile::Default ? DCSettingsPaths::defaultDevice : DCSettingsPaths::grabbersDevice[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allDevicesS;
                for(auto grabber : tGrabbers){
                    allDevicesS.push_back(&grabber->device);
                }
                io::Settings::save_multi_to_json_str_file(allDevicesS, DCSettingsPaths::allGrabbersDevice.string());

            }
            break;
        case SType::Filters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->filters.save_to_json_str_file((file == SFile::Default ? DCSettingsPaths::defaultFilters : DCSettingsPaths::grabbersFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allFilters;
                for(auto grabber : tGrabbers){
                    allFilters.push_back(&grabber->filters);
                }
                io::Settings::save_multi_to_json_str_file(allFilters, DCSettingsPaths::allGrabbersFilters.string());
            }
            break;
        case SType::CalibrationFilters:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->calibrationFilters.save_to_json_str_file((file == SFile::Default ? DCSettingsPaths::defaultCalibrationFilters : DCSettingsPaths::grabbersCalibrationFilters[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allCalibrationFilters;
                for(auto grabber : tGrabbers){
                    allCalibrationFilters.push_back(&grabber->calibrationFilters);
                }
                io::Settings::save_multi_to_json_str_file(allCalibrationFilters, DCSettingsPaths::allGrabbersCalibrationFilters.string());
            }
            break;
        case SType::Color:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    grabber->color.save_to_json_str_file((file == SFile::Default ? DCSettingsPaths::defaultColor : DCSettingsPaths::grabbersColor[grabber->id]).string());
                }
            }else{
                std::vector<io::Settings*> allColors;
                for(auto grabber : tGrabbers){
                    allColors.push_back(&grabber->color);
                }
                io::Settings::save_multi_to_json_str_file(allColors, DCSettingsPaths::allGrabbersColor.string());
            }
            break;
        case SType::Model:
            if(file != SFile::HostAllInOne){
                for(auto grabber : tGrabbers){
                    if(file == SFile::Host){
                        grabber->model.save_to_json_str_file(DCSettingsPaths::grabbersModel[grabber->id].string());
                    }
                }
            }else{
                std::vector<io::Settings*> allModels;
                for(auto grabber : tGrabbers){
                    allModels.push_back(&grabber->model);
                }
                io::Settings::save_multi_to_json_str_file(allModels, DCSettingsPaths::allGrabbersModel.string());
            }
            break;
        case SType::Network:
            devicesConnectionsS.save_to_json_str_file(DCSettingsPaths::hostNetwork.string());
            break;
        }
        break;
    }
}

auto DCMSettings::update_model(size_t id, const DCModelSettings &model) -> void {
    grabbersS[id].model = model;
}


auto DCMSettings::update_color_settings_from_device(size_t id, const DCColorSettings &color) -> void{
    grabbersS[id].color = color;
}

auto DCMSettings::trigger_all_models() -> void {
    for(const auto &grabber : grabbersS){
        tool::DCMSignals::get()->update_model_settings_signal(grabber.id, grabber.model);
    }       
}

auto DCMSettings::trigger_all_cloud_display() -> void {
    for(const auto &grabber : grabbersS){
        tool::DCMSignals::get()->update_cloud_display_settings_signal(grabber.id, grabber.cloudDisplay);
    }
}




// auto DCMSettings::update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<uint8_t> value) -> void{

//     static_cast<void>(value);
//     if(idB != 0 && idB != 1){
//         return;
//     }

//     auto dRes = depth_resolution(grabbersSet[idC].device.configS.mode);
//     auto dW   = depth_width(dRes);
//     auto dH   = depth_height(dRes);
//     for(const auto &idP : pencils[grabbersSet[idC].filters.idPencil]){
//         auto pt = idP + pixel;
//         if(pt.x() >= 0 && pt.x() < dW && pt.y() >= 0 && pt.y() < dH){

//             auto id = pt.y()*dW + pt.x();
//             if(globalSet.ui.settingsFiltersNormalSubPanelDisplayed){
//                 grabbersSet[idC].filters.depthMask.set(id, idB == 0);
//             }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
//                 grabbersSet[idC].calibrationFilters.depthMask.set(id, idB == 0);
//             }
//         }
//     }

//     if(globalSet.ui.settingsFiltersNormalSubPanelDisplayed){
//         DCMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].filters);
//     }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
//         DCMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].calibrationFilters);
//     }
// }
