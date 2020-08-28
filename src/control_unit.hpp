#pragma once

#include <functional> // std::function.
#include <typeindex>
#include <vector>
#include <any>
#include <map>

#include "messages.hpp"

/// Summary:
class control_unit {
public:
    auto process_messages(const std::vector<messages::to_program::any>& messages)
        -> std::vector<messages::to_units::any>;

protected:
    template <typename Msg, typename Func>
    auto listen_for(Func&& handler) -> void;
    template <typename Msg>
    auto unlisten_for() -> void;
    template <typename Msg>
    auto call_handler_for(Msg&& message) -> void;

    enum class states {
        initialization, 
        normal,
        degraded,
        rescue,
        emergency_stop,
    };
    // Called when state is entered.
    auto on_init() -> void;
    auto on_normal() -> void;
    auto on_degraded() -> void;
    auto on_rescue() -> void;
    auto on_emergency_stop() -> void;
    // The handler map is cleared every time the
    // state changes so make sure to take that into
    // account when writing the ::on_* methods.
    auto switch_state(states newstate) -> void;

private:
    states state = states::initialization;
    std::map<std::size_t, std::any> handlers;
    std::vector<messages::to_units::any> to_units;
};

/// Implementation:
template <typename Msg, typename Func>
auto control_unit::listen_for(Func&& handler) -> void
{
    handlers.insert(std::make_pair(
        std::type_index(typeid(Msg)).hash_code(),
        std::function< void(Msg) >{std::forward<Func>(handler)}
    ));
}

template <typename Msg>
auto control_unit::unlisten_for() -> void
{
    handlers.erase(
        std::type_index(typeid(Msg)).hash_code()
    );
}

template <typename Msg>
auto control_unit::call_handler_for(Msg&& message) -> void
{
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
