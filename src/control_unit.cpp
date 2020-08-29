#include "control_unit.hpp"

control_unit::control_unit() { switch_state(states::initialization); }

auto control_unit::process_messages(
    const std::vector<messages::to_program::any>& messages)
    -> std::vector<messages::to_units::any>
{
    response.clear();

    for (auto& message : messages) {
        notify_listener_of(message);
    }

    return response;
}

auto control_unit::on_init() -> void
{
    listen_to<messages::from_units::steam_boiler_waiting>([&](auto msg) {
        auto all_ok = false;
        if (all_ok) {
            response.push_back(messages::to_units::program_ready{});

            listen_to<messages::from_units::physical_units_ready>(
                [&](auto msg) { switch_state(states::normal); });
        }
    });
}

auto control_unit::on_normal() -> void {}

auto control_unit::on_degraded() -> void {}

auto control_unit::on_rescue() -> void {}

auto control_unit::on_emergency_stop() -> void {}

auto control_unit::switch_state(states newstate) -> void
{
    state = newstate;
    listeners.clear();

    switch (state) {
        case states::initialization: {
            on_init();
        }; break;
        case states::normal: {
            on_normal();
        }; break;
        case states::degraded: {
            on_degraded();
        }; break;
        case states::rescue: {
            on_rescue();
        } break;
        case states::emergency_stop: {
            on_emergency_stop();
        } break;
    }
}
