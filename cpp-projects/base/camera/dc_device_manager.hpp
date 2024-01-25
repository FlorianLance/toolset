
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
#include "thirdparty/sigslot/signal.hpp"
#include "settings/dc_device_settings.hpp"
#include "settings/dc_delay_settings.hpp"
#include "settings/dc_color_settings.hpp"
#include "settings/dc_filters_settings.hpp"
#include "dc_frame.hpp"
#include "dc_compressed_frame.hpp"

namespace tool::cam {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class DCDeviceManager{
public:

    DCDeviceManager();
    ~DCDeviceManager();
    auto clean() -> void;

    auto update_device_settings(const DCDeviceSettings &deviceS) -> void;
    auto update_filters_settings(const DCFiltersSettings &filters) -> void;
    auto update_color_settings(const DCColorSettings &colorS) -> void;
    auto update_delay_settings(const DCDelaySettings &delayS) -> void;

    auto is_device_initialized() const noexcept -> bool;
    auto is_opened() const noexcept -> bool;
    auto is_reading() const noexcept -> bool;

    auto get_capture_duration_ms() -> std::int64_t;
    auto get_processing_duration_ms() -> std::int64_t;
    auto get_compressing_duration_ms() -> std::int64_t;
    auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::int64_t;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::int64_t;
    auto get_nb_capture_per_second() -> float;

    SSS<std::shared_ptr<DCFrame>> new_frame_signal;
    SSS<std::shared_ptr<DCCompressedFrame>> new_compressed_frame_signal;
    SSS<DCImuSample> new_imu_sample_signal;
    SSS<DCColorSettings> color_settings_reset_signal;
    SSS<int, std::string> update_device_name_signal;

private:

    auto initialize_device(DCType typeDevice) -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
