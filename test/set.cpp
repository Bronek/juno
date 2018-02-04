// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <gtest/gtest.h>

#include <type_traits>

#include <set.hpp>

TEST(set, type_set__basic)
{
    // sanity checks of basic use cases
    using namespace juno;

    // incomplete types are OK
    struct Foo; struct Bar; struct Fuz; union Baz;

    // empty set_impl is an empty set_impl
    static_assert(set<>::is_same_set<set<>>());
    static_assert(set<>::is_same<>());
    static_assert(set<>::empty());
    static_assert(set<>::size() == 0);

    // void can be used to denote a non-element
    static_assert(set<void>::empty());
    static_assert(set<void>::size() == 0);
    static_assert(set<void, void>::empty());
    static_assert(set<void, void>::size() == 0);
    static_assert(set<>::is_same_set<set<void>>());
    static_assert(set<>::is_same_set<set<void, void>>());
    static_assert(set<>::is_same<void, void, void>());
    static_assert(set<void>::is_same_set<set<>>());
    static_assert(set<void>::is_same_set<set<void>>());
    static_assert(set<void, void>::is_same<>());
    static_assert(set<int, void>::is_same_set<set<int>>());
    static_assert(set<int, void>::size() == 1);
    static_assert(not set<int, void>::empty());
    static_assert(set<int, void, void>::size() == 1);
    static_assert(set<void, void, int, void, void>::size() == 1);
    static_assert(set<int>::is_same<void, int, void>());
    static_assert(not set<int>::is_same_set<set<void>>());
    static_assert(not set<void>::is_same_set<set<int>>());

    // empty set can be found in any set
    static_assert(set<>::contains_set<set<>>());
    static_assert(set<void>::contains_set<set<>>());
    static_assert(set<void>::contains<>());
    static_assert(set<>::contains<void>());
    static_assert(set<>::contains_set<set<void, void>>());
    static_assert(set<>::contains<void, void>());
    static_assert(set<void, int>::contains_set<set<void>>());
    static_assert(set<void, int>::contains_set<set<>>());
    static_assert(set<Foo, int>::contains_set<set<>>());
    static_assert(set<int>::contains<>());
    static_assert(set<int, Baz>::contains<>());
    static_assert(set<int>::contains<void>());

    // for "intersects_set" operation, we are looking for a non-empty intersection of two sets
    static_assert(not set<>::intersects_set<set<>>());
    static_assert(not set<void>::intersects<>());
    static_assert(not set<>::intersects<int, long>());
    static_assert(not set<int, long>::intersects<>());
    static_assert(not set<int, long>::intersects<Foo, Bar>());
    static_assert(set<int, long>::intersects_set<set<Foo, Bar, long>>());
    static_assert(set<Fuz, long>::intersects<Fuz, Bar, int>());
    static_assert(set<Fuz, long, Bar>::intersects<Bar>());
    static_assert(not set<Fuz, long>::intersects_set<set<int, Foo, Bar>>());
    static_assert(not set<Fuz, long>::intersects_set<set<void>>());
    static_assert(not set<Fuz, long>::intersects<void, int>());
    static_assert(set<Fuz, long>::intersects<void, long>());

    // ordering of elements is ignored and duplicate elements are ignored
    static_assert(set<int, long>::is_same<int, long>());
    static_assert(set<int, long>::is_same<long, int>());
    static_assert(set<int, long>::is_same<int, long, int>());
    static_assert(set<int, long>::size() == 2);
    static_assert(set<long, int, long>::is_same<int, long, int>());
    static_assert(set<long, int, long>::size() == 2);
    static_assert(not set<long, int, long>::is_same<int>());
    static_assert(not set<long, int, long>::is_same<long, long>());
    static_assert(set<int, long, void>::is_same<long, long, int, int>());
    static_assert(set<int, long, void>::size() == 2);

    // elements are stripped of reference and cv-qualifiers before any operation
    typedef set<Foo, Bar, int, const int, int&&> setA;
    static_assert(setA::is_same<Bar, Foo, int>());
    static_assert(setA::is_same<Foo, Bar, const int&>());
    static_assert(not setA::empty());
    static_assert(setA::size() == 3);
    typedef set<const Foo, void, const volatile Fuz, Baz&> setB;
    static_assert(setB::is_same<Baz&&, Foo&&, Fuz&&>());
    static_assert(not setB::is_same_set<setA>());
    static_assert(not setB::empty());
    static_assert(setB::size() == 3);
    static_assert(set<void, void, int, const int&&, void, void>::size() == 1);
    static_assert(set<
            int, const int, volatile int, const volatile int
            , int&, const int&, volatile int&, const volatile int&
            , int&&, const int&&, volatile int&&, const volatile int&&
            , long, const long, volatile long, const volatile long
            , long&, const long&, volatile long&, const volatile long&
            , long&&, const long&&, volatile long&&, const volatile long&&>::size() == 2);
    static_assert(set<
            int, const int, volatile int, const volatile int, void
            , int&, const int&, volatile int&, const volatile int&, void, void
            , int&&, const int&&, volatile int&&, const volatile int&&, void, void, void
            , long, const long, volatile long, const volatile long, void, void, void, void
            , long&, const long&, volatile long&, const volatile long&, void, void, void, void, void
            , long&&, const long&&, volatile long&&, const volatile long&&, void, void, void, void, void, void
    >::is_same_set<set<int, long>>());

    // "join_set" performs union of sets
    typedef decltype(setA::join_set<setB>()) setAB;
    static_assert(setAB::is_same_set<set<int, Foo, Fuz, Baz, Bar>>());
    static_assert(setAB::is_same<Bar, Baz, int, Foo, Fuz>());
    static_assert(not setAB::empty());
    static_assert(setAB::size() == 5);
    static_assert(not setAB::is_same<Foo>());
    static_assert(not setAB::is_same_set<setA>());
    static_assert(not setAB::is_same_set<setB>());
    static_assert(not setAB::is_same_set<set<Bar, Baz, int>>());
    static_assert(setAB::is_same_set<decltype(setA::join<Fuz, Baz>())>());
    // union of sets is commutative
    static_assert(setAB::is_same_set<decltype(setB::join_set<setA>())>());
    static_assert(setAB::is_same_set<decltype(setB::join<int, Bar, Foo>())>());

    // more testing of join_set
    static_assert(set<int, long, unsigned long>::is_same_set<
            decltype(set<int, long>::join<unsigned long>())
    >());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long>::join<long>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long>::join<int>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long>::join<int, long>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long>::join<void>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long>::join<>())>());

    static_assert(set<void>::is_same_set<decltype(set<>::join_set<set<>>())>());
    static_assert(set<>::is_same_set<decltype(set<>::join_set<set<void>>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<>::join_set<set<const int, long&&>>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int>::join_set<set<int, long>>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long>::join_set<set<int, long>>())>());
    static_assert(set<int, long>::is_same_set<decltype(set<int, long&&>::join_set<set<const int&>>())>());

    // intersection of sets
    static_assert(set<>::is_same_set<decltype(set<>::cross_set<set<>>())>());
    static_assert(set<>::is_same_set<decltype(set<>::cross<>())>());
    static_assert(set<>::is_same_set<decltype(set<int>::cross<>())>());
    static_assert(set<>::is_same_set<decltype(set<>::cross<int>())>());
    static_assert(set<>::is_same_set<decltype(set<int, Foo>::cross<>())>());
    static_assert(set<>::is_same_set<decltype(set<>::cross<int, Foo>())>());
    static_assert(set<>::is_same_set<decltype(set<int, Foo>::cross<Bar>())>());
    static_assert(set<>::is_same_set<decltype(set<Bar>::cross<int, Foo>())>());
    static_assert(set<Baz>::is_same_set<decltype(set<int, Foo, Baz>::cross<Bar, Baz>())>());
    static_assert(set<Baz>::is_same_set<decltype(set<Bar, Baz>::cross<int, Foo, Baz>())>());
    static_assert(set<Baz, Bar, int>::is_same_set<decltype(set<Baz, const Bar, int&>::cross<int&&, Baz, Bar>())>());
    static_assert(set<Baz, Bar>::is_same_set<decltype(set<int, Baz, Foo, void, Bar>::cross<void, long, Fuz, Bar, Baz>())>());

    // difference of sets
    static_assert(set<>::is_same_set<decltype(set<>::less_set<set<>>())>());
    static_assert(set<>::is_same_set<decltype(set<>::less<>())>());
    static_assert(set<int>::is_same_set<decltype(set<int>::less<>())>());
    static_assert(set<>::is_same_set<decltype(set<>::less<int>())>());
    static_assert(set<int, Foo>::is_same_set<decltype(set<int, Foo>::less<>())>());
    static_assert(set<>::is_same_set<decltype(set<>::less<int, Foo>())>());
    static_assert(set<int, Foo>::is_same_set<decltype(set<int, Foo>::less<Bar>())>());
    static_assert(set<Bar>::is_same_set<decltype(set<Bar>::less<int, Foo>())>());
    static_assert(set<int, Foo>::is_same_set<decltype(set<int, Foo, Baz>::less<Bar, Baz>())>());
    static_assert(set<Bar>::is_same_set<decltype(set<Bar, Baz>::less<int, Foo, Baz>())>());
    static_assert(set<>::is_same_set<decltype(set<Baz, const Bar, int&>::less<int&&, Baz, Bar>())>());
    static_assert(set<int, Foo>::is_same_set<decltype(set<int, Baz, Foo, void, Bar>::less<void, long, Fuz, Bar, Baz>())>());

    SUCCEED();
}

TEST(set, type_set__details_unique)
{
    // test that duplicate elements are ignored and void is used to denote non-element
    // these are implementation details of set_impl::unique which itself is implementation
    // detail of set
    using namespace juno;
    struct Foo; struct Bar;

    // can't find actual type in an empty list
    static_assert(std::is_same<juno_impl_set::contains<int>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Foo>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, void>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Foo, void, void>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Foo, void, void, void>::result, std::false_type>::value);

    // can find void, i.e. no-element, in any list including empty one
    static_assert(std::is_same<juno_impl_set::contains<void>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<void, void>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<void, Foo>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<void, Foo, void, bool>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<void, Foo, Bar, int, bool>::result, std::true_type>::value);

    // look for actual type where we expect to find it
    static_assert(std::is_same<juno_impl_set::contains<int, int>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, int, void>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, void, int>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, void, int, void>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, void, void, int>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, int, long, Foo>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, Foo, Bar, long, int>::result, std::true_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<int, Foo, void, void, int, void>::result, std::true_type>::value);

    // look for actual type where we do not expect to find it
    static_assert(std::is_same<juno_impl_set::contains<Bar, int>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, int, void>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, void, int>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, void, int, void>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, void, void, int>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, int, long, Foo>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, Foo, bool, long, int>::result, std::false_type>::value);
    static_assert(std::is_same<juno_impl_set::contains<Bar, Foo, void, void, int, void>::result, std::false_type>::value);

    // unique means no duplicate types, no voids
    static_assert(std::is_same<
            juno_impl_set::set_impl<>::unique
            , juno_impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void>::unique
            , juno_impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void, void>::unique
            , juno_impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void, void, void>::unique
            , juno_impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<int>::unique
            , juno_impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<int, void, int>::unique
            , juno_impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<int, int, int>::unique
            , juno_impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void, int, int, int, int, void, void>::unique
            , juno_impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<long, int, void>::unique
            , juno_impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<long, int, int>::unique
            , juno_impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<long, int, int, void, void, int>::unique
            , juno_impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void, int, int, long>::unique
            , juno_impl_set::set_impl<int, long>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<long, int, int, long, int>::unique
            , juno_impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<int, int, long, long, long ,int, int, int, int>::unique
            , juno_impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void, void, long, int, void, int, long, long, long, int, int, long, int>::unique
            , juno_impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            juno_impl_set::set_impl<void, long, void, int, long, int, int, long, int, long, long, long, long, void>::unique
            , juno_impl_set::set_impl<int, long>>::value);

    SUCCEED();
}

TEST(set, type_set__extensive__void_and_ref_cv_stripping)
{
    using namespace juno;

    // test removal of duplicate types and "void" from set
    static_assert(set<>::is_same_set<set<void>>());
    static_assert(set<>::is_same_set<set<void, void>>());
    static_assert(set<>::is_same_set<set<void, void, void>>());
    static_assert(set<void>::is_same_set<set<>>());
    static_assert(set<void, void>::is_same_set<set<>>());
    static_assert(set<void, void, void>::is_same_set<set<>>());
    static_assert(set<void, void, void>::is_same_set<set<void>>());
    static_assert(set<void, void, void>::is_same_set<set<void, void>>());
    static_assert(set<void, void>::is_same_set<set<void, void>>());
    static_assert(set<void, void>::is_same_set<set<void>>());
    static_assert(set<int>::is_same_set<set<void, int, void>>());
    static_assert(set<void, int>::is_same_set<set<void, int, void>>());
    static_assert(set<int>::is_same_set<set<void, const int, void, volatile int>>());
    static_assert(set<int>::is_same_set<set<void, int&&, void, const int&, void, void>>());
    static_assert(set<int>::is_same_set<set<void, volatile int, void, void>>());
    static_assert(set<int>::is_same_set<set<void, void, void, const int&>>());
    static_assert(set<void, void, int, void, const int&&, void>::is_same_set<set<void, void, void, const int&>>());

    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void, void>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<int>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<long&&>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const unsigned long&>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<volatile long, int>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void, int&, long&>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long&&, const unsigned long&>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long&, unsigned long&&, int&&>>());
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long, unsigned long, int, void, void>>());
    static_assert(set<int, long, unsigned long>::contains_set<set<const long&, unsigned long&&, int&&>>());
    static_assert(set<int, long, unsigned long>::contains_set<set<long, unsigned long, int>>());
    static_assert(not set<int, long, unsigned long>::contains_set<set<long, unsigned long, int, char>>());
    static_assert(not set<int, long, unsigned long>::contains_set<set<void, unsigned int, void>>());

    // all of these are the same set_impl<int>
    constexpr auto i = set<int>();
    constexpr auto ci = set<const int>();
    constexpr auto vi = set<volatile int>();
    constexpr auto cvi = set<const volatile int>();
    constexpr auto ri = set<int&, void, int&>();
    constexpr auto rci = set<void, const int&, void, const int&>();
    constexpr auto rvi = set<void, void, volatile int&, void, void, volatile int&, void, void>();
    constexpr auto rcvi = set<void, const volatile int&, const volatile int&, void, void>();
    constexpr auto rri = set<int&&, int, void>();
    constexpr auto rrci = set<void, void, const int&&, int&>();
    constexpr auto rrvi = set<void, volatile int&&, void, int&&, void>();
    constexpr auto rrcvi = set<void, void, const volatile int&&, void, const int&, void, void, void>();

    static_assert(decltype(i)::is_same_set<decltype(ci)>());
    static_assert(decltype(i)::is_same_set<decltype(vi)>());
    static_assert(decltype(i)::is_same_set<decltype(cvi)>());
    static_assert(decltype(i)::is_same_set<decltype(ri)>());
    static_assert(decltype(i)::is_same_set<decltype(rci)>());
    static_assert(decltype(i)::is_same_set<decltype(rvi)>());
    static_assert(decltype(i)::is_same_set<decltype(rcvi)>());
    static_assert(decltype(i)::is_same_set<decltype(rri)>());
    static_assert(decltype(i)::is_same_set<decltype(rrci)>());
    static_assert(decltype(i)::is_same_set<decltype(rrvi)>());
    static_assert(decltype(i)::is_same_set<decltype(rrcvi)>());
    static_assert(decltype(i)::contains_set<decltype(ci)>());
    static_assert(decltype(i)::contains_set<decltype(vi)>());
    static_assert(decltype(i)::contains_set<decltype(cvi)>());
    static_assert(decltype(i)::contains_set<decltype(ri)>());
    static_assert(decltype(i)::contains_set<decltype(rci)>());
    static_assert(decltype(i)::contains_set<decltype(rvi)>());
    static_assert(decltype(i)::contains_set<decltype(rcvi)>());
    static_assert(decltype(i)::contains_set<decltype(rri)>());
    static_assert(decltype(i)::contains_set<decltype(rrci)>());
    static_assert(decltype(i)::contains_set<decltype(rrvi)>());
    static_assert(decltype(i)::contains_set<decltype(rrcvi)>());

    static_assert(not i.empty());
    static_assert(not ci.empty());
    static_assert(not vi.empty());
    static_assert(not cvi.empty());
    static_assert(not ri.empty());
    static_assert(not rci.empty());
    static_assert(not rvi.empty());
    static_assert(not rcvi.empty());
    static_assert(not rri.empty());
    static_assert(not rrci.empty());
    static_assert(not rrvi.empty());
    static_assert(not rrcvi.empty());
    static_assert(i.size() == 1);
    static_assert(ci.size() == 1);
    static_assert(vi.size() == 1);
    static_assert(cvi.size() == 1);
    static_assert(ri.size() == 1);
    static_assert(rci.size() == 1);
    static_assert(rvi.size() == 1);
    static_assert(rcvi.size() == 1);
    static_assert(rri.size() == 1);
    static_assert(rrci.size() == 1);
    static_assert(rrvi.size() == 1);
    static_assert(rrcvi.size() == 1);

    static_assert(i.contains<int>());
    static_assert(i.contains<const int>());
    static_assert(i.contains<volatile int>());
    static_assert(i.contains<const volatile int>());
    static_assert(i.contains<int&>());
    static_assert(i.contains<const int&>());
    static_assert(i.contains<volatile int&>());
    static_assert(i.contains<const volatile int&>());
    static_assert(i.contains<int&&>());
    static_assert(i.contains<const int&&>());
    static_assert(i.contains<volatile int&&>());
    static_assert(i.contains<const volatile int&&>());
    static_assert(i.contains<signed int>());
    static_assert(i.contains<const signed int>());
    static_assert(i.contains<volatile signed int>());
    static_assert(i.contains<const volatile signed int>());
    static_assert(i.contains<signed int&>());
    static_assert(i.contains<const signed int&>());
    static_assert(i.contains<volatile signed int&>());
    static_assert(i.contains<const volatile signed int&>());
    static_assert(i.contains<signed int&&>());
    static_assert(i.contains<const signed int&&>());
    static_assert(i.contains<volatile signed int&&>());
    static_assert(i.contains<const volatile signed int&&>());
    static_assert(ci.contains<int>());
    static_assert(ci.contains<const int>());
    static_assert(ci.contains<volatile int>());
    static_assert(ci.contains<const volatile int>());
    static_assert(ci.contains<int&>());
    static_assert(ci.contains<const int&>());
    static_assert(ci.contains<volatile int&>());
    static_assert(ci.contains<const volatile int&>());
    static_assert(ci.contains<int&&>());
    static_assert(ci.contains<const int&&>());
    static_assert(ci.contains<volatile int&&>());
    static_assert(ci.contains<const volatile int&&>());
    static_assert(ci.contains<signed int>());
    static_assert(ci.contains<const signed int>());
    static_assert(ci.contains<volatile signed int>());
    static_assert(ci.contains<const volatile signed int>());
    static_assert(ci.contains<signed int&>());
    static_assert(ci.contains<const signed int&>());
    static_assert(ci.contains<volatile signed int&>());
    static_assert(ci.contains<const volatile signed int&>());
    static_assert(ci.contains<signed int&&>());
    static_assert(ci.contains<const signed int&&>());
    static_assert(ci.contains<volatile signed int&&>());
    static_assert(ci.contains<const volatile signed int&&>());
    static_assert(vi.contains<int>());
    static_assert(vi.contains<const int>());
    static_assert(vi.contains<volatile int>());
    static_assert(vi.contains<const volatile int>());
    static_assert(vi.contains<int&>());
    static_assert(vi.contains<const int&>());
    static_assert(vi.contains<volatile int&>());
    static_assert(vi.contains<const volatile int&>());
    static_assert(vi.contains<int&&>());
    static_assert(vi.contains<const int&&>());
    static_assert(vi.contains<volatile int&&>());
    static_assert(vi.contains<const volatile int&&>());
    static_assert(vi.contains<signed int>());
    static_assert(vi.contains<const signed int>());
    static_assert(vi.contains<volatile signed int>());
    static_assert(vi.contains<const volatile signed int>());
    static_assert(vi.contains<signed int&>());
    static_assert(vi.contains<const signed int&>());
    static_assert(vi.contains<volatile signed int&>());
    static_assert(vi.contains<const volatile signed int&>());
    static_assert(vi.contains<signed int&&>());
    static_assert(vi.contains<const signed int&&>());
    static_assert(vi.contains<volatile signed int&&>());
    static_assert(vi.contains<const volatile signed int&&>());
    static_assert(cvi.contains<int>());
    static_assert(cvi.contains<const int>());
    static_assert(cvi.contains<volatile int>());
    static_assert(cvi.contains<const volatile int>());
    static_assert(cvi.contains<int&>());
    static_assert(cvi.contains<const int&>());
    static_assert(cvi.contains<volatile int&>());
    static_assert(cvi.contains<const volatile int&>());
    static_assert(cvi.contains<int&&>());
    static_assert(cvi.contains<const int&&>());
    static_assert(cvi.contains<volatile int&&>());
    static_assert(cvi.contains<const volatile int&&>());
    static_assert(cvi.contains<signed int>());
    static_assert(cvi.contains<const signed int>());
    static_assert(cvi.contains<volatile signed int>());
    static_assert(cvi.contains<const volatile signed int>());
    static_assert(cvi.contains<signed int&>());
    static_assert(cvi.contains<const signed int&>());
    static_assert(cvi.contains<volatile signed int&>());
    static_assert(cvi.contains<const volatile signed int&>());
    static_assert(cvi.contains<signed int&&>());
    static_assert(cvi.contains<const signed int&&>());
    static_assert(cvi.contains<volatile signed int&&>());
    static_assert(cvi.contains<const volatile signed int&&>());
    static_assert(ri.contains<int>());
    static_assert(ri.contains<const int>());
    static_assert(ri.contains<volatile int>());
    static_assert(ri.contains<const volatile int>());
    static_assert(ri.contains<int&>());
    static_assert(ri.contains<const int&>());
    static_assert(ri.contains<volatile int&>());
    static_assert(ri.contains<const volatile int&>());
    static_assert(ri.contains<int&&>());
    static_assert(ri.contains<const int&&>());
    static_assert(ri.contains<volatile int&&>());
    static_assert(ri.contains<const volatile int&&>());
    static_assert(ri.contains<signed int>());
    static_assert(ri.contains<const signed int>());
    static_assert(ri.contains<volatile signed int>());
    static_assert(ri.contains<const volatile signed int>());
    static_assert(ri.contains<signed int&>());
    static_assert(ri.contains<const signed int&>());
    static_assert(ri.contains<volatile signed int&>());
    static_assert(ri.contains<const volatile signed int&>());
    static_assert(ri.contains<signed int&&>());
    static_assert(ri.contains<const signed int&&>());
    static_assert(ri.contains<volatile signed int&&>());
    static_assert(ri.contains<const volatile signed int&&>());
    static_assert(rci.contains<int>());
    static_assert(rci.contains<const int>());
    static_assert(rci.contains<volatile int>());
    static_assert(rci.contains<const volatile int>());
    static_assert(rci.contains<int&>());
    static_assert(rci.contains<const int&>());
    static_assert(rci.contains<volatile int&>());
    static_assert(rci.contains<const volatile int&>());
    static_assert(rci.contains<int&&>());
    static_assert(rci.contains<const int&&>());
    static_assert(rci.contains<volatile int&&>());
    static_assert(rci.contains<const volatile int&&>());
    static_assert(rci.contains<signed int>());
    static_assert(rci.contains<const signed int>());
    static_assert(rci.contains<volatile signed int>());
    static_assert(rci.contains<const volatile signed int>());
    static_assert(rci.contains<signed int&>());
    static_assert(rci.contains<const signed int&>());
    static_assert(rci.contains<volatile signed int&>());
    static_assert(rci.contains<const volatile signed int&>());
    static_assert(rci.contains<signed int&&>());
    static_assert(rci.contains<const signed int&&>());
    static_assert(rci.contains<volatile signed int&&>());
    static_assert(rci.contains<const volatile signed int&&>());
    static_assert(rvi.contains<int>());
    static_assert(rvi.contains<const int>());
    static_assert(rvi.contains<volatile int>());
    static_assert(rvi.contains<const volatile int>());
    static_assert(rvi.contains<int&>());
    static_assert(rvi.contains<const int&>());
    static_assert(rvi.contains<volatile int&>());
    static_assert(rvi.contains<const volatile int&>());
    static_assert(rvi.contains<int&&>());
    static_assert(rvi.contains<const int&&>());
    static_assert(rvi.contains<volatile int&&>());
    static_assert(rvi.contains<const volatile int&&>());
    static_assert(rvi.contains<signed int>());
    static_assert(rvi.contains<const signed int>());
    static_assert(rvi.contains<volatile signed int>());
    static_assert(rvi.contains<const volatile signed int>());
    static_assert(rvi.contains<signed int&>());
    static_assert(rvi.contains<const signed int&>());
    static_assert(rvi.contains<volatile signed int&>());
    static_assert(rvi.contains<const volatile signed int&>());
    static_assert(rvi.contains<signed int&&>());
    static_assert(rvi.contains<const signed int&&>());
    static_assert(rvi.contains<volatile signed int&&>());
    static_assert(rvi.contains<const volatile signed int&&>());
    static_assert(rcvi.contains<int>());
    static_assert(rcvi.contains<const int>());
    static_assert(rcvi.contains<volatile int>());
    static_assert(rcvi.contains<const volatile int>());
    static_assert(rcvi.contains<int&>());
    static_assert(rcvi.contains<const int&>());
    static_assert(rcvi.contains<volatile int&>());
    static_assert(rcvi.contains<const volatile int&>());
    static_assert(rcvi.contains<int&&>());
    static_assert(rcvi.contains<const int&&>());
    static_assert(rcvi.contains<volatile int&&>());
    static_assert(rcvi.contains<const volatile int&&>());
    static_assert(rcvi.contains<signed int>());
    static_assert(rcvi.contains<const signed int>());
    static_assert(rcvi.contains<volatile signed int>());
    static_assert(rcvi.contains<const volatile signed int>());
    static_assert(rcvi.contains<signed int&>());
    static_assert(rcvi.contains<const signed int&>());
    static_assert(rcvi.contains<volatile signed int&>());
    static_assert(rcvi.contains<const volatile signed int&>());
    static_assert(rcvi.contains<signed int&&>());
    static_assert(rcvi.contains<const signed int&&>());
    static_assert(rcvi.contains<volatile signed int&&>());
    static_assert(rcvi.contains<const volatile signed int&&>());
    static_assert(rri.contains<int>());
    static_assert(rri.contains<const int>());
    static_assert(rri.contains<volatile int>());
    static_assert(rri.contains<const volatile int>());
    static_assert(rri.contains<int&>());
    static_assert(rri.contains<const int&>());
    static_assert(rri.contains<volatile int&>());
    static_assert(rri.contains<const volatile int&>());
    static_assert(rri.contains<int&&>());
    static_assert(rri.contains<const int&&>());
    static_assert(rri.contains<volatile int&&>());
    static_assert(rri.contains<const volatile int&&>());
    static_assert(rri.contains<signed int>());
    static_assert(rri.contains<const signed int>());
    static_assert(rri.contains<volatile signed int>());
    static_assert(rri.contains<const volatile signed int>());
    static_assert(rri.contains<signed int&>());
    static_assert(rri.contains<const signed int&>());
    static_assert(rri.contains<volatile signed int&>());
    static_assert(rri.contains<const volatile signed int&>());
    static_assert(rri.contains<signed int&&>());
    static_assert(rri.contains<const signed int&&>());
    static_assert(rri.contains<volatile signed int&&>());
    static_assert(rri.contains<const volatile signed int&&>());
    static_assert(rrci.contains<int>());
    static_assert(rrci.contains<const int>());
    static_assert(rrci.contains<volatile int>());
    static_assert(rrci.contains<const volatile int>());
    static_assert(rrci.contains<int&>());
    static_assert(rrci.contains<const int&>());
    static_assert(rrci.contains<volatile int&>());
    static_assert(rrci.contains<const volatile int&>());
    static_assert(rrci.contains<int&&>());
    static_assert(rrci.contains<const int&&>());
    static_assert(rrci.contains<volatile int&&>());
    static_assert(rrci.contains<const volatile int&&>());
    static_assert(rrci.contains<signed int>());
    static_assert(rrci.contains<const signed int>());
    static_assert(rrci.contains<volatile signed int>());
    static_assert(rrci.contains<const volatile signed int>());
    static_assert(rrci.contains<signed int&>());
    static_assert(rrci.contains<const signed int&>());
    static_assert(rrci.contains<volatile signed int&>());
    static_assert(rrci.contains<const volatile signed int&>());
    static_assert(rrci.contains<signed int&&>());
    static_assert(rrci.contains<const signed int&&>());
    static_assert(rrci.contains<volatile signed int&&>());
    static_assert(rrci.contains<const volatile signed int&&>());
    static_assert(rrvi.contains<int>());
    static_assert(rrvi.contains<const int>());
    static_assert(rrvi.contains<volatile int>());
    static_assert(rrvi.contains<const volatile int>());
    static_assert(rrvi.contains<int&>());
    static_assert(rrvi.contains<const int&>());
    static_assert(rrvi.contains<volatile int&>());
    static_assert(rrvi.contains<const volatile int&>());
    static_assert(rrvi.contains<int&&>());
    static_assert(rrvi.contains<const int&&>());
    static_assert(rrvi.contains<volatile int&&>());
    static_assert(rrvi.contains<const volatile int&&>());
    static_assert(rrvi.contains<signed int>());
    static_assert(rrvi.contains<const signed int>());
    static_assert(rrvi.contains<volatile signed int>());
    static_assert(rrvi.contains<const volatile signed int>());
    static_assert(rrvi.contains<signed int&>());
    static_assert(rrvi.contains<const signed int&>());
    static_assert(rrvi.contains<volatile signed int&>());
    static_assert(rrvi.contains<const volatile signed int&>());
    static_assert(rrvi.contains<signed int&&>());
    static_assert(rrvi.contains<const signed int&&>());
    static_assert(rrvi.contains<volatile signed int&&>());
    static_assert(rrvi.contains<const volatile signed int&&>());
    static_assert(rrcvi.contains<int>());
    static_assert(rrcvi.contains<const int>());
    static_assert(rrcvi.contains<volatile int>());
    static_assert(rrcvi.contains<const volatile int>());
    static_assert(rrcvi.contains<int&>());
    static_assert(rrcvi.contains<const int&>());
    static_assert(rrcvi.contains<volatile int&>());
    static_assert(rrcvi.contains<const volatile int&>());
    static_assert(rrcvi.contains<int&&>());
    static_assert(rrcvi.contains<const int&&>());
    static_assert(rrcvi.contains<volatile int&&>());
    static_assert(rrcvi.contains<const volatile int&&>());
    static_assert(rrcvi.contains<signed int>());
    static_assert(rrcvi.contains<const signed int>());
    static_assert(rrcvi.contains<volatile signed int>());
    static_assert(rrcvi.contains<const volatile signed int>());
    static_assert(rrcvi.contains<signed int&>());
    static_assert(rrcvi.contains<const signed int&>());
    static_assert(rrcvi.contains<volatile signed int&>());
    static_assert(rrcvi.contains<const volatile signed int&>());
    static_assert(rrcvi.contains<signed int&&>());
    static_assert(rrcvi.contains<const signed int&&>());
    static_assert(rrcvi.contains<volatile signed int&&>());
    static_assert(rrcvi.contains<const volatile signed int&&>());

    static_assert(not i.contains<long>());
    static_assert(not ci.contains<long>());
    static_assert(not i.contains<unsigned int>());
    static_assert(not rci.contains<unsigned int>());
    struct Foo;
    static_assert(not i.contains<Foo>());
    static_assert(not rrvi.contains<Foo>());

    // all of these are the same set_impl<int, long>, except for ..
    constexpr auto rci_rrl = set<const int&, long&&>();
    constexpr auto i_cl = set<int, const long>();
    constexpr auto i_rrl = set<int, long&&>();
    constexpr auto i_rsl = set<int, signed long&>();
    constexpr auto ri_rrcsl = set<int&, const signed long&&>();
    constexpr auto rri_rrsl = set<int&&, signed long&&>();
    // ... the one here, which is set_impl<int, unsigned long>
    constexpr auto rri_rrul = set<int&&, unsigned long&&>();

    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_cl)>());
    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_rrl)>());
    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_rsl)>());
    static_assert(decltype(rci_rrl)::is_same_set<decltype(ri_rrcsl)>());
    static_assert(decltype(rci_rrl)::is_same_set<decltype(rri_rrsl)>());
    static_assert(not decltype(rri_rrsl)::is_same_set<decltype(rri_rrul)>());

    static_assert(not rci_rrl.empty());
    static_assert(not i_cl.empty());
    static_assert(not i_rrl.empty());
    static_assert(not i_rsl.empty());
    static_assert(not ri_rrcsl.empty());
    static_assert(not rri_rrsl.empty());
    static_assert(not rri_rrul.empty());
    static_assert(rci_rrl.size() == 2);
    static_assert(i_cl.size() == 2);
    static_assert(i_rrl.size() == 2);
    static_assert(i_rsl.size() == 2);
    static_assert(ri_rrcsl.size() == 2);
    static_assert(rri_rrsl.size() == 2);
    static_assert(rri_rrul.size() == 2);

    static_assert(rci_rrl.contains<int>());
    static_assert(rci_rrl.contains<const int>());
    static_assert(rci_rrl.contains<volatile int>());
    static_assert(rci_rrl.contains<const volatile int>());
    static_assert(rci_rrl.contains<int&>());
    static_assert(rci_rrl.contains<const int&>());
    static_assert(rci_rrl.contains<volatile int&>());
    static_assert(rci_rrl.contains<const volatile int&>());
    static_assert(rci_rrl.contains<int&&>());
    static_assert(rci_rrl.contains<const int&&>());
    static_assert(rci_rrl.contains<volatile int&&>());
    static_assert(rci_rrl.contains<const volatile int&&>());
    static_assert(rci_rrl.contains<signed int>());
    static_assert(rci_rrl.contains<const signed int>());
    static_assert(rci_rrl.contains<volatile signed int>());
    static_assert(rci_rrl.contains<const volatile signed int>());
    static_assert(rci_rrl.contains<signed int&>());
    static_assert(rci_rrl.contains<const signed int&>());
    static_assert(rci_rrl.contains<volatile signed int&>());
    static_assert(rci_rrl.contains<const volatile signed int&>());
    static_assert(rci_rrl.contains<signed int&&>());
    static_assert(rci_rrl.contains<const signed int&&>());
    static_assert(rci_rrl.contains<volatile signed int&&>());
    static_assert(rci_rrl.contains<const volatile signed int&&>());
    static_assert(rci_rrl.contains<long>());
    static_assert(rci_rrl.contains<const long>());
    static_assert(rci_rrl.contains<volatile long>());
    static_assert(rci_rrl.contains<const volatile long>());
    static_assert(rci_rrl.contains<long&>());
    static_assert(rci_rrl.contains<const long&>());
    static_assert(rci_rrl.contains<volatile long&>());
    static_assert(rci_rrl.contains<const volatile long&>());
    static_assert(rci_rrl.contains<long&&>());
    static_assert(rci_rrl.contains<const long&&>());
    static_assert(rci_rrl.contains<volatile long&&>());
    static_assert(rci_rrl.contains<const volatile long&&>());
    static_assert(rci_rrl.contains<signed long>());
    static_assert(rci_rrl.contains<const signed long>());
    static_assert(rci_rrl.contains<volatile signed long>());
    static_assert(rci_rrl.contains<const volatile signed long>());
    static_assert(rci_rrl.contains<signed long&>());
    static_assert(rci_rrl.contains<const signed long&>());
    static_assert(rci_rrl.contains<volatile signed long&>());
    static_assert(rci_rrl.contains<const volatile signed long&>());
    static_assert(rci_rrl.contains<signed long&&>());
    static_assert(rci_rrl.contains<const signed long&&>());
    static_assert(rci_rrl.contains<volatile signed long&&>());
    static_assert(rci_rrl.contains<const volatile signed long&&>());
    static_assert(i_cl.contains<long>());
    static_assert(i_cl.contains<const long>());
    static_assert(i_cl.contains<volatile long>());
    static_assert(i_cl.contains<const volatile long>());
    static_assert(i_cl.contains<long&>());
    static_assert(i_cl.contains<const long&>());
    static_assert(i_cl.contains<volatile long&>());
    static_assert(i_cl.contains<const volatile long&>());
    static_assert(i_cl.contains<long&&>());
    static_assert(i_cl.contains<const long&&>());
    static_assert(i_cl.contains<volatile long&&>());
    static_assert(i_cl.contains<const volatile long&&>());
    static_assert(i_cl.contains<signed long>());
    static_assert(i_cl.contains<const signed long>());
    static_assert(i_cl.contains<volatile signed long>());
    static_assert(i_cl.contains<const volatile signed long>());
    static_assert(i_cl.contains<signed long&>());
    static_assert(i_cl.contains<const signed long&>());
    static_assert(i_cl.contains<volatile signed long&>());
    static_assert(i_cl.contains<const volatile signed long&>());
    static_assert(i_cl.contains<signed long&&>());
    static_assert(i_cl.contains<const signed long&&>());
    static_assert(i_cl.contains<volatile signed long&&>());
    static_assert(i_cl.contains<const volatile signed long&&>());

    static_assert(not rci_rrl.contains<unsigned int>());
    static_assert(not rci_rrl.contains<Foo>());

    static_assert(rri_rrul.contains<int>());
    static_assert(rri_rrul.contains<int&&>());
    static_assert(rri_rrul.contains<const int>());
    static_assert(rri_rrul.contains<unsigned long>());
    static_assert(rri_rrul.contains<const unsigned long>());
    static_assert(rri_rrul.contains<volatile unsigned long>());
    static_assert(rri_rrul.contains<const volatile unsigned long>());
    static_assert(rri_rrul.contains<unsigned long&>());
    static_assert(rri_rrul.contains<const unsigned long&>());
    static_assert(rri_rrul.contains<volatile unsigned long&>());
    static_assert(rri_rrul.contains<const volatile unsigned long&>());
    static_assert(rri_rrul.contains<unsigned long&&>());
    static_assert(rri_rrul.contains<const unsigned long&&>());
    static_assert(rri_rrul.contains<volatile unsigned long&&>());
    static_assert(rri_rrul.contains<const volatile unsigned long&&>());

    static_assert(not rri_rrul.contains<unsigned int>());
    static_assert(not rri_rrul.contains<signed long>());
    static_assert(not rri_rrul.contains<const Foo&>());
}
