
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
#include <thirdparty/taskflow/taskflow.hpp>

// local
#include "utility/time_diff.hpp"
#include "utility/monitoring.hpp"
#include "data/fastpfor_encoding.hpp"
#include "data/jpeg_encoding.hpp"
#include "frame/dc_depth_indices.hpp"
#include "frame/dc_device_data.hpp"
#include "dc_device.hpp"
#include "utility/logger.hpp"
#include "graphics/color/hsv.hpp"

// #define VCL_NAMESPACE
// #include "vectorclass.h"


namespace tool::cam {

struct DCSettings{
    DCConfigSettings config;
    DCDeviceDataSettings data;
    DCFiltersSettings filters;
    DCColorSettings color;
    DCMiscSettings delay;
};

struct DCFramesBuffer{

    auto add_frame(std::shared_ptr<cam::DCFrame> frame) -> void;
    auto add_data_frame(std::shared_ptr<cam::DCDataFrame> cFrame) -> void;
    auto take_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, std::int64_t delayMs) -> std::shared_ptr<cam::DCFrame>;
    auto get_data_frame_with_delay(std::chrono::nanoseconds afterCaptureTS, std::int64_t delayMs) -> std::shared_ptr<cam::DCDataFrame>;

    // delay buffer
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<DCFrame>>> frames;
    std::vector<std::tuple<std::chrono::nanoseconds, std::shared_ptr<cam::DCDataFrame>>> dataFrames;
};


struct DCDeviceImpl{

    DCDeviceImpl();
    virtual ~DCDeviceImpl();

    // actions
    virtual auto open(const DCConfigSettings &newConfigS) -> bool = 0;
    virtual auto close() -> void = 0;

    // process
    auto process_frames() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>;
    auto process_frames_task() -> tf::Taskflow*{return &m_processFramesT;}

    // settings
    auto set_data_settings(const DCDeviceDataSettings &dataS) -> void;
    auto set_filters_settings(const DCFiltersSettings &filtersS) -> void;
    auto set_color_settings(const DCColorSettings &colorS) -> void;
    auto set_delay_settings(const DCMiscSettings &delayS) -> void;
    virtual auto update_from_colors_settings() -> void{}

    // getters
    virtual auto is_opened() const noexcept -> bool = 0;
    // virtual auto nb_devices() const noexcept -> std::uint32_t = 0;
    virtual auto device_name() const noexcept -> std::string = 0;

    // profiling
    auto get_duration_ms(std::string_view id) -> std::optional<std::chrono::milliseconds>;
    auto get_duration_micro_s(std::string_view id) -> std::optional<std::chrono::microseconds>;
    auto get_average_framerate() -> float;

    // signals
    sigslot::signal<std::shared_ptr<DCFrame>> new_frame_signal;
    sigslot::signal<std::shared_ptr<DCDataFrame>> new_data_frame_signal;

    // current frames
    std::shared_ptr<DCFrame> frame = nullptr;
    std::shared_ptr<DCDataFrame> dFrame = nullptr;

protected:

    auto initialize(const DCConfigSettings &newConfig) -> void;

    // read data
    auto read_frame() -> bool;
    virtual auto capture_frame(std::int32_t timeoutMs)  -> bool{static_cast<void>(timeoutMs);return false;}    
    virtual auto read_color_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_depth_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_infra_image(bool enable)          -> bool{static_cast<void>(enable); return false;}
    virtual auto read_audio(bool enable)                -> void{static_cast<void>(enable);}
    virtual auto read_IMU(bool enable)                  -> void{static_cast<void>(enable);}
    virtual auto read_body_tracking(bool enable)        -> void{static_cast<void>(enable);}
    auto check_data_validity() -> bool;

    // process data
    auto process_data() -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>;
    virtual auto resize_images() -> void{}

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
    auto simd_filter() -> void; // test
    // # depth sized color processing
    auto filter_depth_sized_color_from_depth() -> void;
    auto mix_depth_sized_color_with_body_tracking() -> void{}
    // # infra processing
    auto filter_infra_from_depth() -> void;
    auto mix_infra_with_body_tracking() -> void{}

    // frame generation
    // # local
    auto update_frame_original_size_color() -> void;
    auto update_frame_depth_sized_color() -> void;
    auto update_frame_depth() -> void;
    auto update_frame_infra() -> void;
    auto update_frame_cloud() -> void;
    auto update_frame_audio() -> void;
    auto update_frame_imu() -> void;
    auto update_frame_body_tracking() -> void;
    auto update_frame_calibration() -> void;
    // # compressed
    auto update_data_frame_color() -> void;
    auto update_data_frame_depth_sized_color() -> void;
    auto update_data_frame_depth() -> void;
    auto update_data_frame_infra() -> void;
    auto update_data_frame_cloud() -> void;
    auto update_data_frame_audio() -> void;
    auto update_data_frame_imu() -> void;
    auto update_data_frame_body_tracking() -> void;
    auto update_data_frame_calibration() -> void;

    // flow
    tf::Taskflow m_processFramesT;

    // states
    DCSettings settings;
    DCModeInfos mInfos;
    DCDepthIndices dIndices;
    DCFramesBuffer frames;
    DCDeviceData fData;
    tool::s_umap<std::string_view, TimeElem> times;
    bool captureSuccess = false;
    bool dataIsValid = false;

    std::mutex timesLocker;
    FramerateBuffer framerateB;

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

    AverageBuffer v;


    // working values
    std::uint16_t minW;
    std::uint16_t maxW;
    std::uint16_t minH;
    std::uint16_t maxH;
    geo::Pt2f dRange;
    float diffRange;
    std::uint16_t minD;
    std::uint16_t maxD;
    ColorHSV hsvDiffColor;
    ColorHSV maxDiffColor;
    bool fdc;

    // plane filtering parameters
    geo::Pt3f p1;
    geo::Pt3f p2;
    geo::Pt3f p3;
    geo::Pt3f meanPt;
    geo::Vec3f AB;
    geo::Vec3f AC;
    geo::Vec3f normalV;
    geo::Pt3f pSphere;
    float squareMaxDistanceFromPoint;
    geo::Vec3f oobRot;
    geo::Mat3f oobOrientation;
    geo::Mat3f invO;
    geo::Vec3f halfDimensions;
    geo::Pt3f oobPos;

    int minNeighBoursFilteringIterations = 0;
    int erosionsIterations = 0;

    size_t currentZoneId = 1;
    int biggestZone = -1;
    size_t sizeBiggestZone = 0;
    std::uint16_t minDist;
    std::uint16_t maxDist;


    // BinaryImageBuffer *colorImage = nullptr;
    // BinaryImageBuffer *depthSizedColorImage = nullptr;
    // BinaryImageBuffer *depthData = nullptr;
    // BinaryImageBuffer *infraData = nullptr;



    // tf::Executor executor;

    // SIMD
    // std::vector<VCL_NAMESPACE::Vec16us> xIdsSIMD;
    // std::vector<VCL_NAMESPACE::Vec16us> yIdsSIMD;
    // std::vector<VCL_NAMESPACE::Vec16us> depthSIMD;
    // std::vector<VCL_NAMESPACE::Vec16sb> maskSIMD;
    // std::vector<std::uint16_t> filterDepthSIMD;

    // std::vector<VCL_NAMESPACE::Vec32us> xIdsSIMD;
    // std::vector<VCL_NAMESPACE::Vec32us> yIdsSIMD;
    // std::vector<VCL_NAMESPACE::Vec32us> depthSIMD;
    // std::vector<VCL_NAMESPACE::Vec32sb> maskSIMD;
    // std::vector<VCL_NAMESPACE::Vec16f> rgbSIMD;
    // std::vector<std::uint16_t> filterDepthSIMD;
    // std::vector<VCL_NAMESPACE::Vec32us> xIdsSIMD32;
    // std::vector<VCL_NAMESPACE::Vec32us> yIdsSIMD32;
    // std::vector<VCL_NAMESPACE::Vec32us> depthSIMD32;

private:

    // depth filtering    
    auto maximum_local_depth_difference(DCConnectivity connectivity, float max) -> void;
    auto erode(std::uint8_t nbLoops, DCConnectivity connectivity, std::uint8_t nbMinValid) -> void;
    auto keep_only_biggest_cluster() -> void;
    // auto mininum_neighbours(std::uint8_t nbLoops, std::uint8_t nbMinNeighbours, DCConnectivity connectivity) -> void;

};
}



// tasks
// auto read_data_taskflow() -> std::unique_ptr<tf::Taskflow>;
// auto process_data_taskflow() -> std::unique_ptr<tf::Taskflow>;
// auto process_frame_taskflow(tf::Taskflow &readDataTF, tf::Taskflow &processDataTF) -> std::unique_ptr<tf::Taskflow>;

// threads/tasks
// std::atomic_bool readFramesFromCameras = false;
// std::unique_ptr<std::thread> loopT = nullptr;
