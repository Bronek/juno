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
    template <typename ...L> struct type_set;
    namespace d {
        enum false_e { };
        enum true_e { };

        template <typename T> struct to_bool;
        template <> struct to_bool<false_e> {
            constexpr static bool value = false;
        };
        template <> struct to_bool<true_e> {
            constexpr static bool value = true;
        };

        template <typename T, typename U, typename W> struct select;
        template <typename U, typename W> struct select<true_e, U, W> {
            using result = U;
        };
        template <typename U, typename W> struct select<false_e, U, W> {
            using result = W;
        };

        template <typename ...L> struct and_;
        template <typename ...L> struct and_<false_e, L...> {
            using result = false_e;
        };
        template <> struct and_<true_e> {
            using result = true_e;
        };
        template <typename ...L> struct and_<true_e, L...> {
            using result = typename and_<L...>::result;
        };

        template <typename ...L> struct is_in;
        template <typename T> struct is_in<T> {
            using result = false_e;
        };
        template <typename T, typename ...L> struct is_in<void, T, L...> {
            using result = true_e;
        };
        template <typename T, typename ...L> struct is_in<T, T, L...> {
            using result = true_e;
        };
        template <typename T, typename U, typename ...L> struct is_in<T, U, L...> {
            using result = typename is_in<T, L...>::result;
        };

        template <typename ...L> struct set;
        template <typename ...L> struct is_in_set;
        template <typename ...P, typename ...L>  struct is_in_set<set<P...>, set<L...>> {
            using set_ = set<P...>;
            using result = typename and_<
                typename is_in<typename set_::head, L...>::result
                , typename is_in_set<typename set_::tail, set<L...>>::result
            >::result;
        };
        template <typename ...P, typename ...L>  struct is_in_set<type_set<P...>, set<L...>> {
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
            using result = true_e;
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
            using unique = typename select<
                typename is_in<T, L...>::result
                , typename set<L...>::unique
                , typename join<T, typename set<L...>::unique>::result
                >::result;
            using head = typename select<
                typename is_in<T, L...>::result
                , typename set<L...>::head
                , T
            >::result;
            using tail = typename select<
                typename is_in<T, L...>::result
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
