// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <iostream>
#include <string>

#include <mpl.hpp>
#include <set.hpp>
#include <map.hpp>
#include <tagset.hpp>
#include <cassert>

struct Foo; struct Bar; struct Fuz;

template <typename T, typename Map>
void print(const Map& map) {
    std::cout << map.template get<T>() << std::endl;
}

struct NoCopy{
    const int val;
    NoCopy(int v) : val(v) {}
    NoCopy(const NoCopy&) = delete;
};

std::ostream& operator<< (std::ostream& s, const NoCopy& c) {
    return (s << c.val);
}

int main() {
    using namespace juno;
    using fb = set<Foo, Bar>;
    using fbf = typename fb::insert<Fuz, Bar>::type;
    static_assert(fbf::is_same<Fuz, Foo, Bar>);
    static_assert(fbf::contains<Bar, Fuz>);
    static_assert(not fbf::contains<long, Fuz>);
    int i = 0;
    constexpr auto fn = [](auto*, int& i) { return (i++ < 3); };
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

    const auto m2 = map<>().add<Foo>(0).add<Bar>("Hello");
    print<Foo>(m2);
    print<Bar>(m2);
    const auto m3 = m2.add<Fuz>(std::string("World"));
    print<Fuz>(m3);

    const auto mm = map<>().add<Foo>([] (const auto& a) {
        std::cout << "Hello " << a << std::endl;
    }).add<Bar>([] (const auto& a) {
        std::cout << "Bye " << a << "!" << std::endl;
    }).add<Fuz>([&i] (int a) {
        return NoCopy(a * i);
    }).add<int>(42);
    mm.template run<Foo>("There");
    i = 3;
    const auto n = mm.template run<Fuz>(14);
    mm.template run<Bar>(n);
    mm.template get<Bar>()(mm.template get<int>());
}
