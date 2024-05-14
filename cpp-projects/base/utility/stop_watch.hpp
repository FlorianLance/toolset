
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

namespace tool{

class StopWatch{
public:

    auto reset() noexcept -> void;
    auto start() noexcept -> void;
    auto stop() noexcept -> void;
    auto set_current_time(double timeMs) noexcept -> void;

    [[nodiscard]] constexpr auto is_started() const noexcept -> bool {return m_isStarted;}

    [[nodiscard]] auto ellapsed_s() const noexcept -> double;
    [[nodiscard]] auto ellapsed_milli_s() const noexcept -> double;
    [[nodiscard]] auto ellapsed_micro_s() const noexcept -> double;
    [[nodiscard]] auto ellapsed_nano_s() const noexcept -> std::chrono::nanoseconds;

private :

    bool m_isStarted = false;
    std::chrono::nanoseconds m_lastStart  = std::chrono::nanoseconds(0);
    std::chrono::nanoseconds m_total      = std::chrono::nanoseconds(0);
};


}
