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
    static_assert(fb::contains<Foo, Bar>::value);
    static_assert(fb::contains<Bar>::value);
    static_assert(fb::contains<>::value);
    static_assert(not fb::contains<Fuz, Bar>::value);
    static_assert(fb::intersects<Fuz, Bar>::value);
    using fbf = typename fb::join<Fuz, Bar>::type;
    static_assert(fbf::is_same<Foo, Fuz, Bar>::value);
    using ff = typename fbf::less<Bar>::type;
    static_assert(ff::is_same_set<set<Foo, Fuz>>::value);
}
