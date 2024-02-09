
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


#include "dc_compressed_frames.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

using namespace tool::cam;

//DCCompressedFrames &DCCompressedFrames::operator=(const DCCompressedFrames &other){
//    this->transform = other.transform;
//    this->frames.reserve(other.frames.size());
//    for(const auto &frame : other.frames){
//        auto cf = std::make_shared<DCCompressedFrame>();
//        *cf = *frame;
//        this->frames.push_back(std::move(cf));
//    }
//    return *this;
//}

auto DCCompressedFrames::nb_frames() const noexcept -> size_t{
    return frames.size();
}

auto DCCompressedFrames::first_frame_ptr() const -> DCCompressedFrame*{
    if(!frames.empty()){
        return frames.front().get();
    }
    Logger::error("[DCCameraData::first_frame] No frame available.\n");
    return nullptr;
}

auto DCCompressedFrames::last_frame_ptr() const -> DCCompressedFrame*{
    if(!frames.empty()){
        return frames.back().get();
    }
    Logger::error("[DCCameraData::last_frame] No frame available.\n");
    return nullptr;
}

auto DCCompressedFrames::get_frame_ptr(size_t idFrame) const -> DCCompressedFrame*{
    if(idFrame < nb_frames()){
        return frames[idFrame].get();
    }
    Logger::error(std::format("[DCCameraData::get_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return nullptr;
}

auto DCCompressedFrames::first_frame_received_timestamp() const noexcept -> std::int64_t {
    if(!frames.empty()){
        return frames.front()->receivedTS;
    }
    return -1;
}

auto DCCompressedFrames::last_frame_received_timestamp() const noexcept ->  std::int64_t {
    if(!frames.empty()){
        return frames.back()->receivedTS;
    }
    return -1;
}

auto DCCompressedFrames::duration_ms() const noexcept -> double {
    using namespace std::chrono;
    auto lfc = last_frame_received_timestamp();
    auto ffc = first_frame_received_timestamp();
    if(lfc != -1&& ffc != -1 ){
        return duration_cast<microseconds>(nanoseconds(lfc-ffc)).count()*0.001;
    }
    return 0.0;
}

auto DCCompressedFrames::valid_vertices_count(size_t idFrame) const noexcept -> size_t{
    if(auto frame = get_frame_ptr(idFrame)){
        return frame->validVerticesCount;
    }
    return 0;
}


auto DCCompressedFrames::get_compressed_frame(size_t idFrame) const noexcept -> std::weak_ptr<DCCompressedFrame>{
    if(idFrame < nb_frames()){
        return frames[idFrame];
    }
    Logger::error(std::format("[DCCameraData::get_compressed_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return {};
}

auto DCCompressedFrames::add_compressed_frame(std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(!frames.empty()){
        if(frame->idCapture == frames.back()->idCapture){
            return;
        }
    }
    frames.push_back(std::move(frame));
}


auto DCCompressedFrames::remove_frames_until(size_t idFrame) -> void{
    if(idFrame < nb_frames()){
        frames.erase(frames.begin(), frames.begin() + idFrame);
    }
}

auto DCCompressedFrames::remove_frames_after(size_t idFrame) -> void{
    if(idFrame < nb_frames()){
        frames.erase(frames.begin()+idFrame, frames.end());
    }
}

auto DCCompressedFrames::clean() noexcept -> void{
    frames.clear();
}
