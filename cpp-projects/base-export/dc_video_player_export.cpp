

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

#include "dc_video_player_export.hpp"

// std
#include <algorithm>
#include <execution>

using namespace tool;
using namespace tool::cam;
using namespace tool::geo;

DCVideoPlayer* create__dc_video_player(){
    return new DCVideoPlayer();
}

void delete__dc_video_player(DCVideoPlayer *dcPlayer){
    delete dcPlayer;
}

int load_from_file__dc_video_player(DCVideoPlayer *dcPlayer, const char* pathDcVideoFile){
    return dcPlayer->load_from_file(pathDcVideoFile) ? 1 : 0;
}

void set_looping_state__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int doLoops){
    DCVideoPlayerSettings playerS;
    playerS.doLoop = doLoops == 1;
    dcPlayer->update_settings(playerS);
}

float duration_ms__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<float>(dcPlayer->video()->duration_ms());
}

int cameras_count__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<int>(dcPlayer->video()->nb_cameras());
}

int frames_count__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return static_cast<int>(dcPlayer->video()->nb_frames(idCamera));
}

int current_frame_id__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return static_cast<int>(dcPlayer->current_frame_id(idCamera));
}

void get_transform__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera, float *transformData){
    tool::geo::Mat4f transform = dcPlayer->video()->get_transform(idCamera).conv<float>();
    std::copy(transform.array.begin(), transform.array.end(), transformData);
}

void start_playing__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->start_playing();
}

void stop_playing__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->stop_playing();
}

void restart__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->restart();
}

int is_playing__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer){
    return dcPlayer->is_playing() ? 1 : 0;
}

void set_current_time__dc_video_player(DCVideoPlayer *dcPlayer, float timeMs){
    dcPlayer->set_current_time(timeMs);
}

float current_time_ms__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<float>(dcPlayer->current_time_ms());
}

void update__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->update();
}

int get_current_compressed_frame_valid_vertices_count__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return dcPlayer->current_compressed_frame_cloud_size(idCamera);
}

int get_current_frame_valid_vertices_count__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return dcPlayer->current_frame_cloud_size(idCamera);
}

int get_current_frames_total_cloud_size__dc_video_player(DCVideoPlayer *dcPlayer){
    return dcPlayer->current_frames_total_cloud_size();
}

int copy_camera_cloud__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera, DCVertexMeshData *vertices, int verticesCount, int applyModelTransform){
    return dcPlayer->copy_current_cloud(idCamera, std::span<DCVertexMeshData>(vertices, verticesCount), applyModelTransform == 1);
}

int copy_all_current_clouds__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform){
    return dcPlayer->copy_all_current_clouds(std::span<DCVertexMeshData>(vertices, verticesCount), applyModelTransform == 1);
}

int copy_camera_cloud_vfx__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera, tool::geo::Pt4f *positions, tool::geo::Pt4f *colors, int verticesCount, int applyModelTransform, float *minMax){
    return dcPlayer->copy_current_cloud(
        idCamera,
        std::span<tool::geo::Pt4f>(positions, verticesCount),
        std::span<tool::geo::Pt4f>(colors, verticesCount),
        applyModelTransform == 1,
        std::span<tool::geo::Pt3f,2>(reinterpret_cast<tool::geo::Pt3f*>(minMax), 2)
    );
}
