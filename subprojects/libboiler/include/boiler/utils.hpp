#pragma once

#include <type_traits>

namespace boiler::utils {
    template<typename T>
    struct remove_cv_ref
    {
        using type = std::remove_reference_t<std::remove_cv_t<T>>;
    };
    template<typename T>
    using remove_cv_ref_t = typename remove_cv_ref<T>::type;
}