
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
#include "depth-camera/dc_video_player.hpp"

// local
#include "dc_video_recorder_export.hpp"

extern "C"{

    DECL_EXPORT tool::cam::DCVideoPlayer* create__dc_video_player();
    DECL_EXPORT void delete__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT int load_from_file__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, const char* pathDcVideoFile);
    DECL_EXPORT void set_video_from_recorder__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::cam::DCVideoRecorderExport *dcRecorderExport);

    DECL_EXPORT float duration_ms__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT float start_time_ms__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT float end_time_ms__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT int is_started__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT int is_playing__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT int is_looping__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT float current_time_ms__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);

    DECL_EXPORT void start_video__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT void stop_video__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT void pause_video__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT void unpause_video__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT void go_to_start_time__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT void go_to_end_time__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT void set_current_time__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, float timeMs);
    DECL_EXPORT void set_player_settings__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int doLoops, float startTimeMs, float endTimeMs);

    DECL_EXPORT void update__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);

    DECL_EXPORT int cameras_count__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);
    DECL_EXPORT int get_max_frame_cloud_size__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera);
    DECL_EXPORT int frames_count__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera);
    DECL_EXPORT int current_frame_id__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera);
    DECL_EXPORT void get_transform__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera, float *transformData);

    DECL_EXPORT int get_current_compressed_frame_valid_vertices_count__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera);
    DECL_EXPORT int get_current_frame_valid_vertices_count__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera);
    DECL_EXPORT int get_current_frames_total_cloud_size__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer);

    DECL_EXPORT int copy_camera_cloud__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform);
    DECL_EXPORT int copy_all_current_clouds__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::cam::DCVertexMeshData *vertices, int verticesCount, int applyModelTransform);
    DECL_EXPORT int copy_voxels_cloud_vfx__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, int verticesCount);
    DECL_EXPORT int copy_camera_cloud_vfx__dc_video_player(tool::cam::DCVideoPlayer *dcPlayer, int idCamera, tool::geo::Pt3f *positions, tool::geo::Pt3f *colors, tool::geo::Pt3f *normals, int verticesCount, int applyModelTransform);
}
