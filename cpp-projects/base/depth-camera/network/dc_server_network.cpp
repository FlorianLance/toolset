



/*******************************************************************************
** Toolset-base                                                               **
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

#include "dc_server_network.hpp"

// std
#include <format>
#include <execution>

// local
#include "utility/logger.hpp"
#include "dc_server_local_device.hpp"
#include "dc_server_remote_device.hpp"


using namespace tool::net;
using namespace tool::cam;
using namespace std::literals::string_view_literals;

struct DCServerNetwork::Impl{
    std::vector<std::unique_ptr<DCServerDevice>> devices;   
};

DCServerNetwork::DCServerNetwork() : i(std::make_unique<DCServerNetwork::Impl>()){
}

DCServerNetwork::~DCServerNetwork(){
    clean();
}

auto DCServerNetwork::initialize(const std::vector<net::ReadSendNetworkInfos> &clientsInfo) -> void{

    if(devices_nb() != 0){
        clean();
    }

    size_t idDevice = 0;
    for(const auto &clientInfo : clientsInfo){

        std::unique_ptr<DCServerDevice> device = nullptr;

        if(clientInfo.local){
            Logger::message("[DCServerNetwork::initialize] Create local device. \n");
            auto lDevice = std::make_unique<DCServerLocalDevice>();
            lDevice->local_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCFrame> frame){
                this->local_frame_signal(idDevice, std::move(frame));
            });
            device = std::move(lDevice);
        }else{
            Logger::message("[DCServerNetwork::initialize] Create remote device. \n");
            auto rDevice = std::make_unique<DCServerRemoteDevice>();
            rDevice->remote_synchro_signal.connect([this,idDevice](std::int64_t averageTimestampDiffNs){
                this->remote_synchro_signal(idDevice, averageTimestampDiffNs);
            });
            rDevice->remote_feedback_signal.connect([this,idDevice](net::Feedback feedback){
                this->remote_feedback_signal(idDevice, feedback);
            });
            rDevice->remote_status_signal.connect([this,idDevice](net::UdpReceivedStatus status){
                this->remote_status_signal(idDevice, status);
            });

            rDevice->remote_frame_signal.connect([this,idDevice](std::shared_ptr<cam::DCCompressedFrame> cFrame){
                this->remote_frame_signal(idDevice,std::move(cFrame));
            });
            device = std::move(rDevice);
        }

        Logger::message("[DCServerNetwork::initialize] Initialize device. \n");
        if(!device->initialize(clientInfo)){
            Logger::error(std::format("[DCServerNetwork::initialize] Cannot initialize device with name {} \n",clientInfo.sendingAdress));
        }

        Logger::message("[DCServerNetwork::initialize] Add device to list.\n");
        i->devices.push_back(std::move(device));
        ++idDevice;
    }
}


auto DCServerNetwork::clean() -> void{

    // clean devices
    for(auto &device : i->devices){
        device->clean();
    }

    // remove devices
    i->devices.clear();
}

auto DCServerNetwork::init_connection(size_t idG) -> void{
    if(idG < i->devices.size()){
        if(i->devices[idG]->type() == DCServerType::remote){
            dynamic_cast<DCServerRemoteDevice*>(i->devices[idG].get())->init_remote_connection();
        }
    }else{
        Logger::error(std::format("[DCServerNetwork::init_connection] Invalid id [{}], nb of devices available [{}].\n"sv, idG, i->devices.size()));
    }
}

auto DCServerNetwork::apply_command(size_t idG, Command command) -> void {
    if(idG < i->devices.size()){
        i->devices[idG]->apply_command(command);
    }else{
        Logger::error(std::format("[DCServerNetwork::send_command] Invalid id [{}], nb of devices available [{}].\n"sv, idG, i->devices.size()));
    }
}

auto DCServerNetwork::update_device_settings(size_t idG, const cam::DCDeviceSettings &deviceS) -> void {
    if(idG < i->devices.size()){
        i->devices[idG]->update_device_settings(deviceS);
    }else{
        Logger::error(std::format("[DCServerNetwork::send_device_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idG, i->devices.size()));
    }
}

auto DCServerNetwork::update_color_settings(size_t idG, const cam::DCColorSettings &colorS) -> void {
    if(idG < i->devices.size()){
        i->devices[idG]->update_color_settings(colorS);
    }else{
        Logger::error(std::format("[DCServerNetwork::send_color_settings] Invalid id [{}], nb of devices available [{}].\n"sv, idG, i->devices.size()));
    }
}

auto DCServerNetwork::update_filters_settings(size_t idG, const cam::DCFiltersSettings &filters) -> void {
    if(idG < i->devices.size()){
        i->devices[idG]->update_filters_settings(filters);
    }else{
        Logger::error(std::format("[DCServerNetwork::send_filters] Invalid id [{}], nb of devices available [{}].\n"sv, idG, i->devices.size()));
    }
}

auto DCServerNetwork::update_delay_settings(size_t idG, const cam::DCDelaySettings &delay) -> void{
    if(idG < i->devices.size()){
        i->devices[idG]->update_delay_settings(delay);
    }else{
        Logger::error(std::format("[DCServerNetwork::send_delay] Invalid id [{}], nb of devices available [{}].\n"sv, idG, i->devices.size()));
    }
}

auto DCServerNetwork::devices_nb() const noexcept -> size_t {
    return i->devices.size();
}

auto DCServerNetwork::device_connected(size_t idG) const noexcept -> bool{
    if(idG < i->devices.size()){
        return i->devices[idG]->device_connected();
    }
    return false;
}

auto DCServerNetwork::read_data_from_network(size_t idG) -> size_t{

    if(idG < i->devices.size()){
        if(i->devices[idG]->type() == DCServerType::remote){
            return dynamic_cast<DCServerRemoteDevice*>(i->devices[idG].get())->read_data_from_network();
        }else{
            // ...
        }
    }

    return 0;
}



