
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

// taskflow
#include "thirdparty/taskflow/taskflow.hpp"

// local
#include "dc_frame_indices.hpp"
#include "dc_frame_data.hpp"
#include "dc_frame_timing.hpp"
#include "dc_device.hpp"

#include "data/fastpfor_encoding.hpp"
#include "data/jpeg_encoding.hpp"

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

    DCDeviceImpl();
    virtual ~DCDeviceImpl();

    // actions
    virtual auto open(std::uint32_t deviceId) -> bool{static_cast<void>(deviceId); return false;}
    virtual auto open(std::string_view deviceId) -> bool{static_cast<void>(deviceId); return false;}
    virtual auto open_file(const std::string &path) -> bool{static_cast<void>(path); return false;}
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
    auto get_duration_ms(std::string_view id) -> std::optional<std::chrono::milliseconds>;
    auto get_duration_micro_s(std::string_view id) -> std::optional<std::chrono::microseconds>;

    DCDevice *dcDevice = nullptr;
    std::atomic_bool readFramesFromCameras = false;

protected:

    auto do_loop() -> void;

    // initialization
    auto initialize(const DCConfigSettings &newConfig) -> void;
    virtual auto initialize_device_specific() -> void{}
    virtual auto update_from_colors_settings() -> void{}
    virtual auto update_from_data_settings() -> void{}

    // thread
    auto start_reading_thread() -> void;
    auto stop_reading_thread() -> void;

    // read data
    virtual auto read_calibration() -> void{}
    virtual auto capture_frame(std::int32_t timeoutMs)  -> bool{static_cast<void>(timeoutMs);return false;}
    virtual auto read_color_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_depth_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_infra_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_audio(bool enable)                -> void{static_cast<void>(enable);}
    virtual auto read_IMU(bool enable)                  -> void{static_cast<void>(enable);}
    virtual auto read_body_tracking(bool enable)        -> void{static_cast<void>(enable);}
    auto check_data_validity() -> bool;

    // process data
    virtual auto resize_color_image_to_depth_size() -> void{}
    auto convert_color_image() -> void;
    virtual auto generate_cloud(bool enable) -> void{static_cast<void>(enable);}

    // # images preprocessing
    auto preprocess_color_image() -> void{}
    auto preprocess_depth_sized_color_image() -> void{}
    auto preprocess_depth_image() -> void{}
    auto preprocess_infra_image() -> void{}
    auto preprocess_cloud_image() -> void{}
    auto preprocess_body_tracking_image() -> void{}

    // # depth processing
    auto filter_depth_basic()                   -> void;
    auto filter_depth_from_depth_sized_color()  -> void;
    auto filter_depth_from_infra()              -> void;
    auto filter_depth_from_cloud()              -> void;
    auto filter_depth_from_body_tracking()      -> void;
    auto filter_depth_complex()                 -> void;
    auto update_valid_depth_values() -> void;

    // # depth sized color processing
    auto filter_depth_sized_color_from_depth() -> void;
    auto mix_depth_sized_color_with_body_tracking() -> void{}

    // # infra processing
    auto filter_infra_from_depth() -> void;
    auto mix_infra_with_body_tracking() -> void{}

    // frame generation
    // # local
    auto update_frame_color() -> void;
    auto update_frame_depth_sized_color() -> void;
    auto update_frame_depth() -> void;
    auto update_frame_infra() -> void;
    auto update_frame_cloud() -> void;
    auto update_frame_audio() -> void;
    auto update_frame_imu() -> void;
    auto update_frame_bodies() -> void;
    auto update_frame_calibration() -> void;
    // # compressed
    auto update_compressed_frame_color() -> void;
    auto update_compressed_frame_depth_sized_color() -> void;
    auto update_compressed_frame_depth() -> void;
    auto update_compressed_frame_infra() -> void;
    auto update_compressed_frame_cloud() -> void;
    auto update_compressed_frame_audio() -> void;
    auto update_compressed_frame_imu() -> void;
    auto update_compressed_frame_bodies() -> void;
    auto update_compressed_frame_calibration() -> void;

    // states
    DCSettings settings;
    DCModeInfos mInfos;
    DCFrameIndices fIndices;
    DCFramesBuffer frames;
    DCFrameData fData;
    tool::s_umap<std::string_view, TimeElem> times;
    bool captureSuccess = false;
    bool dataIsValid = false;
    std::shared_ptr<DCFrame> frame = nullptr;
    std::shared_ptr<DCCompressedFrame> cFrame = nullptr;
    std::mutex parametersM;

    // current settings
    DCFiltersSettings cFiltersS;
    DCDataSettings cDataS;
    DCDelaySettings cDelayS;

    // profiling
    TimeDiffManager timeM;

    // encoders
    data::JpegEncoder jpegColorEncoder;
    data::JpegEncoder jpegDepthSizedColorEncoder;
    data::JpegEncoder jpegBodiesIdEncoder;
    data::FastPForEncoder fastPForDepthEncoder;
    data::FastPForEncoder fastPForInfraEncoder;
    data::FastPForEncoder fastPForCloudEncoder;
    // decoders
    data::JpegDecoder jpegColorDecoder;

    // threads/tasks
    std::unique_ptr<std::thread> loopT = nullptr;
    std::unique_ptr<tf::Executor> executor = nullptr;
    std::unique_ptr<tf::Taskflow> processDataTF = nullptr;

private:

    // depth filtering
    auto maximum_local_depth_difference(const DCFrameIndices &ids, std::span<std::uint16_t> depthBuffer, float max, DCConnectivity connectivity) -> void;
    auto keep_only_biggest_cluster() -> void;
    auto mininum_neighbours(std::uint8_t nbLoops, std::uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void;
    auto erode(std::uint8_t nbLoops, DCConnectivity connectivity, std::uint8_t nbMinValid) -> void;

    // tasks
    auto read_data_taskflow() -> std::unique_ptr<tf::Taskflow>;
    auto process_data_taskflow() -> std::unique_ptr<tf::Taskflow>;
    auto process_frame_taskflow(tf::Taskflow &readDataTF, tf::Taskflow &processDataTF) -> std::unique_ptr<tf::Taskflow>;
};
}

