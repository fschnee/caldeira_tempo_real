#pragma once

#include <chrono>

namespace boiler {
    struct constants
    {
        struct boiler_constants
        {
            float capacity;
            float max_limit;
            float max_normal;
            float min_normal;
            float min_limit;
        } boiler;
        struct steam_constants
        {
            float max_throughput;
            float max_gradient;
            float min_gradient;
        } steam;
        float pump_capacity; // in liters/sec.
        // cycle_time has the precision of a millisecond
        // but it 5s by default.
        std::chrono::milliseconds cycle_time = std::chrono::seconds{ 5 };
    };
}
