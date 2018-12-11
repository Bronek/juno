// Copyright (c) 2018 Bronek (Bronislaw) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include <type_traits>

namespace juno {

template <template <typename> typename Check, typename... L> class set;

namespace set_impl {
    template <template <typename> typename Check, typename... L> struct contains_detail;

    template <template <typename> typename Check, typename U> struct contains_detail<Check, U> {
        using type = typename Check<U>::type;
        constexpr static bool value = std::is_void_v<type>;
    };

    template <template <typename> typename Check, typename U, typename T, typename... L>
    struct contains_detail<Check, U, T, L...> {
        using type = typename Check<U>::type;
        using head = typename Check<T>::type;
        // clang-format off
        constexpr static bool value =
                std::is_void_v<type>
                || std::is_same_v<type, head>
                || contains_detail<Check, U, L...>::value;
        // clang-format on
    };

    template <template <typename> typename Check, typename... L> struct insert_detail;

    template <template <typename> typename Check, typename T, typename... L>
    struct insert_detail<Check, T, set<Check, L...>> {
        using head = typename Check<T>::type;
        using type = set<Check, head, L...>;
    };

    template <typename... L> struct unchecked_list {};

    template <typename T> struct cracker_list;

    template <template <typename> typename Check, typename... L>
    struct cracker_list<set<Check, L...>> {
        using type = unchecked_list<L...>;
    };

    template <typename T> struct cracker;

    template <template <typename> typename Check, typename... L>
    struct cracker<set<Check, L...>> {
        using list = typename cracker_list<typename set<Check, L...>::unique>::type;
        constexpr static std::size_t size = set<Check, L...>::unique::size;
    };

    template <typename T> struct dummy_check {
        using type = T;
    };

    template <typename... L> struct intersect_detail;

    template <typename... T> struct intersect_detail<unchecked_list<>, unchecked_list<T...>> {
        constexpr static std::size_t size = 0;
    };

    template <typename V, typename... L, typename... T>
    struct intersect_detail<unchecked_list<V, L...>, unchecked_list<T...>> {
        // clang-format off
        constexpr static std::size_t size =
                intersect_detail<unchecked_list<L...>, unchecked_list<T...>>::size
                + (not std::is_void_v<V> && contains_detail<dummy_check, V, T...>::value);
        // clang-format on
    };

    template <std::size_t, std::size_t Cross, std::size_t> struct is_cross {
        constexpr static bool value = Cross > 0;
    };

    template <std::size_t, std::size_t Cross, std::size_t Theirs> struct is_super {
        constexpr static bool value = Cross == Theirs;
    };

    template <std::size_t Mine, std::size_t Cross, std::size_t Theirs> struct is_same {
        constexpr static bool value = Mine == Cross && Cross == Theirs;
    };

    template <template <typename> typename Check, typename... L> struct unique;

    template <template <typename> typename Check> struct unique<Check> {
        using type = set<Check>;
        constexpr static std::size_t size = 0;

        template <template <std::size_t, std::size_t, std::size_t> typename How, typename,
                  std::size_t Size>
        constexpr static bool compare = How<0, 0, Size>::value;

        template <typename U> constexpr static bool test = contains_detail<Check, U>::value;
        template <typename F> constexpr static bool apply(const F&) noexcept { return true; }
    };

    template <template <typename> typename Check, typename T, typename... L>
    struct unique<Check, T, L...> {
        // clang-format off
        using type =
                typename std::conditional<
                    contains_detail<Check, T, L...>::value
                    , typename unique<Check, L...>::type
                    , typename insert_detail<Check, T, typename unique<Check, L...>::type>::type
                >::type;

        constexpr static std::size_t size =
                contains_detail<Check, T, L...>::value
                ? unique<Check, L...>::size
                : unique<Check, L...>::size + 1;

        template <template <std::size_t, std::size_t, std::size_t> typename How
                , typename U, std::size_t Size>
        constexpr static bool compare =
                How<size, intersect_detail<unchecked_list<T, L...>, U>::size, Size>::value;
        // clang-format on

        template <typename U>
        constexpr static bool test = contains_detail<Check, U, T, L...>::value;

        template <typename F> constexpr static bool apply(const F& fn)
        {
            if constexpr (not std::is_void_v<T> && not contains_detail<Check, T, L...>::value) {
                if (not fn((T*)nullptr)) return false;
            }
            return unique<Check, L...>::apply(fn);
        }
    };
}  // namespace set_impl

template <template <typename> typename Check, typename... L> class set {
    using impl = set_impl::unique<Check, L...>;

public:
    constexpr explicit set() = default;
    using unique = typename impl::type;

    constexpr static std::size_t size = impl::size;
    constexpr static bool        empty = size == 0;

    // clang-format off
    template <typename T>
    constexpr static bool is_same =
                impl::template compare<
                    set_impl::is_same
                    , typename set_impl::cracker<T>::list
                    , set_impl::cracker<T>::size
                >;

    template <typename T>
    constexpr static bool is_cross =
                impl::template compare<
                    set_impl::is_cross
                    , typename set_impl::cracker<T>::list
                    , set_impl::cracker<T>::size
                >;

    template <typename T>
    constexpr static bool is_super =
                impl::template compare<
                    set_impl::is_super
                    , typename set_impl::cracker<T>::list
                    , set_impl::cracker<T>::size
                >;

    template <typename T> constexpr static bool test = impl::template test<T>;

    template <typename T>
    using type =
                typename std::enable_if<
                    (not std::is_void_v<T> && test<T>)
                    , typename Check<T>::type
                >::type;
    // clang-format on

    template <typename T> using insert = typename set_impl::unique<Check, T, L...>::type;

    template <typename F> constexpr static bool for_each(const F& fn)
    {
        return impl::apply(fn);
    }
};

}  // namespace juno
