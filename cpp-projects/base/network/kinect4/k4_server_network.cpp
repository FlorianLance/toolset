



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

#include "k4_server_network.hpp"

// local
#include "utility/logger.hpp"
#include "utility/format.hpp"

using namespace tool::network;
using namespace tool::camera;
using namespace std::literals::string_view_literals;

K4ServerNetwork::~K4ServerNetwork(){
    clean();
}

auto K4ServerNetwork::initialize(const std::vector<network::ReadSendNetworkInfos> &infos) -> bool{

    if(connections.size() != 0){
        clean();
    }

    size_t id = 0;
    for(const auto &info : infos){
        auto connection = std::make_unique<K4ServerConnection>();
        if(!connection->initialize(id, info)){
            return false;
        }
        connections.push_back(std::move(connection));
        ++id;
    }

    return true;
}

auto K4ServerNetwork::clean() -> void{
    for(auto &connection : connections){
        connection->clean();
    }
    connections.clear();
}

auto K4ServerNetwork::init_connection(size_t idG) -> void{
    if(idG < connections.size()){
        connections[idG]->init_connection_with_grabber();
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto K4ServerNetwork::send_command(size_t idG, K4Command command) -> void {

    if(idG < connections.size()){

        if(!connections[idG]->grabber_connected_to_server()){
            Logger::error(fmt("Grabber id [{}] not connected to server yet.\n"sv, idG));
            return;
        }

        switch(command){
        case K4Command::Disconnect:
            connections[idG]->disconnect_grabber();
            break;
        case K4Command::Quit:
            connections[idG]->quit_grabber();
            break;
        case K4Command::Shutdown:
            connections[idG]->shutdown_grabber_computer();
            break;
        case K4Command::Restart:
            connections[idG]->restart_grabber_computer();
            break;
        case K4Command::UpdateDeviceList:
            connections[idG]->update_device_list();
            break;
        }
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto K4ServerNetwork::send_device_settings(size_t idG, const camera::K4DeviceSettings &deviceS) -> void {
    if(idG < connections.size()){
        connections[idG]->udpSender.send_update_device_settings_message(deviceS);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto K4ServerNetwork::send_color_settings(size_t idG, const camera::K4ColorSettings &colorS) -> void {
    if(idG < connections.size()){
        connections[idG]->udpSender.send_update_color_settings_message(colorS);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto K4ServerNetwork::send_filters(size_t idG, const camera::K4Filters &filters) -> void {
    if(idG < connections.size()){
        connections[idG]->udpSender.send_update_filters_settings_message(filters);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto K4ServerNetwork::send_delay(size_t idG, camera::K4Delay delay) -> void{
    if(idG < connections.size()){
        connections[idG]->udpSender.send_delay_settings_message(delay);
    }else{
        Logger::error(fmt("Invalid id [{}], nb of connections available [{}].\n"sv, idG, connections.size()));
    }
}

auto K4ServerNetwork::do_not_use_global_signals() -> void {
    for(auto &connection : connections){
        connection->do_not_use_global_signals();
    }
}
