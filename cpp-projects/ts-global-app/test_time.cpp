
/*******************************************************************************
** Toolset-ts-global-app                                                      **
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

#include "tests.hpp"

// std
#include <iostream>

// base
#include "utility/time.hpp"

auto time() -> void{

    // auto now = system_clock::now();
    // auto t = now.time_since_epoch();

    using namespace tool;
    using namespace std::chrono;

    {
        auto t1 = Time::nanoseconds_since_epoch();
        auto t2 = t1 - nanoseconds(10000);
        auto t3 = t1 + nanoseconds(10000);


        auto t4 = t2 - t1;
        auto t5 = t1 - t2;

        auto t6 = t3 - t1;
        auto t7 = t1 - t3;

        std::cout << "t4 " << t4.count() << "\n";
        std::cout << "t5 " << t5.count() << "\n";
        std::cout << "t6 " << t6.count() << "\n";
        std::cout << "t7 " << t7.count() << "\n";
    }

    // Time::difference_micro_s(nanoseconds(diff), Time::nanoseconds_since_epoch()).count(),
    // auto timestamp1 = system_clock::now().time_since_epoch();
    // std::this_thread::sleep_for(1000ms);
    // auto timestamp2 = system_clock::now().time_since_epoch();
    // std::cout << "timestamp1: " << timestamp1.count() << "\n";
    // std::cout << "timestamp2: " << timestamp2.count() << "\n";
    // std::cout << "timestamp2: " << (timestamp2 - timestamp1).count() << "\n";
    // std::cout << "timestamp : " << duration_cast<microseconds>(timestamp2 - timestamp1).count() << "\n";

    // std::cout << "timestampT: " << duration_cast<microseconds>(nanoseconds(timestamp2) - nanoseconds(timestamp1)).count() << "\n";


}
