#pragma once

#include <variant>

#include <iostream>

#include "type_aliases.hpp"

namespace messages::to_units {
    using namespace ta;

    struct program_ready {};
    struct valve {};
    struct open_pump {u8 n;};
    struct close_pump {u8 n;};
    struct pump_failure_detection {u8 n;};
    struct pump_control_failure_detection {u8 n;};
    struct level_failure_detection {};
    struct steam_failure_detection {};
    struct pump_repaired_acknowledgement {u8 n;};
    struct pump_control_repaired_acknowledgement {u8 n;};
    struct level_repaired_acknowledgement {};
    struct steam_repaired_acknowledgement {};

    using any = std::variant<
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
        steam_repaired_acknowledgement
    >;

    template <typename Msg, typename... Args>
    auto make(Args&&... args) -> any {return Msg{std::forward<Args>(args)...};}
}

namespace messages::to_program {
    using namespace ta;

    struct stop {};
    struct steam_boiler_waiting {};
    struct physical_units_ready {};
    struct pump_state {
        u8 n;
        enum /*class*/ possible_states : bool {open = true, closed = false} state;
    };
    struct pump_control_state {
        u8 n;
        enum /*class*/ possible_states : bool {flowing = true, not_flowing = false} state;
    };
    struct level {float liters;};
    struct steam {float liters_per_sec;};
    struct pump_repaired {u8 n;};
    struct pump_control_repaired {u8 n;};
    struct level_repaired {};
    struct steam_repaired {};
    struct pump_failure_acknowledgement {u8 n;};
    struct pump_control_failure_acknowledgement {u8 n;};
    struct level_failure_acknowledgment {};
    struct steam_outcome_failure_acknowledgment {};

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
        steam_outcome_failure_acknowledgment
    >;

    template <typename Msg, typename... Args>
    auto make(Args&&... args) -> any {return Msg{std::forward<Args>(args)...};}
}

// Just some aliases.
namespace messages {
    namespace from_program = to_units;
    namespace from_units = to_program;
}

// Some boilerplate to be able to print stuff.
namespace messages::to_units {
    auto operator<<(std::ostream& os, const program_ready& val) -> std::ostream& {
        os << "program_ready{}"; return os;
    }
    auto operator<<(std::ostream& os, const valve& val) -> std::ostream& {
        os << "valve{}"; return os;
    }
    auto operator<<(std::ostream& os, const open_pump& val) -> std::ostream& {
        os << "open_pump{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const close_pump& val) -> std::ostream& {
        os << "close_pump{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_failure_detection& val) -> std::ostream& {
        os << "pump_failure_detection{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_control_failure_detection& val) -> std::ostream& {
        os << "pump_control_failure_detection{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const level_failure_detection& val) -> std::ostream& {
        os << "level_failure_detection{}"; return os;
    }
    auto operator<<(std::ostream& os, const steam_failure_detection& val) -> std::ostream& {
        os << "steam_failure_detection{}"; return os;
    }
    auto operator<<(std::ostream& os, const pump_repaired_acknowledgement& val) -> std::ostream& {
        os << "pump_repaired_acknowledgement{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_control_repaired_acknowledgement& val) -> std::ostream& {
        os << "pump_control_repaired_acknowledgement{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const level_repaired_acknowledgement& val) -> std::ostream& {
        os << "level_repaired_acknowledgement{}"; return os;
    }
    auto operator<<(std::ostream& os, const steam_repaired_acknowledgement& val) -> std::ostream& {
        os << "steam_repaired_acknowledgement{}"; return os;
    }
}
namespace messages::to_program {
    auto operator<<(std::ostream& os, const pump_state::possible_states& val) -> std::ostream& {
        os << (val ? "open" : "closed"); return os;
    }
    auto operator<<(std::ostream& os, const pump_control_state::possible_states& val) -> std::ostream&{
        os << (val ? "flowing" : "not_flowing"); return os;
    }
    auto operator<<(std::ostream& os, const stop& val) -> std::ostream& {
        os << "stop{}"; return os;
    }
    auto operator<<(std::ostream& os, const steam_boiler_waiting& val) -> std::ostream& {
        os << "steam_boiler_waiting{}"; return os;
    }
    auto operator<<(std::ostream& os, const physical_units_ready& val) -> std::ostream& {
        os << "physical_units_ready{}"; return os;
    }
    auto operator<<(std::ostream& os, const pump_state& val) -> std::ostream& {
        os << "pump_state{n: " << u16{val.n} << ", state: " << val.state << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_control_state& val) -> std::ostream& {
        os << "pump_control_state{n: " << u16{val.n} << ", state: " << val.state << '}'; return os;
    }
    auto operator<<(std::ostream& os, const level& val) -> std::ostream& {
        os << "level{liters: " << val.liters << '}'; return os;
    }
    auto operator<<(std::ostream& os, const steam& val) -> std::ostream& {
        os << "steam{liters_per_sec: " << val.liters_per_sec << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_repaired& val) -> std::ostream& {
        os << "pump_repaired{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_control_repaired& val) -> std::ostream& {
        os << "pump_control_repaired{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const level_repaired& val) -> std::ostream& {
        os << "level_repaired{}"; return os;
    }
    auto operator<<(std::ostream& os, const steam_repaired& val) -> std::ostream& {
        os << "steam_repaired{}"; return os;
    }
    auto operator<<(std::ostream& os, const pump_failure_acknowledgement& val) -> std::ostream& {
        os << "pump_failure_acknowledgement{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const pump_control_failure_acknowledgement& val) -> std::ostream& {
        os << "pump_control_failure_acknowledgement{n: " << u16{val.n} << '}'; return os;
    }
    auto operator<<(std::ostream& os, const level_failure_acknowledgment& val) -> std::ostream& {
        os << "level_failure_acknowledgment{}"; return os;
    }
    auto operator<<(std::ostream& os, const steam_outcome_failure_acknowledgment& val) -> std::ostream& {
        os << "steam_outcome_failure_acknowledgment{}"; return os;
    }
}

