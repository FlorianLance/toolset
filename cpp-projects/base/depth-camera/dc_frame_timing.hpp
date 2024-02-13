
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
#include <string_view>
#include <optional>
#include <chrono>

// local
#include "utility/string_unordered_map.hpp"

namespace tool::cam{

struct DCFrameTiming{

    auto reset() -> void;
    auto swap_local_timestamps() -> void;
    auto update_local(std::string_view name, std::chrono::nanoseconds timeNs) -> void;
    auto update_local(std::string_view name) -> void;
    auto compute_capture_framerate() -> void;

    auto get_local(std::string_view name) const -> std::chrono::nanoseconds;
    auto get_duration_between_ms(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::milliseconds>;
    auto get_duration_between_micro_s(std::string_view from, std::string_view to)  noexcept -> std::optional<std::chrono::microseconds>;

    // profiling
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> timestamps;
    s_umap<std::string_view, std::optional<std::chrono::nanoseconds>> localTimestamps;
    float nbCapturePerSecond = 0.f;
    std::vector<std::chrono::nanoseconds> capturesTimes;
};
}
