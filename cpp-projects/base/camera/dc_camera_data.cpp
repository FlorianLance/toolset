
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


#include "dc_camera_data.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

using namespace tool::camera;

//DCCameraData &DCCameraData::operator=(const DCCameraData &other){
//    this->transform = other.transform;
//    this->frames.reserve(other.frames.size());
//    for(const auto &frame : other.frames){
//        auto cf = std::make_shared<DCCompressedFrame>();
//        *cf = *frame;
//        this->frames.push_back(std::move(cf));
//    }
//    return *this;
//}

auto DCCameraData::nb_frames() const noexcept -> size_t{
    return frames.size();
}

auto DCCameraData::first_frame_ptr() const -> DCCompressedFrame*{
    if(!frames.empty()){
        return frames.front().get();
    }
    Logger::error("[DCCameraData::first_frame] No frame available.\n");
    return nullptr;
}

auto DCCameraData::last_frame_ptr() const -> DCCompressedFrame*{
    if(!frames.empty()){
        return frames.back().get();
    }
    Logger::error("[DCCameraData::last_frame] No frame available.\n");
    return nullptr;
}

auto DCCameraData::get_frame_ptr(size_t idFrame) const -> DCCompressedFrame*{
    if(idFrame < nb_frames()){
        return frames[idFrame].get();
    }
    Logger::error(std::format("[DCCameraData::get_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return nullptr;
}

auto DCCameraData::first_frame_capture_timestamp() const noexcept -> std::int64_t {
    if(!frames.empty()){
        return frames.front()->afterCaptureTS;
    }
    return -1;
}

auto DCCameraData::last_frame_capture_timestamp() const noexcept ->  std::int64_t {
    if(!frames.empty()){
        return frames.back()->afterCaptureTS;
    }
    return -1;
}

auto DCCameraData::valid_vertices_count(size_t idFrame) const noexcept -> size_t{
    if(auto frame = get_frame_ptr(idFrame)){
        return frame->validVerticesCount;
    }
    return 0;
}


auto DCCameraData::get_compressed_frame(size_t idFrame) const noexcept -> std::weak_ptr<DCCompressedFrame>{
    if(idFrame < nb_frames()){
        return frames[idFrame];
    }
    Logger::error(std::format("[DCCameraData::get_compressed_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return {};
}

auto DCCameraData::add_compressed_frame(std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(!frames.empty()){
        if(frame->idCapture == frames.back()->idCapture){
            return;
        }
    }
    frames.push_back(std::move(frame));
}


auto DCCameraData::remove_frames_until(size_t idFrame) -> void{
    if(idFrame < nb_frames()){
        frames.erase(frames.begin(), frames.begin() + idFrame);
    }
}

auto DCCameraData::remove_frames_after(size_t idFrame) -> void{
    if(idFrame < nb_frames()){
        frames.erase(frames.begin()+idFrame, frames.end());
    }
}

auto DCCameraData::clean() noexcept -> void{
    frames.clear();
}
