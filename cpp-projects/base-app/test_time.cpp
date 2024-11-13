

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
