
#pragma once

// std
#include <concepts>
#include <numeric>
#include <cmath>

// local
#include "buffer.hpp"

namespace tool{

template<typename ElementType>
concept Arithmetic = std::integral<ElementType> or std::floating_point<ElementType>;

template<typename ElementType> requires Arithmetic<ElementType>
class NumericSpan : public std::span<ElementType>{
    using Elem = ElementType;
public:

    // NumericSpan() : std::span<Elem>(){}
    // NumericSpan(Elem *data, size_t size) : std::span<Elem>(data, size){
    // }

    [[nodiscard]] constexpr auto sum() const noexcept -> Elem{
        return std::accumulate(std::span<Elem>::cbegin(), std::span<Elem>::cend(), Elem{0});
    }

    [[nodiscard]] constexpr auto mean() const noexcept -> Elem{
        if(!std::span<Elem>::empty()){
            return sum()/static_cast<Elem>(std::span<Elem>::size());
        }
        return {};
    }
};

template<typename ElementType> requires Arithmetic<ElementType>
struct NumericBuffer : public Buffer<ElementType>{

    using Elem = ElementType;

    // get
    [[nodiscard]] constexpr auto min_id() const noexcept -> size_t{
        if(!Buffer<Elem>::empty()){
            auto min = std::min_element(Buffer<Elem>::cbegin(), Buffer<Elem>::cend(), [](const Elem &a, const Elem &b){
                return a < b;
            });
            return std::distance(Buffer<Elem>::cbegin(), min);
        }
        return {};
    }
    [[nodiscard]] constexpr auto max_id() const noexcept -> size_t{
        if(!Buffer<Elem>::empty()){
            auto max = std::max_element(Buffer<Elem>::cbegin(), Buffer<Elem>::cend(), [](const Elem &a, const Elem &b){
                return a < b;
            });
            return std::distance(Buffer<Elem>::cbegin(), max);
        }
        return {};
    }

    [[nodiscard]] constexpr auto min() const noexcept -> Elem{
        if(!Buffer<Elem>::empty()){
            return *std::min_element(Buffer<Elem>::cbegin(), Buffer<Elem>::cend());
        }
        return {};
    }
    [[nodiscard]] constexpr auto max() const noexcept -> Elem{
        if(!Buffer<Elem>::empty()){
            return *std::max_element(Buffer<Elem>::cbegin(), Buffer<Elem>::cend());
        }
        return {};
    }

    [[nodiscard]] constexpr auto diff() const noexcept -> Elem{
        if(!Buffer<Elem>::empty()){
            return max()-min();
        }
        return {};
    }

    [[nodiscard]] auto num_span() noexcept -> NumericSpan<Elem> {
        return static_cast<NumericSpan<Elem>>(Buffer<Elem>::span());
    }

    [[nodiscard]] auto num_sub_span(size_t start, size_t subSize) noexcept -> NumericSpan<Elem> {
        return static_cast<NumericSpan<Elem>>(Buffer<Elem>::sub_span(start, subSize));
    }

    [[nodiscard]] constexpr auto sum(size_t start, size_t lenght) const noexcept -> Elem{

        if(start >= Buffer<Elem>::size()){
            return {};
        }
        if(start + lenght >= Buffer<Elem>::size()){
            lenght = Buffer<Elem>::size() - start;
        }
        return std::accumulate(Buffer<Elem>::cbegin() + start, Buffer<Elem>::cbegin() + start + lenght, Elem{0});
    }
    [[nodiscard]] constexpr auto sum() const noexcept -> Elem{
        if(!Buffer<Elem>::empty()){
            return std::accumulate(Buffer<Elem>::cbegin(), Buffer<Elem>::cend(), Elem{0});
        }
        return {};
    }

    [[nodiscard]] constexpr auto mean(size_t start, size_t lenght) const noexcept -> Elem{
        if(!Buffer<Elem>::empty()){
            return sum(start, lenght)/static_cast<Elem>(lenght);
        }
        return {};
    }
    [[nodiscard]] constexpr auto mean() const noexcept -> Elem{
        if(!Buffer<Elem>::empty()){
            return sum()/static_cast<Elem>(Buffer<Elem>::size());
        }
        return {};
    }

    [[nodiscard]] constexpr auto variance() const noexcept -> Elem{
        Elem variance{0};
        auto meanV = mean();
        for(const auto &v : Buffer<Elem>::values){
            variance += std::pow(v - meanV, 2);
        }
        return variance;
    }

    [[nodiscard]] constexpr auto standard_deviation() const noexcept -> Elem{
        return std::sqrt(variance() / Buffer<Elem>::size());
    }

    // modify
    constexpr auto sort_ascendant() noexcept -> void{
        std::sort(Buffer<Elem>::begin(), Buffer<Elem>::end(), std::less<Elem>());
    }
    constexpr auto sort_descendant() noexcept -> void{
        std::sort(Buffer<Elem>::begin(), Buffer<Elem>::end(), std::greater<Elem>());
    }
};


}

