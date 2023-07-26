
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
#include "k4_recorder_settings.hpp"
#include "k4_model.hpp"
#include "camera/kinect4/k4_volumetric_video.hpp"

namespace tool::camera {

class K4Recorder{
public:

    auto initialize(size_t nbGrabbers) -> void;

    // update
    auto update_frames() -> void;

    // video
    auto video() -> K4VolumetricVideo*;

    // frames
    auto uncompress_frame(size_t idCamera, K4Frame &frame) -> bool;
    auto add_compressed_frame_to_default_camera(std::shared_ptr<K4CompressedFrame> frame) -> void;
    auto add_compressed_frame(size_t idCamera, std::shared_ptr<K4CompressedFrame> frame) -> void;    

    // recordings
    auto is_recording() const noexcept -> bool;
    auto start_recording() -> void;
    auto stop_recording() -> void;
    auto reset_recording() -> void;
    auto set_time(double timeMs) -> void;

    // settings
    auto update_settings(K4RecorderSettings recordingsS) noexcept -> void;
    auto update_model(size_t id, const camera::K4Model &model) -> void;

    // i/o
    auto save_to_file(std::string_view path) -> bool{return m_videoResource.save_to_file(path);}

    // signals
    sigslot::signal<K4RecorderStates> states_updated_signal;
    sigslot::signal<size_t, std::shared_ptr<camera::K4Frame>> new_frame_signal;

private:

    std::vector<std::shared_ptr<K4CompressedFrame>> m_currentCompressedFrames;
    std::vector<std::shared_ptr<K4Frame>> m_currentFrames;

    K4RecorderStates m_states;
    K4RecorderSettings m_settings;

    StopWatch m_sw;
    K4VolumetricVideo m_videoResource;
};
}
