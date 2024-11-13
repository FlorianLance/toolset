

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
void set_video_from_recorder__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::cam::DCVideoRecorderExport *dcRecorderExport){
    dcPlayer->set_video(*dcRecorderExport->recorder.video());
}

int is_started__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer){
    return dcPlayer->is_started() ? 1 : 0;
}
int is_playing__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer){
    return dcPlayer->is_playing() ? 1 : 0;
}
int is_looping__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer){
    return dcPlayer->is_looping() ? 1 : 0;
}
float current_time_ms__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<float>(dcPlayer->current_time_ms());
}
float duration_ms__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<float>(dcPlayer->duration_ms());
}
float start_time_ms__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer){
    return static_cast<float>(dcPlayer->start_time_ms());
}
float end_time_ms__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer){
    return static_cast<float>(dcPlayer->end_time_ms());
}

void start_video__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->start_video();
}
void stop_video__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->stop_video();
}
void pause_video__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->pause_video();
}
void unpause_video__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->unpause_video();
}
void go_to_start_time__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->go_to_start_time();
}
void go_to_end_time__dc_video_player(DCVideoPlayer *dcPlayer){
    dcPlayer->go_to_end_time();
}
void set_current_time__dc_video_player(DCVideoPlayer *dcPlayer, float timeMs){
    dcPlayer->set_current_time(timeMs);
}
void set_player_settings__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int doLoops, float startTimeMs, float endTimeMs){
    DCVideoPlayerSettings playerS;
    playerS.doLoop = doLoops == 1;
    playerS.startTimeMs = startTimeMs;
    playerS.endTimeMs = endTimeMs;
    dcPlayer->update_settings(playerS);
}
void update__dc_video_player(DCVideoPlayer *dcPlayer){
    if(dcPlayer){
        dcPlayer->update();
    }
}

void get_transform__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera, float *transformData){
    tool::geo::Mat4f transform = dcPlayer->video()->get_transform(idCamera).conv<float>();
    std::copy(transform.array.begin(), transform.array.end(), transformData);
}
int cameras_count__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<int>(dcPlayer->video()->nb_devices());
}
int get_max_frame_cloud_size__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera){

    size_t maxNbFrames = 0;
    for(size_t idF = 0; idF < dcPlayer->video()->nb_frames(idCamera); ++idF){
        if(auto df = dcPlayer->video()->get_data_frame(idCamera, idF).lock(); df){
            if(auto size = df->volume_buffer_size(DCVolumeBufferType::ColoredCloud); size != 0){
                if(size > maxNbFrames){
                    maxNbFrames = size;
                }
            }else if(auto size = df->volume_buffer_size(DCVolumeBufferType::VoxelCloudX14Y12Z14RGB8); size != 0){
                if(size > maxNbFrames){
                    maxNbFrames = size;
                }
            }
        }
    }
    return maxNbFrames;
}

int frames_count__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return static_cast<int>(dcPlayer->video()->nb_frames(idCamera));
}
int current_frame_id__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return static_cast<int>(dcPlayer->current_frame_id(idCamera));
}
int get_current_compressed_frame_valid_vertices_count__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return static_cast<int>(dcPlayer->current_compressed_frame_cloud_size(static_cast<size_t>(idCamera)));
}
int get_current_frame_valid_vertices_count__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera){
    return static_cast<int>(dcPlayer->current_frame_cloud_size(static_cast<size_t>(idCamera)));
}
int get_current_frames_total_cloud_size__dc_video_player(DCVideoPlayer *dcPlayer){
    return static_cast<int>(dcPlayer->current_frames_total_cloud_size());
}

int copy_camera_cloud__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera, DCVertexMeshData *vertices, int verticesCount, int applyModelTransform){
    return static_cast<int>(dcPlayer->copy_current_cloud(idCamera, std::span<DCVertexMeshData>(vertices, verticesCount), applyModelTransform == 1));
}
int copy_all_current_clouds__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform){
    return static_cast<int>(dcPlayer->copy_all_current_clouds(std::span<DCVertexMeshData>(vertices, verticesCount), applyModelTransform == 1));
}
int copy_voxels_cloud_vfx__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, int verticesCount){
    return static_cast<int>(dcPlayer->copy_current_cloud(
        0,
        std::span<tool::geo::Pt3f>(positions, verticesCount),
        std::span<tool::geo::Pt3f>(colors, verticesCount)
    ));
}
int copy_camera_cloud_vfx__dc_video_player(DCVideoPlayer *dcPlayer, int idCamera, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, tool::geo::Pt3f *normals, int verticesCount, int applyModelTransform){
    return static_cast<int>(dcPlayer->copy_current_cloud(
        idCamera,
        std::span<tool::geo::Pt3f>(positions, verticesCount),
        std::span<tool::geo::Pt3f>(colors, verticesCount),
        std::span<tool::geo::Pt3f>(normals, verticesCount),
        applyModelTransform == 1
    ));
}
