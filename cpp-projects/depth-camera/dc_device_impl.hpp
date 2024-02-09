
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

    virtual ~DCDeviceImpl(){}

    DCDevice *dcDevice = nullptr;
    DCSettings settings;
    DCModeInfos mInfos;
    DCFrameIndices fIndices;
    DCFrameTiming fTiming;
    DCFramesBuffer frames;
    DCFrameData fData;

    // encoders
    data::JpegEncoder jpegColorEncoder;
    data::JpegEncoder jpegDepthSizedColorEncoder;
    data::JpegEncoder jpegBodiesIdEncoder;
    data::FastPForEncoder fastPForDepthEncoder;
    data::FastPForEncoder fastPForInfraEncoder;
    data::FastPForEncoder fastPForCloudEncoder;
    // decoders
    data::JpegDecoder jpegColorDecoder;
        
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


    // read data
    virtual auto read_calibration() -> void{}
    auto read_frames() -> void;        
    virtual auto capture_frame(std::int32_t timeoutMs)  -> bool{static_cast<void>(timeoutMs);return false;}
    virtual auto read_color_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_depth_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_infra_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_audio(bool enable)                -> void{static_cast<void>(enable);}
    virtual auto read_IMU(bool enable)                  -> void{static_cast<void>(enable);}
    virtual auto read_body_tracking(bool enable)        -> void{static_cast<void>(enable);}
    auto check_data_validity() -> bool;


    virtual auto generate_cloud(bool enable)            -> void{static_cast<void>(enable);}


    // depth sized color processing
    auto filter_depth_sized_color_from_depth() -> void{}
    auto mix_depth_sized_color_with_body_tracking() -> void{}

    // infra processing
    auto filter_infra_from_depth() -> void{}
    auto mix_infra_with_body_tracking() -> void{}


    // process data
    virtual auto resize_color_image_to_depth_size() -> void{}
    auto convert_color_image() -> void;
    auto filter_depth_image(const DCFiltersSettings &filtersS) -> void;
    auto filter_depth_sized_color_image(const DCFiltersSettings &filtersS) -> void;
    auto filter_infrared_image(const DCFiltersSettings &filtersS) -> void;
    auto filter_cloud_image(const DCFiltersSettings &filtersS) -> void;
    auto update_valid_depth_values() -> void;

    // frame generation
    // # local
    auto create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>;
    auto update_infos(DCFrame *dFrame) -> void;
    auto update_color(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_depth_sized_color(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_depth(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_infra(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_cloud(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_audio(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_imu(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_bodies(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    auto update_calibration(const DCDataSettings &dataS, DCFrame *dFrame) -> void;
    // # compressed
    auto compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>;
    auto update_compressed_frame_infos(DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_color(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_depth_sized_color(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_depth(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_infra(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_cloud(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_audio(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_imu(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_bodies(const DCDataSettings &dataS, const DCFiltersSettings &filtersS, DCCompressedFrame *cFrame) -> void;
    auto update_compressed_frame_calibration(const DCDataSettings &dataS, DCCompressedFrame *cFrame) -> void;

    // auto debug_save_images(std::string parentPath) -> void;

    // TODO:
    auto read_frames2() -> void;
    // images preprocessing
    auto preprocess_color_image() -> void{}
    auto preprocess_depth_sized_color_image() -> void{}
    auto preprocess_depth_image() -> void{}
    auto preprocess_infra_image() -> void{}
    auto preprocess_body_tracking_image() -> void{}

    // depth processing
    auto filter_depth_basic()                   -> void{ /** depth / width / height */}
    auto filter_depth_from_depth_sized_color()  -> void{ /** invalidate depth from depth-sized color */}
    auto filter_depth_from_infra()              -> void{ /** invalidate depth from infra */}
    auto filter_depth_from_cloud()              -> void{ /** invalidate depth from cloud geometry */}
    auto filter_depth_from_body_tracking()      -> void{ /** invalidate depth from body tracking */}
    auto filter_depth_complex()                 -> void{
        //  -> local diff filtering
        //  -> min neighbours
        //  -> erode
        //  -> dilate
        //  -> bigger cluster
    }


private:

    // depth filtering
    auto maximum_local_depth_difference(const DCFrameIndices &ids, std::span<std::uint16_t> depthBuffer, float max, DCConnectivity connectivity) -> void;
    auto keep_only_biggest_cluster() -> void;
    auto mininum_neighbours(std::uint8_t nbLoops, std::uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void;
    auto erode(std::uint8_t nbLoops, DCConnectivity connectivity) -> void;
};
}
