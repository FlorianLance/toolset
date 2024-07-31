
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


#include "dc_compressed_frame_buffer.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

using namespace tool::cam;

auto DCCompressedFrameBuffer::nb_frames() const noexcept -> size_t{
    return frames.size();
}

auto DCCompressedFrameBuffer::first_frame_ptr() const -> DCCompressedFrame*{
    if(!frames.empty()){
        return frames.front().get();
    }
    Logger::error("[DCCameraData::first_frame] No frame available.\n");
    return nullptr;
}

auto DCCompressedFrameBuffer::last_frame_ptr() const -> DCCompressedFrame*{
    if(!frames.empty()){
        return frames.back().get();
    }
    Logger::error("[DCCameraData::last_frame] No frame available.\n");
    return nullptr;
}

auto DCCompressedFrameBuffer::get_frame_ptr(size_t idFrame) const -> DCCompressedFrame*{
    if(idFrame < nb_frames()){
        return frames[idFrame].get();
    }
    Logger::error(std::format("[DCCameraData::get_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return nullptr;
}

auto DCCompressedFrameBuffer::first_frame_received_timestamp() const noexcept -> std::int64_t {
    if(!frames.empty()){
        return frames.front()->receivedTS;
    }
    return -1;
}

auto DCCompressedFrameBuffer::last_frame_received_timestamp() const noexcept ->  std::int64_t {
    if(!frames.empty()){
        return frames.back()->receivedTS;
    }
    return -1;
}

auto DCCompressedFrameBuffer::duration_ms() const noexcept -> double {
    using namespace std::chrono;
    auto lfc = last_frame_received_timestamp();
    auto ffc = first_frame_received_timestamp();
    if(lfc != -1&& ffc != -1 ){
        return duration_cast<microseconds>(nanoseconds(lfc-ffc)).count()*0.001;
    }
    return 0.0;
}

auto DCCompressedFrameBuffer::valid_vertices_count(size_t idFrame) const noexcept -> size_t{
    if(auto frame = get_frame_ptr(idFrame)){
        return frame->validVerticesCount;
    }
    return 0;
}


auto DCCompressedFrameBuffer::get_compressed_frame(size_t idFrame) const noexcept -> std::weak_ptr<DCCompressedFrame>{
    if(idFrame < nb_frames()){
        return frames[idFrame];
    }
    Logger::error(std::format("[DCCameraData::get_compressed_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return {};
}

auto DCCompressedFrameBuffer::add_compressed_frame(std::shared_ptr<DCCompressedFrame> frame) -> void{
    if(!frames.empty()){
        if(frame->idCapture == frames.back()->idCapture){
            return;
        }
    }
    frames.push_back(std::move(frame));
}


auto DCCompressedFrameBuffer::remove_frames_until(size_t idFrame) -> void{
    frames.remove_before(idFrame);
}

auto DCCompressedFrameBuffer::remove_frames_after(size_t idFrame) -> void{
    frames.remove_after(idFrame);
}

auto DCCompressedFrameBuffer::clean() noexcept -> void{
    frames.clear();
}
