

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

#include "monitoring.hpp"

// std
#include <numeric>

// local
#include "utility/time.hpp"

using namespace tool;
using namespace std::chrono;

Framerate::Framerate(){
    rTimes.resize(nbMaxValues, std::chrono::nanoseconds(0));
}

auto Framerate::add_frame() -> void{
    rTimes.set_current(Time::nanoseconds_since_epoch());
    rTimes.increment();
}

auto Framerate::get_framerate() -> float{
    size_t count = 0;

    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    for(const auto time : rTimes.span()){
        if(Time::difference_ms(time, currentTimestampNS).count() < 5000){
            ++count;
        }
    }

    return count / 5.f;
}

AverageSynch::AverageSynch(){
    diffNs.resize(nbMaxValues, std::chrono::nanoseconds(0));
}

auto AverageSynch::update_average_difference(int64_t timestampNS) -> void{

    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    diffNs.set_current(currentTimestampNS - nanoseconds(timestampNS));
    diffNs.increment();

    auto span = diffNs.span();
    averageDiffNs = static_cast<std::int64_t>(
        1.0 * std::accumulate(std::begin(span), std::end(span), nanoseconds(0)).count() / nbMaxValues
    );
}

AverageLatency::AverageLatency(){
    latencies.resize(nbMaxValues, 0);
}

auto AverageLatency::update_average_latency(int64_t latency) -> void{

    latencies.set_current(latency);
    latencies.increment();

    auto span = latencies.span();
    averageLatency = static_cast<std::int64_t>(
        1.0 * std::accumulate(std::begin(span), std::end(span), std::int64_t{0}) / nbMaxValues
        );
}

AverageBandwidth::AverageBandwidth(){
    bytesReceived.resize(nbMaxValues, {std::chrono::nanoseconds(0), 0});
}

auto AverageBandwidth::add_size(size_t nbBytes) -> void{
    bytesReceived.set_current({Time::nanoseconds_since_epoch(), nbBytes});
    bytesReceived.increment();
}

auto AverageBandwidth::get_bandwidth() -> size_t{

    size_t total = 0;
    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    for(const auto &time : bytesReceived.span()){
        if(Time::difference_ms(std::get<0>(time), currentTimestampNS).count() < 1000){
            total += std::get<1>(time);
        }
    }
    return total;
}

