// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
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
        template <typename ...L> struct join<void, set<L...>> {
            using result = set<L...>;
        };
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

        template <typename ...L> struct is_any_set;
        template <typename ...P, typename ...L> struct is_any_set<set<P...>, set<L...>> {
            using set_ = set<P...>;
            using result = typename or_<
                    typename is_in<typename set_::head, L...>::result
                    , typename is_any_set<typename set_::tail, set<L...>>::result
            >::result;
        };
        template <typename ...L> struct is_any_set<set<>, set<L...>> {
            using result = false_;
        };

        template <typename ...L> struct join_set;
        template <typename ...P, typename ...L> struct join_set<set<P...>, set<L...>> {
            using result = typename set<P..., L...>::unique;
        };

        template <typename ...L> struct cross_set;
        template <typename ...L> struct cross_set<set<>, set<L...>> {
            using result = set<>;
        };
        template <typename T, typename ...P, typename ...L> struct cross_set<set<T, P...>, set<L...>> {
            using tail = typename cross_set<set<P...>, set<L...>>::result;
            using result = typename if_<
                    typename is_in<T, L...>::result
                    , typename join<T, tail>::result
                    , tail
            >::result;
        };

        template <typename ...L> struct less_set;
        template <typename ...L> struct less_set<set<>, set<L...>> {
            using result = set<>;
        };
        template <typename T, typename ...P, typename ...L> struct less_set<set<T, P...>, set<L...>> {
            using tail = typename less_set<set<P...>, set<L...>>::result;
            using result = typename if_<
                    typename is_in<T, L...>::result
                    , tail
                    , typename join<T, tail>::result
            >::result;
        };

        template <> struct set<> {
            using unique = set<>;
            using head = void;
            using tail = set<>;
            using make = type_set<>;
            using empty = true_;

            template <typename U>
            inline constexpr static bool is_in() {
                return is_in_set<U, unique>::result::value;
            }

            template <typename U>
            inline constexpr static bool is_any() {
                return false;
            }

            template <typename U>
            inline constexpr static bool is_same() {
                return and_<
                        typename is_in_set<U, unique>::result
                        , typename is_in_set<unique, U>::result
                >::result::value;
            }

            template <typename U>
            struct join {
                using result = typename U::unique;
            };

            template <typename U>
            struct cross {
                using result = set<>;
            };

            template <typename U>
            struct less {
                using result = set<>;
            };

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
            using make = type_set<T, L...>;
            using empty = false_;

            template <typename U>
            inline constexpr static bool is_in() {
                return is_in_set<U, unique>::result::value;
            }

            template <typename U>
            inline constexpr static bool is_any() {
                return is_any_set<U, unique>::result::value;
            }

            template <typename U>
            inline constexpr static bool is_same() {
                return and_<
                        typename is_in_set<U, unique>::result
                        , typename is_in_set<unique, U>::result
                >::result::value;
            }

            template <typename U>
            struct join {
                using result = typename if_<typename d::is_in_set<U, unique>::result
                        , unique
                        , typename d::join_set<U, unique>::result
                >::result;
            };

            template <typename U>
            struct cross {
                using result = typename d::cross_set<U, unique>::result;
            };

            template <typename U>
            struct less {
                using result = typename d::less_set<unique, U>::result;
            };

            inline constexpr static std::size_t size() { return 1 + set<L...>::size(); }
        };
        template <typename ...L> struct set<void, L...> {
            using unique = typename set<L...>::unique;
            using head = typename set<L...>::head;
            using tail = typename set<L...>::tail;
            using make = typename set<L...>::make;
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
        inline constexpr static bool is_any() {
            return set_::template is_any<typename T::set_>();
        }

        template <typename ...P>
        inline constexpr static bool is_any_setof() {
            return set_::template is_any<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>();
        }

        template <typename T>
        inline constexpr static auto join() {
            return typename set_::template join<typename T::set_>::result::make();
        }

        template <typename ...P>
        inline constexpr static auto join_setof() {
            return typename set_::template join<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>::result::make();
        }

        template <typename T>
        inline constexpr static auto cross() {
            return typename set_::template cross<typename T::set_>::result::make();
        }

        template <typename ...P>
        inline constexpr static auto cross_setof() {
            return typename set_::template cross<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>::result::make();
        }

        template <typename T>
        inline constexpr static auto less() {
            return typename set_::template less<typename T::set_>::result::make();
        }

        template <typename ...P>
        inline constexpr static auto less_setof() {
            return typename set_::template less<typename d::set<
                    typename std::remove_cv<typename std::remove_reference<P>::type>::type ...
            >::unique>::result::make();
        }

        inline constexpr static std::size_t size() { return set_::size(); }
        inline constexpr static bool empty() { return set_::empty::value; }
    };

}
