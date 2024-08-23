
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
#include "depth-camera/dc_types.hpp"
#include "camera/frame.hpp"
#include "utility/image_buffer.hpp"

namespace tool::cam{

enum class DCCompressedDataType : std::int32_t{
    Color, DepthSizedColor, DepthData, InfraData,
    Calibration,Imu, Cloud,
};

struct TestFrame : Frame{

    std::unordered_map<DCCompressedDataType, BinaryImageBuffer> imagesBuffers;
    std::unordered_map<DCCompressedDataType, BinaryBuffer> dataBuffers;

    auto write() -> void{
        // write frame
        // write mode
        // write validVerticesCount
        // write number of image buffers
        // foreach buffer in imagesBuffers
        //  write type
        //  write width
        //  write height
        //  write comression mode
        //  write data
        // write number of data buffers
        // foreach buffer in dataBuffers
        //  write type
        //  write size
        //  write comression mode
        //  write data
    }

    auto read() -> void{
        // read frame
        // read mode
        // read validVerticesCount
    }
};

struct DCCompressedFrame : Frame{

    // infos
    DCMode mode;    
    size_t validVerticesCount = 0;

    // jpeg encoding
    BinaryImageBuffer jpegRGBA8Color;
    BinaryImageBuffer jpegRGBA8DepthSizedColor;
    BinaryImageBuffer jpegG8BodiesIdMap;

    // fast PFor encoding
    BinaryImageBuffer fpfDepth;
    BinaryImageBuffer fpfInfra;
    BinaryBuffer fpfColoredCloud;

    // binary encoding
    BinaryBuffer calibration;
    BinaryBuffer imu;

    // no encoding
    Buffer<DCBody> bodyTracking;
    std::vector<std::array<float, 7>> audioFrames;

    // test

    // auto init_from_json(const nlohmann::json &json) -> void override;
    // auto convert_to_json() const -> nlohmann::json override;

    // # legacy
    // getters
    auto data_size() const noexcept -> size_t override;

    // init
    auto init_from_file_stream(std::ifstream &file) -> void override;
    auto init_from_data(std::span<const std::byte> data, size_t &offset) -> void override;
    // write
    auto write_to_file_stream(std::ofstream &file) -> void override;
    auto write_to_data(std::span<std::byte> data, size_t &offset) -> void override;

    // ## super legacy
    auto init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void;
    auto init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void;
};
}
