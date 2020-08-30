#pragma once

#include <cstddef> // For std::size_t.
#include <tuple> // For std::tuple_element and std::tuple_size.

#include "limbo/ppu.hpp"

/// Summary:
namespace limbo
{
    // Just a big wrapper around limbo::ppu utilities with some few other goodies.
    template<typename... Types>
    struct type_list
    {
        static const auto size = sizeof...(Types);

        /// Observers.
        template<std::size_t Index>
        using before =
            typename ppu::types_before<Index, Types...>::type::template recover_to<type_list>;
        template<std::size_t Index>
        using after =
            typename ppu::types_after<Index, Types...>::type::template recover_to<type_list>;
        template<std::size_t Index>
        using at = typename ppu::accessors_for<Types...>::template at<Index>;
        // Accepts numbers from (-size, size).
        template<long long Index>
        using get = typename ppu::accessors_for<Types...>::template get<Index>;

        /// Utils.
        // Use when you need to get the types out of the type list
        // and into another type, such as std::variant for example:
        //     type_list<Types...>::recover_to<std::variant>
        // will yield std::variant<Types...>.
        template<template<typename...> typename Target>
        using recover_to = Target<Types...>;

        /// Mutators (altough they don't really mutate, just return another type).
        template <template <typename> typename T>
        using apply_type = type_list< T<Types>... >;

        template<typename... OtherTypes>
        using push_front = type_list<OtherTypes..., Types...>;
        template<typename... OtherTypes>
        using push_back = type_list<Types..., OtherTypes...>;

        using reverse = typename ppu::reverse<Types...>::type::template recover_to<type_list>;

        template<std::size_t Index>
        using erase =
            typename before<Index>::template recover_to<after<Index>::template push_front>;

        template<std::size_t Index, typename... OtherTypes>
        using insert =
            typename before<Index>::template push_back<OtherTypes...>::template push_back<
                at<Index>>::template recover_to<after<Index>::template push_front>;

        template<std::size_t Index, typename... OtherTypes>
        using swap = typename erase<Index>::template insert<Index, OtherTypes...>;

        template<std::size_t Amount>
        using drop = typename after<Amount>::template push_front<at<Amount>>;

        template <typename T>
        static const bool contains = (std::is_same_v<T, Types> || ...);
    };

    // Empty list case.
    template<>
    struct type_list<>
    {
        static const auto size = 0;

        template<std::size_t Index>
        using before = std::conditional_t<Index == 0, type_list<>, void>;
        template<std::size_t Index>
        using after = std::conditional_t<Index == 0, type_list<>, void>;

        template<template<typename...> typename Target>
        using recover_to = Target<>;

        using reverse = type_list<>;
        template<typename... OtherTypes>
        using push_front = type_list<OtherTypes...>;
        template<typename... OtherTypes>
        using push_back = type_list<OtherTypes...>;
        template<std::size_t Index, typename... Types>
        using insert = std::conditional_t<Index == 0, type_list<Types...>, void>;

        template <typename T>
        static const bool contains = false;
    };

    namespace detail
    {
        template <typename TupleLikeClass>
        struct tuple_element_wrapper
        {
            template <std::size_t I>
            using get = typename std::tuple_element<I, TupleLikeClass>::type;
        };
    }

    template <typename TupleLikeClass>
    using tl_from_tuple_like = 
        typename ppu::make_integer_accumulator<
            typename std::tuple_size<TupleLikeClass>::value_type,
            std::tuple_size<TupleLikeClass>::value
        >::type
        ::template apply_type< detail::tuple_element_wrapper<TupleLikeClass>::template get>
        ::template recover_to<type_list>;
}

// Allows better compatibility with the STL (makes a type_list a tuple_like_class).
namespace std {
    template<typename... Types>
    struct tuple_size< limbo::type_list<Types...> > 
        : public integral_constant< size_t, limbo::type_list<Types...>::size > {};
    
    template<size_t I, typename... Types>
    struct tuple_element<I, limbo::type_list<Types...> > {
        using type = typename limbo::type_list<Types...>::template at<I>;
    };
}
