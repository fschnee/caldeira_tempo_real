#pragma once

#include <functional> // std::function and std::less.
#include <string>
#include <vector>
#include <map>
#include <any>

#include "boiler/common.hpp"
#include "boiler/messages.hpp"

#include "limbo/limbo.hpp" // limbo::type_name

/// Summary:
namespace boiler {
    class control_unit
    {
    public:
        using mode           = messages::to_units::mode::possible_modes;
        using msg_from_units = boiler::messages::from_units::any;
        using msg_to_units   = boiler::messages::to_units::any;

        control_unit(boiler::constants c);
        virtual ~control_unit() = default;

        auto process_messages(std::vector<msg_from_units> messages)
            -> std::vector<msg_to_units>;

        // §1.15 exige que esse modo pode ser "setado" por fora.
        auto emergency_stop() -> void;

    protected:
        auto init_routine() -> void;
        auto normal_routine() -> void;
        auto degraded_routine() -> void;
        auto rescue_routine() -> void;
        auto emergency_stop_routine() -> void;
        auto run_mode_routine() -> void;

        auto switch_mode(mode newmode) -> void;

        struct msg_handler
        {
            enum class response
            {
                keep_listening,
                unlisten
            };
            std::function<response(void)> on_missing;
            std::function<response(msg_from_units)> on_present;
        };

        template<typename Msg>
        [[nodiscard]] auto expect();

        template<typename Msg>
        [[nodiscard]] auto send(Msg&&);

        auto run_last(std::function<void(void)> func) -> void;

        const boiler::constants constants;

        struct physical_units_readings {
            std::vector<boiler::messages::to_program::pump_state::possible_states> pump_states;
            std::vector<boiler::messages::to_program::pump_control_state::possible_states> pump_control_states;
            float level_liters;
            float steam_liters_per_sec;
        } readings;

    private:
        std::vector<msg_to_units> response;

        mode mode_of_operation;

        // We need to explicitly use std::less to be able to key
        // the map using string_views.
        std::map<std::string, msg_handler, std::less<>> expected_handlers;
        auto handle_expected(std::vector<msg_from_units>&) -> void;

        std::vector<std::function<void(void)>> run_last_handlers;
    };
}

/// Implementation:
template<typename Msg>
auto boiler::control_unit::expect()
{
    struct impl
    {
        impl(control_unit& ctrl, std::string_view msgname)
            : ctrl{ ctrl }
            , msgname{ msgname }
        {}

        auto eventually(std::function<void(msg_from_units)> on_receipt) &&
        {
            ctrl.expected_handlers.erase(msgname);

            auto wrapped_handler = msg_handler{
                .on_missing = []() { return msg_handler::response::keep_listening; },
                .on_present =
                    [handler = std::move(on_receipt)](auto msg) {
                        handler(msg);
                        return msg_handler::response::unlisten;
                    }
            };

            ctrl.expected_handlers.insert(
                std::make_pair(std::move(msgname), std::move(wrapped_handler)));
        }

        auto always(msg_handler handler) &&
        {
            ctrl.expected_handlers.erase(msgname);
            ctrl.expected_handlers.insert(
                std::make_pair(std::move(msgname), std::move(handler)));
        }

    private:
        control_unit& ctrl;
        std::string msgname;
    };

    return impl{ *this, limbo::type_name<boiler::utils::remove_cv_ref_t<Msg>>() };
}

#include <stdexcept> // std::domain_error.

template<typename Msg>
auto boiler::control_unit::send(Msg&& msg)
{
    struct impl
    {
        using msg_t = boiler::utils::remove_cv_ref_t<Msg>;
        using ack_t = typename boiler::messages::acknowledgement_of<msg_t>::type;

        impl(control_unit& ctrl, Msg&& msg)
            : ctrl{ ctrl }
            , msg{ std::forward<Msg>(msg) }
        {}

        auto now() && -> void { ctrl.response.push_back(msg); }

        auto until_ack(std::function<void(msg_from_units)> on_ack) && -> void
        {
            if constexpr (std::is_same_v<ack_t, limbo::nonesuch>) {
                throw std::domain_error{
                    "message doesn't have a corresponding acknowledgement"
                };
            } else {

                auto wrapped_handler = msg_handler{
                    .on_missing =
                        [msg = std::move(msg), ctrl = ctrl]() {
                            ctrl.send(msg).now();
                            return control_unit::msg_handler::response::keep_listening;
                        },
                    .on_present =
                        [handler = std::move(on_ack)](auto m) {
                            handler(m);
                            return control_unit::msg_handler::response::unlisten;
                        }
                };

                ctrl.send(msg).now();
                ctrl.expect<ack_t>().always(std::move(wrapped_handler));
            }
        }

    private:
        boiler::control_unit& ctrl;
        Msg msg;
    };

    return impl{
        *this,
        std::forward<Msg>(msg),
    };
}
