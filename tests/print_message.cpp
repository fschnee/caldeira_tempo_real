#include "boiler/messages.hpp"

#include <iostream>

template <typename T>
auto print_type()
{
    auto t_ref = T{};
    std::cout
        << limbo::type_name<T>() << ':'
        << "\n\t" << T{} 
        << "\n\t" << t_ref << '\n';
}

template <typename... Types>
struct type_printer {
    static auto print() {
        (
            print_type<Types>(),
            ...
        );
    }
};

int main()
{
    boiler::messages::to_units::types::recover_to<type_printer>::print();
    boiler::messages::to_program::types::recover_to<type_printer>::print();
}