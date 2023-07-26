

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

// std
#include <expected>

// local
#include "geometry/matrix4.hpp"
#include "k4_frame_uncompressor.hpp"


namespace tool::camera{

class K4VolumetricVideo;

struct K4CameraData{

    K4CameraData() = default;
    K4CameraData& operator=(const K4CameraData& other){
        this->transform = other.transform;
        this->frames.reserve(other.frames.size());
        for(const auto &frame : other.frames){
            auto cf = std::make_shared<K4CompressedFrame>();
            *cf = *frame;
            this->frames.push_back(std::move(cf));
        }
        return *this;
    }

    geo::Mat4d transform = geo::Mat4d::identity();
    std::vector<std::shared_ptr<K4CompressedFrame>> frames;

    // getters
    auto nb_frames() const noexcept -> size_t;
    auto valid_vertices_count(size_t idFrame) const noexcept -> size_t;
    auto first_frame_capture_timestamp() const noexcept -> std::expected<std::int64_t, std::string_view>;
    auto last_frame_capture_timestamp() const noexcept -> std::expected<std::int64_t, std::string_view>;

    // modifiers
    auto add_compressed_frame(std::shared_ptr<K4CompressedFrame> frame) -> void;
    auto get_compressed_frame(size_t idFrame) const noexcept -> std::weak_ptr<K4CompressedFrame>;
    auto remove_frames_until(size_t idFrame) -> void;
    auto remove_frames_after(size_t idFrame) -> void;
    auto clean() noexcept -> void;

    friend K4VolumetricVideo;

private:

    auto first_frame_ptr() const -> K4CompressedFrame*;
    auto last_frame_ptr() const -> K4CompressedFrame*;
    auto get_frame_ptr(size_t idFrame) const -> K4CompressedFrame*;
};

class K4VolumetricVideo{

public:

    K4VolumetricVideo() = default;
    K4VolumetricVideo& operator=(const K4VolumetricVideo& other);

    auto initialize(size_t nbCameras) -> void;
    auto initialize_from(const K4VolumetricVideo &video) -> void;

    // cameras
    auto nb_cameras() const noexcept -> size_t;
    auto get_camera_data(size_t idCamera) const noexcept -> const K4CameraData*;

    // times
    auto first_frame_capture_timestamp() const noexcept -> std::expected<std::int64_t, std::string_view>;
    auto last_frame_capture_timestamp() const noexcept -> std::expected<std::int64_t, std::string_view>;
//    auto set_video_start_timestamp(std::chrono::nanoseconds videoStartTS) noexcept -> void;
//    auto set_video_end_timestamp(std::chrono::nanoseconds videoEndTS) noexcept -> void;
    auto duration_ms() const noexcept -> double;
    static auto get_timestamp_diff_time_ms(std::int64_t t1, std::int64_t t2) noexcept -> double;

    // frames
    auto nb_frames(size_t idCamera) const noexcept -> size_t;
    auto total_nb_frames() const noexcept -> size_t;
    auto min_nb_frames() const noexcept -> size_t;
    auto closest_frame_id_from_time(size_t idCamera, double timeMs) const noexcept -> std::expected<size_t, std::string_view>;
    // # add
    auto add_compressed_frame(size_t idCamera, std::shared_ptr<K4CompressedFrame> frame) -> void;
    // # get
    auto get_compressed_frame(size_t idCamera, size_t idFrame) -> std::weak_ptr<K4CompressedFrame>;
    auto count_frames_from_all_cameras() const noexcept -> size_t;
    // # uncompress
    auto uncompressor(size_t idCamera) noexcept -> K4FrameUncompressor*;
    auto uncompress_frame(size_t idCamera, size_t idFrame, K4Frame &frame) -> bool;
    auto uncompress_frame(size_t idCamera, K4CompressedFrame *cFrame, K4Frame &frame) -> bool;
    // # remove
    auto remove_compressed_frames_until(size_t idCamera, size_t idFrame) -> void;
    auto remove_compressed_frames_after(size_t idCamera, size_t idFrame) -> void;
    auto keep_only_one_camera(size_t idCamera) -> void;
    // # clean
    auto clean_all_cameras_compressed_frames() noexcept -> void;
    auto clean_camera_compressed_frames(size_t idCamera) noexcept -> void;
    // # merge
    auto merge_cameras_clouds(size_t idFrame, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, camera::K4Frame &frame) -> void;
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

    std::vector<std::unique_ptr<K4FrameUncompressor>> m_uncompressors;
    std::vector<K4CameraData> m_framesPerCamera;            
//    std::chrono::nanoseconds m_videoStartTS{};
//    std::chrono::nanoseconds m_videoEndTS{};

private:

    // i/o files
    auto read_file(std::ifstream &file) -> bool;
    auto write_file(std::ofstream &file) -> void;
    // # legacy
    auto read_legacy_cloud_video_file(std::ifstream &file) -> void;
    auto read_legacy_full_video_file(std::ifstream &file) -> void;
};

}
