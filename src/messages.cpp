#include "messages.hpp"

namespace messages::to_units {
    auto operator<<(std::ostream& os, const mode::states& val) -> std::ostream&
    {
        static constexpr const char* repr[] = {
            "initialization", "normal", "degraded", "rescue", "emergency_stop",
        };
        os << repr[val];
        return os;
    }
}

namespace messages::to_program {
    auto operator<<(std::ostream& os, const pump_state::possible_states& val)
        -> std::ostream&
    {
        os << (val ? "open" : "closed");
        return os;
    }
    auto operator<<(
        std::ostream& os,
        const pump_control_state::possible_states& val) -> std::ostream&
    {
        os << (val ? "flowing" : "not_flowing");
        return os;
    }
}