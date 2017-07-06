// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file license.txt
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include "mpl.hpp"

#include <type_traits>
#include <cstdint>

namespace juno {
    template <typename ...L> struct type_set;

    // details of type_set implementation, to ensure uniqueness of types stored in a set
    namespace d {
        template<typename ...L> struct is_in;
        template<> struct is_in<void> {
            using result = true_;
        };
        template<typename T> struct is_in<T> {
            using result = false_;
        };
        template<typename T, typename ...L> struct is_in<T, T, L...> {
            using result = true_;
        };
        template<typename T, typename U, typename ...L> struct is_in<T, U, L...> {
            using result = typename is_in<T, L...>::result;
        };

        template <typename ...L> struct set;
        template <typename ...L> struct join;
        template <typename T, typename ...L> struct join<T, set<L...>> {
            using result = set<T, L...>;
        };

        template <typename ...L> struct is_in_set;
        template <typename ...P, typename ...L> struct is_in_set<set<P...>, set<L...>> {
            using set_ = set<P...>;
            using result = typename and_<
                    typename is_in<typename set_::head, L...>::result
                    , typename is_in_set<typename set_::tail, set<L...>>::result
            >::result;
        };
        template <typename ...L> struct is_in_set<set<>, set<L...>> {
            using result = true_;
        };

        template <typename ...L> struct join_set;
        template <typename ...P, typename ...L> struct join_set<set<P...>, set<L...>> {
            using result = typename set<P..., L...>::unique;
        };

        template <> struct set<> {
            using unique = set<>;
            using head = void;
            using tail = set<>;
            using myset = type_set<>;
            using empty = true_;

            template <typename U>
            inline constexpr static bool is_in() {
                return to_bool<typename is_in_set<U, unique>::result>::value;
            }

            template <typename U>
            inline constexpr static bool is_same() {
                return to_bool<typename and_<
                        typename is_in_set<U, unique>::result
                        , typename is_in_set<unique, U>::result
                >::result>::value;
            }

            template <typename U>
            inline constexpr static auto join() {
                using result = typename U::unique::myset;
                return result();
            }

            inline constexpr static std::size_t size() { return 0; }
        };
        template <typename T, typename ...L> struct set<T, L...> {
            using is_in_ = typename d::is_in<T, L...>::result;
            using unique = typename if_<
                    is_in_
                    , typename set<L...>::unique
                    , typename d::join<T, typename set<L...>::unique>::result
            >::result;
            using head = typename if_<
                    is_in_
                    , typename set<L...>::head
                    , T
            >::result;
            using tail = typename if_<
                    is_in_
                    , typename set<L...>::tail
                    , set<L...>
            >::result;
            using myset = type_set<T, L...>;
            using empty = false_;

            template <typename U>
            inline constexpr static bool is_in() {
                return to_bool<typename is_in_set<U, unique>::result>::value;
            }

            template <typename U>
            inline constexpr static bool is_same() {
                return to_bool<typename and_<
                        typename is_in_set<U, unique>::result
                        , typename is_in_set<unique, U>::result
                >::result>::value;
            }

            template <typename U>
            inline constexpr static auto join() {
                using result = typename if_<typename d::is_in_set<U, unique>::result
                        , unique
                        , typename d::join_set<U, unique>::result
                >::result::myset;
                return result();
            }

            inline constexpr static std::size_t size() { return 1 + set<L...>::size(); }
        };
        template <typename ...L> struct set<void, L...> {
            using unique = typename set<L...>::unique;
            using head = typename set<L...>::head;
            using tail = typename set<L...>::tail;
            using myset = typename set<L...>::myset;
            using empty = typename set<L...>::empty;
        };
    }

    template <typename ...L> struct type_set {
    private:
        template <typename ...P> friend struct d::is_in_set;
        template <typename ...P> friend struct type_set;
        using set_ = typename d::set<
                typename std::remove_cv<typename std::remove_reference<L>::type>::type ...
        >::unique;

    public:
        template <typename T>
        inline constexpr static bool is_same() {
            return set_::template is_same<typename T::set_>();
        }

        template <typename ...P>
        inline constexpr static bool is_same_setof() {
            return set_::template is_same<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>();
        }

        template <typename T>
        inline constexpr static bool is_in() {
            return set_::template is_in<typename T::set_>();
        }

        template <typename ...P>
        inline constexpr static bool is_in_setof() {
            return set_::template is_in<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>();
        }

        template <typename T>
        inline constexpr static auto join() {
            return set_::template join<typename T::set_>();
        }

        template <typename ...P>
        inline constexpr static auto join_setof() {
            return set_::template join<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>();
        }

        inline constexpr static std::size_t size() { return set_::size(); }
        inline constexpr static bool empty() { return to_bool<typename set_::empty>::value; }
    };
}
