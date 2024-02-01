
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

// k4a
#include "thirdparty/k4a/k4astaticimageproperties.h"
#include "thirdparty/k4a/k4amicrophonelistener.h"
#include "thirdparty/k4a/k4aaudiomanager.h"

// local
#include "camera/dc_types.hpp"
#include "camera/settings/dc_color_settings.hpp"
#include "camera/settings/dc_config_settings.hpp"
#include "graphics/color.hpp"
#include "azure_utility.hpp"


namespace tool::cam {

struct AzureBaseDevice{

    AzureBaseDevice();

    // initialization
    auto initialize(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> void;
    auto update_camera_from_colors_settings(bool isCameraReading, const DCColorSettings &colorS) ->void;

    // actions
    auto open_device(uint32_t deviceId) -> bool;
    auto start_device(const DCConfigSettings &configS) -> bool;
    auto stop_device() -> void;
    auto close_device() -> void;

    // getters
    auto is_opened() const noexcept -> bool;
    auto nb_devices() const noexcept -> size_t;
    auto device_name() const noexcept -> std::string;

    // read data
    auto capture_frame(int32_t timeoutMs) -> bool;
    auto read_color_image() -> std::span<std::int8_t>;
    auto read_depth_image() -> std::span<std::uint16_t>;
    auto read_infra_image() -> std::span<std::uint16_t>;
    auto read_from_microphones() -> std::pair<size_t, std::span<float>>;
    auto read_from_imu() -> std::optional<DCImuSample>;
    auto read_bodies(std::vector<DCBody> &bodies) -> std::span<uint8_t>;

    // process data
    auto resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData) -> std::span<ColorRGBA8>;
    auto generate_cloud() -> std::span<geo::Pt3<std::int16_t>>;

    auto calibration_data() -> std::span<std::int8_t>{
        return std::span<std::int8_t>{
            reinterpret_cast<std::int8_t*>(&calibration),
            static_cast<size_t>(sizeof(k4a_calibration_t))
        };
    }

private:

    // misc
    static auto generate_config(bool synchInConnected, bool synchOutConnected, const DCConfigSettings &config) -> k4a_device_configuration_t;
    static auto generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t;
    static auto set_property_value(k4a::device *dev, k4a_color_control_command_t pId, std::int32_t value, bool manual) -> void;
    static auto update_k4_body(DCBody &body, const k4abt_body_t &k4aBody) -> void;

    // // infos
    // DCColorResolution colorResolution = DCColorResolution::OFF;
    // DCDepthResolution depthResolution = DCDepthResolution::OFF;

    // device
    std::unique_ptr<k4a::device> device = nullptr;
    k4a::calibration calibration;
    k4a::transformation transformation;
    std::unique_ptr<k4abt::tracker> bodyTracker = nullptr;
    k4a_device_configuration_t k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    k4abt_tracker_configuration_t k4aBtConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
    std::unique_ptr<k4a::capture> capture = nullptr;

    // audio
    std::shared_ptr<k4a::K4AMicrophone> microphone = nullptr;
    std::shared_ptr<k4a::K4AMicrophoneListener> audioListener = nullptr;
    size_t lastFrameCount = 0;
    std::vector<k4a::K4AMicrophoneFrame> audioFrames;

    // frames data
    k4a::image colorImage         = nullptr;
    k4a::image depthImage         = nullptr;
    k4a::image infraredImage      = nullptr;
    k4a::image pointCloudImage    = nullptr;
    k4a::image bodiesIndexImage   = nullptr;
    k4a::image depthSizedColorImage = nullptr;
};

}
