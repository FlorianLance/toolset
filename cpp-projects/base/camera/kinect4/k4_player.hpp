
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
#include "utility/stop_watch.hpp"
#include "camera/kinect4/k4_volumetric_video.hpp"
#include "camera/kinect4/k4_player_settings.hpp"
#include "camera/kinect4/k4_model.hpp"

namespace tool::camera {

class K4Player{
public:

    // update
    auto update_time() -> void;
    auto update_frames() ->void;

    // video
    auto video() -> K4VolumetricVideo*;
    auto set_video(const K4VolumetricVideo &video) -> void;
    auto display_infos() -> void;;

    // frames
    auto uncompress_frame(size_t idCamera, K4Frame &frame) -> bool;
    auto current_frame_id(size_t idCamera) const -> size_t;
    auto current_frame_id_capture(size_t idCamera) const -> size_t;    

    // navigation
    auto is_playing() const -> bool;
    auto start_playing() -> void;
    auto stop_playing() -> void;
    auto restart() -> void;
    auto set_current_time(double timeMs) -> void;
    auto current_time_ms() const noexcept -> double;
    auto is_looping() const noexcept -> bool;

    // edit
    auto remove_until_current_frame() -> void;
    auto remove_after_current_frame() -> void;
    auto merge() -> void;
    auto merge_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void;  
    auto remove_empty_cameras() -> void;

    // settings
    auto update_settings(const K4PlayerSettings &playerS) noexcept -> void;

    // i/o
    auto load_from_file(std::string_view path) -> bool;
    auto save_to_file(std::string_view path) -> bool;
    auto merge_before(K4VolumetricVideo &other) -> void;
    auto save_cloud_to_file(std::string_view path) -> bool;

    // signals
    sigslot::signal<std::vector<K4Model>> initialize_signal;
    sigslot::signal<K4PlayerStates> states_updated_signal;
    sigslot::signal<size_t, std::shared_ptr<K4Frame>> new_frame_signal;

private:

    auto update_states() -> void;

    K4PlayerStates m_states;
    K4PlayerSettings m_settings;

    StopWatch m_sw;
    std::vector<std::shared_ptr<K4CompressedFrame>> m_currentCompressedFrames;
    std::vector<std::shared_ptr<K4Frame>> m_currentFrames;
    K4VolumetricVideo m_videoResource;
};
}
