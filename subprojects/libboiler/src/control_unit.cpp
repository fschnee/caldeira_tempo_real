#include "boiler/control_unit.hpp"

#include <string_view>

boiler::control_unit::control_unit(boiler::constants c) : constants{c}
{
    switch_mode(mode::initialization);
}

auto boiler::control_unit::process_messages(std::vector<msg_from_units> messages)
    -> std::vector<msg_to_units>
{
    response.clear();

    handle_expected(messages);

    for (auto& deferred : run_last_handlers) { deferred(); }
    run_last_handlers.clear();

    send(boiler::messages::to_units::mode{ mode_of_operation }).now();

    return response;
}

auto boiler::control_unit::emergency_stop() -> void { switch_mode(mode::emergency_stop); }

auto boiler::control_unit::init_routine() -> void {
    expect<boiler::messages::to_program::steam_boiler_waiting>().eventually([&](auto msg){
        // Run last since we don't know if the messages have been handled yet.
        if(mode_of_operation != mode::initialization) {
            run_last([&]{
                // Transmission error (was not expecting this message).
                emergency_stop();
            });
        } else {
            run_last([&]{
                // Check stuff.
                if (readings.steam_liters_per_sec != 0) {
                    emergency_stop();
                    return;
                }

                if (readings.level_liters > constants.boiler.capacity || readings.level_liters < 0) {
                    emergency_stop();
                    return;
                }
                
                if (readings.level_liters > constants.boiler.max_normal) {
                    send(boiler::messages::to_units::valve{}).now();
                } else if (readings.level_liters < constants.boiler.min_normal) {
                    send(boiler::messages::to_units::open_pump{1}).now();
                } else {
                    send(boiler::messages::to_units::close_pump{1}).now();

                    send(boiler::messages::to_units::program_ready{}).until_ack([](auto ack){
                        run_last([&]{
                            if (assumptions.pump_broken ||
                                assumptions.pump_control_broken ||
                                assumptions.steam_broken ||
                                assumptions.level_broken)
                            {
                                switch_mode(mode::degraded);
                            } else {
                                switch_mode(mode::normal);
                            }
                        });
                    });
                }
            });
        }
    });
}

auto boiler::control_unit::normal_routine() -> void {

    if (readings.level_liters > constants.boiler.capacity || readings.level_liters < 0) {
        switch_mode(mode::rescue);
    } else if (assumptions.pump_broken ||
        assumptions.pump_control_broken ||
        assumptions.steam_broken ||
        assumptions.level_broken) {

        switch_mode(mode::degraded);
    } else if (readings.level_liters > 0.95 * constants.boiler.max_limit || readings.level_liters < 1.5 * constants.boiler.min_limt) {
        emergency_stop();

    } else if (readings.level_liters < constants.boiler.min_normal) {
        send(boiler::messages::to_units::open_pump{1}).now();

    } else if (readings.level_liters > constants.boiler.max_normal) {
        send(boiler::messages::to_units::close_pump{1}).now();
    }
}

auto boiler::control_unit::degraded_routine() -> void {}

auto boiler::control_unit::rescue_routine() -> void {}

auto boiler::control_unit::emergency_stop_routine() -> void {}

auto boiler::control_unit::run_mode_routine() -> void
{
    switch (mode_of_operation) {
        case mode::initialization: {
            init_routine();
        } break;
        case mode::normal: {
            normal_routine();
        } break;
        case mode::degraded: {
            degraded_routine();
        } break;
        case mode::rescue: {
            rescue_routine();
        } break;
        case mode::emergency_stop: {
            emergency_stop_routine();
        } break;
    }
}

auto boiler::control_unit::run_last(std::function<void(void)> func) -> void
{
    run_last_handlers.push_back(std::move(func));
}

auto boiler::control_unit::handle_expected(std::vector<msg_from_units>& messages) -> void
{
    auto unhandled = std::vector<std::string_view>(expected_handlers.size());
    for (const auto& it : expected_handlers) { unhandled.push_back(it.first); }

    auto unlistened = std::vector<std::string_view>(expected_handlers.size());

    const auto handle_message = [&](auto msg) {
        auto msgname = limbo::type_name<decltype(msg)>();

        const auto& it = expected_handlers.find(msgname);
        if (it == expected_handlers.end()) { return false; }

        unhandled.erase(std::find(unhandled.begin(), unhandled.end(), it->first));

        if (it->second.on_present(msg) == msg_handler::response::unlisten) {
            unlistened.push_back(msgname);
        }

        return true;
    };

    for (auto it = messages.begin(); it != messages.end();) {
        auto erase = std::visit(handle_message, *it);
        if (erase) { it = messages.erase(it); }
        if (it != messages.end()) { ++it; }
    }

    for (const auto& missing_message : unhandled) {
        const auto& it = expected_handlers.find(missing_message);

        if (it->second.on_missing() == msg_handler::response::unlisten) {
            unlistened.push_back(it->first);
        }
    }

    for (const auto& msgname : unlistened) {
        expected_handlers.erase(expected_handlers.find(msgname));
    }
}

auto boiler::control_unit::switch_mode(mode newmode) -> void
{
    mode_of_operation = newmode;
    run_mode_routine();
}
