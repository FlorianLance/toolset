
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
#include "depth-camera/client/dc_client.hpp"


typedef void (__stdcall * LogMessageCB)(const char*, int);
typedef void (__stdcall * NewFeedbackCB)(int,int,int);


namespace tool::cam{

class DCClientExport{
public:

    DCClientExport(){
        // set connections
        // ...
    }

    auto init_callbacks(LogMessageCB logCB) -> void{
        // ...
    }

    DCClient client;
};
}

// base
#include "utility/export.hpp"

extern "C"{

    DECL_EXPORT tool::cam::DCClientExport* create__dc_client();
    DECL_EXPORT void delete__dc_client(tool::cam::DCClientExport *dcClientE);
    DECL_EXPORT int initialize__dc_client(tool::cam::DCClientExport *dcClientE, const char* clientSettingsPath);
    DECL_EXPORT void init_callbacks__dc_client(tool::cam::DCClientExport *dcClientE, LogMessageCB logCB);

    DECL_EXPORT void update__dc_client(tool::cam::DCClientExport *dcClientE);
    DECL_EXPORT void process_frames__dc_client(tool::cam::DCClientExport *dcClientE);
    DECL_EXPORT void read_network_data__dc_client(tool::cam::DCClientExport *dcClientE, size_t idServer){}
}


// DECL_EXPORT int is_local__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// DECL_EXPORT int read_network_data__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// DECL_EXPORT int retrieve_last_frame__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// // actions
// DECL_EXPORT void connect_to_devices__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
// DECL_EXPORT void disconnect_from_devices__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
// // states
// DECL_EXPORT int devices_nb__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer);
// DECL_EXPORT int is_device_connected__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// DECL_EXPORT int current_frame_id__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// DECL_EXPORT int current_frame_cloud_size__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD);
// // settings
// DECL_EXPORT int update_device_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* deviceSettingsFilePath);
// DECL_EXPORT int update_color_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* colorSettingsFilePath);
// DECL_EXPORT int update_filters_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* filtersSettingsFilePath);
// DECL_EXPORT int update_model_settings__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, const char* modelSettingsFilePath);
// // data
// DECL_EXPORT void copy_transform__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, float *transformData);
// DECL_EXPORT int copy_current_frame_vertices__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform);
// // vfx
// DECL_EXPORT int copy_current_frame_vertices_vfx__dc_network_direct_player(tool::cam::DCNetworkDirectPlayer *dcNetworkDirectPlayer, int idD, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, tool::geo::Pt3f *normals, int verticesCount, int applyModelTransform);

