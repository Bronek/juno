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
class val;

namespace val_impl {
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
        constexpr static void type() noexcept;
    };

    template <template <typename> typename CheckT, template <typename> typename CheckV,
              typename T, typename V, typename... L>
    struct impl<CheckT, CheckV, T, V, L...> : impl<CheckT, CheckV, L...> {
        using check = typename CheckV<V>::type;
        using base = impl<CheckT, CheckV, L...>;
        using selectors = typename base::selectors::template insert<T>;
        static_assert(not base::selectors ::template test<T>);

        constexpr impl(const impl<CheckT, CheckV, L...>& b, const check& v) : base(b), val_(v)
        {}

        using base::pair;
        constexpr const auto& pair(wrap<T>) const noexcept { return val_; }

        using base::type;
        constexpr auto type(wrap<T>) const noexcept { return wrap<V>{}; }

    private:
        check val_;
    };
}  // namespace val_impl

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
class val {
    template <template <typename> typename, template <typename> typename, typename...>
    friend class val;

    using impl = val_impl::impl<CheckT, CheckV, L...>;
    impl val_;

    constexpr explicit val(const impl& v) : val_(v) {}

public:
    constexpr val() = default;
    template <typename T, typename V> constexpr auto insert(const V& v) const noexcept
    {
        using result = val<CheckT, CheckV, T, V, L...>;
        using rimpl = typename result::impl;
        return result(rimpl(val_, v));
    }

    using set = typename impl::selectors;
    template <typename U> using type = typename decltype(impl::type(val_impl::wrap<U>()))::type;

    template <typename U> constexpr auto get() const noexcept
    {
        return val_.pair(val_impl::wrap<U>());
    }

    template <typename U, typename... A>
    constexpr auto run(A&&... a) const
        -> decltype(val_.pair(val_impl::wrap<U>())(std::forward<A>(a)...))
    {
        return get<U>()(std::forward<A>(a)...);
    }
};

}  // namespace juno
