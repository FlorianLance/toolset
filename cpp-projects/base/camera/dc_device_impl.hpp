
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
#include "dc_frame_compressor.hpp"
#include "dc_frame_uncompressor.hpp"
#include "dc_frame_indices.hpp"
#include "dc_frame_data.hpp"
#include "dc_frame_timing.hpp"
#include "dc_device.hpp"

namespace tool::cam {

struct DCSettings{
    DCConfigSettings config;
    DCDataSettings data;
    DCFiltersSettings filters;
    DCColorSettings color;
    DCDelaySettings delay;
};

struct DCFramesBuffer{

    auto add_frame(std::shared_ptr<cam::DCFrame> frame) -> void;
    auto add_compressed_frame(std::shared_ptr<cam::DCCompressedFrame> cFrame) -> void;
    auto take_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, std::int64_t delayMs) -> std::shared_ptr<cam::DCFrame>;
    auto get_compressed_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, std::int64_t delayMs) -> std::shared_ptr<cam::DCCompressedFrame>;

    // delay buffer
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<DCFrame>>> frames;
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<cam::DCCompressedFrame>>> compressedFrames;
};

struct DCDeviceImpl{

    virtual ~DCDeviceImpl(){}

    DCDevice *dcDevice = nullptr;

    DCSettings settings;
    DCModeInfos mInfos;
    DCFrameIndices fIndices;
    DCFrameTiming fTiming;
    DCFramesBuffer frames;
    DCFrameData fData;
    DCFrameCompressor fCompressor;
    DCFrameUncompressor fUncompressor;

    // state
    std::atomic_bool readFramesFromCameras = false;
    std::atomic_bool sendData = true;

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

    // state
    auto check_data_validity() -> bool;

    // read data
    auto read_frames() -> void;
    virtual auto read_calibration() -> void{}
    virtual auto capture_frame(std::int32_t timeoutMs) -> bool{static_cast<void>(timeoutMs);return false;}
    virtual auto read_color_image(bool enable)      -> bool{static_cast<void>(enable); return false;}
    virtual auto read_depth_image(bool enable)      -> bool{static_cast<void>(enable); return false;}
    virtual auto read_infra_image(bool enable)      -> bool{static_cast<void>(enable); return false;}
    virtual auto read_from_microphones(bool enable) -> void{static_cast<void>(enable);}
    virtual auto read_from_imu(bool enable)         -> void{static_cast<void>(enable);}
    virtual auto read_bodies(bool enable)           -> void{static_cast<void>(enable);}
    virtual auto generate_cloud(bool enable)        -> void{static_cast<void>(enable);}

    // process data
    virtual auto convert_color_image() -> void;
    virtual auto resize_color_image_to_depth_size() -> void{}
    auto filter_depth_image(const DCFiltersSettings &filtersS) -> void;
    auto filter_depth_sized_color_image(const DCFiltersSettings &filtersS) -> void;
    auto filter_infrared_image(const DCFiltersSettings &filtersS) -> void;
    virtual auto filter_cloud_image(const DCFiltersSettings &filtersS) -> void{static_cast<void>(filtersS);}
    auto update_valid_depth_values() -> void;

    // frame generation
    // # local
    virtual auto create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>{ static_cast<void>(dataS);return nullptr;}
    auto update_infos(DCFrame *dFrame) -> void;
    auto update_color(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_depth_sized_color(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_depth(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_infra(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_cloud(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_audio(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_imu(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_bodies(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_calibration(DCFrame *dFrame) -> void;
    // # compressed
    virtual auto compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{
        static_cast<void>(filtersS);static_cast<void>(dataS);return nullptr;}
    auto update_compressed_frame_infos(DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_color(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_depth_sized_color(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_depth(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_infra(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_cloud(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_audio(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_imu(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_bodies(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_calibration(DCCompressedFrame *cFrame) -> void;

    // auto debug_save_images(std::string parentPath) -> void;

private:

    // depth filtering
    auto maximum_local_depth_difference(const DCFrameIndices &ids, std::span<std::uint16_t> depthBuffer, float max, DCConnectivity connectivity) -> void;
    auto keep_only_biggest_cluster() -> void;
    auto mininum_neighbours(std::uint8_t nbLoops, std::uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void;
    auto erode(std::uint8_t nbLoops, DCConnectivity connectivity) -> void;
};
}
