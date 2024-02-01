
/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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
#include <span>
#include <chrono>

// local
#include "utility/string_unordered_map.hpp"
#include "dc_types.hpp"
#include "geometry/cloud.hpp"
#include "graphics/color.hpp"

namespace tool::cam{

struct DCFrameData{

    auto reset() -> void;

    // images
    std::span<std::int8_t> rawColor;
    std::span<ColorRGBA8> color;
    std::span<ColorRGBA8> dephtSizedColor;
    std::span<std::uint16_t> depth;
    std::span<std::uint16_t> infra;
    std::span<geo::Pt3<std::int16_t>> depthCloud;
    std::span<std::uint8_t> bodiesIdDepth;

    // others
    std::pair<size_t, std::span<float>> audioChannels;
    std::optional<DCImuSample> imuSample = std::nullopt;
    std::vector<DCBody> bodies;
};

struct DCFrameIndices{

    auto initialize(const DCModeInfos &infos) -> void;

    // colors
    // # 1D
    std::vector<size_t> colors1D;
    // depths
    // # 1D
    std::vector<size_t> depths1D;
    std::vector<size_t> depths1DNoBorders;
    std::vector<std::array<std::int32_t,4>> neighbours4Depth1D;
    std::vector<std::array<std::int32_t,8>> neighbours8Depth1D;
    // # 3D
    std::vector<geo::Pt3<size_t>> depths3D;
    // # Correspondance
    std::vector<std::tuple<size_t, std::int32_t>> depthVertexCorrrespondance;
    // std::vector<std::tuple<size_t, std::int16_t>> depthsSortedCorrespondanceNoBorders;
};

struct DCFrameTiming{

    auto reset() -> void;
    auto swap_local_timestamps() -> void;
    auto update_local(std::string_view name) -> void;
    auto compute_capture_framerate() -> void;

    auto get_local(std::string_view name) const -> std::chrono::nanoseconds;
    auto get_duration_between_ms(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::milliseconds>;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::microseconds>;

    // profiling
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> timestamps;
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> localTimestamps;
    float nbCapturePerSecond = 0.f;
    std::vector<std::chrono::nanoseconds> capturesTimes;
};


struct Frame{
    std::int32_t idCapture      = 0;
    std::int64_t afterCaptureTS = 0;
    std::int64_t receivedTS     = 0;
    virtual ~Frame(){}
};


template<typename T>
struct DCImageBuffer{
    size_t width = 0;
    size_t height = 0;
    Buffer<T> data;

    auto reset() noexcept -> void{
        width  = 0;
        height = 0;
        data.clear();
    }
};

struct DCFrame : Frame{

    // info
    DCMode mode;

    // color
    DCImageBuffer<ColorRGBA8> imageColor;
    // size_t colorWidth = 0;
    // size_t colorHeight = 0;
    // std::vector<geo::Pt4<std::uint8_t>> imageColorData;

    // depth sized color
    size_t depthSizedColorWidth = 0;
    size_t depthSizedColorHeight = 0;
    std::vector<geo::Pt4<std::uint8_t>> depthSizedImageColorData;

    // depth image
    size_t depthWidth = 0;
    size_t depthHeight = 0;
    std::vector<std::uint16_t> depthData;
    std::vector<geo::Pt3<std::uint8_t>> imageDepthData;

    // infra
    size_t infraWidth = 0;
    size_t infraHeight = 0;
    std::vector<std::uint16_t> infraData;
    std::vector<geo::Pt3<std::uint8_t>> imageInfraData;

    // cloud
    geo::ColoredCloudData cloud;

    // imu
    std::optional<DCImuSample> imuSample;

    // audio
    // TODO: add nbChannels variables and use a simple vector
    std::vector<std::array<float, 7>> audioFrames;

    // bodies
    std::vector<DCBody> bodies;

//    virtual auto copy(const DCFrame &source) -> void{}
};
}
