
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

// opencv
#include <opencv2/imgproc.hpp>

// k4a
#include "k4a/k4a.hpp"

// orbbec
#include <libobsensor/hpp/Filter.hpp>
#include <libobsensor/hpp/Frame.hpp>
#include <libobsensor/hpp/Pipeline.hpp>
#include <libobsensor/hpp/Device.hpp>
#include <libobsensor/hpp/Context.hpp>

// local
#include "camera/settings/dc_color_settings.hpp"
#include "camera/settings/dc_config_settings.hpp"
#include "graphics/color.hpp"

namespace tool::cam {

struct OrbbecDevice{

    // device
    std::unique_ptr<ob::Context> context        = nullptr;
    std::shared_ptr<ob::Device> device          = nullptr;
    std::vector<std::shared_ptr<ob::Device>> deviceList;
    std::shared_ptr<ob::SensorList> sensorList  = nullptr;
    std::unique_ptr<ob::Pipeline> pipe          = nullptr;
    OBCameraParam cameraParam;
    OBCalibrationParam calibrationParam;

    // frames data
    std::shared_ptr<ob::FrameSet> frameSet     = nullptr;
    std::shared_ptr<ob::ColorFrame> colorImage = nullptr;
    std::shared_ptr<ob::DepthFrame> depthImage = nullptr;
    std::shared_ptr<ob::IRFrame> infraredImage = nullptr;

    // processing
    // cv::Mat colorConvBuffer;
    // std::shared_ptr<ob::ColorFrame> convertedColorImage = nullptr;

    OrbbecDevice();

    // initialization
    auto query_devices(std::string_view deviceTypeName, bool ethernet) -> void;
    auto update_camera_from_colors_settings(bool isCameraReading, const DCColorSettings &colorS) ->void;

    // actions
    auto open_pipe(uint32_t deviceId) -> bool;
    auto start_pipe(const DCConfigSettings &configS) -> bool;
    auto stop_pipe() -> void;
    auto close_pipe() -> void;

    // getters
    auto is_opened() const noexcept -> bool;
    auto nb_devices() const noexcept -> size_t;
    auto device_name() const noexcept -> std::string;

    // read data
    auto capture_frame(int32_t timeoutMs) -> bool;
    auto read_color_image() -> std::span<std::int8_t>;
    auto read_depth_image() -> std::span<std::uint16_t>;
    auto read_infra_image() -> std::span<std::uint16_t>;

    // calibration
    auto convert_to_k4_calibration(DCMode mode) -> k4a::calibration;

private:

    auto set_property_value(OBPropertyID pId, bool value) -> void;
    auto set_property_value(OBPropertyID pId, std::int32_t value) -> void;
};

}
