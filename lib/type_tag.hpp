// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file license.txt
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include <type_traits>
#include <cstdint>

namespace juno {
    namespace d {
        template<typename T> struct type_tag {
            using type = T;
            constexpr static char tag_ = 0;
            const static uintptr_t value;
        };

        template<typename T>
        constexpr char type_tag<T>::tag_;

        template<typename T>
        const uintptr_t type_tag<T>::value = reinterpret_cast<uintptr_t>(&tag_);
    }

    template<typename T> class type_tag
            : private d::type_tag<typename std::remove_cv<typename std::remove_reference<T>::type>::type> {
        using impl = d::type_tag<typename std::remove_cv<typename std::remove_reference<T>::type>::type>;

    public:
        using type = typename impl::type;
        using impl::value;
    };
}
