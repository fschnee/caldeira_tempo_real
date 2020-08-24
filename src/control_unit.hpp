#pragma once

#include <vector>

#include "messages.hpp"

class control_unit {
public:
    auto process_messages(const std::vector<messages::to_program::any>& messages)
        -> std::vector<messages::to_units::any>
    {
        return {};
    }
};
