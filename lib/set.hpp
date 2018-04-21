// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include "mpl.hpp"

#include <type_traits>
#include <cstdint>
#include <utility>

namespace juno {
    template <typename ...L> struct set;

    // details of set implementation, to ensure uniqueness of types stored in a set_impl
    namespace impl_set {
        template<typename ...L> struct contains;
        template<> struct contains<void> {
            constexpr static bool value = true;
        };
        template<typename T> struct contains<T> {
            constexpr static bool value = false;
        };
        template<typename T, typename ...L> struct contains<T, T, L...> {
            constexpr static bool value = true;
        };
        template<typename T, typename U, typename ...L> struct contains<T, U, L...> {
            constexpr static bool value = contains<T, L...>::value;
        };

        template <typename ...L> struct set_impl;
        template <typename ...L> struct join;
        template <typename ...L> struct join<set_impl<L...>, void> {
            using type = set_impl<L...>;
        };
        template <typename T, typename ...L> struct join<set_impl<L...>, T> {
            using type = set_impl<T, L...>; // T must come first, so set_impl can drop "void"
        };

        template <typename ...L> struct contains_set;
        template <typename ...P, typename ...L> struct contains_set<set_impl<P...>, set_impl<L...>> {
            using Set = set_impl<L...>;
            constexpr static bool value = contains<typename Set::head, P...>::value
                    && contains_set<set_impl<P...>, typename Set::tail>::value;
        };
        template <typename ...L> struct contains_set<set_impl<L...>, set_impl<>> {
            constexpr static bool value = true;
        };

        template <typename ...L> struct intersects_set;
        template <typename ...P, typename ...L> struct intersects_set<set_impl<P...>, set_impl<L...>> {
            using Set = set_impl<P...>;
            constexpr static bool value = contains<typename Set::head, L...>::value
                    || intersects_set<typename Set::tail, set_impl<L...>>::value;
        };
        template <typename ...L> struct intersects_set<set_impl<>, set_impl<L...>> {
            constexpr static bool value = false;
        };

        template <typename ...L> struct join_set;
        template <typename ...P, typename ...L> struct join_set<set_impl<P...>, set_impl<L...>> {
            using type = typename set_impl<P..., L...>::unique;
        };

        template <typename ...L> struct cross_set;
        template <typename ...L> struct cross_set<set_impl<>, set_impl<L...>> {
            using type = set_impl<>;
        };
        template <typename T, typename ...P, typename ...L> struct cross_set<set_impl<T, P...>, set_impl<L...>> {
            using Tail = typename cross_set<set_impl<P...>, set_impl<L...>>::type;
            using type = typename if_<
                    contains<T, L...>::value
                    , typename join<Tail, T>::type
                    , Tail
            >::type;
        };

        template <typename ...L> struct less_set;
        template <typename ...L> struct less_set<set_impl<>, set_impl<L...>> {
            using type = set_impl<>;
        };
        template <typename T, typename ...P, typename ...L> struct less_set<set_impl<T, P...>, set_impl<L...>> {
            using Tail = typename less_set<set_impl<P...>, set_impl<L...>>::type;
            using type = typename if_<
                    contains<T, L...>::value
                    , Tail
                    , typename join<Tail, T>::type
            >::type;
        };

        template <typename ...L> struct apply_impl;
        template <> struct apply_impl<set_impl<>> {
            template <typename U, typename ...A> static bool apply(U&& , A&& ... ) noexcept { return true; }
        };
        template <typename T, typename ...L> struct apply_impl<set_impl<T, L...>> {
            template <typename U, typename ...A> static bool apply(U&& fn, A&& ... a) noexcept {
                if (not fn(static_cast<T*>(nullptr), std::forward<A>(a)...))
                    return false;
                return apply_impl<set_impl<L...>>::apply(std::forward<U>(fn), std::forward<A>(a)...);
            }
        };

        template <typename ...L> struct set_impl<void, L...> {
            using unique = typename set_impl<L...>::unique;
            using head = typename set_impl<L...>::head;
            using tail = typename set_impl<L...>::tail;
        };
        template <> struct set_impl<> {
            using unique = set_impl<>;
            using head = void;
            using tail = set_impl<>;
            using type = set<>;
            using empty = std::true_type;

            constexpr static std::size_t size = 0;
        };
        template <typename T, typename ...L> struct set_impl<T, L...> {
            constexpr static bool Contains = contains<T, L...>::value;
            using unique = typename if_<Contains
                    , typename set_impl<L...>::unique
                    , typename join<typename set_impl<L...>::unique, T>::type
            >::type;
            using head = typename if_<Contains
                    , typename set_impl<L...>::head
                    , T
            >::type;
            using tail = typename if_<Contains
                    , typename set_impl<L...>::tail
                    , set_impl<L...>
            >::type;
            using type = set<T, L...>;
            using empty = std::false_type;

            constexpr static std::size_t size = 1 + set_impl<L...>::size;
        };

        template <typename ...P>
        using impl = typename impl_set::set_impl<
                typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>::unique;
    }

    template <typename ...L> struct set {
    private:
        template <typename ...P> friend struct set;
        using impl = impl_set::impl<L...>;

    public:
        constexpr static std::size_t size = impl::size;
        constexpr static bool empty = impl::empty::value;

        template <typename T>
        constexpr static bool is_same_set = impl_set::contains_set<impl, typename T::impl>::value
                                            && impl_set::contains_set<typename T::impl, impl>::value;

        template <typename ...P>
        constexpr static bool is_same = is_same_set<typename impl_set::impl<P...>::type>;

        template <typename T>
        constexpr static bool contains_set = impl_set::contains_set<impl, typename T::impl>::value;

        template <typename ...P>
        constexpr static bool contains = contains_set<typename impl_set::impl<P...>::type>;

        template <typename T>
        constexpr static bool intersects_set = impl_set::intersects_set<impl, typename T::impl>::value;

        template <typename ...P>
        constexpr static bool intersects = intersects_set<typename impl_set::impl<P...>::type>;

        // Mathematical term is "union"
        template <typename T>
        struct insert_set {
            using type = typename impl_set::join_set<impl, typename T::impl>::type::type;
        };

        template <typename ...P>
        struct insert {
            using type = typename insert_set<typename impl_set::impl<P...>::type>::type;
        };

        // Mathematical term is "intersection"
        template <typename T>
        struct cross_set {
            using type = typename impl_set::cross_set<impl, typename T::impl>::type::type;
        };

        template <typename ...P>
        struct cross {
            using type = typename cross_set<typename impl_set::impl<P...>::type>::type;
        };

        // Mathematical term is "relative complement"
        template <typename T>
        struct remove_set {
            using type = typename impl_set::less_set<impl, typename T::impl>::type::type;
        };

        template <typename ...P>
        struct remove {
            using type = typename remove_set<typename impl_set::impl<P...>::type>::type;
        };

        template <typename T, typename ...A>
        static bool for_each(T&& fn, A&& ... a) noexcept {
            return impl_set::apply_impl<impl>::apply(std::forward<T>(fn), std::forward<A>(a)...);
        }
    };
}
