
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

#pragma once

// base
#include "depth-camera/settings/dc_delay_settings.hpp"
#include "depth-camera/client/dc_client.hpp"
#include "utility/logger.hpp"

typedef void (__stdcall * LogMessageCB)(const char*, int);
typedef void (__stdcall * NewFeedbackCB)(int,int,int);

namespace tool::cam{

struct DCClientExport{

    cam::DCClient client;

    // frames
    std::vector<std::shared_ptr<DCFrame>> framesToDisplay;

    // state
    std::vector<size_t> ids;

    // callbacks
    std::unique_ptr<LogMessageCB> logMessageCBP = nullptr;
    std::unique_ptr<NewFeedbackCB> newFeedbackCBP = nullptr;

    DCClientExport();
    ~DCClientExport();

    auto dll_log_message(const std::string &message) -> void{
        if(logMessageCBP != nullptr){
            (*logMessageCBP)(message.c_str(),0);
        }else{
            tool::Logger::message(message);
        }
    }
    auto dll_log_warning(const std::string &message) -> void{
        if(logMessageCBP != nullptr){
            (*logMessageCBP)(message.c_str(),1);
        }else{
            tool::Logger::warning(message);
        }
    }
    auto dll_log_error(const std::string &message) -> void{
        if(logMessageCBP != nullptr){
            (*logMessageCBP)(message.c_str(),2);
        }else{
            tool::Logger::error(message);
        }
    }

    auto init_callbacks(
        LogMessageCB logCB,
        NewFeedbackCB newFeedbackCB) -> void;
    auto initialize(const std::string &clientSettingsFilePath, bool startThreads) -> bool;

    // update functions
    // # main thread
    auto update() -> void;
    // # separated thread
    // ## foreach device
    auto read_network_data(size_t idDevice) -> size_t;
    auto trigger_received_packets(size_t idDevice) -> void;
    auto process_data(size_t idDevice) -> void;

    // actions
    auto connect_to_devices() -> void;
    auto disconnect_from_devices() -> void;
    auto shutdown_devices() -> void;

    // states
    auto devices_nb() const noexcept -> size_t;
    auto is_device_connected(size_t idD) const noexcept -> bool;
    auto current_frame_id(size_t idD) -> size_t;
    auto current_frame(size_t idD) -> std::shared_ptr<DCFrame>;
    auto current_frame_cloud_size(size_t idD) -> size_t;
    auto device_model(size_t idD) -> geo::Mat4f;

    // settings
    auto apply_device_settings() -> void;
    auto apply_color_settings() -> void;
    auto apply_filters_settings() -> void;
    auto update_delay(size_t idD, cam::DCDelaySettings delayS) -> void;

    // data
    auto copy_current_frame_vertices(size_t idD, std::span<tool::cam::DCVertexMeshData> vertices, bool applyModelTransform) -> size_t;
    auto copy_current_frame_vertices(size_t idD, std::span<geo::Pt3f> positions, std::span<geo::Pt3f> colors, std::span<geo::Pt3f> normals, bool applyModelTransform) -> size_t;

private:

    auto clean() -> void;
};
}

// base
#include "utility/export.hpp"

extern "C"{

// create / delete
DECL_EXPORT tool::cam::DCClientExport* create__dc_client_export();
DECL_EXPORT void delete__dc_client_export(tool::cam::DCClientExport *dcClientExport);

// init
DECL_EXPORT void init_callbacks__dc_client_export(tool::cam::DCClientExport *dcClientExport, LogMessageCB logCB, NewFeedbackCB newFeedbackCB);
DECL_EXPORT int initialize__dc_client_export(tool::cam::DCClientExport *dcClientExport, const char* networkSettingsFilePath, int startThreads);

// update
DECL_EXPORT void update__dc_client_export(tool::cam::DCClientExport *dcClientExport);
DECL_EXPORT int read_network_data__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);
DECL_EXPORT void trigger_received_packets__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);
DECL_EXPORT void process_data__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);

// actions
DECL_EXPORT void connect_to_devices__dc_client_export(tool::cam::DCClientExport *dcClientExport);
DECL_EXPORT void disconnect_from_devices__dc_client_export(tool::cam::DCClientExport *dcClientExport);

// states
DECL_EXPORT int is_local__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);
DECL_EXPORT int devices_nb__dc_client_export(tool::cam::DCClientExport *dcClientExport);
DECL_EXPORT int is_device_connected__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);
DECL_EXPORT int current_frame_id__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);
DECL_EXPORT int current_frame_cloud_size__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);

// settings
DECL_EXPORT void apply_device_settings__dc_client_export(tool::cam::DCClientExport *dcClientExport);
DECL_EXPORT void apply_color_settings__dc_client_export(tool::cam::DCClientExport *dcClientExport);
DECL_EXPORT void apply_filters_settings__dc_client_export(tool::cam::DCClientExport *dcClientExport);

// frames
DECL_EXPORT int is_frame_available__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);
DECL_EXPORT void invalidate_frame__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD);

// data
DECL_EXPORT void copy_transform__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD, float *transformData);
DECL_EXPORT int copy_current_frame_vertices__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform);
// vfx
DECL_EXPORT int copy_current_frame_vertices_vfx__dc_client_export(tool::cam::DCClientExport *dcClientExport, int idD, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, tool::geo::Pt3f *normals, int verticesCount, int applyModelTransform);

}

