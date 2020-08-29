#pragma once

#include <ostream>
#include <variant>

#include "type_aliases.hpp"

namespace messages::to_units {
    using namespace ta;

    struct mode
    {
        enum states
        {
            initialization,
            normal,
            degraded,
            rescue,
            emergency_stop,
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

    using any = std::variant<
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

    template<typename Msg, typename... Args>
    auto make(Args&&... args) -> any
    {
        return Msg{ std::forward<Args>(args)... };
    }
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

    using any = std::variant<
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

    template<typename Msg, typename... Args>
    auto make(Args&&... args) -> any
    {
        return Msg{ std::forward<Args>(args)... };
    }
}

// Just some aliases.
namespace messages {
    namespace from_program = to_units;
    namespace from_units   = to_program;
}

// Some boilerplate to be able to print stuff.
namespace messages::to_units {
    auto operator<<(std::ostream& os, const mode::states& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const mode& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const program_ready& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const valve& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const open_pump& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const close_pump& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_failure_detection& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_control_failure_detection& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const level_failure_detection& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const steam_failure_detection& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_repaired_acknowledgement& val)
        -> std::ostream&;
    auto operator<<(
        std::ostream& os,
        const pump_control_repaired_acknowledgement& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const level_repaired_acknowledgement& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const steam_repaired_acknowledgement& val)
        -> std::ostream&;
}
namespace messages::to_program {
    auto operator<<(std::ostream& os, const pump_state::possible_states& val)
        -> std::ostream&;
    auto operator<<(
        std::ostream& os,
        const pump_control_state::possible_states& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const stop& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const steam_boiler_waiting& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const physical_units_ready& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_state& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_control_state& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const level& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const steam& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_repaired& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_control_repaired& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const level_repaired& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const steam_repaired& val)
        -> std::ostream&;
    auto operator<<(std::ostream& os, const pump_failure_acknowledgement& val)
        -> std::ostream&;
    auto operator<<(
        std::ostream& os,
        const pump_control_failure_acknowledgement& val) -> std::ostream&;
    auto operator<<(std::ostream& os, const level_failure_acknowledgment& val)
        -> std::ostream&;
    auto operator<<(
        std::ostream& os,
        const steam_outcome_failure_acknowledgment& val) -> std::ostream&;
}
