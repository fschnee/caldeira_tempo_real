#pragma once

#include <cstddef>     // std::size_t.
#include <type_traits> // std::integral_constant.
#include <utility>     // std::integer_sequence.

/// Summary:
namespace limbo::ppu {
    // Not a full-fledged type_list, just somewhere
    // to ::append and ::recover types from.
    template<typename... Types>
    struct type_accumulator
    {
        template<template<typename...> typename T>
        using recover_to = T<Types...>;

        template<typename T>
        using append = type_accumulator<Types..., T>;
    };

    // A custom integer_sequence implementation that allows mutating
    // easily with ::append, among other things.
    // You can convert it to a std::integer_sequence with ::recover_to.
    template<typename IntegerType, IntegerType... Ints>
    struct integer_accumulator
    {
        using integer_type = IntegerType;

        template<template<typename IT, IT...> typename T>
        using recover_to = T<IntegerType, Ints...>;

        template<IntegerType I>
        using append = integer_accumulator<IntegerType, Ints..., I>;
        template<IntegerType N>
        using add = integer_accumulator<IntegerType, (Ints + N)...>;
        template<IntegerType N>
        using multiply = integer_accumulator<IntegerType, (Ints * N)...>;
        template<template<IntegerType> typename Functor>
        using apply_type = type_accumulator<Functor<Ints>...>;
    };

    // Makes an integer_accumulator containing [0, End).
    // Recover using ::type.
    template<typename IntegerType, IntegerType End>
    struct make_integer_accumulator;

    // Makes an integer_accumulator containing [Begin, End).
    // Recover using ::type.
    template<typename IntegerType, IntegerType Begin, IntegerType End>
    struct make_integer_accumulator_range;

    // Recover using ::type.
    template<std::size_t Index, typename Head, typename... Tail>
    struct nth_type;

    // Just a simple wrapper around nth_type for ease of use.
    template<typename... Types>
    struct accessors_for
    {
        template<std::size_t Index>
        using at = typename nth_type<Index, Types...>::type;
        // Can also access negative indexes.
        // -1 is the last type and -sizeof...(Types) is the first.
        template<long long Index>
        using get =
            typename nth_type<(Index >= 0 ? Index : sizeof...(Types) + Index), Types...>::
                type;
    };

    // Result doesn't include the Index'th type.
    // Index starts at 0.
    // Recover to a type_accumulator using ::type.
    template<std::size_t Index, typename... Types>
    struct types_before;

    // Result doesn't include the Index'th type.
    // Index starts at 0.
    // Recover to a type_accumulator using ::type.
    template<std::size_t Index, typename... Types>
    struct types_after;

    // Works on empty type packs.
    // Recover to a type_accumulator using ::type.
    template<typename... Types>
    struct reverse;

    // A builder for type_accumulators and integer_accumulators.
    // For example:
    //     iterate_through<int, bool, char, std::string>
    //     ::template using_condition< template std::is_integral >
    //     ::template accumulating_the_types::when_true;
    // Would yield type_accumulator<int, bool, char>; While:
    //     iterate_through<int, bool, char, std::string>
    //     ::template using_condition< template std::is_integral >
    //     ::template accumulating_the_indexes::when_true;
    // Would yield integer_accumulator<std::size_t , 0, 1, 2>.
    template<typename... Types>
    struct iterate_through
    {
        template<template<typename> typename Pred>
        struct using_condition
        {
            // Has ::on<bool>, ::on_true, ::on_false and the
            // aliases ::when<bool>, ::when_true and ::when_false.
            // Recover to type_accumulator using any of those.
            struct accumulating_the_types;
            // Just some aliases.
            using to_accumulate_the_types = accumulating_the_types;
            using keeping_the_types       = accumulating_the_types;
            using getting_the_types       = accumulating_the_types;

            // Has ::on<bool>, ::on_true, ::on_false and the
            // aliases ::when<bool>, ::when_true and ::when_false.
            // Recover to integer_accumulator<std::size_t, ...> using any of
            // those.
            struct accumulating_the_indexes;
            // Just some aliases.
            using to_accumulate_the_indexes = accumulating_the_indexes;
            using keeping_the_indexes       = accumulating_the_indexes;
            using getting_the_indexes       = accumulating_the_indexes;
        };
    };

    // Recover to type_accumulator using ::type.
    template<template<typename> typename Pred, typename... Types>
    struct remove_if;
    // Curried version of remove_if.
    template<template<typename> typename Pred>
    struct remove_if<Pred>
    {
        template<typename... Types>
        using from = remove_if<Pred, Types...>;
    };
    // Inverse curried version of remove_if (pass the types first
    // and the predicate second).
    template<typename... Types>
    struct remove_from
    {
        template<template<typename> typename Pred>
        using when = remove_if<Pred, Types...>;
    };

    // Recover to type_accumulator using ::type.
    template<template<typename> typename Pred, typename... Types>
    struct keep_if;
    // Curried version of keep_if.
    template<template<typename> typename Pred>
    struct keep_if<Pred>
    {
        template<typename... Types>
        using from = keep_if<Pred, Types...>;
    };
    // Inverse curried version of keep_if (pass the types first
    // and the predicate second).
    template<typename... Types>
    struct keep_from
    {
        template<template<typename> typename Pred>
        using when = keep_if<Pred, Types...>;
    };
}

/// Implementation:
/// TODO: make cleaner all-around.
namespace limbo::ppu /* A.K.A parameter_pack_utils */
{
    namespace detail {
        // Recursive case.
        template<typename IntegralConstant>
        struct make_integer_accumulator_helper
        {
            using current = IntegralConstant;

            static const auto next_value =
                current::value > typename current::value_type{ 0 }
                    ? current::value - 1
                    : current::value == typename current::value_type{ 0 }
                          ? typename current::value_type{ 0 }
                          : current::value + 1;

            using type = typename make_integer_accumulator_helper<
                std::integral_constant<typename current::value_type, next_value>>::type ::
                template append<current::value>;
        };
        // Base case.
        template<typename IntegerType>
        struct make_integer_accumulator_helper<std::integral_constant<IntegerType, 0>>
        {
            using current = std::integral_constant<IntegerType, 0>;

            using type = integer_accumulator<IntegerType, 0>;
        };
    } // namespace detail
    template<typename IntegerType, IntegerType CurrentInt>
    struct make_integer_accumulator
    {
        using current = std::integral_constant<IntegerType, CurrentInt>;

        static const auto next_value =
            current::value > typename current::value_type{ 0 }
                ? current::value - 1
                : current::value == typename current::value_type{ 0 }
                      ? typename current::value_type{ 0 }
                      : current::value + 1;

        using type = typename std::conditional_t<
            current::value == typename current::value_type{ 0 },
            integer_accumulator<IntegerType>,
            typename detail::make_integer_accumulator_helper<
                std::integral_constant<typename current::value_type, next_value>>::type>;
    };
    template<typename IntegerType, IntegerType Begin, IntegerType End>
    struct make_integer_accumulator_range
    {
        using type = typename make_integer_accumulator<IntegerType, End - Begin>::type::
            template add<Begin>;
    };

    // Pops one type at a time from the pack.
    template<std::size_t Index, typename PoppedType, typename... Types>
    struct nth_type
    {
        static_assert(Index < sizeof...(Types) + 1, "Index out of bounds");
        using type = typename nth_type<Index - 1, Types...>::type;
    };
    // Base case.
    template<typename NthType, typename... Types>
    struct nth_type<0, NthType, Types...>
    {
        using type = NthType;
    };

    template<std::size_t Index, typename... Types>
    struct types_before
    {
        static_assert(Index < sizeof...(Types), "Index out of bounds");

        using type = typename make_integer_accumulator<std::size_t, Index>::type ::
            template apply_type<accessors_for<Types...>::template at>;
    };
    template<typename... Types>
    struct types_before<0, Types...>
    {
        using type = type_accumulator<>;
    };

    template<std::size_t Index, typename... Types>
    struct types_after
    {
        static_assert(Index < sizeof...(Types), "Index out of bounds");

        using type = typename make_integer_accumulator_range<
            std::size_t,
            Index + 1,
            sizeof...(
                Types)>::type ::template apply_type<accessors_for<Types...>::template at>;
    };

    template<typename... Types>
    struct reverse
    {
        using type = typename make_integer_accumulator_range<
            long long,
            -1,
            -static_cast<long long>(sizeof...(Types) + 1)>::type ::
            template apply_type<accessors_for<Types...>::template get>;
    };
    template<>
    struct reverse<>
    {
        using type = type_accumulator<>;
    };

    namespace detail {
        template<
            template<typename>
            typename Pred,
            bool InvertPred,
            typename Accumulator,
            typename CurrentType,
            typename... Rest>
        constexpr static auto recurse_with_pred_getting_types()
        {
            constexpr auto should_append_to_accumulator =
                InvertPred ? !Pred<CurrentType>::value : Pred<CurrentType>::value;
            using accumulator = std::conditional_t<
                should_append_to_accumulator,
                typename Accumulator::template append<CurrentType>,
                Accumulator>;

            constexpr auto should_end_recursion = sizeof...(Rest) == 0;
            if constexpr (should_end_recursion) {
                return accumulator{};
            } else {
                return recurse_with_pred_getting_types<
                    Pred,
                    InvertPred,
                    accumulator,
                    Rest...>();
            }
        }
        template<
            template<typename>
            typename Pred,
            bool InvertPred,
            typename Accumulator,
            std::size_t CurrentIndex,
            typename CurrentType,
            typename... Rest>
        constexpr static auto recurse_with_pred_getting_indexes()
        {
            constexpr auto should_append_to_accumulator =
                InvertPred ? !Pred<CurrentType>::value : Pred<CurrentType>::value;
            using accumulator = std::conditional_t<
                should_append_to_accumulator,
                typename Accumulator::template append<CurrentIndex>,
                Accumulator>;

            constexpr auto should_end_recursion = sizeof...(Rest) == 0;
            if constexpr (should_end_recursion) {
                return accumulator{};
            } else {
                return recurse_with_pred_getting_indexes<
                    Pred,
                    InvertPred,
                    accumulator,
                    CurrentIndex + 1,
                    Rest...>();
            }
        }
    } // namespace detail
    template<typename... Types>
    template<template<typename> typename Pred>
    struct iterate_through<Types...>::using_condition<Pred>::accumulating_the_types
    {
        template<bool Cond>
        using on       = decltype(detail::recurse_with_pred_getting_types<
                            Pred,
                            !Cond,
                            type_accumulator<>,
                            Types...>());
        using on_true  = on<true>;
        using on_false = on<false>;
        template<bool Cond>
        using when       = on<Cond>;
        using when_true  = on<true>;
        using when_false = on<false>;
    };
    template<typename... Types>
    template<template<typename> typename Pred>
    struct iterate_through<Types...>::using_condition<Pred>::accumulating_the_indexes
    {
        template<bool Cond>
        using on =
            typename decltype(detail::recurse_with_pred_getting_indexes<
                              Pred,
                              !Cond,
                              integer_accumulator<std::size_t>,
                              0,
                              Types...>())::template recover_to<std::integer_sequence>;
        using on_true  = on<true>;
        using on_false = on<false>;
        template<bool Cond>
        using when       = on<Cond>;
        using when_true  = on<true>;
        using when_false = on<false>;
    };

    template<template<typename> typename Pred, typename... Types>
    struct remove_if
    {
        using type = typename iterate_through<Types...>::template using_condition<
            Pred>::accumulating_the_types::when_false;
    };

    template<template<typename> typename Pred, typename... Types>
    struct keep_if
    {
        using type = typename iterate_through<Types...>::template using_condition<
            Pred>::accumulating_the_types::when_true;
    };
}
