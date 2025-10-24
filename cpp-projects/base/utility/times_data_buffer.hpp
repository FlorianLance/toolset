

/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2025] [Florian Lance]                                       **
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
#include <memory>
#include <execution>
#include <ranges>

// local
#include "utility/numeric_buffer.hpp"

namespace  tool {

template <typename Value>
struct TimeValuesBuffer{

    NumericBuffer<double> times;
    Buffer<Value> values;

    [[nodiscard]] auto nb_samples() const noexcept -> size_t{
        return values.size();
    }

    auto reset(size_t capacity) -> void{
        times.clear();
        times.reserve(capacity);
        values.clear();
        values.reserve(capacity);
    }

    auto add_data(double time, Value value, size_t limit) -> void{

        if(times.size() < limit){
            times.push_back(time);
            values.push_back(value);
        }else if(times.size() == limit){
            times.remove_first();
            times.push_back(time);
            values.remove_first();
            values.push_back(value);
        }else{
            times.remove_from_to(0, times.size()+1-limit);
            times.push_back(time);
            values.remove_from_to(0, values.size()+1-limit);
            values.push_back(value);
        }
    }

    auto remove_left_part_if_smaller(size_t wantedSize) -> void{
        if(wantedSize < times.size()){
            auto diff = times.size() - wantedSize;
            times.remove_before(diff);
            values.remove_before(diff);
        }
    }


    auto add_last_data(const TimeValuesBuffer &data, size_t limit) -> void{
        add_data(data.times.back(), data.values.back(), limit);
    }

    [[nodiscard]] auto copy_last_values(size_t size) -> std::shared_ptr<TimeValuesBuffer>{
        auto subset = std::make_shared<TimeValuesBuffer>();
        times.copy_last_values_to(size, subset->times);
        values.copy_last_values(size, subset->values);
        return subset;
    }


};


template <typename ValueContainer, typename Value>
struct TimeChannelsC{

    NumericBuffer<double> times;
    Buffer<ValueContainer> channels;

private:

    Buffer<size_t> idChannels;

public:

    auto clean() -> void{
        times.clear();
        channels.clear();
        idChannels.clear();
    }

    auto reset(size_t nbChannels, std::optional<size_t> capacity = std::nullopt) -> void{

        times.clear();
        if(capacity.has_value()){
            times.reserve(capacity.value());
        }
        channels.resize(nbChannels);
        for(auto &channel : channels){
            channel.clear();
            if(capacity.has_value()){
                channel.reserve(capacity.value());
            }
        }
        idChannels.resize(nbChannels);
        std::iota(idChannels.begin(), idChannels.end(), 0);
    }

    auto resize_samples(size_t newSize) -> void{
        times.resize(newSize);
        for(auto &channel : channels){
            channel.resize(newSize);
        }
    }

    auto remove_left_part_if_smaller(size_t wantedSize) -> void{

        if(wantedSize < times.size()){

            auto diff = times.size() - wantedSize;
            times.remove_before(diff);

            for(auto &channel : channels){
                channel.remove_before(diff);
            }
        }
    }

    auto remove_left_part(double minTime) -> void{

        size_t idT = 0;
        for(const auto &t : times){
            if(minTime < t){
                break;
            }
            ++idT;
        }
        if(idT != 0){
            times.remove_before(idT);
            for(auto &channel : channels){
                channel.remove_before(idT);
            }
        }
    }

    auto destructive_merge(TimeChannelsC *iData) -> void{
        if(channels.size() == iData->channels.size()){
            times.merge(iData->times);
            for (const auto& [channel, iChannel] : std::views::zip(channels, iData->channels)){
                channel.merge(iChannel);
            }
        }
    }

    [[nodiscard]] auto nb_channels() const noexcept -> size_t{
        return channels.size();
    }

    [[nodiscard]] auto nb_samples() const noexcept -> size_t{
        if(!channels.empty()){
            return channels.front().size();
        }
        return 0;
    }



    auto check_limits(size_t limit) -> size_t{

        if(times.size() > limit){
            size_t removed = times.size();
            times.remove_from_to(0, times.size()+1-limit);
            std::for_each(std::execution::par_unseq, std::begin(channels), std::end(channels), [&](auto &channel){
                channel.remove_from_to(0, channel.size() + 1 - limit);
            });
            removed -= times.size();
            return removed;
        }
        return 0;
    }

    auto add_channel_data(size_t idChannel, Value data) -> void{

        if(idChannel >= channels.size()){
            return;
        }

        channels[idChannel].push_back(data);
    }

    auto add_data(double time, std::span<Value> eData) -> void{

        if(eData.size() > channels.size()){
            return;
        }

        times.push_back(time);

        std::for_each(std::execution::par_unseq, std::begin(idChannels), std::begin(idChannels) + eData.size() , [&](size_t id){
            channels[id].push_back(eData[id]);
        });
    }

    auto add_data(double time, std::span<std::span<Value>> eData) -> void{

        if(eData.empty()){
            return;
        }

        // add times
        for(size_t id = 0; id < eData.size(); ++id){
            times.push_back(time + id);
        }

        std::for_each(std::execution::par_unseq, std::begin(idChannels), std::begin(idChannels) + eData.back().size(), [&](size_t idC){
            for(size_t idS = 0; idS < eData.size(); ++idS){
                channels[idC].push_back(eData[idS][idC]);
            }
        });
    }

    [[nodiscard]] auto copy_last_values(size_t size, std::span<std::int8_t> disablingMask) -> std::shared_ptr<TimeChannelsC>{

        auto subset = std::make_shared<TimeChannelsC>();
        times.copy_last_values_to(size, subset->times);

        if(disablingMask.size() == channels.size()){

            int maskedNbChannels = static_cast<int>(disablingMask.size() - std::accumulate(disablingMask.cbegin(), disablingMask.cend(), int{0}));
            subset->channels.resize(maskedNbChannels);

            size_t idMC = 0;
            for(size_t idC = 0; idC < channels.size(); ++idC){
                if(disablingMask[idC] != 1){
                    channels[idC].copy_last_values_to(size, subset->channels[idMC++]);
                }
            }
        }else{
            subset->channels.resize(channels.size());
            for(size_t idC = 0; idC < channels.size(); ++idC){
                channels[idC].copy_last_values_to(size, subset->channels[idC]);
            }
        }

        return subset;
    }

    [[nodiscard]] auto divide() -> std::pair<std::shared_ptr<TimeChannelsC>,std::shared_ptr<TimeChannelsC>>{

        auto subset1 = std::make_shared<TimeChannelsC>();
        auto subset2 = std::make_shared<TimeChannelsC>();

        auto midSamples = nb_samples()/2;
        subset1->times.reserve(midSamples);
        subset1->channels.resize(nb_channels());
        for(size_t idS = 0; idS < midSamples; ++idS){
            subset1->times.push_back(times[idS]);
        }
        for(auto &nChannel : subset1->channels){
            nChannel.reserve(midSamples);
            for(size_t idS = 0; idS < midSamples; ++idS){
                nChannel.push_back(nChannel[idS]);
            }
        }

        subset2->times.reserve(midSamples);
        subset2->channels.resize(nb_channels());
        for(size_t idS = midSamples; idS < nb_samples(); ++idS){
            subset2->times.push_back(times[idS]);
        }
        for(auto &nChannel : subset2->channels){
            nChannel.reserve(midSamples);
            for(size_t idS = midSamples; idS < nb_samples(); ++idS){
                nChannel.push_back(nChannel[idS]);
            }
        }

        return {std::move(subset1),std::move(subset2)};
    }


    [[nodiscard]] auto copy() -> std::shared_ptr<TimeChannelsC>{
        auto copied = std::make_shared<TimeChannelsC>();
        times.copy(copied->times);
        copied->channels.resize(channels.size());
        for(size_t idC = 0; idC < channels.size(); ++idC){
            channels[idC].copy(copied->channels[idC]);
        }
        return copied;
    }



    // auto copy_after_time(double time) -> std::shared_ptr<TimeChannelsC>{

    //     size_t idT = 0;
    //     for(const auto &t : times){
    //         if(time <= t){
    //             break;
    //         }
    //         ++idT;
    //     }

    //     auto timesSpan  = times.sub_span(idT, times.size()-idT);

    //     Buffer<std::span<
    //     auto channelsSpan =
    //     // NumericBuffer<double> times;
    //     // Buffer<ValueContainer> channels;

    // }


    // auto merge_remove_input_overlapping(TimeChannelsC *iData) -> void{

    //     // iData->times.last();


    // }


    // auto add_data(double time, const tool::Buffer<Value> &eData, size_t limit) -> void{

        //     if(times.size() < limit){

        //         times.push_back(time);
        //         for(size_t idC = 0; idC < channels.size(); ++idC){
        //             channels[idC].push_back(eData[idC]);
        //         }

    //     }else if(times.size() == limit){

        //         times.remove_first();
        //         times.push_back(time);

    //         for(size_t idC = 0; idC < channels.size(); ++idC){
    //             channels[idC].remove_first();
    //             channels[idC].push_back(eData[idC]);
    //         }

    //     }else{
    //         times.remove_from_to(0, times.size()+1-limit);
    //         times.push_back(time);

    //         for(size_t idC = 0; idC < channels.size(); ++idC){
    //             channels[idC].remove_from_to(0, channels[idC].size()+1-limit);
    //             channels[idC].push_back(eData[idC]);
    //         }
    //     }
    // }
};

template<typename Value>
using TimeChannelsBuffer = TimeChannelsC<Buffer<Value>, Value>;

template<typename Value>
using TimeChannelsNBuffer = TimeChannelsC<NumericBuffer<Value>, Value>;

}
