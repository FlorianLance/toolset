
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
#include "k4_frame.hpp"
#include "k4_compressed_frame.hpp"
#include "k4_color_settings.hpp"
#include "k4_filters.hpp"
#include "k4_data_settings.hpp"
#include "k4_config_settings.hpp"
#include "k4_delay.hpp"

namespace tool::camera {


class K4Device {

public:

    K4Device();
    ~K4Device();

    auto open(std::uint32_t deviceId) -> bool;
    auto refresh_devices_list() -> void;
    auto close() -> void;
    auto clean() -> void;

    // getters
    auto nb_devices() const noexcept -> std::uint32_t;
    auto device_name() const noexcept-> std::string;
    auto device_id() const noexcept -> std::uint32_t;
    auto is_opened() const noexcept -> bool;
    auto cameras_started() const noexcept -> bool;
    auto is_sync_in_connected() const noexcept -> bool;
    auto is_sync_out_connected() const noexcept -> bool;
    auto get_nb_capture_per_second() const noexcept -> float;
    auto get_capture_duration_ms() noexcept -> std::int64_t;
    auto get_processing_duration_ms() noexcept -> std::int64_t;
    auto get_compressing_duration_ms() noexcept -> std::int64_t;
    auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::int64_t;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::int64_t;

    // # config
    auto mode() const noexcept -> K4Mode;
    auto is_LED_disabled() const noexcept -> bool;
    auto synch_mode() const noexcept -> K4SynchronisationMode;
    auto subordinate_delay_usec() const noexcept -> int;
    auto color_and_depth_synchronized() const noexcept -> bool;
    auto delay_between_color_and_depth_usec() const noexcept -> int;

    // cameras
    auto start_cameras(const K4ConfigSettings &configS) -> bool;
    auto stop_cameras() -> void;

    // settings
    auto set_data_settings(const K4DataSettings &dataS) -> void;
    auto set_filters(const K4Filters &filters) -> void;
    auto set_color_settings(const K4ColorSettings &colorS) -> void;
    auto send_data_state(bool state) -> void;
    auto set_delay(K4Delay delay) -> void;

    // signals
    sigslot::signal<std::shared_ptr<K4Frame>> new_frame_signal;
    sigslot::signal<std::shared_ptr<K4CompressedFrame>> new_compressed_frame_signal;
    sigslot::signal<K4ImuSample> new_imu_sample_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}

