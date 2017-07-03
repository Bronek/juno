// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file license.txt
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include <type_traits>
#include <cstdint>

namespace juno {
    template<typename T> struct type_tag;

    template<typename T> class type_tag_impl {
        template<typename U> friend struct type_tag;

        constexpr static char tag_ = 0;
        const static uintptr_t value;
    };

    template<typename T>
    constexpr char type_tag_impl<T>::tag_;

    template<typename T>
    const uintptr_t type_tag_impl<T>::value = reinterpret_cast<uintptr_t>(&tag_);

    template<typename T> struct type_tag
            : private type_tag_impl<typename std::remove_cv<typename std::remove_reference<T>::type>::type> {
        using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
        using type_tag_impl<type>::value;
    };
}
