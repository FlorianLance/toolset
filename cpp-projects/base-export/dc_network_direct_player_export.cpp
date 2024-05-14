
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


using namespace tool::cam;
using namespace tool::net;
using namespace tool::geo;

DCNetworkDirectPlayer::DCNetworkDirectPlayer(){

    size_t maxCloudSize = 1024 * 1024;
    ids.resize(maxCloudSize);
    std::iota(std::begin(ids), std::end(ids), 0);

    // set connections
    serverNetwork.remote_feedback_signal.connect([&](size_t idCamera, net::Feedback feedback){
        if(newFeedbackCBP != nullptr){
            (*newFeedbackCBP)(static_cast<int>(idCamera), static_cast<int>(feedback.receivedMessageType), static_cast<int>(feedback.feedback));
        }
    });

    serverNetwork.remote_frame_signal.connect([&](size_t idCamera, std::shared_ptr<cam::DCCompressedFrame> cFrame){
        devicesD[idCamera].frameLocker->lock();
        devicesD[idCamera].lastCompressedFrame = std::move(cFrame);
        devicesD[idCamera].frameLocker->unlock();
        devicesD[idCamera].nbFramesReceived++;
    });

    serverNetwork.local_frame_signal.connect([&](size_t idCamera, std::shared_ptr<cam::DCFrame> frame){
        devicesD[idCamera].frameLocker->lock();
        devicesD[idCamera].lastFrame = std::move(frame);
        devicesD[idCamera].frameLocker->unlock();
        devicesD[idCamera].nbFramesReceived++;
    });
}

DCNetworkDirectPlayer::~DCNetworkDirectPlayer(){
    clean();
}

auto DCNetworkDirectPlayer::init_callbacks(NewFeedbackCB newFeedbackCB) -> void{
    newFeedbackCBP = std::make_unique<NewFeedbackCB>(newFeedbackCB);
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
    for(auto &info : serverNetworkS.clientsInfo){
        Logger::message(std::format("ID:[{}] RI:[{}] RA:[{}] RP:[{}] SA:[{}] SP:[{}].\n",
        id, info.idReadingInterface, info.readingAdress, info.readingPort, info.sendingAdress, info.sendingPort));
        ++id;

        info.startReadingThread = false;
    }

    auto nbConnections = serverNetworkS.clientsInfo.size();
    if(nbConnections == 0){
        Logger::error("[DCNetworkDirectPlayer::initialize] No connection defined in network file.\n");
        return false;
    }

    serverNetwork.initialize(serverNetworkS.clientsInfo);
    serverData.initialize(serverNetwork.devices_nb(), false);

    devicesD.clear();
    devicesD.resize(nbConnections);
    for(size_t idD = 0; idD < nbConnections; ++idD){
        devicesD[idD].id = idD;
        devicesD[idD].frameLocker = std::make_unique<std::mutex>();
        devicesD[idD].feedbackLocker = std::make_unique<std::mutex>();
    }

    return true;
}


auto DCNetworkDirectPlayer::uncompress_frame(size_t idDevice) -> bool{

    if(idDevice >= devicesD.size()){
        return false;
    }

    auto &deviceD = devicesD[idDevice];
    deviceD.frameLocker->lock();

    if(deviceD.lastCompressedFrame != nullptr){
        auto cFrame = std::move(deviceD.lastCompressedFrame);
        deviceD.lastCompressedFrame = nullptr;
        deviceD.frameLocker->unlock();

        deviceD.frameToDisplay = serverData.uncompress_frame(deviceD.id, std::move(cFrame));
        return true;

    }else if(deviceD.lastFrame != nullptr){
        auto frame = std::move(deviceD.lastFrame);
        deviceD.lastFrame = nullptr;
        deviceD.frameLocker->unlock();

        deviceD.frameToDisplay = std::move(frame);
        return true;
    }

    deviceD.frameLocker->unlock();

    return false;
}

auto DCNetworkDirectPlayer::clean() -> void{
    serverNetwork.clean();
    serverData.clean();
    devicesD.clear();
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

auto DCNetworkDirectPlayer::devices_nb() const noexcept -> size_t{
    return serverNetwork.devices_nb();
}

auto DCNetworkDirectPlayer::is_device_connected(size_t idD) const noexcept -> bool{
    return serverNetwork.device_connected(idD);
}

auto DCNetworkDirectPlayer::update_device_settings(const std::string &deviceSettingsPath) -> bool{

    if(deviceSettingsPath.length() == 0){
        Logger::warning("Device settings file with path is empty.\n");
        return false;
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

auto DCNetworkDirectPlayer::read_network_data(size_t idDevice) -> size_t{
    return serverNetwork.read_data_from_network(idDevice);
}

auto DCNetworkDirectPlayer::current_frame_id(size_t idD) -> size_t{
    if(idD < devicesD.size()){
        if(devicesD[idD].frameToDisplay != nullptr){
            return devicesD[idD].frameToDisplay->idCapture;
        }
    }
    return 0;
}

auto DCNetworkDirectPlayer::current_frame(size_t idD) -> std::shared_ptr<DCFrame>{
    if(idD < devicesD.size()){
        return devicesD[idD].frameToDisplay;
    }
    return nullptr;
}

auto DCNetworkDirectPlayer::current_frame_cloud_size(size_t idD) -> size_t{
    if(idD < devicesD.size()){
        if(devicesD[idD].frameToDisplay != nullptr){
            return devicesD[idD].frameToDisplay->cloud.size();
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

auto DCNetworkDirectPlayer::copy_current_frame_vertices(size_t idD, std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t{

    if(auto frame = current_frame(idD); frame != nullptr){
        size_t verticesCountToCopy = std::min(frame->cloud.size(), vertices.size());

        auto tr = device_model(idD);

        if(applyModelTransform){
            std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                const auto &pt = frame->cloud.vertices[id];
                vertices[id].pos = geo::Pt3f(tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz());
                vertices[id].col = geo::Pt4<std::uint8_t>{
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].x()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].y()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].z()),
                    255
                };
            });
        }else{
            std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                vertices[id].pos = frame->cloud.vertices[id];
                vertices[id].col = geo::Pt4<std::uint8_t>{
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].x()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].y()),
                    static_cast<std::uint8_t>(255.f*frame->cloud.colors[id].z()),
                    255
                };
            });
        }
        return verticesCountToCopy;
    }
    return 0;
}

auto DCNetworkDirectPlayer::copy_current_frame_vertices(size_t idD, std::span<geo::Pt4f> positions, std::span<geo::Pt4f> colors, bool applyModelTransform) -> size_t{

    if(auto frame = current_frame(idD); frame != nullptr){

        auto verticesCountToCopy = std::min(frame->cloud.size(), positions.size());
        auto tr = device_model(idD);

        if(applyModelTransform){

            std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                const auto &pt = frame->cloud.vertices[id];
                positions[id] = tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f});
                const auto &col = frame->cloud.colors[id];
                colors[id] = {
                    col.x(), col.y(), col.z(), 1.f
                };
            });
        }else{

            std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                const auto &pt = frame->cloud.vertices[id];;
                positions[id] = geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f};
                const auto &col = frame->cloud.colors[id];
                colors[id] = {
                    col.x(), col.y(), col.z(), 1.f
                };
            });
        }
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

void init_callbacks__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, NewFeedbackCB newFeedbackCB){
    dcNetworkDirectPlayer->init_callbacks(newFeedbackCB);
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

int read_network_data__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD){
    return static_cast<int>(dcNetworkDirectPlayer->read_network_data(idD));
}

void process_feedbacks__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer){
    // TODO: remove
}

int uncompress_frame__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD){
    return static_cast<int>(dcNetworkDirectPlayer->uncompress_frame(idD)) ? 1 : 0;
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

int copy_current_frame_vertices__dc_network_direct_player(DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, DCVertexMeshData *vertices, int verticesCount, int applyModelTransform){
    return static_cast<int>(dcNetworkDirectPlayer->copy_current_frame_vertices(idD, {vertices, static_cast<size_t>(verticesCount)}, applyModelTransform == 1));
}

int copy_current_frame_vertices_vfx__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, tool::geo::Pt4f *positions, tool::geo::Pt4f *colors, int verticesCount, int applyModelTransform){
    return dcNetworkDirectPlayer->copy_current_frame_vertices(
        idD,
        std::span<tool::geo::Pt4f>(positions, verticesCount),
        std::span<tool::geo::Pt4f>(colors, verticesCount),
        applyModelTransform == 1
    );
}
