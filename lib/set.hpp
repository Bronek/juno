// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include "mpl.hpp"

#include <type_traits>
#include <cstdint>

namespace juno {
    template <typename ...L> struct set;

    // details of set implementation, to ensure uniqueness of types stored in a set_impl
    namespace juno_impl_set {
        template<typename ...L> struct contains;
        template<> struct contains<void> {
            using result = true_;
        };
        template<typename T> struct contains<T> {
            using result = false_;
        };
        template<typename T, typename ...L> struct contains<T, T, L...> {
            using result = true_;
        };
        template<typename T, typename U, typename ...L> struct contains<T, U, L...> {
            using result = typename contains<T, L...>::result;
        };

        template <typename ...L> struct set_impl;
        template <typename ...L> struct join;
        template <typename ...L> struct join<set_impl<L...>, void> {
            using result = set_impl<L...>;
        };
        template <typename T, typename ...L> struct join<set_impl<L...>, T> {
            using result = set_impl<T, L...>; // T must come first, so set_impl can drop "void"
        };

        template <typename ...L> struct contains_set;
        template <typename ...P, typename ...L> struct contains_set<set_impl<P...>, set_impl<L...>> {
            using set_ = set_impl<L...>;
            using result = typename and_<
                    typename contains<typename set_::head, P...>::result
                    , typename contains_set<set_impl<P...>, typename set_::tail>::result
            >::result;
        };
        template <typename ...L> struct contains_set<set_impl<L...>, set_impl<>> {
            using result = true_;
        };

        template <typename ...L> struct intersects_set;
        template <typename ...P, typename ...L> struct intersects_set<set_impl<P...>, set_impl<L...>> {
            using set_ = set_impl<P...>;
            using result = typename or_<
                    typename contains<typename set_::head, L...>::result
                    , typename intersects_set<typename set_::tail, set_impl<L...>>::result
            >::result;
        };
        template <typename ...L> struct intersects_set<set_impl<>, set_impl<L...>> {
            using result = false_;
        };

        template <typename ...L> struct join_set;
        template <typename ...P, typename ...L> struct join_set<set_impl<P...>, set_impl<L...>> {
            using result = typename set_impl<P..., L...>::unique;
        };

        template <typename ...L> struct cross_set;
        template <typename ...L> struct cross_set<set_impl<>, set_impl<L...>> {
            using result = set_impl<>;
        };
        template <typename T, typename ...P, typename ...L> struct cross_set<set_impl<T, P...>, set_impl<L...>> {
            using tail = typename cross_set<set_impl<P...>, set_impl<L...>>::result;
            using result = typename if_<
                    typename contains<T, L...>::result
                    , typename join<tail, T>::result
                    , tail
            >::result;
        };

        template <typename ...L> struct less_set;
        template <typename ...L> struct less_set<set_impl<>, set_impl<L...>> {
            using result = set_impl<>;
        };
        template <typename T, typename ...P, typename ...L> struct less_set<set_impl<T, P...>, set_impl<L...>> {
            using tail = typename less_set<set_impl<P...>, set_impl<L...>>::result;
            using result = typename if_<
                    typename contains<T, L...>::result
                    , tail
                    , typename join<tail, T>::result
            >::result;
        };

        template <typename ...L> struct set_impl<void, L...> {
            using unique = typename set_impl<L...>::unique;
            using head = typename set_impl<L...>::head;
            using tail = typename set_impl<L...>::tail;
            using make = typename set_impl<L...>::make;
            using empty = typename set_impl<L...>::empty;
        };
        template <> struct set_impl<> {
            using unique = set_impl<>;
            using head = void;
            using tail = set_impl<>;
            using make = set<>;
            using empty = true_;

            inline constexpr static std::size_t size() { return 0; }
        };
        template <typename T, typename ...L> struct set_impl<T, L...> {
            using contains_ = typename contains<T, L...>::result;
            using unique = typename if_<
                    contains_
                    , typename set_impl<L...>::unique
                    , typename join<typename set_impl<L...>::unique, T>::result
            >::result;
            using head = typename if_<
                    contains_
                    , typename set_impl<L...>::head
                    , T
            >::result;
            using tail = typename if_<
                    contains_
                    , typename set_impl<L...>::tail
                    , set_impl<L...>
            >::result;
            using make = set<T, L...>;
            using empty = false_;

            inline constexpr static std::size_t size() { return 1 + set_impl<L...>::size(); }
        };
    }

    template <typename ...L> struct set {
    private:
        template <typename ...P> friend struct set;
        using impl = typename juno_impl_set::set_impl<
                typename std::remove_cv<typename std::remove_reference<L>::type>::type ...>::unique;

    public:
        template <typename T>
        inline constexpr static bool is_same_set() {
            return and_<
                    typename juno_impl_set::contains_set<impl, typename T::impl>::result
                    , typename juno_impl_set::contains_set<typename T::impl, impl>::result
            >::result::value;
        }

        template <typename ...P>
        inline constexpr static bool is_same() {
            using U =  typename juno_impl_set::set_impl<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>;
            return and_<
                    typename juno_impl_set::contains_set<impl, typename U::unique>::result
                    , typename juno_impl_set::contains_set<typename U::unique, impl>::result
            >::result::value;
        }

        template <typename T>
        inline constexpr static bool contains_set() {
            return juno_impl_set::contains_set<impl, typename T::impl>::result::value;
        }

        template <typename ...P>
        inline constexpr static bool contains() {
            using U =  typename juno_impl_set::set_impl<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>;
            return juno_impl_set::contains_set<impl, typename U::unique>::result::value;
        }

        template <typename T>
        inline constexpr static bool intersects_set() {
            return juno_impl_set::intersects_set<impl, typename T::impl>::result::value;
        }

        template <typename ...P>
        inline constexpr static bool intersects() {
            using U =  typename juno_impl_set::set_impl<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>;
            return juno_impl_set::intersects_set<impl, typename U::unique>::result::value;
        }

        // Mathematical term is "union"
        template <typename T>
        inline constexpr static auto join_set() {
            return typename juno_impl_set::join_set<impl, typename T::impl>::result::make();
        }

        template <typename ...P>
        inline constexpr static auto join() {
            using U =  typename juno_impl_set::set_impl<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>;
            return typename juno_impl_set::join_set<impl, typename U::unique>::result::make();
        }

        // Mathematical term is "intersection"
        template <typename T>
        inline constexpr static auto cross_set() {
            return typename juno_impl_set::cross_set<impl, typename T::impl>::result::make();
        }

        template <typename ...P>
        inline constexpr static auto cross() {
            using U =  typename juno_impl_set::set_impl<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>;
            return typename juno_impl_set::cross_set<impl, typename U::unique>::result::make();
        }

        // Mathematical term is "relative complement"
        template <typename T>
        inline constexpr static auto less_set() {
            return typename juno_impl_set::less_set<impl, typename T::impl>::result::make();
        }

        template <typename ...P>
        inline constexpr static auto less() {
            using U =  typename juno_impl_set::set_impl<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...>;
            return typename juno_impl_set::less_set<impl, typename U::unique>::result::make();
        }

        inline constexpr static std::size_t size() { return impl::size(); }
        inline constexpr static bool empty() { return impl::empty::value; }
    };
}
