#include <iostream>
#include <chrono>
#include <thread>

#include "control_unit.hpp"
#include "physical_units.hpp"

int main()
{
    namespace ch = std::chrono;
    using namespace std::literals::chrono_literals;

    physical_units pu{ .update_speed = 20 };
    control_unit ctrl;

    auto max_cycle_time = 5s;

    auto exchange_messages = [&](const auto& start) {
        auto throw_on_timeout = [&] {
            auto now = ch::high_resolution_clock::now();
            if (now - start >= max_cycle_time) throw now;
        };

        auto from_pu = pu.get_messages();
        throw_on_timeout();
        auto to_pu = ctrl.process_messages(std::move(from_pu));
        throw_on_timeout();
        pu.process_messages(std::move(to_pu));
        throw_on_timeout();
    };

    while (true) {
        auto start = ch::high_resolution_clock::now();

        try {
            exchange_messages(start);

            auto end      = ch::high_resolution_clock::now();
            auto duration = ch::duration_cast<ch::milliseconds>(end - start);
            auto slack    = max_cycle_time - duration;
            std::cout << "OK:            cycle took " << duration.count()
                      << "ms; "
                      << ch::duration_cast<ch::milliseconds>(slack).count()
                      << "ms of slack\n";
            std::this_thread::sleep_for(slack);
        } catch (const ch::high_resolution_clock::time_point& end) {
            auto duration = ch::duration_cast<ch::nanoseconds>(end - start);
            std::cout << "ERROR_OVERRUN: cycle took " << duration.count()
                      << " nanoseconds\n";
        }
    }
}
