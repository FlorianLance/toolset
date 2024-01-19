
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
#include <chrono>
#include <span>

// local
#include "utility/string_unordered_map.hpp"
#include "camera/dc_frame_compressor.hpp"
#include "camera/dc_frame_uncompressor.hpp"
#include "dc_device.hpp"

namespace tool::camera {

struct DCInfos{

    auto initialize(DCMode mode) -> void;

    size_t idCapture    = 0;
    size_t colorWidth   = 0;
    size_t colorHeight  = 0;
    size_t colorSize    = 0;
    DCDepthResolution initialDepthResolution;
    size_t depthWidth   = 0;
    size_t depthHeight  = 0;
    size_t depthSize    = 0;
    size_t infraWidth   = 0;
    size_t infraHeight  = 0;
    size_t infraSize    = 0;
    DCColorResolution initialColorResolution;
    DCImageFormat imageFormat;
    DCDepthResolution depthMode;
    DCFramerate fps;
    std::int32_t timeoutMs;
};

struct DCIndices{

    auto initialize(const DCInfos &infos) -> void;

    // colors
    std::vector<size_t> colors1D;
    // infrared
    std::vector<size_t> infras1D;
    // depths
    std::vector<size_t> depths1D;
    std::vector<size_t> depths1DNoBorders;
    std::vector<std::array<std::int32_t,4>> neighbours4Depth1D;
    std::vector<std::array<std::int32_t,8>> neighbours8Depth1D;
    std::vector<std::tuple<size_t, std::int32_t>> depthVertexCorrrespondance;
    std::vector<std::tuple<size_t, std::int16_t>> depthsSortedCorrespondanceNoBorders;
    std::vector<geo::Pt3<size_t>> depths3D;

};

struct DCTiming{

    auto reset() -> void;
    auto swap_local_timestamps() -> void;
    auto update_local(std::string_view name) -> void;
    auto compute_capture_framerate() -> void;

    auto get_local(std::string_view name) const -> std::chrono::nanoseconds;
    auto get_duration_between_ms(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::milliseconds>;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::microseconds>;

    // timestamps
    std::chrono::nanoseconds colorImageTS       = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds depthImageTS       = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds infraredImageTS    = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds bodiesTS           = std::chrono::nanoseconds{0};

    // profiling
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> timestamps;
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> localTimestamps;
    float nbCapturePerSecond = 0.f;
    std::vector<std::chrono::nanoseconds> capturesTimes;
};

struct DCSettings{
    DCConfigSettings config;
    DCDataSettings data;
    DCFiltersSettings filters;
    DCColorSettings color;
    DCDelaySettings delay;
};

struct DCFrames{

    // test
    // cFrame->afterCaptureTS = afterCompressingTS.count() - cFrame->afterCaptureTS;
    auto add_frame(std::shared_ptr<camera::DCFrame> frame) -> void;
    auto add_compressed_frame(std::shared_ptr<camera::DCCompressedFrame> cFrame) -> void;
    auto take_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, std::int64_t delayMs) -> std::shared_ptr<camera::DCFrame>;
    auto get_compressed_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, std::int64_t delayMs) -> std::shared_ptr<camera::DCCompressedFrame>;

    // delay buffer
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<DCFrame>>> frames;
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<camera::DCCompressedFrame>>> compressedFrames;

    // compression
    DCFrameCompressor frameCompressor;
    DCFrameUncompressor frameUncompressor;
};


struct DCData{

    // images
    // std::span<ColorRGBA8> colorDephSized;
    // std::span<std::uint16_t> depth;
    // std::span<std::uint16_t> infra;

    // imu
    DCImuSample imuSample;

    // tracking
    std::vector<DCBody> bodies;
};

// struct DCCurrentSettings{
//     DCFiltersSettings filters;
//     DCColorSettings color;
//     DCDelaySettings delay;
// };

struct DCDeviceImpl{

    virtual ~DCDeviceImpl(){}

    DCDevice *dcDevice = nullptr;

    DCSettings settings;
    DCInfos infos;
    DCIndices indices;
    DCTiming timing;
    DCFrames frames;
    DCData data;

    // state
    std::atomic_bool readFramesFromCameras = false;
    std::atomic_bool sendData = true;
    size_t validDepthValues = 0;
    size_t meanBiggestZoneId = 0;

    std::vector<std::int8_t> filteringMask;
    std::vector<int> depthMask;
    std::vector<int> zonesId;
    std::vector<std::int16_t> depthFiltering;

    // thread/lockers
    std::mutex parametersM; /**< mutex for reading parameters at beginning of a new frame in thread function */
    std::unique_ptr<std::thread> frameReaderT = nullptr;

    // actions
    virtual auto open(std::uint32_t deviceId) -> bool = 0;
    virtual auto start_reading(const DCConfigSettings &newConfigS) -> bool = 0;
    virtual auto stop_reading() -> void = 0;
    virtual auto close() -> void = 0;

    // settings
    auto set_data_settings(const DCDataSettings &dataS) -> void;
    auto set_filters_settings(const DCFiltersSettings &filtersS) -> void;
    auto set_color_settings(const DCColorSettings &colorS) -> void;
    auto set_delay_settings(const DCDelaySettings &delayS) -> void;

    // getters
    virtual auto is_opened() const noexcept -> bool = 0;
    virtual auto nb_devices() const noexcept -> std::uint32_t = 0;
    virtual auto device_name() const noexcept -> std::string = 0;

    // profiling
    auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::milliseconds>;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::optional<std::chrono::microseconds>;

protected:

    // initialization
    auto initialize() -> void;
    virtual auto initialize_device_specific() -> void{}
    virtual auto update_camera_from_colors_settings() -> void{}

    // thread
    auto start_reading_thread() -> void;
    auto stop_reading_thread() -> void;

    // get data
    virtual auto color_data() -> std::span<ColorRGBA8> {return {};}
    virtual auto depth_data() -> std::span<std::uint16_t> {return {};}
    virtual auto infra_data() -> std::span<std::uint16_t> {return {};}
    virtual auto bodies_index_data() -> std::span<std::uint8_t> {return {};}

    // read data
    auto read_frames() -> void;
    virtual auto capture_frame(std::int32_t timeoutMs) -> bool{static_cast<void>(timeoutMs);return false;}
    virtual auto read_color_image() -> bool{return false;}
    virtual auto read_depth_image() -> bool{return false;}
    virtual auto read_infra_image() -> bool{return false;}
    virtual auto read_from_microphones() -> void{}
    virtual auto read_from_imu() -> void{}
    virtual auto read_bodies() -> void{}
    virtual auto generate_cloud() -> void{}

    // process data
    virtual auto convert_color_image() -> void{}
    virtual auto resize_images() -> void{}
    auto filter_depth_image(const DCFiltersSettings &filtersS, std::span<std::uint16_t> depthBuffer, std::span<ColorRGBA8> colorBuffer, std::span<uint16_t> infraBuffer) -> void;
    auto filter_color_image(const DCFiltersSettings &filtersS) -> void;
    auto filter_infrared_image(const DCFiltersSettings &filtersS) -> void;
    virtual auto filter_cloud_image(const DCFiltersSettings &filtersS) -> void{}
    auto update_valid_depth_values() -> void;

    // frame generation
    virtual auto compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{
        static_cast<void>(filtersS);static_cast<void>(dataS);return nullptr;}
    virtual auto create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>{
        static_cast<void>(dataS);return nullptr;}

private:

    // depth filtering
    auto maximum_local_depth_difference(std::span<std::uint16_t> depthBuffer, float max, Connectivity connectivity) -> void;
    auto keep_only_biggest_cluster() -> void;
    auto mininum_neighbours(std::uint8_t nbLoops, std::uint8_t nbMinNeighbours, Connectivity connectivity) -> void;
    auto erode(std::uint8_t nbLoops, Connectivity connectivity) -> void;
};
}
