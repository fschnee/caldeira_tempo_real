#pragma once

#include <functional> // std::function and std::less.
#include <string>
#include <vector>
#include <map>
#include <any>

#include "messages.hpp"
#include "utils.hpp"

/// Summary:
class control_unit
{
public:
    auto process_messages(
        const std::vector<messages::to_program::any>& messages)
        -> std::vector<messages::to_units::any>;

    control_unit();
    virtual ~control_unit() = default;

protected:
    template<typename Msg, typename Func>
    auto listen_to(Func&& handler) -> void;
    template<typename Msg>
    auto unlisten_to() -> void;
    template<typename Msg>
    auto notify_listener_of(Msg&& message) -> void;

    enum class states
    {
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
    states state;
    std::
        map<std::string, std::any, std::less<> /* so we can key using
                                                  string_views*/
            >
            listeners;
    std::vector<messages::to_units::any> response;
};

/// Implementation:
template<typename Msg, typename Func>
auto control_unit::listen_to(Func&& handler) -> void
{
    listeners.insert(std::make_pair(
        utils::type_name<utils::remove_cv_ref_t<Msg>>(),
        std::function<void(Msg)>{ std::forward<Func>(handler) }));
}

template<typename Msg>
auto control_unit::unlisten_to() -> void
{
    listeners.erase(utils::type_name<utils::remove_cv_ref_t<Msg>>());
}

template<typename Msg>
auto control_unit::notify_listener_of(Msg&& message) -> void
{
    std::visit(
        [&](auto msg) {
            auto key = utils::type_name<utils::remove_cv_ref_t<Msg>>();

            if (listeners.count(key)) {
                auto& any_handler = listeners.find(key)->second;
                auto handler_ptr =
                    std::any_cast<std::function<void(decltype(msg))>>(
                        &any_handler);
                if (handler_ptr) {
                    return (*handler_ptr)(msg);
                };
            }
        },
        message);
}
