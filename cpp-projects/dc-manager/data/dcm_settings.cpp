
    // /*******************************************************************************
    // ** Toolset-dc-manager                                                         **
    // ** MIT License                                                                **
    // ** Copyright (c) [2018] [Florian Lance]                                       **
    // **                                                                            **
    // ** Permission is hereby granted, free of charge, to any person obtaining a    **
    // ** copy of this software and associated documentation files (the "Software"), **
    // ** to deal in the Software without restriction, including without limitation  **
    // ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
    // ** and/or sell copies of the Software, and to permit persons to whom the      **
    // ** Software is furnished to do so, subject to the following conditions:       **
    // **                                                                            **
    // ** The above copyright notice and this permission notice shall be included in **
    // ** all copies or substantial portions of the Software.                        **
    // **                                                                            **
    // ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
    // ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
    // ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
    // ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
    // ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
    // ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
    // ** DEALINGS IN THE SOFTWARE.                                                  **
    // **                                                                            **
    // ********************************************************************************/

    // #include "dcm_settings.hpp"

    // // std
    // #include <format>
    // #include <filesystem>
    // #include <fstream>

    // // base
    // #include "utility/logger.hpp"
    // #include "utility/string.hpp"
    // #include "utility/format.hpp"
    // #include "depth-camera/settings/dc_settings_paths.hpp"

    // // local
    // #include "dcm_signals.hpp"

    // using namespace tool;
    // using namespace tool::net;
    // using namespace tool::cam;


    // // auto DCMSettings::initialize() -> bool{

    // //     // // init pencils
    // //     // pencils.resize(3);
    // //     // int ray = 10;
    // //     // for(int offR = -ray; offR < ray; ++offR){
    // //     //     for(int offC = -ray; offC < ray; ++offC){
    // //     //         if(offR*offR + offC*offC < ray*ray){
    // //     //             pencils[0].emplace_back(offR, offC);
    // //     //         }
    // //     //     }
    // //     // }
    // //     // ray = 40;
    // //     // for(int offR = -ray; offR < ray; ++offR){
    // //     //     for(int offC = -ray; offC < ray; ++offC){
    // //     //         if(offR*offR + offC*offC < ray*ray){
    // //     //             pencils[1].emplace_back(offR, offC);
    // //     //         }
    // //     //     }
    // //     // }
    // //     // ray = 100;
    // //     // for(int offR = -ray; offR < ray; ++offR){
    // //     //     for(int offC = -ray; offC < ray; ++offC){
    // //     //         if(offR*offR + offC*offC < ray*ray){
    // //     //             pencils[2].emplace_back(offR, offC);
    // //     //         }
    // //     //     }
    // //     // }

    // //     // // init network
    // //     // if(!devicesConnectionsS.initialize(DCSettingsPaths::get_network_settings_file_path())){
    // //     //     devicesConnectionsS.default_initialize();
    // //     // }

    // //     // // init grabbers paths
    // //     // DCSettingsPaths::initialize_grabbers(devicesConnectionsS.connectionsS.size());

    // //     // init grabbers settings
    // //     grabbersS.resize(devicesConnectionsS.connectionsS.size());
    // //     for(size_t idC = 0; idC < devicesConnectionsS.connectionsS.size(); ++idC){
    // //         grabbersS[idC].initialize(idC);
    // //     }

    // //     // calibration
    // //     calibratorS.initialize(devicesConnectionsS.connectionsS.size());

    // //     return true;
    // // }


    // // auto DCMSettings::host_name() -> std::string{
    // //     return Host::get_name();
    // // }

    // // auto DCMSettings::process_settings_action(SAction action, STarget target, SType type, SFile file, size_t id) -> void{


    // // }





    // // auto DCMSettings::update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<uint8_t> value) -> void{

    // //     static_cast<void>(value);
    // //     if(idB != 0 && idB != 1){
    // //         return;
    // //     }

    // //     auto dRes = depth_resolution(grabbersSet[idC].device.configS.mode);
    // //     auto dW   = depth_width(dRes);
    // //     auto dH   = depth_height(dRes);
    // //     for(const auto &idP : pencils[grabbersSet[idC].filters.idPencil]){
    // //         auto pt = idP + pixel;
    // //         if(pt.x() >= 0 && pt.x() < dW && pt.y() >= 0 && pt.y() < dH){

    // //             auto id = pt.y()*dW + pt.x();
    // //             if(globalSet.ui.settingsFiltersNormalSubPanelDisplayed){
    // //                 grabbersSet[idC].filters.depthMask.set(id, idB == 0);
    // //             }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
    // //                 grabbersSet[idC].calibrationFilters.depthMask.set(id, idB == 0);
    // //             }
    // //         }
    // //     }

    // //     if(globalSet.ui.settingsFiltersNormalSubPanelDisplayed){
    // //         DCMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].filters);
    // //     }else if(globalSet.ui.settingsFiltersCalibrationSubPanelDisplayed){
    // //         DCMSignals::get()->update_filters_settings_signal(idC, grabbersSet[idC].calibrationFilters);
    // //     }
    // // }
    // )
