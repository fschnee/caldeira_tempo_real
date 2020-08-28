#pragma once

#include <functional>
#include <typeindex>
#include <optional>
#include <vector>
#include <string>
#include <any>
#include <map>

#include "messages.hpp"


class control_unit {
public:
    enum class states {
        initialization, 
        normal,
        degraded,
        rescue,
        emergency_stop,
    } state = states::initialization;

    auto switch_state(states newstate)
    {
        state = newstate;
        handlers.clear();
    }

    template <typename Msg, typename Func>
    auto listen_for(Func&& handler)
    {
        handlers.insert(std::make_pair(
            std::type_index(typeid(Msg)).hash_code(),
            std::function< void(Msg) >{std::forward<Func>(handler)}
        ));
    }

    auto on_init(const std::vector<messages::to_program::any>& messages)
    {
        listen_for<messages::from_units::steam_boiler_waiting>([&](auto msg) {
            auto all_ok = false;
            if(all_ok) {
                to_units.push_back(messages::to_units::program_ready{});

                listen_for<messages::from_units::physical_units_ready>([&](auto msg){
                    switch_state(states::normal);
                });
            }
        });
    }

    auto on_normal(const std::vector<messages::to_program::any>& messages)
    {}

    auto on_degraded(const std::vector<messages::to_program::any>& messages) 
    {}

    auto on_rescue(const std::vector<messages::to_program::any>& messages)
    {}

    auto on_emergency_stop(const std::vector<messages::to_program::any>& messages)
    {}

    auto process_messages(const std::vector<messages::to_program::any>& messages)
        -> std::vector<messages::to_units::any>
    {
        to_units.clear();

        for(auto& message : messages) {
            std::visit([&](auto msg){
                auto code = std::type_index(typeid(msg)).hash_code();

                if(handlers.count(code)) {
                    auto& any_handler = handlers.find(code)->second;
                    auto handler_ptr = std::any_cast<
                            std::function< void(decltype(msg))>
                        >(&any_handler);
                    if(handler_ptr) {return (*handler_ptr)(msg);};
                }
            }, message);
        }

        switch (state)
        {
            case states::initialization: {on_init(messages);}; break;
            case states::normal: {on_normal(messages);}; break;
            case states::degraded: {on_degraded(messages);}; break;
            case states::rescue: {on_rescue(messages);} break;
            case states::emergency_stop: {on_emergency_stop(messages);} break;
        }

        return to_units;
    }

    std::map<std::size_t, std::any> handlers;
    std::vector<messages::to_units::any> to_units;
};
