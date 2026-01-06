

/*******************************************************************************
** Toolset-ts-base                                                            **
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

#include "stop_watch.hpp"

// local
#include "utility/time.hpp"

using namespace tool;
using namespace std::chrono;

auto StopWatch::reset() noexcept -> void{
    m_isStarted  = false;
    m_total      = nanoseconds(0);
    m_lastStart  = nanoseconds(0);
}

auto StopWatch::start() noexcept -> void{

    if(is_started()){
        return;
    }

    m_lastStart = Time::nanoseconds_since_epoch();
    m_isStarted = true;
}

auto StopWatch::stop() noexcept -> void{

    if(!is_started()){
        return;
    }

    m_total    += (Time::nanoseconds_since_epoch() - m_lastStart);
    m_isStarted = false;
}

auto StopWatch::ellapsed_nano_s() const noexcept -> std::chrono::nanoseconds{
    if(is_started()){
        return m_total + (Time::nanoseconds_since_epoch() - m_lastStart);
    }else{
        return m_total;
    }
}

auto StopWatch::set_current_time(double timeMs) noexcept -> void{

    auto newTime = duration_cast<nanoseconds>(microseconds(static_cast<int>(timeMs*1000.0)));
    if(is_started()){
        m_lastStart = Time::nanoseconds_since_epoch() - newTime;
        m_total = {};
    }else{
        m_total = newTime;
    }
}

auto StopWatch::ellapsed_s() const noexcept -> double{
    return ellapsed_nano_s().count() * 0.000000001;
}

auto StopWatch::ellapsed_micro_s() const noexcept -> double{
    return ellapsed_nano_s().count() * 0.001;
}

auto tool::StopWatch::ellapsed_milli_s() const noexcept -> double{
    return ellapsed_nano_s().count() * 0.000001;
}
