#include "boiler/common.hpp"
#include "boiler/control_unit.hpp"

int main()
{
    auto constants = boiler::constants{};
    auto ctrl = boiler::control_unit{ constants };
}
