
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

    // sizes
    auto infos_size() const noexcept{
        return
            sizeof(std::int32_t) + // id capture
            sizeof(std::int64_t) + // after capture TS
            sizeof(DCMode);
    }

    auto color_size()  const noexcept -> size_t{
        return encodedColorData.size() +
            sizeof(size_t) * 3; // colorWidth, colorHeight, encoded size
    }
    auto depth_size()  const noexcept -> size_t{
        return encodedDepthData.size() +
            sizeof(size_t) * 3; // depthWidth, depthHeight, encoded size
    }
    auto infra_size()  const noexcept -> size_t{
        return encodedInfraData.size() +
            sizeof(size_t) * 3; // infraWidth, infraHeight, encoded size
    }

    auto cloud_vertices_size() const noexcept -> size_t{
        return encodedCloudVerticesData.size() +
            sizeof(size_t)*2; // validVerticesCount, encoded size
    }
    auto cloud_color_size()  const noexcept -> size_t{
        return encodedCloudColorData.size() +
            sizeof(size_t) * 3; // cloudColorWidth, cloudColorHeight, encoded size
    }

    auto imu_sample_size() const noexcept -> size_t{
        return (imuSample.has_value() ? sizeof(DCImuSample) : 0) +
            sizeof(bool); // has IMU
    }
    auto audio_size() const noexcept      -> size_t{
        return audioFrames.size()*7*sizeof(float) +
            sizeof(size_t); // nb audio frames
    }

    virtual auto total_data_size() const -> size_t{
        return
            infos_size() +
            color_size() + cloud_color_size() + depth_size() + infra_size() + cloud_vertices_size() +
            imu_sample_size() + audio_size();
    }

    // file stream
    virtual auto init_from_file_stream(std::ifstream &file) -> void{static_cast<void>(file);}
    virtual auto write_to_file_stream(std::ofstream &file) -> void{static_cast<void>(file);}
    // data
    virtual auto init_from_data(std::int8_t *data) -> void{static_cast<void>(data);}
    virtual auto convert_to_data(std::vector<std::int8_t> &data) -> size_t{static_cast<void>(data);return 0;}
};
}
