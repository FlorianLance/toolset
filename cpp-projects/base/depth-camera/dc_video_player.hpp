
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

#pragma once

// local
#include "thirdparty/sigslot/signal.hpp"
#include "settings/dc_video_player_settings.hpp"
#include "settings/dc_model_settings.hpp"
#include "states/dc_video_player_states.hpp"
#include "dc_video.hpp"

namespace tool::cam {

class DCVideoPlayer{
public:

    DCVideoPlayer();
    ~DCVideoPlayer();
    auto update() -> void;

    // video
    auto set_video(const DCVideo &video) -> void;
    auto video() -> DCVideo*;
    auto display_infos() -> void;;

    // navigation
    auto start_video() noexcept -> void;
    auto stop_video() noexcept -> void;
    auto pause_video() noexcept -> void;
    auto unpause_video() noexcept -> void;
    auto go_to_start_time() noexcept -> void;
    auto go_to_end_time() noexcept -> void;
    auto set_current_time(double timeMs) noexcept -> void;

    auto is_started() const noexcept -> bool;
    auto is_playing() const noexcept -> bool;
    auto duration_ms() const noexcept -> double;
    auto current_time_ms() const noexcept -> double;
    auto is_looping() const noexcept -> bool;    
    auto start_time_ms() const noexcept -> double;
    auto end_time_ms() const noexcept -> double;

    // frames
    auto current_frame_id(size_t idCamera) const -> size_t;
    auto current_compressed_frame_cloud_size(size_t idCamera) -> size_t;
    auto current_frame_cloud_size(size_t idCamera) -> size_t;
    auto current_frame(size_t idCamera) -> std::shared_ptr<DCFrame>;
    auto current_frames_total_cloud_size() -> size_t;
    auto copy_current_cloud(size_t idCamera, std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t;
    auto copy_current_cloud(size_t idCamera, std::span<geo::Pt3f> positions, std::span<geo::Pt3f> colors, std::span<geo::Pt3f> normals, bool applyModelTransform, std::span<geo::Pt3f,2> minMax) -> size_t;
    auto copy_all_current_clouds(std::span<DCVertexMeshData> vertices, bool applyModelTransform) -> size_t;

    // edit
    auto remove_until_current_frame() -> void;
    auto remove_after_current_frame() -> void;
    auto merge() -> void;
    auto merge_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void;
    auto remove_empty_cameras() -> void;

    // settings
    auto update_settings(const DCVideoPlayerSettings &playerS) noexcept -> void;

    // i/o
    auto load_from_file(std::string_view path) -> bool;
    auto save_to_file(std::string_view path) -> bool;
    auto merge_before(DCVideo &other) -> void;
    auto save_cloud_to_file(std::string_view path) -> bool;        

    // signals
    sigslot::signal<std::vector<DCModelSettings>> initialize_signal;
    sigslot::signal<DCVideoPlayerStates> states_updated_signal;
    sigslot::signal<size_t, std::shared_ptr<DCFrame>> new_frame_signal;

private:

    auto update_states() noexcept -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
