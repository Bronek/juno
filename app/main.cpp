// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <cstdio>

#include <mpl.hpp>
#include <set.hpp>
#include <cassert>

struct Foo; struct Bar; struct Fuz;

int main() {
    using namespace juno;
    using fb = set<Foo, Bar>;
    using fbf = typename fb::insert<Fuz, Bar>::type;
    static_assert(fbf::is_same<Fuz, Foo, Bar>::value);
    static_assert(fbf::contains<Bar, Fuz>::value);
    static_assert(not fbf::contains<long, Fuz>::value);
    int i = 0;
    const auto fn = [](auto*, int& i) { return (i++ < 3); };
    assert(fbf::for_each(fn, i));
    assert(i == 3);

    std::puts("Ta-Da!");
    return 0;
}
