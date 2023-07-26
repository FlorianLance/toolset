
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
#include "utility/time.hpp"

namespace tool{

class StopWatch{
public:

    auto reset() noexcept -> void{
        using namespace std::chrono;
        m_isStarted  = false;
        m_total      = nanoseconds(0);
        m_lastStart  = nanoseconds(0);
        m_firstStart = nanoseconds(0);
    }

    auto start() noexcept -> void{

        if(is_started()){
            return;
        }
        using namespace std::chrono;
        auto currentTime = Time::nanoseconds_since_epoch();
        if(m_total == nanoseconds(0)){
            m_firstStart = currentTime;
        }
        m_lastStart = currentTime;
        m_isStarted = true;
    }

    auto stop() noexcept -> void{

        if(!is_started()){
            return;
        }

        using namespace std::chrono;
        auto currentTime = Time::nanoseconds_since_epoch();
        m_total += (currentTime - m_lastStart);
        m_lastStart = {};
        m_isStarted = false;
    }

    auto set_current_time(double timeMs) -> void{

        using namespace std::chrono;
        auto currentTime = Time::nanoseconds_since_epoch();
        auto newTime = duration_cast<nanoseconds>(microseconds(static_cast<int>(timeMs*1000.0)));
        m_firstStart = currentTime - newTime;
        m_total = newTime;

        if(!m_isStarted){
            m_lastStart = {};
        }else{
            m_lastStart = m_firstStart;
        }
    }

    constexpr auto is_started() const noexcept -> bool {
        return m_isStarted;
    }

    auto ellapsed_s() const noexcept -> double{
        using namespace std::chrono;
        return ellapsed_nano_s().count() * 0.000000001;
    }

    auto ellapsed_milli_s() const noexcept -> double{
        using namespace std::chrono;
        return ellapsed_nano_s().count() * 0.000001;
    }

    auto ellapsed_micro_s() const noexcept -> double{
        using namespace std::chrono;
        return ellapsed_nano_s().count() * 0.001;
    }

    auto ellapsed_nano_s() const noexcept -> std::chrono::nanoseconds{
        using namespace std::chrono;
        if(is_started()){
            auto currentTime = Time::nanoseconds_since_epoch();
            return m_total + (currentTime - m_lastStart);
        }else{
            return m_total;
        }
    }

    auto fist_start_timestamp_ns() const noexcept -> std::chrono::nanoseconds{
        return m_firstStart;
    }

private :

    bool m_isStarted = false;
    std::chrono::nanoseconds m_firstStart = std::chrono::nanoseconds(0);
    std::chrono::nanoseconds m_lastStart  = std::chrono::nanoseconds(0);
    std::chrono::nanoseconds m_total      = std::chrono::nanoseconds(0);
};

}
