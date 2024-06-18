
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
#include "depth-camera/dc_server_data.hpp"
#include "depth-camera/network/dc_server_network.hpp"
#include "depth-camera/settings/dc_model_settings.hpp"
#include "depth-camera/settings/dc_delay_settings.hpp"

typedef void (__stdcall * LogMessageCB)(const char*, int);
typedef void (__stdcall * NewFeedbackCB)(int,int,int);


namespace tool::cam{

struct DeviceData{
    bool connected = false;
    size_t id = 0;
    cam::DCFiltersSettings filters;
    cam::DCDeviceSettings device = cam::DCDeviceSettings::default_init_for_manager();
    cam::DCColorSettings color;
    cam::DCModelSettings model;
    cam::DCDelaySettings delay;

    std::unique_ptr<std::mutex> feedbackLocker = nullptr;
    size_t nbFeedbacksReceived =0;
    std::vector<net::Feedback> feedbacks;

    std::unique_ptr<std::mutex> frameLocker = nullptr;
    size_t nbFramesReceived =0;
    size_t nbFramesProcessed =0;
    std::shared_ptr<DCCompressedFrame> lastCompressedFrame = nullptr;
    std::shared_ptr<DCFrame> lastFrame = nullptr;
    std::shared_ptr<DCFrame> frameToDisplay = nullptr;
};

struct DCNetworkDirectPlayer{

    // network
    net::DCServerNetwork serverNetwork;
    net::UdpServerNetworkSettings serverNetworkS;

    // data
    DCServerData serverData;
    std::vector<DeviceData> devicesD;

    // state
    std::vector<size_t> ids;

    // callbacks
    std::unique_ptr<LogMessageCB> logMessageCBP = nullptr;
    std::unique_ptr<NewFeedbackCB> newFeedbackCBP = nullptr;

    DCNetworkDirectPlayer();
    ~DCNetworkDirectPlayer();

    auto dll_log_message(const std::string &message) -> void{
        (*logMessageCBP)(message.c_str(),0);
    }
    auto dll_log_warning(const std::string &message) -> void{
        (*logMessageCBP)(message.c_str(),1);
    }
    auto dll_log_error(const std::string &message) -> void{
        (*logMessageCBP)(message.c_str(),2);
    }

    auto init_callbacks(
        LogMessageCB logCB,
        NewFeedbackCB newFeedbackCB) -> void;
    auto initialize(const std::string &networkSettingsFilePath) -> bool;

    // function to be called in a while loop in a dedicated thread
    auto read_network_data(size_t idDevice) -> size_t;

    // call every frame
    auto uncompress_frame(size_t idDevice) -> bool;

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
    auto update_device_settings(const std::string &deviceSettingsFilePath) -> bool;
    auto update_color_settings(const std::string &colorSettingsFilePath) -> bool;
    auto update_filters_settings(const std::string &filtersSettingsFilePath) -> bool;    
    auto update_model_settings(const std::string &modelSettingsFilePath) -> bool;
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
DECL_EXPORT tool::cam::DCNetworkDirectPlayer* create__dc_network_direct_player();
DECL_EXPORT void delete__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
DECL_EXPORT void init_callbacks__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer,
    LogMessageCB logCB,
    NewFeedbackCB newFeedbackCB);

DECL_EXPORT int initialize__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* networkSettingsFilePath);
DECL_EXPORT int read_network_data__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
DECL_EXPORT int uncompress_frame__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// actions
DECL_EXPORT void connect_to_devices__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
DECL_EXPORT void disconnect_from_devices__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
// states
DECL_EXPORT int devices_nb__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
DECL_EXPORT int is_device_connected__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
DECL_EXPORT int current_frame_id__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
DECL_EXPORT int current_frame_cloud_size__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// settings
DECL_EXPORT int update_device_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* deviceSettingsFilePath);
DECL_EXPORT int update_color_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* colorSettingsFilePath);
DECL_EXPORT int update_filters_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* filtersSettingsFilePath);
DECL_EXPORT int update_model_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* modelSettingsFilePath);
// data
DECL_EXPORT void copy_transform__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, float *transformData);
DECL_EXPORT int copy_current_frame_vertices__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform);
// vfx
DECL_EXPORT int copy_current_frame_vertices_vfx__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, tool::geo::Pt3f *normals, int verticesCount, int applyModelTransform);

}

