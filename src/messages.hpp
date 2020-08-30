#pragma once

#include <ostream>
#include <variant>

#include "utils.hpp"
#include "type_aliases.hpp"

#include "limbo/type_list.hpp"
#include "limbo/limbo.hpp" // limbo::type_name.

namespace messages::to_units {
    using namespace ta;

    struct mode
    {
        enum states
        {
            initialization = 0,
            normal         = 1,
            degraded       = 2,
            rescue         = 3,
            emergency_stop = 4,
        } m;
    };
    struct program_ready
    {};
    struct valve
    {};
    struct open_pump
    {
        u8 n;
    };
    struct close_pump
    {
        u8 n;
    };
    struct pump_failure_detection
    {
        u8 n;
    };
    struct pump_control_failure_detection
    {
        u8 n;
    };
    struct level_failure_detection
    {};
    struct steam_failure_detection
    {};
    struct pump_repaired_acknowledgement
    {
        u8 n;
    };
    struct pump_control_repaired_acknowledgement
    {
        u8 n;
    };
    struct level_repaired_acknowledgement
    {};
    struct steam_repaired_acknowledgement
    {};

    using types = limbo::type_list<
        mode,
        program_ready,
        valve,
        open_pump,
        close_pump,
        pump_failure_detection,
        pump_control_failure_detection,
        level_failure_detection,
        steam_failure_detection,
        pump_repaired_acknowledgement,
        pump_control_repaired_acknowledgement,
        level_repaired_acknowledgement,
        steam_repaired_acknowledgement>;
    using any = types::recover_to<std::variant>;
}

namespace messages::to_program {
    using namespace ta;

    struct stop
    {};
    struct steam_boiler_waiting
    {};
    struct physical_units_ready
    {};
    struct pump_state
    {
        u8 n;
        enum /*class*/ possible_states : bool
        {
            open   = true,
            closed = false
        } state;
    };
    struct pump_control_state
    {
        u8 n;
        enum /*class*/ possible_states : bool
        {
            flowing     = true,
            not_flowing = false
        } state;
    };
    struct level
    {
        float liters;
    };
    struct steam
    {
        float liters_per_sec;
    };
    struct pump_repaired
    {
        u8 n;
    };
    struct pump_control_repaired
    {
        u8 n;
    };
    struct level_repaired
    {};
    struct steam_repaired
    {};
    struct pump_failure_acknowledgement
    {
        u8 n;
    };
    struct pump_control_failure_acknowledgement
    {
        u8 n;
    };
    struct level_failure_acknowledgment
    {};
    struct steam_outcome_failure_acknowledgment
    {};

    using types = limbo::type_list<
        stop,
        steam_boiler_waiting,
        physical_units_ready,
        pump_state,
        pump_control_state,
        level,
        steam,
        pump_repaired,
        pump_control_repaired,
        level_repaired,
        steam_repaired,
        pump_failure_acknowledgement,
        pump_control_failure_acknowledgement,
        level_failure_acknowledgment,
        steam_outcome_failure_acknowledgment>;
    using any = types::recover_to<std::variant>;
}

// Just some aliases.
namespace messages {
    namespace from_program = to_units;
    namespace from_units   = to_program;
}

// Stuff for printing.
namespace messages::to_units {
    auto operator<<(std::ostream& os, const mode::states& val) -> std::ostream&;
}
namespace messages::to_program {
    auto operator<<(std::ostream& os, const pump_state::possible_states& val)
        -> std::ostream&;
    auto operator<<(
        std::ostream& os,
        const pump_control_state::possible_states& val) -> std::ostream&;
}
namespace messages {
    // Spooky template wizardry.
    namespace detection_exprs {
        template<typename T>
        using has_n_expr = decltype(std::declval<T>().n);
        template<typename T>
        using has_m_expr = decltype(std::declval<T>().m);
        template<typename T>
        using has_state_expr = decltype(std::declval<T>().state);
        template<typename T>
        using has_liters_expr = decltype(std::declval<T>().liters);
        template<typename T>
        using has_liters_per_sec_expr =
            decltype(std::declval<T>().liters_per_sec);
    }

    // General overload, ::to_units and ::to_program defer to this, see below.
    template<
        typename MsgType,
        typename = std::enable_if_t<
            to_program::types::contains<utils::remove_cv_ref_t<MsgType>> ||
            to_units::types::contains<utils::remove_cv_ref_t<MsgType>>>>
    auto operator<<(std::ostream& os, MsgType&& msg) -> std::ostream&
    {
        using namespace ta;
        using namespace detection_exprs;

        // Just some helpers for putting a comma after each member.
        enum class members
        {
            n              = 0,
            m              = 1,
            state          = 2,
            liters         = 3,
            liters_per_sec = 4
        };
        static constexpr bool has_member[] = {
            limbo::is_detected_v<has_n_expr, MsgType>,
            limbo::is_detected_v<has_m_expr, MsgType>,
            limbo::is_detected_v<has_state_expr, MsgType>,
            limbo::is_detected_v<has_liters_expr, MsgType>,
            limbo::is_detected_v<has_liters_per_sec_expr, MsgType>
        };
        [[maybe_unused]] /* Avoid compiler warnings when called with messages
                            that have no state */
        constexpr auto has_member_after = [](members member) {
            for (auto it = has_member + (u32)member + 1;
                 it < (has_member + sizeof(has_member) - 1);
                 ++it) {
                if (*it) return true;
            }
            return false;
        };

        // Actual printing starts here.
        os << limbo::type_name<MsgType>() << '{';

        if constexpr (has_member[(u32)members::n]) {
            os << "n: " << u16{ msg.n };
            if constexpr (has_member_after(members::n)) {
                os << ", ";
            }
        }
        if constexpr (has_member[(u32)members::m]) {
            os << "m: " << msg.m;
            if constexpr (has_member_after(members::m)) {
                os << ", ";
            }
        }
        if constexpr (has_member[(u32)members::state]) {
            os << "state: " << msg.state;
            if constexpr (has_member_after(members::state)) {
                os << ", ";
            }
        }
        if constexpr (has_member[(u32)members::liters]) {
            os << "liters: " << msg.liters;
            if constexpr (has_member_after(members::liters)) {
                os << ", ";
            }
        }
        if constexpr (has_member[(u32)members::liters_per_sec]) {
            os << "liters_per_sec: " << msg.liters_per_sec;
            if constexpr (has_member_after(members::liters_per_sec)) {
                os << ", ";
            }
        }

        os << '}';
        return os;
    }

    // Without these the compiler wouldn't know which overload to choose since
    // messages::operator<< would be invisible (the compiler looks for an
    // overload of operator<< at the same namespace as the type). Also, gcc
    // has a defect where it does fine even without these definitions but we
    // shouldn't rely on that.
    namespace to_units {
        using messages::operator<<;
    }
    namespace to_program {
        using messages::operator<<;
    }
}
