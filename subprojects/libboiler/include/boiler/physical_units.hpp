#pragma once

#include <vector>

#include "boiler/messages.hpp"

namespace boiler {
    class physical_units
    {
    public:
        auto get_messages() -> std::vector<messages::to_program::any> { return {}; }

        auto process_messages(const std::vector<messages::to_units::any>& messages)
            -> void
        {}

        const int update_speed = 100; // in ms.
    };
}
