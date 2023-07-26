

/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

#include "k4sg_settings.hpp"

// base
#include "utility/logger.hpp"

// local
#include "k4sg_signals.hpp"
#include "k4sg_paths.hpp"

using namespace tool;
using namespace tool::network;

auto K4SGSettings::initialize() -> bool{

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
    if(!network.initialize()){
        return false;
    }
    // # read network config file
    if(std::filesystem::exists(K4SGPaths::hostNetwork)){
        netWorkFilePath = K4SGPaths::hostNetwork.string();
    }else if(std::filesystem::exists(K4SGPaths::defaultNetwork)){
        Logger::warning("Cannot find current host network file, use default instead.\n");
        netWorkFilePath = K4SGPaths::defaultNetwork.string();
    }
    if(netWorkFilePath.length() > 0){
        if(!network.init_from_file(netWorkFilePath)){
            return false;
        }
    }else{
        Logger::error("No network file found.\n");
        return false;
    }


    if(network.udpReadingInterfaceId > network.interfaces.size()){
        Logger::error(std::format("Cannot find ipv4 interface with id [{}]. Abort initialization.\n", network.udpReadingInterfaceId));
        return false;
    }

    Logger::message("Local interfaces available:\n");
    for(const auto &interface : network.interfaces){
        Logger::message(" -> " + interface.ipAddress + "\n");
    }
    Logger::message(std::format("UDP reading interface id: [{}]\n", network.udpReadingInterfaceId));
    Logger::message(std::format("UDP reading port [{}]\n", network.udpReadingPort));


    // read filters settings file
    if(std::filesystem::exists(K4SGPaths::hostFilters)){
        filtersFilePath = K4SGPaths::hostFilters.string();
    }else if(std::filesystem::exists(K4SGPaths::defaultFilters)){
        filtersFilePath = K4SGPaths::defaultFilters.string();
    }
    if(filtersFilePath.length() > 0){
        filters.init_from_file(filtersFilePath);
    }else{
        Logger::error("No filters file found.\n");
    }

    // read device settings file
    if(std::filesystem::exists(K4SGPaths::hostDevice)){
        deviceFilePath = K4SGPaths::hostDevice.string();
    }else if(std::filesystem::exists(K4SGPaths::defaultDevice)){
        deviceFilePath = K4SGPaths::defaultDevice.string();
    }
    if(deviceFilePath.length() > 0){
        device.init_from_file(deviceFilePath);
    }else{
        Logger::error("No device settings file found.\n");
    }

    // read color settings file
    if(std::filesystem::exists(K4SGPaths::hostColor)){
        colorFilePath = K4SGPaths::hostColor.string();
    }else if(std::filesystem::exists(K4SGPaths::defaultColor)){
        colorFilePath = K4SGPaths::defaultColor.string();
    }
    if(colorFilePath.length() > 0){
        color.init_from_file(colorFilePath);
    }else{
        Logger::error("No color settings file found.\n");
    }

    // read model settings file
    if(std::filesystem::exists(K4SGPaths::hostModel)){
        modelFilePath = K4SGPaths::hostModel.string();
    }else if(std::filesystem::exists(K4SGPaths::defaultModel)){
        modelFilePath = K4SGPaths::defaultModel.string();
    }
    if(modelFilePath.length() > 0){
        model.init_from_file(modelFilePath);
    }else{
        Logger::error("No calibration model file found.\n");
    }

    device.configS.idDevice = idLocalGrabber;

    return true;
}

auto K4SGSettings::init_network_sending_settings(std::shared_ptr<network::K4NetworkSendingSettings> networkSendingS) -> void{
    network.init_sending_settings(*networkSendingS);
    triggers_init_network_sending_settings();
}

auto K4SGSettings::update_filters(std::shared_ptr<camera::K4Filters> filtersS) -> void {
    filters = *filtersS;
    triggers_filters_settings();
}

auto K4SGSettings::update_device_settings(std::shared_ptr<network::K4UdpDeviceSettings> deviceS) -> void {
    device = std::move(deviceS->data);
    triggers_device_settings();
}

auto K4SGSettings::update_color_settings(std::shared_ptr<network::K4UdpColorSettings> colorS) -> void{
    color = std::move(colorS->data);
    triggers_color_settings();
}

auto K4SGSettings::update_delay(network::K4UdpDelay delayS) -> void{
    delay = std::move(delayS.data);
    triggers_delay_settings();
}

auto K4SGSettings::update_device_name(int id, const std::string &name) -> void {

    if(id < 0){
        devicesNames = {};
        return;
    }

    if(id >= static_cast<int>(devicesNames.size())){
        devicesNames.resize(id + 1 );
    }
    devicesNames[id] = name;
}

auto K4SGSettings::update_filters_depth_mask(size_t idC, size_t idB, geo::Pt2<int> pixel, geo::Pt3<uint8_t> value) -> void{

    static_cast<void>(value);
    if(idC != 0 || (idB != 0 && idB != 1) || !ui.settingsFiltersSubPanelDisplayed){
        return;
    }

    auto dRes = depth_resolution(device.configS.mode);
    for(const auto &idP : pencils[filters.idPencil]){
        auto pt = idP + pixel;
        if(pt.x() >= 0 && pt.x() < dRes.x() && pt.y() >= 0 && pt.y() < dRes.y()){
            auto id = pt.y()*dRes.x() + pt.x();
            filters.depthMask.set(id, idB == 0);
        }
    }

    triggers_filters_settings();
}

auto K4SGSettings::update_imu_sample(camera::K4ImuSample imuSample) -> void{
    this->imuSample = imuSample;
}

auto K4SGSettings::triggers_init_network_sending_settings() -> void {
    K4SGSignals::get()->init_network_sending_settings_signal(&network);
}

auto K4SGSettings::triggers_filters_settings() -> void {
    K4SGSignals::get()->update_filters_signal(filters);
}

auto K4SGSettings::triggers_device_settings() -> void {
    K4SGSignals::get()->update_device_settings_signal(device);
}

void K4SGSettings::triggers_color_settings() {
    K4SGSignals::get()->update_color_settings_signal(color);
}

auto K4SGSettings::triggers_display_settings() -> void {
    K4SGSignals::get()->update_cloud_display_settings_signal(0, cloudDisplay);
}

auto K4SGSettings::triggers_model() -> void {
    K4SGSignals::get()->update_model_signal(0, model);
}

auto K4SGSettings::triggers_delay_settings() -> void {
    K4SGSignals::get()->update_delay_signal(delay);
}

void K4SGSettings::disconnect(){
    network.disconnect_from_manager();
}

auto K4SGSettings::reset_device_settings() -> void{
    Logger::message("reset_device_settings\n");
    device = camera::K4DeviceSettings::default_init_for_grabber();
    triggers_device_settings();
}

auto K4SGSettings::save_device_settings_to_default_file() -> bool{
    return device.save_to_file(K4SGPaths::defaultDevice.string());
}

auto K4SGSettings::save_device_settings_to_current_hostname_file() -> bool{
    return device.save_to_file(K4SGPaths::hostDevice.string());
}

auto K4SGSettings::load_default_device_settings_file() -> bool{
    if(device.init_from_file(K4SGPaths::defaultDevice.string())){
        triggers_device_settings();
        return true;
    }
    return false;
}

auto K4SGSettings::load_current_hostname_device_settings_file() -> bool{
    if(device.init_from_file(K4SGPaths::hostDevice.string())){
        triggers_device_settings();
        return true;
    }
    return false;
}

auto K4SGSettings::reset_filters() -> void{
    filters = camera::K4Filters();
    triggers_device_settings();
}

auto K4SGSettings::save_filters_to_default_file() -> bool{
    return filters.save_to_file(K4SGPaths::defaultFilters.string());
}

auto K4SGSettings::save_filters_to_current_hostname_file() -> bool{
    return filters.save_to_file(K4SGPaths::hostFilters.string());
}

auto K4SGSettings::load_default_filters_file() -> bool{
    if(filters.init_from_file(K4SGPaths::defaultFilters.string())){
        triggers_filters_settings();
        return true;
    }
    return false;
}

auto K4SGSettings::load_current_hostname_filters_file() -> bool{
    if(filters.init_from_file(K4SGPaths::hostFilters.string())){
        triggers_filters_settings();
        return true;
    }
    return false;
}

auto K4SGSettings::reset_color_settings() -> void{
    color = camera::K4ColorSettings();
    triggers_color_settings();
}

auto K4SGSettings::save_color_settings_to_default_file() -> bool{
    return color.save_to_file(K4SGPaths::defaultColor.string());
}

auto K4SGSettings::save_color_settings_to_current_hostname_file() -> bool{
    return color.save_to_file(K4SGPaths::hostColor.string());
}

auto K4SGSettings::load_default_settings_color_file() -> bool{
    if(color.init_from_file(K4SGPaths::defaultColor.string())){
        triggers_color_settings();
        return true;
    }
    return false;
}

auto K4SGSettings::load_current_hostname_color_settings_file() -> bool{
    if(color.init_from_file(K4SGPaths::hostColor.string())){
        triggers_color_settings();
        return true;
    }
    return false;
}

auto K4SGSettings::reset_model() -> void{
    model = camera::K4Model();
    triggers_model();
}

auto K4SGSettings::save_model_to_default_file() -> bool{
    return model.save_to_file(K4SGPaths::defaultModel.string());
}

auto K4SGSettings::save_model_to_current_hostname_file() -> bool{
    return model.save_to_file(K4SGPaths::hostModel.string());
}

auto K4SGSettings::load_default_model_file() -> bool{
    if(model.init_from_file(K4SGPaths::defaultModel.string())){
        triggers_model();
        return true;
    }
    return false;
}

auto K4SGSettings::load_current_hostname_model_file() -> bool{
    if(model.init_from_file(K4SGPaths::hostModel.string())){
        triggers_model();
        return true;
    }
    return false;
}



auto K4SGSettings::host_name() -> std::string{
    return Host::get_name();
}

