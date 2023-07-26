
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
#include "camera/kinect4/k4_frame.hpp"
#include "camera/kinect4/k4_compressed_frame.hpp"
#include "camera/kinect4/k4_device_settings.hpp"
#include "camera/kinect4/k4_filters.hpp"
#include "camera/kinect4/k4_delay.hpp"
#include "camera/kinect4/k4_color_settings.hpp"

namespace tool::camera {

template<typename ...arg>
using SSS = sigslot::signal<arg...>;

class K4DeviceManager{
public:

    K4DeviceManager();
    ~K4DeviceManager();

    auto initialize() -> void;
    auto clean() -> void;

    auto update_delay(K4Delay delayMs) -> void;
    auto update_device_list() -> void;
    auto update_settings(const K4DeviceSettings &settings) -> void;
    auto update_filters(const K4Filters &filters) -> void;
    auto update_color_settings(const K4ColorSettings &colorS) -> void;

    auto get_capture_duration_ms() -> std::int64_t;
    auto get_processing_duration_ms() -> std::int64_t;
    auto get_compressing_duration_ms() -> std::int64_t;
    auto get_duration_between_ms(std::string_view from, std::string_view to) noexcept -> std::int64_t;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to) noexcept -> std::int64_t;

    auto get_nb_capture_per_second() -> float;

//    auto get_last_compressed_frame()  -> std::shared_ptr<K4CompressedFrame>;

    sigslot::signal<std::shared_ptr<K4Frame>> new_frame_signal;
    sigslot::signal<std::shared_ptr<K4CompressedFrame>> new_compressed_frame_signal;
    sigslot::signal<K4ImuSample> new_imu_sample_signal;
    static inline SSS<int, std::string> update_device_name_signal;

private:

    auto init_connections() -> void;

    struct Impl;
    std::unique_ptr<Impl> i;
};
}
