

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
#include "geometry/matrix4.hpp"
#include "dc_compressed_frame.hpp"

namespace tool::cam{

class DCVolumetricVideo;

struct DCCameraData{

    DCCameraData() = default;
//    DCCameraData& operator=(const DCCameraData& other); // TODO: ...

    // getters
    auto nb_frames() const noexcept -> size_t;
    auto valid_vertices_count(size_t idFrame) const noexcept -> size_t;
    auto first_frame_capture_timestamp() const noexcept -> std::int64_t;
    auto last_frame_capture_timestamp() const noexcept -> std::int64_t;

    // modifiers
    auto add_compressed_frame(std::shared_ptr<DCCompressedFrame> frame) -> void;
    auto get_compressed_frame(size_t idFrame) const noexcept -> std::weak_ptr<DCCompressedFrame>;
    auto remove_frames_until(size_t idFrame) -> void;
    auto remove_frames_after(size_t idFrame) -> void;
    auto clean() noexcept -> void;

    friend DCVolumetricVideo;

    auto check_same_mode_for_every_frame() const noexcept -> bool{
        if(frames.empty()){
            return true;
        }
        auto mode = frames.front()->mode;
        for(const auto &frame : frames){
            if(mode != frame->mode){
                return false;
            }
        }
        return true;
    }

private:

    auto first_frame_ptr() const -> DCCompressedFrame*;
    auto last_frame_ptr() const -> DCCompressedFrame*;
    auto get_frame_ptr(size_t idFrame) const -> DCCompressedFrame*;

public:

    geo::Mat4d transform = geo::Mat4d::identity();
    std::vector<std::shared_ptr<DCCompressedFrame>> frames;
};

}
