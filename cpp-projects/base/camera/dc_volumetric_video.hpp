

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
#include "dc_camera_data.hpp"
#include "dc_frame_uncompressor.hpp"

namespace tool::cam{

class DCVolumetricVideo{

public:

    DCVolumetricVideo() = default;
    DCVolumetricVideo& operator=(const DCVolumetricVideo& other);

    auto initialize(size_t nbDevices) -> void;

    // cameras
    auto nb_cameras() const noexcept -> size_t;
    auto get_camera_data(size_t idCamera) const noexcept -> const DCCameraData*;

    // times
    auto first_frame_capture_timestamp() const noexcept -> std::int64_t;
    auto last_frame_capture_timestamp() const noexcept -> std::int64_t;
    auto duration_ms() const noexcept -> double;
    static auto get_timestamp_diff_time_ms(std::int64_t t1, std::int64_t t2) noexcept -> double;

    // frames
    auto nb_frames(size_t idCamera) const noexcept -> size_t;
    auto total_nb_frames() const noexcept -> size_t;
    auto min_nb_frames() const noexcept -> size_t;
    auto closest_frame_id_from_time(size_t idCamera, double timeMs) const noexcept -> std::int64_t;
    // # add
    auto add_compressed_frame(size_t idCamera, std::shared_ptr<DCCompressedFrame> frame) -> void;
    // # get
    auto get_compressed_frame(size_t idCamera, size_t idFrame) -> std::weak_ptr<DCCompressedFrame>;
    auto count_frames_from_all_cameras() const noexcept -> size_t;
    // # uncompress
    auto uncompressor(size_t idCamera) noexcept -> DCFrameUncompressor*;
    auto uncompress_frame(size_t idCamera, size_t idFrame, DCFrame &frame) -> bool;
    auto uncompress_frame(size_t idCamera, DCCompressedFrame *cFrame, DCFrame &frame) -> bool;
    // # remove
    auto remove_compressed_frames_until(size_t idCamera, size_t idFrame) -> void;
    auto remove_compressed_frames_after(size_t idCamera, size_t idFrame) -> void;
    auto keep_only_one_camera(size_t idCamera) -> void;
    auto keep_only_cameras_from_id(const std::vector<size_t> &ids) -> void;
    // # clean
    auto clean_all_cameras_compressed_frames() noexcept -> void;
    auto clean_camera_compressed_frames(size_t idCamera) noexcept -> void;
    // # merge
    auto merge_cameras_clouds(size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, cam::DCFrame &frame) -> void;
    auto merge_cameras_clouds(size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound) -> tool::geo::ColoredCloudData;
    auto merge_all_cameras(float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void;

    // audio
    auto total_audio_frames_size(size_t idCamera) const -> size_t;
    auto get_audio_samples_all_channels(size_t idCamera, std::vector<std::vector<float>> &audioBuffer) -> void;
    auto get_audio_samples_all_channels(size_t idCamera, std::vector<float> &audioBuffer) -> void;

    // calibration
    auto get_transform(size_t idCamera) const -> geo::Mat4d;
    auto set_transform(size_t idCamera, geo::Mat4d tr) -> void;

    // i/o
    auto save_to_file(std::string_view path) -> bool;
    auto load_from_file(std::string_view path) -> bool;   

protected:

    std::vector<std::unique_ptr<DCFrameUncompressor>> m_uncompressors;
    std::vector<DCCameraData> m_framesPerCamera;

private:

    // i/o files
    auto read_file(std::ifstream &file) -> bool;
    auto write_file(std::ofstream &file) -> void;
    // # legacy
    auto read_legacy_cloud_video_file(std::ifstream &file) -> void;
    auto read_legacy_full_video_file(std::ifstream &file) -> void;
};

}
