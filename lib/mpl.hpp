#pragma once

#include <type_traits>
#include <cstdint>

namespace juno {
    template<typename T> struct type_tag;

    template <typename T> class type_tag_impl {
        template<typename U> friend struct type_tag;

        constexpr static char tag_ = 0;
        const static uintptr_t value;
    };

    template <typename T>
    constexpr char type_tag_impl<T>::tag_;

    template <typename T>
    const uintptr_t type_tag_impl<T>::value = reinterpret_cast<uintptr_t>(&tag_);

    template <typename T> struct type_tag
            : private type_tag_impl<typename std::remove_cv<typename std::remove_reference<T>::type>::type> {
        using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
        using type_tag_impl<type>::value;
    };

    // details of type_set implementation, to ensure uniqueness of types stored in a set
    namespace d {
        enum false_e { };
        enum true_e { };

        template <typename ...L> struct is_in;
        template <typename T> struct is_in<T> {
            using result = false_e;
        };
        template <typename T, typename ...L> struct is_in<T, T, L...> {
            using result = true_e;
        };
        template <typename T, typename U, typename ...L> struct is_in<T, U, L...> {
            using result = typename is_in<T, L...>::result;
        };

        template <typename T, typename U, typename W> struct select;
        template <typename U, typename W> struct select<true_e, U, W> {
            using result = U;
        };
        template <typename U, typename W> struct select<false_e, U, W> {
            using result = W;
        };

        template <typename ...L> struct set;
        template <typename ...L> struct join;
        template <typename T, typename ...L> struct join<T, set<L...>> {
            using result = set<T, L...>;
        };

        template <> struct set<> {
            using result = set<>;
        };
        template <typename T, typename ...L> struct set<T, L...> {
            using result = typename select<
                typename is_in<T, L...>::result
                , typename set<L...>::result
                , typename join<T, typename set<L...>::result>::result
                >::result;
        };

        template <typename ...L> struct unpack;
        template <typename ...L> struct unpack<set<L...>> {
            using result = L... ;
        };

        template <typename ...L> struct unique {
            using result = typename unpack<typename set<
                typename std::remove_cv<typename std::remove_reference<L>::type>::type ...
                >::result>::result;
        };
    }

    template <typename ...L> class type_set_impl;
    template <typename ...L> struct type_set;

    template <> class type_set_impl<> {
        template<typename ...> friend struct type_set;
        template<typename ...> friend class type_set_impl;

        template <typename U>
        inline constexpr static bool is_in() { return false; }
        inline constexpr static std::size_t size() { return 0; }
    };

    template <typename T, typename ...L> class type_set_impl<T, L...>
            : type_set_impl<L...> {
        template<typename ...> friend struct type_set;
        template<typename ...> friend class type_set_impl;

        template <typename U>
        inline constexpr static bool is_in() {
            using u = typename std::remove_cv<typename std::remove_reference<U>::type>::type;
            return std::is_same<u, T>::value || type_set_impl<L...>::template is_in<U>();
        }

        inline constexpr static std::size_t size() {
            return 1 + type_set_impl<L...>::size();
        }
    };

    template <typename ...L> class type_set_impl<void, L...> : public type_set_impl<L...> { };

    template <typename ...L> struct type_set {
        using type = type_set_impl<typename d::unique<L...>::result... >;

        template <typename T>
        inline constexpr static bool is_in() {
            return type::template is_in<T>();
        }

        inline constexpr static std::size_t size() { return type::size(); }
        inline constexpr static bool empty() { return size() == 0; }
    };
}
