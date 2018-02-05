// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include <type_traits>
#include <cstdint>

namespace juno {
    namespace juno_impl {
        template<typename T> struct tag {
            using type = T;
            constexpr static char Tag = 0;
            const static uintptr_t value;
        };

        template<typename T>
        const uintptr_t tag<T>::value = reinterpret_cast<uintptr_t>(&Tag);
    }

    template<typename T> class tag
            : private juno_impl::tag<typename std::remove_cv<typename std::remove_reference<T>::type>::type> {
        using impl = juno_impl::tag<typename std::remove_cv<typename std::remove_reference<T>::type>::type>;

    public:
        using type = typename impl::type;
        using impl::value;
    };
}
