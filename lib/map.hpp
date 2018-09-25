// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include "mpl.hpp"
#include "set.hpp"

#include <type_traits>

namespace juno {
    template <typename ...L> struct map;

    namespace impl_map {
        template <typename ...L> struct impl;

        template <typename TagT> struct wrap {};

        template <>
        struct impl<> {
            using set = juno::set<>;

            template <typename AnyT>
            void get(wrap<AnyT>) const  = delete;
        };

        template <typename MyT, typename MyV, typename ...L>
        struct impl<MyT, MyV, L...> : impl<L...> {
            static_assert(std::is_same<MyT,
                    typename std::remove_cv<typename std::remove_reference<MyT>::type>::type>::value);
            static_assert(not std::is_same<MyT, void>::value);
            static_assert(not std::is_same<MyV, void>::value);
            static_assert(not impl<L...>::set::template contains<MyT>);

            using set = typename juno::set<MyT>::template insert_set<typename impl<L...>::set>::type;

            explicit impl(MyV&& v) : impl<L...>() , value(v)
            {}

            explicit impl(MyV&& v, const impl<L...>& p)
                : impl<L...>(p)
                , value(v)
            {}

            const auto& get(wrap<MyT>) const {
                return value;
            }

            template <typename TagT>
            const auto& get(wrap<TagT> t) const {
                return impl<L...>::get(t);
            }

        private:
            MyV value;
        };
    }

    template <typename ...L> struct map;

    template <> struct map<> {
        using set = juno::set<>;

        template <typename TagT, typename ValT>
        auto add(ValT&& val) const {
            using result = map<
                    typename std::remove_cv<typename std::remove_reference<TagT>::type>::type, ValT>;
            return result(typename result::impl(std::forward<ValT>(val)));
        }
    };

    template <typename MyT, typename MyV, typename ...L> struct map<MyT, MyV, L...> {
    private:
        template <typename ...P> friend struct map;

        using impl = impl_map::impl<MyT, MyV, L...>;
        impl impl_;

        explicit map(impl&& i) : impl_(i) {}

    public:
        using set = typename impl::set;

        template <typename TagT, typename ValT>
        auto add(ValT&& val) const {
            using result = map<typename std::remove_cv<typename std::remove_reference<TagT>::type>::type,
                    ValT, MyT, MyV, L...>;
            return result(typename result::impl(std::forward<ValT>(val), impl_));
        }

        template <typename TagT>
        const auto& get() const {
            using T = typename std::remove_cv<typename std::remove_reference<TagT>::type>::type;
            return impl_.template get(impl_map::wrap<T>());
        }

        template <typename TagT, typename ...P>
        auto run(P&& ... p) const {
            return this->template get<TagT>()(std::forward<P>(p)...);
        }
    };
}
