
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
#include "dc_frame.hpp"

namespace tool::camera{

struct DCCompressedFrame : Frame{

    DCCompressedFrame();
    ~DCCompressedFrame();

    DCMode mode;

    // color
    size_t colorWidth = 0;
    size_t colorHeight = 0;
    std::vector<std::uint8_t> encodedColorData;

    // depth
    size_t depthWidth = 0;
    size_t depthHeight = 0;
    std::vector<std::uint8_t> encodedDepthData;

    // infrared
    size_t infraWidth = 0;
    size_t infraHeight = 0;
    std::vector<std::uint8_t> encodedInfraData;

    // cloud
    // # vertices
    size_t validVerticesCount = 0;
    std::vector<std::uint8_t> encodedCloudVerticesData;
    // # colors
    size_t cloudColorWidth = 0;
    size_t cloudColorHeight = 0;
    std::vector<std::uint8_t> encodedCloudColorData;

    // imu
    std::optional<DCImuSample> imuSample = std::nullopt;

    // audio
    std::vector<std::array<float, 7>> audioFrames;

    // getters
    // # sizes
    auto infos_size() const noexcept -> size_t;
    auto color_size()  const noexcept -> size_t;
    auto depth_size()  const noexcept -> size_t;
    auto infra_size()  const noexcept -> size_t;
    auto cloud_vertices_size() const noexcept -> size_t;
    auto cloud_color_size()  const noexcept -> size_t;
    auto imu_sample_size() const noexcept -> size_t;
    auto audio_size() const noexcept      -> size_t;
    auto bodies_size() const noexcept -> size_t;
    auto calibration_size() const noexcept -> size_t;
    auto total_data_size() const -> size_t{
        return
            infos_size() +
            color_size() + cloud_color_size() + depth_size() + infra_size() + cloud_vertices_size() +
            calibration_size() + imu_sample_size() + audio_size() + bodies_size();
    }
    // # has
    auto has_calibration() const noexcept -> bool;

    // init
    auto init_from_file_stream(std::ifstream &file) -> void;
    auto init_from_data(std::int8_t *data) -> void;
    // # legacy
    auto init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void;
    auto init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void;
    // write
    auto write_to_file_stream(std::ofstream &file) -> void;
    auto write_to_data(std::vector<std::int8_t> &data) -> size_t;
    // # calibration
    auto write_calibration_to_data(std::int8_t *data) -> void;
    auto update_calibration_from_data(std::int8_t *data) -> void;

private:

    auto read_infos_from_file_stream(std::ifstream &file) -> void;
    auto read_color_from_file_stream(std::ifstream &file) -> void;
    auto read_depth_from_file_stream(std::ifstream &file) -> void;
    auto read_infra_from_file_stream(std::ifstream &file) -> void;
    auto read_cloud_from_file_stream(std::ifstream &file) -> void;
    auto read_calibration_from_file_stream(std::ifstream &file) -> void;
    auto read_imu_from_file_stream(std::ifstream &file) -> void;
    auto read_audio_from_file_stream(std::ifstream &file) -> void;
    auto read_bodies_from_file_stream(std::ifstream &file) -> void;

    auto write_infos_to_file_stream(std::ofstream &file) -> void;
    auto write_color_to_file_stream(std::ofstream &file) -> void;
    auto write_depth_to_file_stream(std::ofstream &file) -> void;
    auto write_infra_to_file_stream(std::ofstream &file) -> void;
    auto write_cloud_to_file_stream(std::ofstream &file) -> void;
    auto write_calibration_to_file_stream(std::ofstream &file) -> void;
    auto write_imu_to_file_stream(std::ofstream &file) -> void;
    auto write_audio_to_file_stream(std::ofstream &file) -> void;
    auto write_bodies_to_file_stream(std::ofstream &file) -> void;

    auto init_infos_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_color_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_depth_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_infra_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_cloud_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_calibration_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_imu_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_audio_from_data(std::int8_t *data, size_t &offset) -> void;
    auto init_bodies_from_data(std::int8_t *data, size_t &offset) -> void;

    auto write_infos_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_color_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_depth_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_infra_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_cloud_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_calibration_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_imu_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_audio_to_data(std::int8_t *data, size_t &offset) -> void;
    auto write_bodies_to_data(std::int8_t *data, size_t &offset) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
