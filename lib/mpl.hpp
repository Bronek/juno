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
        using type = type_set_impl<typename std::remove_cv<typename std::remove_reference<L>::type>::type ...>;

        template <typename T>
        inline constexpr static bool is_in() {
            return type::template is_in<T>();
        }

        inline constexpr static std::size_t size() { return type::size(); }
        inline constexpr static bool empty() { return size() == 0; }
    };
}
