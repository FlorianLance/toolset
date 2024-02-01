
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
#include "frame.hpp"
#include "dc_enums.hpp"
#include "utility/image_buffer.hpp"

namespace tool::cam{

struct DCCompressedFrame : Frame{

    // DCCompressedFrame();
    // ~DCCompressedFrame();

    // infos
    DCMode mode;
    size_t validVerticesCount = 0;

    // images (jpeg encoding)
    ImageBuffer<std::uint8_t> encodedColorImage;
    ImageBuffer<std::uint8_t> encodedDepthSizedColorImage;

    // data (lossless std::uint16_t encoding)
    ImageBuffer<std::uint8_t> encodedDepthData;
    ImageBuffer<std::uint8_t> encodedInfraData;
    Buffer<std::uint8_t> encodedColoredCloudData;

    // calibration
    Buffer<std::uint8_t> calibrationData;

    // // imu
    // // Buffer<std::uint8_t> imuData; // TODO
    // std::optional<DCImuSample> imuSample = std::nullopt;

    // // audio
    // // Buffer<Buffer<float>> audioData; // TODO
    std::vector<std::array<float, 7>> audioFrames;

    // // bodies
    // // ...

    // getters
    auto data_size() const noexcept -> size_t override;

    // init
    auto init_from_file_stream(std::ifstream &file) -> void override;
    auto init_from_data(std::span<const std::int8_t> data, size_t &offset) -> void override;
    // write
    auto write_to_file_stream(std::ofstream &file) -> void override;
    auto write_to_data(std::span<int8_t> data, size_t &offset) -> void override;

    // // # calibration
    // auto write_calibration_content_to_data(int8_t * const data, size_t &offset, size_t sizeData) -> void;
    // auto init_calibration_from_data(DCType type, std::int8_t const * const data, size_t &offset, size_t sizeData) -> void;
    // auto calibration_data_size() const noexcept -> size_t;

    // # legacy
    auto init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void;
    auto init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void;

// private:

//     struct Impl;
//     std::unique_ptr<Impl> i;
};
}
