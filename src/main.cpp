#include <iostream>
#include <chrono>

#include "physical_units.hpp"
#include "control_unit.hpp"

int main() {
    namespace ch = std::chrono;

    physical_units pu;
    control_unit ctrl;

    while(true) {
        auto start = ch::high_resolution_clock::now();

        pu.process_messages(
            ctrl.process_messages(
                pu.get_messages()
            )
        );

        auto end = ch::high_resolution_clock::now();
        auto duration = ch::duration_cast<ch::nanoseconds>(end - start);
        std::cout << "cycle took " << duration.count() << " nanoseconds\n";
    }
}
