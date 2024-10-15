
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

#include "dc_client_export.hpp"

// std
#include <filesystem>
#include <execution>

// utility
#include "utility/logger.hpp"

using namespace tool::cam;
using namespace tool::net;
using namespace tool::geo;

DCClientExport::DCClientExport(){

    size_t maxCloudSize = 1024 * 1024;
    ids.resize(maxCloudSize);
    std::iota(std::begin(ids), std::end(ids), 0);

    // set connections
    client.feedback_received_signal.connect([&](size_t idD, tool::net::Feedback feedback){
        if(newFeedbackCBP != nullptr){
            (*newFeedbackCBP)(static_cast<int>(idD), static_cast<int>(feedback.type), static_cast<int>(feedback.receivedMessageType));
        }
    });
    client.new_frame_signal.connect([&](size_t idD, std::shared_ptr<cam::DCFrame> frame){
        framesToDisplay[idD] = std::move(frame);
    });
}

DCClientExport::~DCClientExport(){
    framesToDisplay.clear();
    client.clean();
}

auto DCClientExport::dll_log_message(const std::string &message) -> void{
    if(logMessageCBP != nullptr){
        (*logMessageCBP)(message.c_str(),0);
    }else{
        tool::Logger::message(message);
    }
}

auto DCClientExport::dll_log_warning(const std::string &message) -> void{
    if(logMessageCBP != nullptr){
        (*logMessageCBP)(message.c_str(),1);
    }else{
        tool::Logger::warning(message);
    }
}

auto DCClientExport::dll_log_error(const std::string &message) -> void{
    if(logMessageCBP != nullptr){
        (*logMessageCBP)(message.c_str(),2);
    }else{
        tool::Logger::error(message);
    }
}

auto DCClientExport::init_callbacks(
    LogMessageCB logCB,
    NewFeedbackCB newFeedbackCB) -> void{
    logMessageCBP  = std::make_unique<LogMessageCB>(logCB);
    newFeedbackCBP = std::make_unique<NewFeedbackCB>(newFeedbackCB);
}

auto DCClientExport::initialize(const std::string &clientSettingsFilePath, bool startThreads) -> bool{

    dll_log_message(std::format("[DLL][DCClientExport::initialize] Initialize from path: [{}]", clientSettingsFilePath));

    if(clientSettingsFilePath.empty()){
        dll_log_error("[DLL][DCClientExport::initialize] Client settings file with path is empty.");
        return false;
    }

    if(!std::filesystem::exists(clientSettingsFilePath)){
        dll_log_error(std::format("[DLL][DCClientExport::initialize] Client settings file with path [{}] doesn't exist.", clientSettingsFilePath));
        return false;
    }

    // initialize client
    if(!client.initialize(clientSettingsFilePath, startThreads)){
        dll_log_error(std::format("[DLL][DCClientExport::initialize] Cannot initialize client from client settings file with path [{}].", clientSettingsFilePath));
        return false;
    }
    dll_log_message("[DLL][DCClientExport::initialize] Client settings file loaded:\n");
    dll_log_message(std::format("[Client]\n{}", client.settings.convert_to_json_str()));

    dll_log_message("[DLL][DCClientExport::initialize] Connections infos:");
    size_t id = 0;

    for(auto &deviceS : client.settings.devicesS){
        if(deviceS.connectionS.connectionType == DCClientType::Remote){
            dll_log_message(std::format("[REMOTE]\n\tId device: [{}]\n\tProtocol: [{}]\n\tId reading interface: [{}]\n\tReading address: [{}]\n\tReading port: [{}]\n\tSending address: [{}]\n\tSending port: [{}]\n\t",
                id,
                (deviceS.connectionS.protocol == Protocol::ipv4) ? "IPV4"sv : "IPV6"sv,
                deviceS.connectionS.idReadingInterface,
                deviceS.connectionS.readingAddress,
                deviceS.connectionS.readingPort,
                deviceS.connectionS.processedSendingAddress,
                deviceS.connectionS.sendingPort));
        }else if(deviceS.connectionS.connectionType == DCClientType::Local){
            dll_log_message(std::format("[LOCAL]\n\tId device: [{}]", id));
        }
        ++id;
    }

    auto nbDevices = client.settings.devicesS.size();
    if(nbDevices == 0){
        dll_log_error(std::format("[DLL][DCClientExport::initialize] No device defined in client settings file with path [{}].", clientSettingsFilePath));
        return false;
    }

    framesToDisplay.clear();
    framesToDisplay.resize(nbDevices);
    for(size_t idD = 0; idD < nbDevices; ++idD){
        framesToDisplay[idD] = nullptr;
    }

    return true;
}

auto DCClientExport::update() -> void{
    client.update();
}

auto DCClientExport::connect_to_devices() -> void{

    if(devices_nb() == 0){
        dll_log_error("[DLL][DCClientExport::connect_to_devices] No devices available to be connected.");
        return;
    }

    for(const auto &deviceS : client.settings.devicesS){
        if(deviceS.connectionS.connectionType == DCClientType::Remote){
            dll_log_message(std::format("[DLL][DCClientExport::connect_to_devices] Init connection with device: {}", deviceS.id));
            client.init_connection_with_remote_device(deviceS.id);
        }
    }
}

auto DCClientExport::disconnect_from_devices() -> void{
    for(size_t idD = 0; idD < client.devices_nb(); ++idD){
        client.apply_command(idD, Command::disconnect);
    }
}

auto DCClientExport::shutdown_devices() -> void{
    for(size_t idD = 0; idD < client.devices_nb(); ++idD){
        client.apply_command(idD, Command::shutdown);
    }
}

auto DCClientExport::devices_nb() const noexcept -> size_t{
    return client.devices_nb();
}

auto DCClientExport::is_device_connected(size_t idD) const noexcept -> bool{
    return client.device_connected(idD);
}

auto DCClientExport::apply_device_settings(size_t idD) -> void{
    client.apply_device_settings(idD);
    dll_log_message(std::format("[G{}][Config]\n{}", idD, client.settings.devicesS[idD].deviceS.configS.convert_to_json_str()));
    dll_log_message(std::format("[G{}][Data]\n{}", idD, client.settings.devicesS[idD].deviceS.dataS.convert_to_json_str()));
}

auto DCClientExport::apply_color_settings() -> void{
    for(size_t idD = 0; idD < client.devices_nb(); ++idD){
        client.apply_color_settings(idD);
        dll_log_message(std::format("[G{}][Color]\n{}", idD, client.settings.devicesS[idD].colorS.convert_to_json_str()));
    }
}

auto DCClientExport::apply_filters_settings() -> void{
    for(size_t idD = 0; idD < client.devices_nb(); ++idD){
        client.update_filters_settings(idD, client.settings.devicesS[idD].filtersS);
        dll_log_message(std::format("[G{}][Filters]\n{}", idD, client.settings.devicesS[idD].filtersS.convert_to_json_str()));
    }
}

auto DCClientExport::update_delay(size_t idD, cam::DCMiscSettings miscS) -> void{
    if(idD >= devices_nb()){
        return;
    }
    client.update_misc_settings(idD, miscS);
}

auto DCClientExport::read_data_from_external_thread(size_t idD) -> size_t{
    return client.read_data_from_external_thread(idD);
}

auto DCClientExport::trigger_packets_from_external_tread(size_t idD) -> void{
    client.trigger_packets_from_remote_device(idD);
}

auto DCClientExport::process_frames_from_external_thread(size_t idD) -> void{
    client.process_frames_from_external_thread(idD);
}

auto DCClientExport::current_frame_id(size_t idD) -> size_t{
    if(idD < framesToDisplay.size()){
        if(framesToDisplay[idD] != nullptr){
            return framesToDisplay[idD]->idCapture;
        }
    }
    return 0;
}

auto DCClientExport::current_frame(size_t idD) -> std::shared_ptr<DCFrame>{
    if(idD < framesToDisplay.size()){
        return framesToDisplay[idD];
    }
    return nullptr;
}

auto DCClientExport::current_frame_cloud_size(size_t idD) -> size_t{
    if(idD < framesToDisplay.size()){
        if(framesToDisplay[idD] != nullptr){
            if(framesToDisplay[idD]->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
                return framesToDisplay[idD]->volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud)->size();
            }
        }
    }
    return 0;
}

auto DCClientExport::device_model(size_t idD) -> Mat4f{
    if(idD < client.devices_nb()){
        return client.settings.devicesS[idD].modelS.transformation;
    }
    return Mat4f::identity();
}

auto DCClientExport::copy_current_frame_vertices(size_t idD, std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t{

    if(auto frame = current_frame(idD); frame != nullptr){

        if(auto cloud = frame->volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud)){

            size_t verticesCountToCopy = std::min(cloud->size(), vertices.size());
            auto tr = device_model(idD);

            if(applyModelTransform){
                std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                    const auto &pt = cloud->vertices[id];
                    vertices[id].pos = geo::Pt3f(tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz());
                    vertices[id].col = geo::Pt4<std::uint8_t>{
                        static_cast<std::uint8_t>(255.f*cloud->colors[id].x()),
                        static_cast<std::uint8_t>(255.f*cloud->colors[id].y()),
                        static_cast<std::uint8_t>(255.f*cloud->colors[id].z()),
                        255
                    };
                });
            }else{
                std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                    vertices[id].pos = cloud->vertices[id];
                    vertices[id].col = geo::Pt4<std::uint8_t>{
                        static_cast<std::uint8_t>(255.f*cloud->colors[id].x()),
                        static_cast<std::uint8_t>(255.f*cloud->colors[id].y()),
                        static_cast<std::uint8_t>(255.f*cloud->colors[id].z()),
                        255
                    };
                });
            }
            return verticesCountToCopy;
        }
    }
    return 0;
}

auto DCClientExport::copy_current_frame_vertices(size_t idD, std::span<geo::Pt3f> positions, std::span<geo::Pt3f> colors, std::span<geo::Pt3f> normals, bool applyModelTransform) -> size_t{

    if(auto frame = current_frame(idD); frame != nullptr){

        if(auto cloud = frame->volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud)){

            auto verticesCountToCopy = std::min(cloud->size(), positions.size());
            auto tr = device_model(idD);

            if(applyModelTransform){
                std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                    const auto &pt = cloud->vertices[id];
                    positions[id] = tr.multiply_point(geo::Pt4f{pt.x(), pt.y(), pt.z(), 1.f}).xyz();
                    positions[id].x() *= -1.f;
                    const auto &col = cloud->colors[id];
                    colors[id] = {
                        col.x(), col.y(), col.z()
                    };
                    const auto &norm = cloud->normals[id];
                    normals[id] = normalize(tr.multiply_vector(geo::Pt4f{norm.x(), norm.y(), norm.z(), 1.f}).xyz());
                    normals[id].x() *= -1.f;
                });
            }else{
                std::for_each(std::execution::par_unseq, std::begin(ids), std::begin(ids) + verticesCountToCopy, [&](size_t id){
                    const auto &pt = cloud->vertices[id];;
                    positions[id] = geo::Pt3f{pt.x(), pt.y(), pt.z()};
                    positions[id].x() *= -1.f;
                    const auto &col = cloud->colors[id];
                    colors[id] = {
                        col.x(), col.y(), col.z()
                    };
                    normals[id] = cloud->normals[id];
                    normals[id].x() *= -1.f;
                });
            }
            return verticesCountToCopy;
        }
    }
    return 0;
}

DCClientExport* create__dc_client_export(){
    return new DCClientExport();
}
void delete__dc_client_export(DCClientExport *dcCE){
    delete dcCE;
}

void init_callbacks__dc_client_export(tool::cam::DCClientExport *dcCE, LogMessageCB logCB, NewFeedbackCB newFeedbackCB){
    dcCE->init_callbacks(logCB, newFeedbackCB);
}
int initialize__dc_client_export(DCClientExport *dcCE, const char* networkSettingsFilePath, int startThreads){
    return dcCE->initialize(networkSettingsFilePath, startThreads == 1) ? 1 : 0;
}

void connect_to_devices__dc_client_export(DCClientExport *dcCE){
    dcCE->connect_to_devices();
}
void disconnect_from_devices__dc_client_export(DCClientExport *dcCE){
    dcCE->disconnect_from_devices();
}

void apply_device_settings__dc_client_export(DCClientExport *dcCE, int idD){
    dcCE->apply_device_settings(idD);
}
void apply_color_settings__dc_client_export(DCClientExport *dcCE){
    dcCE->apply_color_settings();
}
void apply_filters_settings__dc_client_export(DCClientExport *dcCE){
    dcCE->apply_filters_settings();
}

void update__dc_client_export(tool::cam::DCClientExport *dcCE){
    dcCE->update();
}
int read_data_from_external_thread__dc_client_export(tool::cam::DCClientExport *dcCE, int idD){
    return static_cast<int>(dcCE->read_data_from_external_thread(idD));
}

void trigger_packets_from_external_thread__dc_client_export(tool::cam::DCClientExport *dcCE, int idD){
    if(idD < dcCE->client.devices_nb()){
        dcCE->trigger_packets_from_external_tread(idD);
    }
}
void process_frames_from_external_thread__dc_client_export(tool::cam::DCClientExport *dcCE, int idD){
    if(idD < dcCE->client.devices_nb()){
        dcCE->process_frames_from_external_thread(idD);
    }
}

int is_local__dc_client_export(tool::cam::DCClientExport *dcCE, int idD){
    if(idD < dcCE->client.devices_nb()){
        return dcCE->client.settings.devicesS[idD].connectionS.connectionType == DCClientType::Local;
    }
    return 0;
}
int devices_nb__dc_client_export(DCClientExport *dcCE){
    return static_cast<int>(dcCE->devices_nb());
}
int is_device_connected__dc_client_export(DCClientExport *dcCE, int idD){
    return dcCE->is_device_connected(idD) ? 1 : 0;
}
int current_frame_id__dc_client_export(DCClientExport *dcCE, int idD){
    return static_cast<int>(dcCE->current_frame_id(idD));
}
int current_frame_cloud_size__dc_client_export(DCClientExport *dcCE, int idD){
    return static_cast<int>(dcCE->current_frame_cloud_size(idD));
}

int is_frame_available__dc_client_export(tool::cam::DCClientExport *dcCE, int idD){
    if(idD < dcCE->framesToDisplay.size()){
        return (dcCE->framesToDisplay[idD] != nullptr) ? 1 : 0;
    }
    return 0;
}
void invalidate_frame__dc_client_export(tool::cam::DCClientExport *dcCE, int idD){
    if(idD < dcCE->framesToDisplay.size()){
        dcCE->framesToDisplay[idD] = nullptr;
    }
}

void copy_transform__dc_client_export(DCClientExport *dcCE, int idD, float *transformData){
    tool::geo::Mat4f transform = dcCE->device_model(idD);
    std::copy(transform.array.begin(), transform.array.end(), transformData);
}
int copy_current_frame_vertices__dc_client_export(DCClientExport *dcCE, int idD, DCVertexMeshData *vertices, int verticesCount, int applyModelTransform){
    return static_cast<int>(dcCE->copy_current_frame_vertices(idD, {vertices, static_cast<size_t>(verticesCount)}, applyModelTransform == 1));
}
int copy_current_frame_vertices_vfx__dc_client_export(tool::cam::DCClientExport *dcCE, int idD, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, tool::geo::Pt3f *normals, int verticesCount, int applyModelTransform){
    return static_cast<int>(dcCE->copy_current_frame_vertices(
        idD,
        std::span<tool::geo::Pt3f>(positions, verticesCount),
        std::span<tool::geo::Pt3f>(colors, verticesCount),
        std::span<tool::geo::Pt3f>(normals, verticesCount),
        applyModelTransform == 1
    ));
}
