

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
#include <cstdint>
#include <string>

// local
#include "thirdparty/sigslot/signal.hpp"
#include "settings/dc_color_settings.hpp"
#include "settings/dc_data_settings.hpp"
#include "settings/dc_delay_settings.hpp"
#include "settings/dc_config_settings.hpp"
#include "settings/dc_filters_settings.hpp"
#include "dc_compressed_frame.hpp"
#include "dc_frame.hpp"

namespace tool::cam {

class DCDevice{
public:

    DCDevice(DCType type);
    ~DCDevice();

    // actions
    auto open(std::uint32_t deviceId) -> bool;
    auto start_reading(const DCConfigSettings &configS) -> bool;
    auto stop_reading() -> void;
    auto close() -> void;
    auto clean() -> void;

    // settings
    auto set_color_settings(const DCColorSettings &colorS) -> void;
    auto set_data_settings(const DCDataSettings &dataS) -> void;
    auto set_filters_settings(const DCFiltersSettings &filtersS) -> void;
    auto set_delay_settings(const DCDelaySettings &delayS) -> void;

    // getters
    auto nb_devices() const noexcept -> std::uint32_t;
    auto device_name() const noexcept -> std::string;
    auto is_opened() const noexcept -> bool;
    auto is_reading() const noexcept -> bool;
    auto get_capture_duration_ms() noexcept -> std::int64_t;
    auto get_processing_duration_ms() noexcept -> std::int64_t;
    auto get_duration_ms(std::string_view id) noexcept -> std::int64_t;
    auto get_duration_micro_s(std::string_view id) noexcept -> std::int64_t;

    // signals
    sigslot::signal<std::shared_ptr<DCFrame>> new_frame_signal;
    sigslot::signal<std::shared_ptr<DCCompressedFrame>> new_compressed_frame_signal;
    sigslot::signal<DCImuSample> new_imu_sample_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
