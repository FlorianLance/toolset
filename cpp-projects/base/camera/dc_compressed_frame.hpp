
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

namespace tool::cam{

struct DCCompressedFrame : Frame{

    DCCompressedFrame();
    ~DCCompressedFrame();

    // infos
    DCMode mode;
    size_t validVerticesCount = 0;

    // color
    DCImageBuffer<std::uint8_t> encodedImageColor;
    // size_t colorWidth = 0;
    // size_t colorHeight = 0;
    // std::vector<std::uint8_t> encodedColorData;

    // depth-sized color
    size_t depthSizedColorWidth = 0;
    size_t depthSizedColorHeight = 0;
    std::vector<std::uint8_t> encodedDepthSizedColorData;

    // depth
    size_t depthWidth = 0;
    size_t depthHeight = 0;
    std::vector<std::uint8_t> encodedDepthData;

    // infrared
    size_t infraWidth = 0;
    size_t infraHeight = 0;
    std::vector<std::uint8_t> encodedInfraData;

    // cloud
    std::vector<std::uint8_t> encodedCloudVerticesData;

    // imu
    std::optional<DCImuSample> imuSample = std::nullopt;

    // audio
    std::vector<std::array<float, 7>> audioFrames;

    // bodies
    // ...

    // calibration
    // std::vector<std::int8_t> calibrationData;

    // getters
    // # sizes
    auto infos_size()               const noexcept -> size_t;
    auto color_size()               const noexcept -> size_t;
    auto depth_sized_color_size()   const noexcept -> size_t;
    auto depth_size()               const noexcept -> size_t;
    auto infra_size()               const noexcept -> size_t;
    auto cloud_size()               const noexcept -> size_t;
    auto imu_sample_size()          const noexcept -> size_t;
    auto audio_size()               const noexcept -> size_t;
    auto bodies_size()              const noexcept -> size_t;
    auto calibration_size()         const noexcept -> size_t;
    auto total_data_size()          const -> size_t;
    // # has
    auto has_calibration()          const noexcept -> bool;

    // init
    auto init_from_file_stream(std::ifstream &file) -> void;
    auto init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void;
    // # legacy
    auto init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void;
    auto init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void;
    // write
    auto write_to_file_stream(std::ofstream &file) -> void;
    auto write_to_data(int8_t * const data, size_t &offset, size_t sizeData) -> void;
    // # calibration
    auto write_calibration_content_to_data(int8_t * const data, size_t &offset, size_t sizeData) -> void;
    auto init_calibration_from_data(DCType type, std::int8_t const * const data, size_t &offset, size_t sizeData) -> void;
    auto calibration_data_size() const noexcept -> size_t;

private:
    struct Impl;
    std::unique_ptr<Impl> i;
};
}
