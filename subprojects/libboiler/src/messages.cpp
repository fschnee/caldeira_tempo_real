#include "boiler/messages.hpp"

#include <type_traits> // std::underlying_type.

namespace boiler::messages::to_units {
    auto operator<<(std::ostream& os, const mode::possible_modes& val) -> std::ostream&
    {
        static constexpr const char* repr[] = {
            "initialization", "normal", "degraded", "rescue", "emergency_stop",
        };
        os << repr[static_cast<std::underlying_type<mode::possible_modes>::type>(val)];
        return os;
    }
}

namespace boiler::messages::to_program {
    auto operator<<(std::ostream& os, const pump_state::possible_states& val)
        -> std::ostream&
    {
        os << (val == pump_state::possible_states::open ? "open" : "closed");
        return os;
    }
    auto operator<<(std::ostream& os, const pump_control_state::possible_states& val)
        -> std::ostream&
    {
        os
            << (val == pump_control_state::possible_states::flowing ? "flowing"
                                                                    : "not_flowing");
        return os;
    }
}