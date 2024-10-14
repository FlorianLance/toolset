

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
#include "frame/dc_frame_generator.hpp"
#include "frame/dc_data_frame_buffer.hpp"
#include "settings/dc_data_frame_generation_settings.hpp"

namespace tool::cam{

enum class VideoType : std::int8_t{
    Legacy1 = 0,
    Legacy2 = 1,
    Legacy3 = 2,
    Current = 3,
};

[[maybe_unused]] static constexpr auto valid(VideoType vt) noexcept -> bool{
    return (vt == VideoType::Legacy3) || (vt == VideoType::Current);
}


class DCVideo{

public:

    DCVideo() = default;
    DCVideo& operator=(const DCVideo& other);

    // init / clean
    auto initialize(size_t nbDevices) -> void;
    auto clean() -> void;

    // get
    // # devices
    auto nb_devices() const noexcept -> size_t;
    // # calibration
    auto get_transform(size_t idD) const -> geo::Mat4d;
    // # times
    auto duration_ms() const noexcept -> double;
    auto first_frame_received_timestamp() const noexcept -> std::int64_t;
    auto last_frame_received_timestamp() const noexcept -> std::int64_t;
    auto device_duration_ms(size_t idD) const noexcept -> double;
    auto device_first_frame_received_timestamp(size_t idD) const noexcept -> std::int64_t;
    auto device_last_frame_received_timestamp(size_t idD) const noexcept -> std::int64_t;
    static auto get_timestamp_diff_time_ms(std::int64_t t1, std::int64_t t2) noexcept -> double;
    // # frames
    auto nb_frames(size_t idD) const noexcept -> size_t;
    auto count_frames_from_all_devices() const noexcept -> size_t;
    auto min_nb_frames() const noexcept -> size_t;
    auto closest_frame_id_from_time(size_t idD, double timeMs) const noexcept -> std::int64_t;
    auto get_data_frame(size_t idD, size_t idFrame) -> std::weak_ptr<DCDataFrame>;
    auto get_data_frames_ptr(size_t idD) const noexcept -> const DCDataFrameBuffer*;
    auto generator(size_t idD) noexcept -> DCFrameGenerator*;
    // # audio
    // auto total_audio_frames_size(size_t idD) const -> size_t;
    // auto get_audio_samples_all_channels(size_t idD, std::vector<std::vector<float>> &audioBuffer) -> void;
    // auto get_audio_samples_all_channels(size_t idD, std::vector<float> &audioBuffer) -> void;

    // generate
    auto generate_frame(const DCFrameGenerationSettings &dfgS, size_t idD, size_t idF, DCFrame2 &frame) -> bool;
    auto generate_frame(const DCFrameGenerationSettings &dfgS, size_t idD, DCDataFrame *cFrame, DCFrame2 &frame) -> bool;

    // modify
    // # devices
    auto add_device() -> void;
    auto keep_only_one_device(size_t idD) -> void;
    auto keep_only_devices_from_id(const std::vector<size_t> &ids) -> void;
    auto remove_last_device() -> void;
    // # modify devices/frames
    // #### TEST
    auto merge_all_devices(const DCFrameGenerationSettings &gSettings, float voxelSize, tool::geo::Pt3f minBound, tool::geo::Pt3f maxBound) -> void;
    auto merge_devices_frame_id(const DCFrameGenerationSettings &gSettings, size_t idF, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, cam::DCFrame2 &frame) -> void;
    auto merge_devices_frame_id(const DCFrameGenerationSettings &gSettings, size_t idF, float sizeVoxel, geo::Pt3f minBound, geo::Pt3f maxBound, tool::geo::ColorCloud &cloud) -> void;
    // # frames
    auto add_data_frame(size_t idD, std::shared_ptr<DCDataFrame> frame) -> void;
    auto remove_data_frames_until(size_t idD, size_t idF) -> void;
    auto remove_data_frames_after(size_t idD, size_t idF) -> void;
    auto remove_all_data_frames(size_t idD) noexcept -> void;
    auto remove_all_devices_data_frames() noexcept -> void;
    // # replace
    auto replace_data_frame(size_t idD, size_t idF, std::shared_ptr<DCDataFrame> frame) -> void;

    // # calibration
    auto set_transform(size_t idD, geo::Mat4d tr) -> void;

    // i/o
    auto save_to_file(std::string_view path) -> bool;
    auto load_from_file(std::string_view path) -> bool;   

protected:

    Buffer<std::unique_ptr<DCFrameGenerator>> m_devicesFrameGenerators;
    Buffer<DCDataFrameBuffer> m_devicesDataFrames;
    Buffer<geo::Mat4d> m_devicesTransforms;

private:

    // i/o files
    auto read_file(std::ifstream &file) -> bool;
    auto write_file(std::ofstream &file) -> void;
    // # legacy
    // auto read_legacy_cloud_video_file(std::ifstream &file) -> void;
    // auto read_legacy_full_video_file(std::ifstream &file) -> void;
};

}
