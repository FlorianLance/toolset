
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

// k4a
#include "thirdparty/k4a/k4astaticimageproperties.h"
#include "thirdparty/k4a/k4amicrophonelistener.h"
#include "thirdparty/k4a/k4aaudiomanager.h"
// kabt
#include <kabt/k4abt.hpp>

// local
#include "camera/dc_device_impl.hpp"

namespace tool::camera {

struct AzureKinectDeviceImpl : public DCDeviceImpl{

    AzureKinectDeviceImpl();

    // device
    std::unique_ptr<k4a::device> device = nullptr;
    k4a::calibration calibration;
    k4a::transformation transformation;
    k4abt::tracker tracker;
    k4a_device_configuration_t k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    k4abt_tracker_configuration_t k4aBtConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
    std::unique_ptr<k4a::capture> capture = nullptr;

    // images
    std::optional<k4a::image> colorImage         = std::nullopt;
    std::optional<k4a::image> depthImage         = std::nullopt;
    std::optional<k4a::image> infraredImage      = std::nullopt;
    std::optional<k4a::image> pointCloudImage    = std::nullopt;
    // # processing
    std::optional<k4a::image> convertedColorImage = std::nullopt;
    std::optional<k4a::image> depthSizedColorImage = std::nullopt;

    // audio
    std::shared_ptr<k4a::K4AMicrophone> microphone = nullptr;
    std::shared_ptr<k4a::K4AMicrophoneListener> audioListener = nullptr;
    size_t lastFrameCount = 0;
    std::vector<k4a::K4AMicrophoneFrame> audioFrames;

    // actions
    auto open(std::uint32_t deviceId) -> bool override;
    auto start_reading(const DCConfigSettings &newConfigS) -> bool override;
    auto stop_reading() -> void override;
    auto close() -> void override;

    // getters
    auto is_opened() const noexcept -> bool override;
    auto nb_devices() const noexcept -> std::uint32_t override;
    auto device_name() const noexcept -> std::string override;

private:

    // initialization
    auto initialize_device_specific() -> void override;
    static auto generate_config(bool synchInConnected, bool synchOutConnected,const DCConfigSettings &config) -> k4a_device_configuration_t;
    static auto generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t;
    auto update_camera_from_colors_settings() -> void override;

    // get data
    auto color_data() -> std::span<ColorRGBA8> override;
    auto depth_data() -> std::span<std::uint16_t> override;
    auto infra_data() -> std::span<std::uint16_t> override;

    // read data
    auto capture_frame(std::int32_t timeoutMs) -> bool override;
    auto read_color_image() -> bool override;
    auto read_depth_image() -> bool override;
    auto read_infra_image() -> bool override;
    auto read_from_microphones() -> void override;
    auto read_from_imu() -> void override;
    auto read_bodies() -> void override;

    // process data    
    auto convert_color_image() -> void override;
    auto resize_images() -> void override;
    auto generate_cloud() -> void override;

    // frame generation
    auto compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame> override;
    auto create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame> override;

};

}
