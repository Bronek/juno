// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <cstdio>

#include <mpl.hpp>
#include <set.hpp>
#include <tagset.hpp>
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

    auto&& ts = tagset{}.insert_set<fbf>();
    assert((ts.contains<Bar, Fuz>()));
    assert(ts.contains<>());
    assert((not ts.contains_set<set<int, Bar, Fuz>>()));

    const auto ts1 = std::move(ts);
    assert(not ts1.empty());
    assert((ts1.intersects<int, Foo>()));
    assert(not ts1.intersects<>());

    std::puts("Ta-Da!");
    return 0;
}
