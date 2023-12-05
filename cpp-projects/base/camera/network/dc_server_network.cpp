



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

// local
#include "utility/logger.hpp"
#include "utility/format.hpp"

using namespace tool::network;
using namespace tool::camera;
using namespace std::literals::string_view_literals;

DCServerNetwork::~DCServerNetwork(){
    clean();
}

auto DCServerNetwork::initialize(const std::vector<network::ReadSendNetworkInfos> &infos) -> bool{

    if(connections.size() != 0){
        clean();
    }

    size_t id = 0;
    for(const auto &info : infos){
        auto connection = std::make_unique<DCServerConnection>();
        if(!connection->initialize(id, info)){
            return false;
        }
        connections.push_back(std::move(connection));
        ++id;
    }

    return true;
}

auto DCServerNetwork::clean() -> void{
    for(auto &connection : connections){
        connection->clean();
    }
    connections.clear();
}

auto DCServerNetwork::init_connection(size_t idG) -> void{
    if(idG < connections.size()){
        connections[idG]->init_connection_with_grabber();
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto DCServerNetwork::send_command(size_t idG, Command command) -> void {

    if(idG < connections.size()){

        if(!connections[idG]->grabber_connected_to_server()){
            Logger::error(fmt("Grabber id [{}] not connected to server yet.\n"sv, idG));
            return;
        }

        switch(command){
        case Command::Disconnect:
            connections[idG]->disconnect_grabber();
            break;
        case Command::Quit:
            connections[idG]->quit_grabber();
            break;
        case Command::Shutdown:
            connections[idG]->shutdown_grabber_computer();
            break;
        case Command::Restart:
            connections[idG]->restart_grabber_computer();
            break;
        case Command::UpdateDeviceList:
            connections[idG]->update_device_list();
            break;
        }
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto DCServerNetwork::send_device_settings(size_t idG, const camera::DCDeviceSettings &deviceS) -> void {
    if(idG < connections.size()){
        connections[idG]->udpSender.send_update_device_settings_message(deviceS);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto DCServerNetwork::send_color_settings(size_t idG, const camera::DCColorSettings &colorS) -> void {
    if(idG < connections.size()){
        connections[idG]->udpSender.send_update_color_settings_message(colorS);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto DCServerNetwork::send_filters(size_t idG, const camera::DCFiltersSettings &filters) -> void {
    if(idG < connections.size()){
        connections[idG]->udpSender.send_update_filters_settings_message(filters);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto DCServerNetwork::send_delay(size_t idG, camera::DCDelaySettings delay) -> void{
    if(idG < connections.size()){
        connections[idG]->udpSender.send_delay_settings_message(delay);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto DCServerNetwork::do_not_use_global_signals() -> void {
    for(auto &connection : connections){
        connection->do_not_use_global_signals();
    }
}
