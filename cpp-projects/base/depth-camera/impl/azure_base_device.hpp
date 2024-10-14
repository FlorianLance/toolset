
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

// local
#include "depth-camera/dc_types.hpp"
#include "depth-camera/settings/dc_color_settings.hpp"
#include "depth-camera/settings/dc_config_settings.hpp"
#include "depth-camera/settings/dc_device_data_settings.hpp"
#include "graphics/color/rgb.hpp"
#include "utility/buffer.hpp"

namespace tool::cam {

struct AzureBaseDevice{

    AzureBaseDevice();
    ~AzureBaseDevice();

    // actions
    auto open(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> bool;
    auto close() -> void;

    // settings
    auto update_from_colors_settings(const DCColorSettings &colorS) ->void;

    // getters
    auto is_opened() const noexcept -> bool;
    auto nb_devices() const noexcept -> size_t;
    auto device_name() const noexcept -> std::string;

    // read data
    auto capture_frame(int32_t timeoutMs) -> bool;
    auto read_calibration()         -> BinarySpan;
    auto read_color_image()         -> BinarySpan;
    auto read_depth_image()         -> std::span<std::uint16_t>;
    auto read_infra_image()         -> std::span<std::uint16_t>;
    auto read_body_tracking()       -> std::tuple<std::span<ColorGray8>, std::span<DCBody>>;

    auto read_from_microphones()    -> std::pair<size_t, std::span<float>>;
    auto read_from_imu()            -> BinarySpan;

    // process data
    auto resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData) -> std::span<ColorRGBA8>;
    auto generate_cloud() -> std::span<geo::Pt3<std::int16_t>>;

private:

    auto initialize(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> bool;

    struct Impl;
    std::unique_ptr<Impl> i;
};

}
