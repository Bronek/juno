// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

namespace juno {
    template<bool T, typename U, typename W> struct if_;
    template<typename U, typename W> struct if_<true, U, W> {
        using type = U;
    };
    template<typename U, typename W> struct if_<false, U, W> {
        using type = W;
    };
}
