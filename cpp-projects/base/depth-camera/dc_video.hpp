

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
#include "dc_compressed_frames.hpp"
#include "dc_frame_uncompressor.hpp"

namespace tool::cam{

class DCVideo{

public:

    DCVideo() = default;
    DCVideo& operator=(const DCVideo& other);

    // init / clean
    auto initialize(size_t nbDevices) -> void;
    auto clean() -> void;

    // get
    // # cameras
    auto nb_cameras() const noexcept -> size_t;
    // # calibration
    auto get_transform(size_t idCamera) const -> geo::Mat4d;
    // # times
    auto duration_ms() const noexcept -> double;
    auto first_frame_received_timestamp() const noexcept -> std::int64_t;
    auto last_frame_received_timestamp() const noexcept -> std::int64_t;
    auto camera_duration_ms(size_t idCamera) const noexcept -> double;
    auto camera_first_frame_received_timestamp(size_t idCamera) const noexcept -> std::int64_t;
    auto camera_last_frame_received_timestamp(size_t idCamera) const noexcept -> std::int64_t;
    static auto get_timestamp_diff_time_ms(std::int64_t t1, std::int64_t t2) noexcept -> double;
    // # frames
    auto nb_frames(size_t idCamera) const noexcept -> size_t;
    auto count_frames_from_all_cameras() const noexcept -> size_t;
    auto min_nb_frames() const noexcept -> size_t;
    auto closest_frame_id_from_time(size_t idCamera, double timeMs) const noexcept -> std::int64_t;
    auto get_compressed_frame(size_t idCamera, size_t idFrame) -> std::weak_ptr<DCCompressedFrame>;
    auto get_compressed_frames_ptr(size_t idCamera) const noexcept -> const DCCompressedFrames*;
    auto uncompressor(size_t idCamera) noexcept -> DCFrameUncompressor*;
    // # audio
    auto total_audio_frames_size(size_t idCamera) const -> size_t;
    auto get_audio_samples_all_channels(size_t idCamera, std::vector<std::vector<float>> &audioBuffer) -> void;
    auto get_audio_samples_all_channels(size_t idCamera, std::vector<float> &audioBuffer) -> void;

    // uncomress
    auto uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idCamera, size_t idFrame, DCFrame &frame) -> bool;
    auto uncompress_frame(const DCFrameGenerationSettings &gSettings, size_t idCamera, DCCompressedFrame *cFrame, DCFrame &frame) -> bool;

    // modify
    // # remove cameras/frames
    auto keep_only_one_camera(size_t idCamera) -> void;
    auto keep_only_cameras_from_id(const std::vector<size_t> &ids) -> void;
    auto remove_all_cameras_compressed_frames() noexcept -> void;
    // # modify cameras/frames
    auto merge_all_cameras(const DCFrameGenerationSettings &gSettings, float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void;
    auto merge_cameras_frame_id(const DCFrameGenerationSettings &gSettings, size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, cam::DCFrame &frame) -> void;
    auto merge_cameras_frame_id(const DCFrameGenerationSettings &gSettings, size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, tool::geo::ColoredCloudData &cloud) -> void;
    // # add/remove
    auto add_compressed_frame(size_t idCamera, std::shared_ptr<DCCompressedFrame> frame) -> void;
    auto remove_compressed_frames_until(size_t idCamera, size_t idFrame) -> void;
    auto remove_compressed_frames_after(size_t idCamera, size_t idFrame) -> void;
    auto remove_all_compressed_frames(size_t idCamera) noexcept -> void;


    // # calibration
    auto set_transform(size_t idCamera, geo::Mat4d tr) -> void;

    // i/o
    auto save_to_file(std::string_view path) -> bool;
    auto load_from_file(std::string_view path) -> bool;   

protected:

    std::vector<std::unique_ptr<DCFrameUncompressor>> m_camerasUncompressors;
    std::vector<DCCompressedFrames> m_camerasCompressedFrames;
    std::vector<geo::Mat4d> m_camerasTransforms;

private:

    // i/o files
    auto read_file(std::ifstream &file) -> bool;
    auto write_file(std::ofstream &file) -> void;
    // # legacy
    auto read_legacy_cloud_video_file(std::ifstream &file) -> void;
    auto read_legacy_full_video_file(std::ifstream &file) -> void;
};

}
