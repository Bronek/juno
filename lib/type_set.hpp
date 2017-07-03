#pragma once

#include "mpl.hpp"

#include <type_traits>
#include <cstdint>

namespace juno {
    template <typename ...L> struct type_set;

    // details of type_set implementation, to ensure uniqueness of types stored in a set
    namespace d {
        template<typename ...L> struct is_in;
        template<typename T> struct is_in<T> {
            using result = false_;
        };
        template<typename T, typename ...L> struct is_in<void, T, L...> {
            using result = true_;
        };
        template<typename T, typename ...L> struct is_in<T, T, L...> {
            using result = true_;
        };
        template<typename T, typename U, typename ...L> struct is_in<T, U, L...> {
            using result = typename is_in<T, L...>::result;
        };

        template <typename ...L> struct set;
        template <typename ...L> struct is_in_set;
        template <typename ...P, typename ...L> struct is_in_set<set<P...>, set<L...>> {
            using set_ = set<P...>;
            using result = typename and_<
                    typename is_in<typename set_::head, L...>::result
                    , typename is_in_set<typename set_::tail, set<L...>>::result
            >::result;
        };
        template <typename ...P, typename ...L> struct is_in_set<type_set<P...>, set<L...>> {
            using set_ = typename type_set<P...>::set_;
            using result = typename and_<
                    typename is_in<typename set_::head, L...>::result
                    , typename is_in_set<typename set_::tail, set<L...>>::result
            >::result;
        };
        template <typename U, typename ...L> struct is_in_set<U, set<L...>> {
            using result = typename is_in<U, L...>::result;
        };
        template <typename ...L> struct is_in_set<set<>, set<L...>> {
            using result = true_;
        };

        template <typename ...L> struct join;
        template <typename T, typename ...L> struct join<T, set<L...>> {
            using result = set<T, L...>;
        };

        template <> struct set<> {
            using unique = set<>;
            using head = void;
            using tail = set<>;

            template <typename U>
            inline constexpr static bool is_in() { return false; }

            template <typename U>
            inline constexpr static bool is_same() {
                return to_bool<typename and_<
                        typename is_in_set<U, unique>::result
                        , typename is_in_set<unique, U>::result
                >::result>::value;
            }

            inline constexpr static std::size_t size() { return 0; }
        };
        template <typename T, typename ...L> struct set<T, L...> {
            using is_in_ = typename d::is_in<T, L...>::result;
            using unique = typename if_<
                    is_in_
                    , typename set<L...>::unique
                    , typename join<T, typename set<L...>::unique>::result
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

            inline constexpr static std::size_t size() { return 1 + set<L...>::size(); }
        };
        template <typename ...L> struct set<void, L...> {
            using unique = typename set<L...>::unique;
            using head = typename set<L...>::head;
            using tail = typename set<L...>::tail;
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
        inline constexpr static bool is_in() {
            return set_::template is_in<
                    typename std::remove_cv<typename std::remove_reference<T>::type>::type
            >();
        }

        template <typename T>
        inline constexpr static bool is_same() {
            return set_::template is_same<typename T::set_>();
        }

        inline constexpr static std::size_t size() { return set_::size(); }
        inline constexpr static bool empty() { return size() == 0; }
    };
}