#pragma once

#include <string_view>
#include <type_traits>

namespace utils {
    template<typename T>
    struct remove_cv_ref
    {
        using type = std::remove_reference_t<std::remove_cv_t<T>>;
    };
    template<typename T>
    using remove_cv_ref_t = typename remove_cv_ref<T>::type;

    template<typename T>
    constexpr std::string_view type_name()
    {
        std::string_view name, prefix, suffix;
#ifdef __clang__
        name   = __PRETTY_FUNCTION__;
        prefix = "std::string_view limbo::type_name() [T = ";
        suffix = "]";
#elif defined(__GNUC__)
        name   = __PRETTY_FUNCTION__;
        prefix = "constexpr std::string_view limbo::type_name() [with T = ";
        suffix = "; std::string_view = std::basic_string_view<char>]";
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix =
            "class std::basic_string_view<char,struct std::char_traits<char> "
            "> __cdecl limbo::type_name<";
        suffix = ">(void)";
#else
#error Compiler not supported!
#endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
}