
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
#include <chrono>

// local
#include "utility/ring_buffer.hpp"

namespace tool{

struct AverageBuffer{
    AverageBuffer();
    auto add_value(double value) -> void;
    auto get() const -> double;
private:
    static constexpr size_t nbMaxValues = 100;
    SingleRingBuffer<double> rValues;
    size_t nbValuesAdded = 0;
};

struct SumBuffer{
    SumBuffer();
    auto add_value(double value) -> void;
    auto get() const -> double;
private:
    static constexpr size_t nbMaxValues = 1000;
    SingleRingBuffer<double> rValues;
    size_t nbValuesAdded = 0;
};

struct FramerateBuffer{
    FramerateBuffer();
    auto add_frame() -> void;
    auto get_framerate() const -> float;
private:
    static constexpr size_t nbMaxValues = 500;
    SingleRingBuffer<std::chrono::nanoseconds> rTimes;
};

struct AverageBandwidthBuffer{
    AverageBandwidthBuffer();
    auto add_size(size_t nbBytes) -> void;
    auto get_bandwidth() -> size_t;
private:
    static constexpr size_t nbMaxValues = 1000;
    SingleRingBuffer<std::tuple<std::chrono::nanoseconds, size_t>> bytesReceived;
};

struct AverageSynchBuffer{
    AverageSynchBuffer();
    // auto update_average_difference(std::int64_t timestampNS) -> void;
    auto update_average_difference(std::int64_t diffNS) -> void;
    std::int64_t averageDiffNS = 0;
private:
    static constexpr size_t nbMaxValues = 300;
    // SingleRingBuffer<std::chrono::nanoseconds> diffNs;
    SingleRingBuffer<std::int64_t> diffNs;
};

struct AverageLatencyBuffer{
    AverageLatencyBuffer();
    auto update_average_latency(std::int64_t latency) -> void;
    std::int64_t averageLatency = 0;
private:
    static constexpr size_t nbMaxValues = 100;
    SingleRingBuffer<std::int64_t> latencies;
};
}
