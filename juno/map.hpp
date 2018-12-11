// Copyright (c) 2018 Bronek (Bronislaw) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include <type_traits>

#include "set.hpp"

namespace juno {

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
class map;

namespace map_impl {
    template <typename T> struct wrap {
        constexpr explicit wrap() = default;
        using type = T;
    };

    template <template <typename> typename CheckT, template <typename> typename CheckV,
              typename... L>
    struct impl;

    template <template <typename> typename CheckT, template <typename> typename CheckV>
    struct impl<CheckT, CheckV> {
        using selectors = set<CheckT>;
        constexpr explicit impl() = default;
        constexpr static void pair() noexcept;
    };

    template <template <typename> typename CheckT, template <typename> typename CheckV,
              typename T, typename V, typename... L>
    struct impl<CheckT, CheckV, T, V, L...> : impl<CheckT, CheckV, L...> {
        using check = typename CheckV<V>::type;
        using base = impl<CheckT, CheckV, L...>;
        using selectors = typename base::selectors::template insert<T>;
        static_assert(not base::selectors ::template test<T>);

        constexpr explicit impl() = default;

        using base::pair;
        constexpr static auto pair(wrap<T>) noexcept { return wrap<check>(); }
    };
}  // namespace map_impl

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
class map {
    using impl = map_impl::impl<CheckT, CheckV, L...>;

public:
    constexpr map() = default;
    template <typename T, typename V> constexpr auto insert() const noexcept
    {
        using result = map<CheckT, CheckV, T, V, L...>;
        return result();
    }

    using set = typename impl::selectors;

    template <typename U> using type = typename decltype(impl::pair(map_impl::wrap<U>()))::type;
};

}  // namespace juno
