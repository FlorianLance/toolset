

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
#include "settings/dc_misc_settings.hpp"
#include "settings/dc_filters_settings.hpp"
#include "depth-camera/frame/dc_data_frame.hpp"
#include "depth-camera/frame/dc_frame.hpp"

#include "settings/dc_device_settings.hpp"

namespace tool::cam {

class DCDevice{
public:

    DCDevice();
    ~DCDevice();

    auto start_thread() -> void;
    auto stop_thread() -> void;
    auto process() -> void;

    // settings
    auto update_device_settings(const DCDeviceSettings &deviceS) -> void;
    auto update_color_settings(const DCColorSettings &colorS) -> void;
    auto update_filters_settings(const DCFiltersSettings &filtersS) -> void;
    auto update_misc_settings(const DCMiscSettings &delayS) -> void;

    // getters
    // # states
    auto is_opened() const noexcept -> bool;
    // # timing
    auto get_capture_duration_ms() noexcept -> std::int64_t;
    auto get_processing_duration_ms() noexcept -> std::int64_t;
    auto get_duration_ms(std::string_view id) noexcept -> std::int64_t;
    auto get_duration_micro_s(std::string_view id) noexcept -> std::int64_t;
    auto get_average_framerate() -> float;
    auto get_proc_usage() const -> double;

    // signals
    sigslot::signal<std::shared_ptr<DCFrame>> new_frame_signal;
    sigslot::signal<std::shared_ptr<DCDataFrame>> new_data_frame_signal;
    sigslot::signal<DCColorSettings> color_settings_reset_signal;
    sigslot::signal<int, std::string> update_device_name_signal;

private:

    struct Impl;
    std::unique_ptr<Impl> i;
};


}
