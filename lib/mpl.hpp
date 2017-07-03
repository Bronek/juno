#pragma once

#include <type_traits>
#include <cstdint>

namespace juno {
    enum false_ { };
    enum true_ { };

    template<typename T, typename U, typename W> struct if_;
    template<typename U, typename W> struct if_<true_, U, W> {
        using result = U;
    };
    template<typename U, typename W> struct if_<false_, U, W> {
        using result = W;
    };

    template<typename ...L> struct and_;
    template<typename ...L> struct and_<false_, L...> {
        using result = false_;
    };
    template<> struct and_<true_> {
        using result = true_;
    };
    template<typename ...L> struct and_<true_, L...> {
        using result = typename and_<L...>::result;
    };

    template<typename T> struct to_bool;
    template<> struct to_bool<false_> {
        constexpr static bool value = false;
    };
    template<> struct to_bool<true_> {
        constexpr static bool value = true;
    };
}
