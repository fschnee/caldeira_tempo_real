#pragma once

#include <type_traits>
#include <string_view>

namespace limbo {
    // Since we're not targeting c++20 we need to implement is_detected ourselves.
    // from: proposal n4502 -> http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf
    // Thanks!
    inline namespace detect
    {
        struct nonesuch {
            nonesuch() = delete;
            ~nonesuch() = delete;
            nonesuch(nonesuch const&) = delete;
            void operator=(nonesuch const&) = delete;
        };
        template <class Default, class AlwaysVoid, template<class...> class Op, class... Args>
        struct detector {
            using value_t = std::false_type;
            using type = Default;
        };
        template <class Default, template<class...> class Op, class... Args>
        struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
            using value_t = std::true_type;
            using type = Op<Args...>;
        };
        template <template<class...> class Op, class... Args>
        using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;
        template <template<class...> class Op, class... Args>
        constexpr bool is_detected_v = is_detected<Op, Args...>::value;
        template <template<class...> class Op, class... Args>
        using detected_t = typename detector<nonesuch, void, Op, Args...>::type;
        template <class Default, template<class...> class Op, class... Args>
        using detected_or = detector<Default, void, Op, Args...>;
        template <class Default, template<class...> class Op, class... Args>
        using detected_or_t = typename detected_or<Default, Op, Args...>::type;
        template <class Expected, template<class...> class Op, class... Args>
        using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;
        template <class Expected, template<class...> class Op, class... Args>
        constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;
        template <class To, template<class...> class Op, class... Args>
        using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;
        template <class To, template<class...> class Op, class... Args>
        constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;
    }

    // from: https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138
    // Thanks!
    template <typename T>
    constexpr std::string_view type_name()
    {
        std::string_view name, prefix, suffix;
    #ifdef __clang__
        name = __PRETTY_FUNCTION__;
        prefix = "std::string_view limbo::type_name() [T = ";
        suffix = "]";
    #elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
        prefix = "constexpr std::string_view limbo::type_name() [with T = ";
        suffix = "; std::string_view = std::basic_string_view<char>]";
    #elif defined(_MSC_VER)
        name = __FUNCSIG__;
        prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl limbo::type_name<";
        suffix = ">(void)";
    #endif
        name.remove_prefix(prefix.size());
        name.remove_suffix(suffix.size());
        return name;
    }
}