
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
#include "camera/dc_device.hpp"

namespace tool::camera {

class K4Device : public DCDevice{

public:

    K4Device();
    ~K4Device();

    auto open(std::uint32_t deviceId) -> bool override;
    auto refresh_devices_list() -> void override;
    auto close() -> void override;
    auto clean() -> void override;

    // getters
    auto nb_devices() const noexcept -> std::uint32_t override;
    auto device_name() const noexcept-> std::string override;
    auto device_id() const noexcept -> std::uint32_t override;
    auto is_opened() const noexcept -> bool override;
    auto cameras_started() const noexcept -> bool override;
    auto is_sync_in_connected() const noexcept -> bool override;
    auto is_sync_out_connected() const noexcept -> bool override;
    auto get_nb_capture_per_second() const noexcept -> float override;
    auto get_capture_duration_ms() noexcept -> std::int64_t override;
    auto get_processing_duration_ms() noexcept -> std::int64_t override;
    auto get_compressing_duration_ms() noexcept -> std::int64_t override;
    auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::int64_t override;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::int64_t override;

    // # config
    auto mode() const noexcept -> DCMode override;
    auto is_LED_disabled() const noexcept -> bool override;
    auto synch_mode() const noexcept -> DCSynchronisationMode override;
    auto subordinate_delay_usec() const noexcept -> int override;
    auto color_and_depth_synchronized() const noexcept -> bool override;
    auto delay_between_color_and_depth_usec() const noexcept -> int override;

    // cameras
    auto start_cameras(const DCConfigSettings &configS) -> bool override;
    auto stop_cameras() -> void override;

    // settings
    auto set_color_settings(const DCColorSettings &colorS) -> void override;
    auto set_data_settings(const DCDataSettings &dataS) -> void override;
    auto set_filters(const DCFilters &filters) -> void override;
    auto send_data_state(bool state) -> void override;
    auto set_delay(DCDelaySettings delay) -> void override;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}

