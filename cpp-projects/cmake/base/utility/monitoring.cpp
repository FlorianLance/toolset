

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

FramerateBuffer::FramerateBuffer(){
    rTimes.resize(nbMaxValues, std::chrono::nanoseconds(0));
}

auto FramerateBuffer::add_frame() -> void{
    rTimes.set_current(Time::nanoseconds_since_epoch());
    rTimes.increment();
}

auto FramerateBuffer::get_framerate() const -> float{
    size_t count = 0;

    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    for(const auto time : rTimes.span()){
        if(Time::difference_ms(time, currentTimestampNS).count() < 1000){
            ++count;
        }
    }

    return count;
}

AverageSynchBuffer::AverageSynchBuffer(){
    // diffNs.resize(nbMaxValues, std::chrono::nanoseconds(0));
    diffNs.resize(nbMaxValues, std::chrono::nanoseconds(0));
}

auto AverageSynchBuffer::update_average_difference(std::chrono::nanoseconds diffNS) -> void{

    // auto currentTimestampNS = Time::nanoseconds_since_epoch();
    diffNs.set_current(diffNS);//currentTimestampNS - nanoseconds(timestampNS));
    diffNs.increment();

    auto span = diffNs.span();
    // averageDiffNs = static_cast<std::int64_t>(
    //     1.0 * std::accumulate(std::begin(span), std::end(span), nanoseconds(0)).count() / nbMaxValues
    // );
    averageDiffNS = std::chrono::nanoseconds(static_cast<std::int64_t>(1.0 * std::accumulate(std::begin(span), std::end(span), nanoseconds(0)).count() / nbMaxValues));
}

AverageLatencyBuffer::AverageLatencyBuffer(){
    latencies.resize(nbMaxValues, 0);
}

auto AverageLatencyBuffer::update_average_latency(int64_t latency) -> void{

    latencies.set_current(latency);
    latencies.increment();

    auto span = latencies.span();
    averageLatency = static_cast<std::int64_t>(
        1.0 * std::accumulate(std::begin(span), std::end(span), std::int64_t{0}) / nbMaxValues
    );
}

AverageBandwidthBuffer::AverageBandwidthBuffer(){
    bytesReceived.resize(nbMaxValues, {std::chrono::nanoseconds(0), 0});
}

auto AverageBandwidthBuffer::add_size(size_t nbBytes) -> void{
    bytesReceived.set_current({Time::nanoseconds_since_epoch(), nbBytes});
    bytesReceived.increment();
}

auto AverageBandwidthBuffer::get_bandwidth() -> size_t{

    size_t total = 0;
    auto currentTimestampNS = Time::nanoseconds_since_epoch();
    for(const auto &time : bytesReceived.span()){
        if(Time::difference_ms(std::get<0>(time), currentTimestampNS).count() < 1000){
            total += std::get<1>(time);
        }
    }
    return total;
}

AverageBuffer::AverageBuffer(){
    rValues.resize(nbMaxValues, 0.0);
}

auto AverageBuffer::add_value(double value) -> void{
    rValues.set_current(value);
    rValues.increment();
    ++nbValuesAdded;
}

auto AverageBuffer::get() const -> double{
    const auto span = rValues.span();
    auto nbValues = nbValuesAdded > nbMaxValues ? nbMaxValues : nbValuesAdded;
    if(nbValues > 0){
        return std::accumulate(std::begin(span), std::end(span), 0.0) / nbValues;
    }
    return 0.0;
}

SumBuffer::SumBuffer(){
    rValues.resize(nbMaxValues, 0.0);
}

auto SumBuffer::add_value(double value) -> void{
    rValues.set_current(value);
    rValues.increment();
    ++nbValuesAdded;
}

auto SumBuffer::get() const -> double{
    const auto span = rValues.span();
    auto nbValues = nbValuesAdded > nbMaxValues ? nbMaxValues : nbValuesAdded;
    return std::accumulate(std::begin(span), std::begin(span) + nbValues, 0.0);
}
