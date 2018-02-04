// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <cstdio>

#include <mpl.hpp>
#include <set.hpp>

int main() {
    std::puts("Hi!");

    using namespace juno;
    struct Foo; struct Bar; struct Fuz;
    using fb = set<Foo, Bar>;
    static_assert(fb::contains<Foo, Bar>());
    static_assert(fb::contains<Bar>());
    static_assert(fb::contains<>());
    static_assert(not fb::contains<Fuz, Bar>());
    static_assert(fb::intersects<Fuz, Bar>());
    using fbf = decltype(fb::join<Fuz, Bar>());
    static_assert(fbf::is_same<Foo, Fuz, Bar>());
    using ff = decltype(fbf::less<Bar>());
    static_assert(ff::is_same_set<set<Foo, Fuz>>());
}
