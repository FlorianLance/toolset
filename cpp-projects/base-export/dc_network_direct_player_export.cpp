
/*******************************************************************************
** Toolset-base-export                                                        **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "dc_network_direct_player_export.hpp"

// std
#include <filesystem>
#include <execution>

// base
#include "utility/logger.hpp"

// debug
#include <iostream>

using namespace tool::cam;
using namespace tool::net;
using namespace tool::geo;

DCNetworkDirectPlayer::DCNetworkDirectPlayer(){

    // set connections
    serverNetwork.remote_feedback_signal.connect([&](size_t id, net::Feedback feedback){
        // std::cout << "feedback " << id << " " << (int)feedback.receivedMessageType << " " << (int)feedback.feedback << "\n";
        readMessagesM.lock();
        messages.emplace_back(id, feedback);
        readMessagesM.unlock();
    });
    serverNetwork.remote_frame_signal.connect([&](size_t idCamera, std::shared_ptr<cam::DCCompressedFrame> cFrame){
        if(!readFrames){
            return;
        }
        framesReceived++;
        serverData.new_compressed_frame(idCamera, std::move(cFrame));
    });
    serverNetwork.local_frame_signal.connect([&](size_t idCamera, std::shared_ptr<cam::DCFrame> frame){
        if(!readFrames){
            return;
        }
        framesReceived++;        
        serverData.new_frame(idCamera, std::move(frame));
    });
}

DCNetworkDirectPlayer::~DCNetworkDirectPlayer(){
    clean();
}

auto DCNetworkDirectPlayer::initialize(const std::string &networkSettingsFilePath) -> bool{

    if(networkSettingsFilePath.empty()){
        Logger::error("Network settings file with path is empty.\n");
        return false;
    }

    if(!std::filesystem::exists(networkSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::initialize] Network settings file with path [{}] doesn't exist.\n", networkSettingsFilePath));
        return false;
    }

    // clean
    if(!serverNetworkS.initialize(networkSettingsFilePath)){
        return false;
    }

    Logger::message("Network infos:\n");
    size_t id = 0;
    for(const auto &info : serverNetworkS.clientsInfo){
        Logger::message(std::format("ID:[{}] RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}].\n",
        id, info.idReadingInterface, info.readingAdress, info.readingPort, info.sendingAdress, info.sendingPort));
        ++id;
    }

    auto nbConnections = serverNetworkS.clientsInfo.size();
    if(nbConnections == 0){
        Logger::error("[DCNetworkDirectPlayer::initialize] No connection defined in network file.\n");
        return false;
    }

    serverNetwork.initialize(serverNetworkS.clientsInfo);
    serverData.initialize(serverNetwork.devices_nb());

    devicesD = {};
    devicesD.resize(nbConnections);
    for(size_t idD = 0; idD < nbConnections; ++idD){
        devicesD[idD].id = idD;
    }

    return true;
}


auto DCNetworkDirectPlayer::clean() -> void{
    serverNetwork.clean();
    serverData.clean();
    devicesD.clear();
    messages.clear();
    messagesR.clear();
    readFrames = false;
}

auto DCNetworkDirectPlayer::connect_to_devices() -> void{

    if(devices_nb() == 0){
        Logger::error("[DCNetworkDirectPlayer::connect_to_devices] No devices available to be connected.\n");
        return;
    }

    for(const auto &deviceD : devicesD){
        Logger::message(std::format("Init connection with device: {}\n", deviceD.id));
        serverNetwork.init_connection(deviceD.id);
    }
}

auto DCNetworkDirectPlayer::disconnect_from_devices() -> void{
    for(const auto &deviceD : devicesD){
        serverNetwork.apply_command(deviceD.id, Command::Disconnect);
    }
}

auto DCNetworkDirectPlayer::shutdown_devices() -> void{
    for(const auto &deviceD : devicesD){
        serverNetwork.apply_command(deviceD.id, Command::Quit);
    }
}

auto DCNetworkDirectPlayer::start_reading() -> void{
    readFrames = true;
}

auto DCNetworkDirectPlayer::stop_reading() -> void{
    readFrames = false;
}

auto DCNetworkDirectPlayer::devices_nb() const noexcept -> size_t{
    return serverNetwork.devices_nb();
}

auto DCNetworkDirectPlayer::is_device_connected(size_t idD) const noexcept -> bool{
    return serverNetwork.device_connected(idD);
}

auto DCNetworkDirectPlayer::update_device_settings(const std::string &deviceSettingsPath) -> bool{

    if(deviceSettingsPath.length() == 0){
        Logger::warning("Device settings file with path is empty.\n");
        return true;
    }

    if(!std::filesystem::exists(deviceSettingsPath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_device_settings] Device settings file with path [{}] doesn't exist.\n", deviceSettingsPath));
        return false;
    }

    std::vector<io::BinaryFileSettings*> deviceSettingsFiles;
    for(auto &grabberS : devicesD){
        deviceSettingsFiles.push_back(&grabberS.device);
    }

    if(!io::BinaryFileSettings::init_from_file(deviceSettingsFiles, deviceSettingsPath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_device_settings] Error while reading device settings file with path [{}].\n", deviceSettingsPath));
        return false;
    }

    // send settings
    for(auto &grabberS : devicesD){
        if(is_device_connected(grabberS.id)){
            serverNetwork.update_device_settings(grabberS.id, grabberS.device);
        }
        serverData.update_device_settings(grabberS.id, grabberS.device);
    }

    return true;
}

auto DCNetworkDirectPlayer::update_color_settings(const std::string &colorSettingsFilePath) -> bool{

    if(colorSettingsFilePath.length() == 0){
        Logger::warning("Color settings file with path is empty.\n");
        return true;
    }

    if(!std::filesystem::exists(colorSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_color_settings] Color settings file with path [{}] doesn't exist.\n", colorSettingsFilePath));
        return false;
    }

    std::vector<io::BinaryFileSettings*> colorSettingsFiles;
    for(auto &grabberS : devicesD){
        colorSettingsFiles.push_back(&grabberS.color);
    }

    if(!io::BinaryFileSettings::init_from_file(colorSettingsFiles, colorSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_color_settings] Error while reading color settings file with path [{}].\n", colorSettingsFilePath));
        return false;
    }

    // send settings
    for(auto &grabberS : devicesD){
        if(is_device_connected(grabberS.id)){
            serverNetwork.update_color_settings(grabberS.id, grabberS.color);
        }
    }

    return true;
}

auto DCNetworkDirectPlayer::update_filters_settings(const std::string &filtersSettingsFilePath) -> bool{

    if(filtersSettingsFilePath.length() == 0){
        Logger::warning("Filters settings file with path is empty.\n");
        return true;
    }

    if(!std::filesystem::exists(filtersSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_filters_settings] Filters settings file with path [{}] doesn't exist.\n", filtersSettingsFilePath));
        return false;
    }

    std::vector<io::BinaryFileSettings*> filtersSettingsFiles;
    for(auto &grabberS : devicesD){
        filtersSettingsFiles.push_back(&grabberS.filters);
    }

    if(!io::BinaryFileSettings::init_from_file(filtersSettingsFiles, filtersSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_filters_settings] Error while reading filters settings file with path [{}].\n", filtersSettingsFilePath));
        return false;
    }

    // send settings
    for(auto &grabberS : devicesD){
        if(is_device_connected(grabberS.id)){
            serverNetwork.update_filters_settings(grabberS.id, grabberS.filters);
        }
    }

    return true;
}

auto DCNetworkDirectPlayer::update_model_settings(const std::string &modelSettingsFilePath) -> bool{

    if(modelSettingsFilePath.length() == 0){
        Logger::warning("Model settings file with path is empty.\n");
        return true;
    }

    if(!std::filesystem::exists(modelSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_model_settings] Model settings file with path [{}] doesn't exist.\n", modelSettingsFilePath));
        return false;
    }

    std::vector<io::TextSettings*> modelsSettingsFiles;
    for(auto &grabberS : devicesD){
        modelsSettingsFiles.push_back(&grabberS.model);
    }

    if(!io::TextSettings::init_from_file(modelsSettingsFiles, modelSettingsFilePath)){
        Logger::error(std::format("[DCNetworkDirectPlayer::update_model_settings] Error while reading model settings file with path [{}].\n", modelSettingsFilePath));
        return false;
    }

    // ...

    return true;
}

auto DCNetworkDirectPlayer::update_delay(size_t idD, cam::DCDelaySettings delayS) -> void{

    if(idD >= devices_nb()){
        return;
    }

    devicesD[idD].delay = delayS;
    if(is_device_connected(idD)){
        serverNetwork.update_delay_settings(idD, devicesD[idD].delay);
    }
}

auto DCNetworkDirectPlayer::update() -> void{

    if(readMessagesM.try_lock()){

        auto size = messages.size();
        if(size != 0){
            for(size_t ii = 0; ii < size; ++ii){
                messagesR.push_back(messages.front());
                messages.pop_front();
            }
        }
        readMessagesM.unlock();
    }

    for(const auto &message : messagesR){
        // log_message(std::format("message [{}] [{}] [{}]\n", message.first, (int)message.second.receivedMessageType, (int)message.second.feedback));
        if(message.second.receivedMessageType == MessageType::init_network_infos && message.second.feedback == FeedbackType::message_received){

            // log_message(std::format("Grabber [{}] connected.\n", message.first));
            // set<int>(
            //     ParametersContainer::Dynamic,
            //     std::format("grabber_connected_{}", message.first),
            //     i->serverNetwork.device_connected(message.first) ? 1 : 0
            //     );

            // if((get<int>(ParametersContainer::InitConfig, "device_init_file") == 1) || (get<int>(ParametersContainer::InitConfig, "device_init_default") == 1)){
            //     i->serverNetwork.update_device_settings(message.first, i->grabbersS[message.first].device);
            // }
            // if((get<int>(ParametersContainer::InitConfig, "color_init_file") == 1) || (get<int>(ParametersContainer::InitConfig, "color_init_default") == 1)){
            //     i->serverNetwork.update_color_settings(message.first, i->grabbersS[message.first].color);
            // }
            // if((get<int>(ParametersContainer::InitConfig, "filters_init_file") == 1) || (get<int>(ParametersContainer::InitConfig, "filters_init_default") == 1)){
            //     i->serverNetwork.update_filters_settings(message.first, i->grabbersS[message.first].filters);
            // }
        }
    }
    messagesR.clear();

    // update current frames
    for(auto &deviceD : devicesD){
        deviceD.lastFrame = serverData.get_frame(deviceD.id);  
    }
}

auto DCNetworkDirectPlayer::current_frame_id(size_t idD) -> size_t{
    if(idD < devicesD.size()){
        if(devicesD[idD].lastFrame != nullptr){
            return devicesD[idD].lastFrame->idCapture;
        }
    }
    return 0;
}

auto DCNetworkDirectPlayer::current_frame(size_t idD) -> std::shared_ptr<DCFrame>{
    if(idD < devicesD.size()){
        return devicesD[idD].lastFrame;
    }
    return nullptr;
}

auto DCNetworkDirectPlayer::current_frame_cloud_size(size_t idD) -> size_t{
    if(idD < devicesD.size()){
        if(devicesD[idD].lastFrame != nullptr){
            return devicesD[idD].lastFrame->cloud.size();
        }
    }
    return 0;
}

auto DCNetworkDirectPlayer::device_model(size_t idD) -> Mat4f{
    if(idD < devicesD.size()){
        return devicesD[idD].model.transformation;
    }
    return Mat4f::identity();
}

auto DCNetworkDirectPlayer::copy_current_frame_vertices(size_t idD, std::span<DCVertexMeshData> vertices) -> size_t{

    if(auto frame = current_frame(idD); frame != nullptr){
        size_t verticesCountToCopy = std::min(frame->cloud.size(), vertices.size());

        if(ids.size() < verticesCountToCopy){
            ids.resize(verticesCountToCopy);
            std::iota(std::begin(ids), std::end(ids), 0);
        }

        std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
            vertices[id].pos = frame->cloud.vertices[id];
            vertices[id].col = geo::Pt4<std::uint8_t>{
                static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].x()),
                static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].y()),
                static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].z()),
                255
            };
        });
        return verticesCountToCopy;
    }
    return 0;
}


DCNetworkDirectPlayer* create__dc_network_direct_player(){
    return new DCNetworkDirectPlayer();
}

void delete__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    delete dcNetworkDirectPlayer;
}

int initialize__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* networkSettingsFilePath){
    return dcNetworkDirectPlayer->initialize(networkSettingsFilePath) ? 1 : 0;
}

void connect_to_devices__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    dcNetworkDirectPlayer->connect_to_devices();
}

void disconnect_from_devices__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    dcNetworkDirectPlayer->disconnect_from_devices();
}

void start_reading__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    dcNetworkDirectPlayer->start_reading();
}

void stop_reading__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    dcNetworkDirectPlayer->stop_reading();
}

int update_device_settings__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* deviceSettingsFilePath){
    return dcNetworkDirectPlayer->update_device_settings(deviceSettingsFilePath) ? 1 : 0;
}

int update_color_settings__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* colorSettingsFilePath){
    return dcNetworkDirectPlayer->update_color_settings(colorSettingsFilePath) ? 1 : 0;
}

int update_filters_settings__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* filtersSettingsFilePath){
    return dcNetworkDirectPlayer->update_filters_settings(filtersSettingsFilePath) ? 1 : 0;
}

int update_model_settings__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* modelSettingsFilePath){
    return dcNetworkDirectPlayer->update_model_settings(modelSettingsFilePath) ? 1 : 0;
}

void update__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    dcNetworkDirectPlayer->update();
}

void copy_transform__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, float *transformData){
    tool::geo::Mat4f transform = dcNetworkDirectPlayer->device_model(idD);
    std::copy(transform.array.begin(), transform.array.end(), transformData);
}

int devices_nb__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    return static_cast<int>(dcNetworkDirectPlayer->devices_nb());
}

int is_device_connected__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD){
    return dcNetworkDirectPlayer->is_device_connected(idD) ? 1 : 0;
}

int current_frame_id__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD){
    return static_cast<int>(dcNetworkDirectPlayer->current_frame_id(idD));
}

int current_frame_cloud_size__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD){
    return static_cast<int>(dcNetworkDirectPlayer->current_frame_cloud_size(idD));
}

int copy_current_frame_vertices__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, DCVertexMeshData *vertices, int verticesCount){
    return static_cast<int>(dcNetworkDirectPlayer->copy_current_frame_vertices(idD, {vertices, static_cast<size_t>(verticesCount)}));
}
