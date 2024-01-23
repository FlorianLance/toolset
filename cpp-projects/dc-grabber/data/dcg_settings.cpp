

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

#include "dcg_settings.hpp"

// base
#include "utility/logger.hpp"

// local
#include "dcg_signals.hpp"
#include "dcg_paths.hpp"

using namespace tool;
using namespace tool::net;

auto DCGSettings::initialize() -> bool{

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

    // network
    // # initialize
    if(!networkS.initialize()){
        return false;
    }
    // # read network config file
    if(std::filesystem::exists(DCGPaths::hostNetwork)){
        netWorkFilePath = DCGPaths::hostNetwork.string();
    }else if(std::filesystem::exists(DCGPaths::defaultNetwork)){
        Logger::warning("Cannot find current host network file, use default instead.\n");
        netWorkFilePath = DCGPaths::defaultNetwork.string();
    }
    if(netWorkFilePath.length() > 0){
        if(!networkS.init_from_file(netWorkFilePath)){
            return false;
        }
    }else{
        Logger::error("No network file found.\n");
        return false;
    }

    const auto &interfaces = (networkS.protocol == Protocol::ipv6) ? networkS.ipv6Interfaces : networkS.ipv4Interfaces;
    if(networkS.udpReadingInterfaceId > interfaces.size()){
        Logger::error(std::format("Cannot find {} interface with id [{}]. Abort initialization.\n",
        networkS.protocol == Protocol::ipv6 ? "ipv6" : "ipv4",
        networkS.udpReadingInterfaceId));
        return false;
    }

    Logger::message("Local interfaces available:\n");
    for(const auto &interface : interfaces){
        Logger::message(" -> " + interface.ipAddress + "\n");
    }
    Logger::message(std::format("UDP reading interface id: [{}]\n", networkS.udpReadingInterfaceId));
    Logger::message(std::format("UDP reading port [{}]\n", networkS.udpReadingPort));


    // read filters settings file
    if(std::filesystem::exists(DCGPaths::hostFilters)){
        filtersFilePath = DCGPaths::hostFilters.string();
    }else if(std::filesystem::exists(DCGPaths::defaultFilters)){
        filtersFilePath = DCGPaths::defaultFilters.string();
    }
    if(filtersFilePath.length() > 0){
        filtersS.init_from_file(filtersFilePath);
    }else{
        Logger::error("No filters file found.\n");
    }

    // read device settings file
    if(std::filesystem::exists(DCGPaths::hostDevice)){
        deviceFilePath = DCGPaths::hostDevice.string();
    }else if(std::filesystem::exists(DCGPaths::defaultDevice)){
        deviceFilePath = DCGPaths::defaultDevice.string();
    }
    if(deviceFilePath.length() > 0){
        deviceS.init_from_file(deviceFilePath);
    }else{
        Logger::error("No device settings file found.\n");
    }

    // read color settings file
    if(std::filesystem::exists(DCGPaths::hostColor)){
        colorFilePath = DCGPaths::hostColor.string();
    }else if(std::filesystem::exists(DCGPaths::defaultColor)){
        colorFilePath = DCGPaths::defaultColor.string();
    }
    if(colorFilePath.length() > 0){
        colorS.init_from_file(colorFilePath);
    }else{
        Logger::error("No color settings file found.\n");
    }

    // read model settings file
    if(std::filesystem::exists(DCGPaths::hostModel)){
        modelFilePath = DCGPaths::hostModel.string();
    }else if(std::filesystem::exists(DCGPaths::defaultModel)){
        modelFilePath = DCGPaths::defaultModel.string();
    }
    if(modelFilePath.length() > 0){
        modelS.init_from_file(modelFilePath);
    }else{
        Logger::error("No calibration model file found.\n");
    }

    // device.configS.idDevice = idLocalGrabber;

    return true;
}

auto DCGSettings::init_network_sending_settings(std::shared_ptr<net::UdpNetworkSendingSettings> networkSendingS) -> void{    
    networkS.init_sending_settings(*networkSendingS);
    triggers_init_network_sending_settings();
}

auto DCGSettings::update_filters(std::shared_ptr<cam::DCFiltersSettings> filtersS) -> void {
    this->filtersS = *filtersS;
    triggers_filters_settings();
}

auto DCGSettings::update_device_settings(std::shared_ptr<net::UdpMonoPacketMessage<cam::DCDeviceSettings>> deviceS) -> void {
    this->deviceS = std::move(deviceS->data);
    triggers_device_settings();
}

#include <iostream>
auto DCGSettings::update_color_settings(std::shared_ptr<net::UdpMonoPacketMessage<cam::DCColorSettings>> colorS) -> void{
    std::cout << "DCGSettings::update_color_settings\n";
    this->colorS = std::move(colorS->data);
    triggers_color_settings();
}

auto DCGSettings::update_color_settings_from_device_manager(const cam::DCColorSettings &colorS) -> void{
    this->colorS = colorS;
}

auto DCGSettings::update_delay(net::UdpMonoPacketMessage<cam::DCDelaySettings> delayS) -> void{
    this->delayS = std::move(delayS.data);
    triggers_delay_settings();
}



// auto DCGSettings::update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<uint8_t> value) -> void{

//     static_cast<void>(value);
//     if(idC != 0 || (idB != 0 && idB != 1) || !ui.settingsFiltersSubPanelDisplayed){
//         return;
//     }

//     auto dRes = depth_resolution(device.configS.mode);
//     auto dW   = depth_width(dRes);
//     auto dH   = depth_height(dRes);
//     for(const auto &idP : pencils[filters.idPencil]){
//         auto pt = idP + pixel;
//         if(pt.x() >= 0 && pt.x() < dW && pt.y() >= 0 && pt.y() < dH){
//             auto id = pt.y()*dW + pt.x();
//             filters.depthMask.set(id, idB == 0);
//         }
//     }

//     triggers_filters_settings();
// }

auto DCGSettings::update_imu_sample(cam::DCImuSample imuSample) -> void{
    this->imuSample = imuSample;
}

auto DCGSettings::triggers_init_network_sending_settings() -> void {
    DCGSignals::get()->init_network_sending_settings_signal(&networkS);
}

auto DCGSettings::triggers_filters_settings() -> void {
    DCGSignals::get()->update_filters_signal(filtersS);
}

auto DCGSettings::triggers_device_settings() -> void {
    DCGSignals::get()->update_device_settings_signal(deviceS);
}

#include <iostream>
auto DCGSettings::triggers_color_settings() -> void{
    std::cout << "triggers_color_settings\n";
    DCGSignals::get()->update_color_settings_signal(colorS);
}

auto DCGSettings::triggers_display_settings() -> void {
    DCGSignals::get()->update_cloud_display_settings_signal(0, displayS.cloudDisplay);
}

auto DCGSettings::triggers_model() -> void {
    DCGSignals::get()->update_model_signal(0, modelS);
}

auto DCGSettings::triggers_delay_settings() -> void {
    DCGSignals::get()->update_delay_signal(delayS);
}

auto DCGSettings::disconnect() -> void{
    networkS.disconnect_from_manager();
}

auto DCGSettings::reset_device_settings() -> void{
    deviceS = cam::DCDeviceSettings::default_init_for_grabber();
    triggers_device_settings();
}

auto DCGSettings::save_device_settings_to_default_file() -> bool{
    return deviceS.save_to_file(DCGPaths::defaultDevice.string());
}

auto DCGSettings::save_device_settings_to_current_hostname_file() -> bool{
    return deviceS.save_to_file(DCGPaths::hostDevice.string());
}

auto DCGSettings::load_default_device_settings_file() -> bool{
    if(deviceS.init_from_file(DCGPaths::defaultDevice.string())){
        triggers_device_settings();
        return true;
    }
    return false;
}

auto DCGSettings::load_current_hostname_device_settings_file() -> bool{
    if(deviceS.init_from_file(DCGPaths::hostDevice.string())){
        triggers_device_settings();
        return true;
    }
    return false;
}

auto DCGSettings::reset_filters() -> void{
    filtersS = cam::DCFiltersSettings();
    triggers_device_settings();
}

auto DCGSettings::save_filters_to_default_file() -> bool{
    return filtersS.save_to_file(DCGPaths::defaultFilters.string());
}

auto DCGSettings::save_filters_to_current_hostname_file() -> bool{
    return filtersS.save_to_file(DCGPaths::hostFilters.string());
}

auto DCGSettings::load_default_filters_file() -> bool{
    if(filtersS.init_from_file(DCGPaths::defaultFilters.string())){
        triggers_filters_settings();
        return true;
    }
    return false;
}

auto DCGSettings::load_current_hostname_filters_file() -> bool{
    if(filtersS.init_from_file(DCGPaths::hostFilters.string())){
        triggers_filters_settings();
        return true;
    }
    return false;
}

auto DCGSettings::reset_color_settings() -> void{
    colorS = cam::DCColorSettings();
    triggers_color_settings();
}

auto DCGSettings::save_color_settings_to_default_file() -> bool{
    return colorS.save_to_file(DCGPaths::defaultColor.string());
}

auto DCGSettings::save_color_settings_to_current_hostname_file() -> bool{
    return colorS.save_to_file(DCGPaths::hostColor.string());
}

auto DCGSettings::load_default_settings_color_file() -> bool{
    if(colorS.init_from_file(DCGPaths::defaultColor.string())){
        triggers_color_settings();
        return true;
    }
    return false;
}

auto DCGSettings::load_current_hostname_color_settings_file() -> bool{
    if(colorS.init_from_file(DCGPaths::hostColor.string())){
        triggers_color_settings();
        return true;
    }
    return false;
}

auto DCGSettings::reset_model() -> void{
    modelS = cam::DCModelSettings();
    triggers_model();
}

auto DCGSettings::save_model_to_default_file() -> bool{
    return modelS.save_to_file(DCGPaths::defaultModel.string());
}

auto DCGSettings::save_model_to_current_hostname_file() -> bool{
    return modelS.save_to_file(DCGPaths::hostModel.string());
}

auto DCGSettings::load_default_model_file() -> bool{
    if(modelS.init_from_file(DCGPaths::defaultModel.string())){
        triggers_model();
        return true;
    }
    return false;
}

auto DCGSettings::load_current_hostname_model_file() -> bool{
    if(modelS.init_from_file(DCGPaths::hostModel.string())){
        triggers_model();
        return true;
    }
    return false;
}



auto DCGSettings::host_name() -> std::string{
    return Host::get_name();
}

