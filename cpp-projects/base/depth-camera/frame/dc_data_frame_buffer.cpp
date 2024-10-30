
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


#include "dc_data_frame_buffer.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

using namespace tool::cam;

auto DCDataFrameBuffer::nb_frames() const noexcept -> size_t{
    return frames.size();
}

auto DCDataFrameBuffer::first_frame_ptr() const -> DCDataFrame*{
    if(!frames.empty()){
        return frames.front().get();
    }
    Log::error("[DCCameraData::first_frame] No frame available.\n");
    return nullptr;
}

auto DCDataFrameBuffer::last_frame_ptr() const -> DCDataFrame*{
    if(!frames.empty()){
        return frames.back().get();
    }
    Log::error("[DCCameraData::last_frame] No frame available.\n");
    return nullptr;
}

auto DCDataFrameBuffer::get_frame_ptr(size_t idFrame) const -> DCDataFrame*{
    if(idFrame < nb_frames()){
        return frames[idFrame].get();
    }
    Log::error(std::format("[DCCameraData::get_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return nullptr;
}

auto DCDataFrameBuffer::first_frame_timestamp() const noexcept -> std::int64_t {
    if(!frames.empty()){
        return frames.front()->afterCaptureTS;
    }
    return -1;
}

auto DCDataFrameBuffer::last_frame_timestamp() const noexcept ->  std::int64_t {
    if(!frames.empty()){
        return frames.back()->afterCaptureTS;
    }
    return -1;
}

auto DCDataFrameBuffer::duration_ms() const noexcept -> double {
    using namespace std::chrono;
    auto lfc = last_frame_timestamp();
    auto ffc = first_frame_timestamp();
    if(lfc != -1&& ffc != -1 ){
        return duration_cast<microseconds>(nanoseconds(lfc-ffc)).count()*0.001;
    }
    return 0.0;
}

auto DCDataFrameBuffer::valid_vertices_count(size_t idFrame) const noexcept -> size_t{
    if(auto frame = get_frame_ptr(idFrame)){
        return frame->validVerticesCount;
    }
    return 0;
}


auto DCDataFrameBuffer::get_data_frame(size_t idFrame) const noexcept -> std::weak_ptr<DCDataFrame>{
    if(idFrame < nb_frames()){
        return frames[idFrame];
    }
    Log::error(std::format("[DCCameraData::get_data_frame] Invalid frame id [{}], current number of frames [{}].\n", idFrame, nb_frames()));
    return {};
}

auto DCDataFrameBuffer::add_data_frame(std::shared_ptr<DCDataFrame> frame) -> void{
    if(!frames.empty()){
        if(frame->idCapture == frames.back()->idCapture){
            return;
        }
    }
    frames.push_back(std::move(frame));
}


auto DCDataFrameBuffer::remove_frames_until(size_t idFrame) -> void{
    frames.remove_before(idFrame);
}

auto DCDataFrameBuffer::remove_frames_after(size_t idFrame) -> void{
    frames.remove_after(idFrame);
}

auto DCDataFrameBuffer::clean() noexcept -> void{
    frames.clear();
}
