
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
#include <chrono>

namespace tool{

struct Time{

    [[nodiscard]] [[maybe_unused]] static auto nanoseconds_since_epoch() noexcept -> std::chrono::nanoseconds{
        using namespace std::chrono;
        return time_point<system_clock, nanoseconds>(system_clock::now()).time_since_epoch();
    }

    [[nodiscard]] [[maybe_unused]] static auto microseconds_since_epoch() noexcept -> std::chrono::microseconds{
        using namespace std::chrono;
        return duration_cast<microseconds>(nanoseconds_since_epoch());
    }

    [[nodiscard]] [[maybe_unused]] static auto milliseconds_since_epoch() noexcept -> std::chrono::milliseconds{
        using namespace std::chrono;
        return duration_cast<milliseconds>(nanoseconds_since_epoch());
    }

    [[nodiscard]] [[maybe_unused]] static auto to_micro_s(std::chrono::nanoseconds t) noexcept -> std::chrono::microseconds{
        using namespace std::chrono;
        return duration_cast<microseconds>(t);
    }

    [[nodiscard]] [[maybe_unused]] static auto to_ms(std::chrono::nanoseconds t) noexcept -> std::chrono::milliseconds{
        using namespace std::chrono;
        return duration_cast<milliseconds>(t);
    }

    [[nodiscard]] [[maybe_unused]] static auto difference_ns(std::chrono::nanoseconds tStart, std::chrono::nanoseconds tEnd) noexcept -> std::chrono::nanoseconds{
        return tEnd-tStart;
    }

    [[nodiscard]] [[maybe_unused]] static auto difference_ms(std::chrono::nanoseconds tStart, std::chrono::nanoseconds tEnd) noexcept -> std::chrono::milliseconds{
        return to_ms(difference_ns(tStart,tEnd));
    }

    [[nodiscard]] [[maybe_unused]] static auto difference_micro_s(std::chrono::nanoseconds tStart, std::chrono::nanoseconds tEnd) noexcept -> std::chrono::microseconds{
        return to_micro_s(difference_ns(tStart,tEnd));
    }

    [[nodiscard]] [[maybe_unused]] static auto now_difference_ns(std::chrono::nanoseconds tStart) noexcept -> std::chrono::nanoseconds{
        return difference_ns(tStart, nanoseconds_since_epoch());
    }

    [[nodiscard]] [[maybe_unused]] static auto now_difference_ms(std::chrono::nanoseconds tStart) noexcept -> std::chrono::milliseconds{
        return to_ms(now_difference_ns(tStart));
    }

    [[nodiscard]] [[maybe_unused]] static auto now_difference_micro_s(std::chrono::nanoseconds tStart) noexcept -> std::chrono::microseconds{
        return to_micro_s(now_difference_ns(tStart));
    }
};

struct TimeElem{
    std::chrono::nanoseconds start      = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds end        = std::chrono::nanoseconds{0};
    std::chrono::nanoseconds lastDiff   = std::chrono::nanoseconds{0};
};

}
