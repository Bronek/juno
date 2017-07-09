// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file license.txt
// or copy at https://opensource.org/licenses/MIT

#include <gtest/gtest.h>

#include <type_traits>

#include <type_set.hpp>

TEST(type_set, type_set__basic)
{
    // sanity checks of basic use cases
    using namespace juno;

    // incomplete types are OK
    struct Foo; struct Bar; struct Fuz; union Baz;

    // empty set is an empty set
    static_assert(type_set<>::is_same<type_set<>>(), "");
    static_assert(type_set<>::is_same_setof<>(), "");
    static_assert(type_set<>::empty(), "");
    static_assert(type_set<>::size() == 0, "");

    // void can be used to denote a non-element
    static_assert(type_set<void>::empty(), "");
    static_assert(type_set<void>::size() == 0, "");
    static_assert(type_set<void, void>::empty(), "");
    static_assert(type_set<void, void>::size() == 0, "");
    static_assert(type_set<>::is_same<type_set<void>>(), "");
    static_assert(type_set<>::is_same<type_set<void, void>>(), "");
    static_assert(type_set<>::is_same_setof<void, void, void>(), "");
    static_assert(type_set<void>::is_same<type_set<>>(), "");
    static_assert(type_set<void>::is_same<type_set<void>>(), "");
    static_assert(type_set<void, void>::is_same_setof<>(), "");
    static_assert(type_set<int, void>::is_same<type_set<int>>(), "");
    static_assert(type_set<int, void>::size() == 1, "");
    static_assert(not type_set<int, void>::empty(), "");
    static_assert(type_set<int, void, void>::size() == 1, "");
    static_assert(type_set<void, void, int, void, void>::size() == 1, "");
    static_assert(type_set<int>::is_same_setof<void, int, void>(), "");
    static_assert(not type_set<int>::is_same<type_set<void>>(), "");
    static_assert(not type_set<void>::is_same<type_set<int>>(), "");

    // empty set can be found in any set
    static_assert(type_set<>::is_in<type_set<>>(), "");
    static_assert(type_set<void>::is_in<type_set<>>(), "");
    static_assert(type_set<void>::is_in_setof<>(), "");
    static_assert(type_set<>::is_in_setof<void>(), "");
    static_assert(type_set<>::is_in<type_set<void, void>>(), "");
    static_assert(type_set<>::is_in_setof<void, void>(), "");
    static_assert(type_set<void, int>::is_in<type_set<void>>(), "");
    static_assert(type_set<void, int>::is_in<type_set<>>(), "");
    static_assert(type_set<Foo, int>::is_in<type_set<>>(), "");
    static_assert(type_set<int>::is_in_setof<>(), "");
    static_assert(type_set<int, Baz>::is_in_setof<>(), "");
    static_assert(type_set<int>::is_in_setof<void>(), "");

    // for "is_any" operation, we are looking for a non-empty intersection of two sets
    static_assert(not type_set<>::is_any<type_set<>>(), "");
    static_assert(not type_set<void>::is_any_setof<>(), "");
    static_assert(not type_set<>::is_any_setof<int, long>(), "");
    static_assert(not type_set<int, long>::is_any_setof<>(), "");
    static_assert(not type_set<int, long>::is_any_setof<Foo, Bar>(), "");
    static_assert(type_set<int, long>::is_any_setof<Foo, Bar, long>(), "");
    static_assert(type_set<Fuz, long>::is_any_setof<Fuz, Bar, int>(), "");
    static_assert(type_set<Fuz, long, Bar>::is_any_setof<Bar>(), "");
    static_assert(not type_set<Fuz, long>::is_any<type_set<int, Foo, Bar>>(), "");
    static_assert(not type_set<Fuz, long>::is_any<type_set<void>>(), "");
    static_assert(not type_set<Fuz, long>::is_any_setof<void, int>(), "");
    static_assert(type_set<Fuz, long>::is_any_setof<void, long>(), "");

    // ordering of elements is ignored and duplicate elements are ignored
    static_assert(type_set<int, long>::is_same_setof<int, long>(), "");
    static_assert(type_set<int, long>::is_same_setof<long, int>(), "");
    static_assert(type_set<int, long>::is_same_setof<int, long, int>(), "");
    static_assert(type_set<int, long>::size() == 2, "");
    static_assert(type_set<long, int, long>::is_same_setof<int, long, int>(), "");
    static_assert(type_set<long, int, long>::size() == 2, "");
    static_assert(not type_set<long, int, long>::is_same_setof<int>(), "");
    static_assert(not type_set<long, int, long>::is_same_setof<long, long>(), "");
    static_assert(type_set<int, long, void>::is_same_setof<long, long, int, int>(), "");
    static_assert(type_set<int, long, void>::size() == 2, "");

    // elements are stripped of reference and cv-qualifiers before any operation
    typedef type_set<Foo, Bar, int, const int, int&&> setA;
    static_assert(setA::is_same_setof<Bar, Foo, int>(), "");
    static_assert(setA::is_same_setof<Foo, Bar, const int&>(), "");
    static_assert(not setA::empty(), "");
    static_assert(setA::size() == 3, "");
    typedef type_set<const Foo, void, const volatile Fuz, Baz&> setB;
    static_assert(setB::is_same_setof<Baz&&, Foo&&, Fuz&&>(), "");
    static_assert(not setB::is_same<setA>(), "");
    static_assert(not setB::empty(), "");
    static_assert(setB::size() == 3, "");
    static_assert(type_set<void, void, int, const int&&, void, void>::size() == 1, "");
    static_assert(type_set<
            int, const int, volatile int, const volatile int
            , int&, const int&, volatile int&, const volatile int&
            , int&&, const int&&, volatile int&&, const volatile int&&
            , long, const long, volatile long, const volatile long
            , long&, const long&, volatile long&, const volatile long&
            , long&&, const long&&, volatile long&&, const volatile long&&>::size() == 2, "");
    static_assert(type_set<
            int, const int, volatile int, const volatile int, void
            , int&, const int&, volatile int&, const volatile int&, void, void
            , int&&, const int&&, volatile int&&, const volatile int&&, void, void, void
            , long, const long, volatile long, const volatile long, void, void, void, void
            , long&, const long&, volatile long&, const volatile long&, void, void, void, void, void
            , long&&, const long&&, volatile long&&, const volatile long&&, void, void, void, void, void, void
    >::is_same<type_set<int, long>>(), "");

    // "join" performs union of sets
    typedef decltype(setA::join<setB>()) setAB;
    static_assert(setAB::is_same<type_set<int, Foo, Fuz, Baz, Bar>>(), "");
    static_assert(setAB::is_same_setof<Bar, Baz, int, Foo, Fuz>(), "");
    static_assert(not setAB::empty(), "");
    static_assert(setAB::size() == 5, "");
    static_assert(not setAB::is_same_setof<Foo>(), "");
    static_assert(not setAB::is_same<setA>(), "");
    static_assert(not setAB::is_same<setB>(), "");
    static_assert(not setAB::is_same<type_set<Bar, Baz, int>>(), "");
    static_assert(setAB::is_same<decltype(setA::join_setof<Fuz, Baz>())>(), "");

    // union of sets is commutative
    static_assert(setAB::is_same<decltype(setB::join<setA>())>(), "");
    static_assert(setAB::is_same<decltype(setB::join_setof<int, Bar, Foo>())>(), "");

    // more testing of join
    static_assert(type_set<int, long, unsigned long>::is_same<
            decltype(type_set<int, long>::join_setof<unsigned long>())
    >(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long>::join_setof<long>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long>::join_setof<int>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long>::join_setof<int, long>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long>::join_setof<void>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long>::join_setof<>())>(), "");

    static_assert(type_set<void>::is_same<decltype(type_set<>::join<type_set<>>())>(), "");
    static_assert(type_set<>::is_same<decltype(type_set<>::join<type_set<void>>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<>::join<type_set<const int, long&&>>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int>::join<type_set<int, long>>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long>::join<type_set<int, long>>())>(), "");
    static_assert(type_set<int, long>::is_same<decltype(type_set<int, long&&>::join<type_set<const int&>>())>(), "");

    SUCCEED();
}

TEST(type_set, type_set__details_unique)
{
    // test that duplicate elements are ignored and void is used to denote non-element
    // these are implementation details of set::unique which itself is implementation
    // detail of type_set
    using namespace juno;
    struct Foo; struct Bar;

    // can't find actual type in an empty list
    static_assert(std::is_same<d::is_in<int>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Foo>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<int, void>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Foo, void, void>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Foo, void, void, void>::result, false_>::value, "");

    // can find void, i.e. no-element, in any list including empty one
    static_assert(std::is_same<d::is_in<void>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<void, void>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<void, Foo>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<void, Foo, void, bool>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<void, Foo, Bar, int, bool>::result, true_>::value, "");

    // look for actual type where we expect to find it
    static_assert(std::is_same<d::is_in<int, int>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, int, void>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, void, int>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, void, int, void>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, void, void, int>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, int, long, Foo>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, Foo, Bar, long, int>::result, true_>::value, "");
    static_assert(std::is_same<d::is_in<int, Foo, void, void, int, void>::result, true_>::value, "");

    // look for actual type where we do not expect to find it
    static_assert(std::is_same<d::is_in<Bar, int>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, int, void>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, void, int>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, void, int, void>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, void, void, int>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, int, long, Foo>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, Foo, bool, long, int>::result, false_>::value, "");
    static_assert(std::is_same<d::is_in<Bar, Foo, void, void, int, void>::result, false_>::value, "");

    // unique means no duplicate types, no voids
    static_assert(std::is_same<
            d::set<>::unique
            , d::set<>>::value, "");
    static_assert(std::is_same<
            d::set<void>::unique
            , d::set<>>::value, "");
    static_assert(std::is_same<
            d::set<void, void>::unique
            , d::set<>>::value, "");
    static_assert(std::is_same<
            d::set<void, void, void>::unique
            , d::set<>>::value, "");
    static_assert(std::is_same<
            d::set<int>::unique
            , d::set<int>>::value, "");
    static_assert(std::is_same<
            d::set<int, void, int>::unique
            , d::set<int>>::value, "");
    static_assert(std::is_same<
            d::set<int, int, int>::unique
            , d::set<int>>::value, "");
    static_assert(std::is_same<
            d::set<void, int, int, int, int, void, void>::unique
            , d::set<int>>::value, "");
    static_assert(std::is_same<
            d::set<long, int, void>::unique
            , d::set<long, int>>::value, "");
    static_assert(std::is_same<
            d::set<long, int, int>::unique
            , d::set<long, int>>::value, "");
    static_assert(std::is_same<
            d::set<long, int, int, void, void, int>::unique
            , d::set<long, int>>::value, "");
    static_assert(std::is_same<
            d::set<void, int, int, long>::unique
            , d::set<int, long>>::value, "");
    static_assert(std::is_same<
            d::set<long, int, int, long, int>::unique
            , d::set<long, int>>::value, "");
    static_assert(std::is_same<
            d::set<int, int, long, long, long ,int, int, int, int>::unique
            , d::set<long, int>>::value, "");
    static_assert(std::is_same<
            d::set<void, void, long, int, void, int, long, long, long, int, int, long, int>::unique
            , d::set<long, int>>::value, "");
    static_assert(std::is_same<
            d::set<void, long, void, int, long, int, int, long, int, long, long, long, long, void>::unique
            , d::set<int, long>>::value, "");

    SUCCEED();
}

TEST(type_set, type_set__extensive__void_and_ref_cv_stripping)
{
    using namespace juno;

    // test removal of duplicate types and "void" from type_set
    static_assert(type_set<>::is_same<type_set<void>>(), "");
    static_assert(type_set<>::is_same<type_set<void, void>>(), "");
    static_assert(type_set<>::is_same<type_set<void, void, void>>(), "");
    static_assert(type_set<void>::is_same<type_set<>>(), "");
    static_assert(type_set<void, void>::is_same<type_set<>>(), "");
    static_assert(type_set<void, void, void>::is_same<type_set<>>(), "");
    static_assert(type_set<void, void, void>::is_same<type_set<void>>(), "");
    static_assert(type_set<void, void, void>::is_same<type_set<void, void>>(), "");
    static_assert(type_set<void, void>::is_same<type_set<void, void>>(), "");
    static_assert(type_set<void, void>::is_same<type_set<void>>(), "");
    static_assert(type_set<int>::is_same<type_set<void, int, void>>(), "");
    static_assert(type_set<void, int>::is_same<type_set<void, int, void>>(), "");
    static_assert(type_set<int>::is_same<type_set<void, const int, void, volatile int>>(), "");
    static_assert(type_set<int>::is_same<type_set<void, int&&, void, const int&, void, void>>(), "");
    static_assert(type_set<int>::is_same<type_set<void, volatile int, void, void>>(), "");
    static_assert(type_set<int>::is_same<type_set<void, void, void, const int&>>(), "");
    static_assert(type_set<void, void, int, void, const int&&, void>::is_same<type_set<void, void, void, const int&>>(), "");

    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<void>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<void, void>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<int>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<long&&>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<const unsigned long&>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<volatile long, int>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<void, int&, long&>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<const long&&, const unsigned long&>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<const long&, unsigned long&&, int&&>>(), "");
    static_assert(type_set<const int, const long, unsigned long&&>::is_in<type_set<const long, unsigned long, int, void, void>>(), "");
    static_assert(type_set<int, long, unsigned long>::is_in<type_set<const long&, unsigned long&&, int&&>>(), "");
    static_assert(type_set<int, long, unsigned long>::is_in<type_set<long, unsigned long, int>>(), "");
    static_assert(not type_set<int, long, unsigned long>::is_in<type_set<long, unsigned long, int, char>>(), "");
    static_assert(not type_set<int, long, unsigned long>::is_in<type_set<void, unsigned int, void>>(), "");

    // all of these are the same set<int>
    constexpr auto i = type_set<int>();
    constexpr auto ci = type_set<const int>();
    constexpr auto vi = type_set<volatile int>();
    constexpr auto cvi = type_set<const volatile int>();
    constexpr auto ri = type_set<int&, void, int&>();
    constexpr auto rci = type_set<void, const int&, void, const int&>();
    constexpr auto rvi = type_set<void, void, volatile int&, void, void, volatile int&, void, void>();
    constexpr auto rcvi = type_set<void, const volatile int&, const volatile int&, void, void>();
    constexpr auto rri = type_set<int&&, int, void>();
    constexpr auto rrci = type_set<void, void, const int&&, int&>();
    constexpr auto rrvi = type_set<void, volatile int&&, void, int&&, void>();
    constexpr auto rrcvi = type_set<void, void, const volatile int&&, void, const int&, void, void, void>();

    static_assert(decltype(i)::is_same<decltype(ci)>(), "");
    static_assert(decltype(i)::is_same<decltype(vi)>(), "");
    static_assert(decltype(i)::is_same<decltype(cvi)>(), "");
    static_assert(decltype(i)::is_same<decltype(ri)>(), "");
    static_assert(decltype(i)::is_same<decltype(rci)>(), "");
    static_assert(decltype(i)::is_same<decltype(rvi)>(), "");
    static_assert(decltype(i)::is_same<decltype(rcvi)>(), "");
    static_assert(decltype(i)::is_same<decltype(rri)>(), "");
    static_assert(decltype(i)::is_same<decltype(rrci)>(), "");
    static_assert(decltype(i)::is_same<decltype(rrvi)>(), "");
    static_assert(decltype(i)::is_same<decltype(rrcvi)>(), "");
    static_assert(decltype(i)::is_in<decltype(ci)>(), "");
    static_assert(decltype(i)::is_in<decltype(vi)>(), "");
    static_assert(decltype(i)::is_in<decltype(cvi)>(), "");
    static_assert(decltype(i)::is_in<decltype(ri)>(), "");
    static_assert(decltype(i)::is_in<decltype(rci)>(), "");
    static_assert(decltype(i)::is_in<decltype(rvi)>(), "");
    static_assert(decltype(i)::is_in<decltype(rcvi)>(), "");
    static_assert(decltype(i)::is_in<decltype(rri)>(), "");
    static_assert(decltype(i)::is_in<decltype(rrci)>(), "");
    static_assert(decltype(i)::is_in<decltype(rrvi)>(), "");
    static_assert(decltype(i)::is_in<decltype(rrcvi)>(), "");

    static_assert(not i.empty(), "");
    static_assert(not ci.empty(), "");
    static_assert(not vi.empty(), "");
    static_assert(not cvi.empty(), "");
    static_assert(not ri.empty(), "");
    static_assert(not rci.empty(), "");
    static_assert(not rvi.empty(), "");
    static_assert(not rcvi.empty(), "");
    static_assert(not rri.empty(), "");
    static_assert(not rrci.empty(), "");
    static_assert(not rrvi.empty(), "");
    static_assert(not rrcvi.empty(), "");
    static_assert(i.size() == 1, "");
    static_assert(ci.size() == 1, "");
    static_assert(vi.size() == 1, "");
    static_assert(cvi.size() == 1, "");
    static_assert(ri.size() == 1, "");
    static_assert(rci.size() == 1, "");
    static_assert(rvi.size() == 1, "");
    static_assert(rcvi.size() == 1, "");
    static_assert(rri.size() == 1, "");
    static_assert(rrci.size() == 1, "");
    static_assert(rrvi.size() == 1, "");
    static_assert(rrcvi.size() == 1, "");

    static_assert(i.is_in_setof<int>(), "");
    static_assert(i.is_in_setof<const int>(), "");
    static_assert(i.is_in_setof<volatile int>(), "");
    static_assert(i.is_in_setof<const volatile int>(), "");
    static_assert(i.is_in_setof<int&>(), "");
    static_assert(i.is_in_setof<const int&>(), "");
    static_assert(i.is_in_setof<volatile int&>(), "");
    static_assert(i.is_in_setof<const volatile int&>(), "");
    static_assert(i.is_in_setof<int&&>(), "");
    static_assert(i.is_in_setof<const int&&>(), "");
    static_assert(i.is_in_setof<volatile int&&>(), "");
    static_assert(i.is_in_setof<const volatile int&&>(), "");
    static_assert(i.is_in_setof<signed int>(), "");
    static_assert(i.is_in_setof<const signed int>(), "");
    static_assert(i.is_in_setof<volatile signed int>(), "");
    static_assert(i.is_in_setof<const volatile signed int>(), "");
    static_assert(i.is_in_setof<signed int&>(), "");
    static_assert(i.is_in_setof<const signed int&>(), "");
    static_assert(i.is_in_setof<volatile signed int&>(), "");
    static_assert(i.is_in_setof<const volatile signed int&>(), "");
    static_assert(i.is_in_setof<signed int&&>(), "");
    static_assert(i.is_in_setof<const signed int&&>(), "");
    static_assert(i.is_in_setof<volatile signed int&&>(), "");
    static_assert(i.is_in_setof<const volatile signed int&&>(), "");
    static_assert(ci.is_in_setof<int>(), "");
    static_assert(ci.is_in_setof<const int>(), "");
    static_assert(ci.is_in_setof<volatile int>(), "");
    static_assert(ci.is_in_setof<const volatile int>(), "");
    static_assert(ci.is_in_setof<int&>(), "");
    static_assert(ci.is_in_setof<const int&>(), "");
    static_assert(ci.is_in_setof<volatile int&>(), "");
    static_assert(ci.is_in_setof<const volatile int&>(), "");
    static_assert(ci.is_in_setof<int&&>(), "");
    static_assert(ci.is_in_setof<const int&&>(), "");
    static_assert(ci.is_in_setof<volatile int&&>(), "");
    static_assert(ci.is_in_setof<const volatile int&&>(), "");
    static_assert(ci.is_in_setof<signed int>(), "");
    static_assert(ci.is_in_setof<const signed int>(), "");
    static_assert(ci.is_in_setof<volatile signed int>(), "");
    static_assert(ci.is_in_setof<const volatile signed int>(), "");
    static_assert(ci.is_in_setof<signed int&>(), "");
    static_assert(ci.is_in_setof<const signed int&>(), "");
    static_assert(ci.is_in_setof<volatile signed int&>(), "");
    static_assert(ci.is_in_setof<const volatile signed int&>(), "");
    static_assert(ci.is_in_setof<signed int&&>(), "");
    static_assert(ci.is_in_setof<const signed int&&>(), "");
    static_assert(ci.is_in_setof<volatile signed int&&>(), "");
    static_assert(ci.is_in_setof<const volatile signed int&&>(), "");
    static_assert(vi.is_in_setof<int>(), "");
    static_assert(vi.is_in_setof<const int>(), "");
    static_assert(vi.is_in_setof<volatile int>(), "");
    static_assert(vi.is_in_setof<const volatile int>(), "");
    static_assert(vi.is_in_setof<int&>(), "");
    static_assert(vi.is_in_setof<const int&>(), "");
    static_assert(vi.is_in_setof<volatile int&>(), "");
    static_assert(vi.is_in_setof<const volatile int&>(), "");
    static_assert(vi.is_in_setof<int&&>(), "");
    static_assert(vi.is_in_setof<const int&&>(), "");
    static_assert(vi.is_in_setof<volatile int&&>(), "");
    static_assert(vi.is_in_setof<const volatile int&&>(), "");
    static_assert(vi.is_in_setof<signed int>(), "");
    static_assert(vi.is_in_setof<const signed int>(), "");
    static_assert(vi.is_in_setof<volatile signed int>(), "");
    static_assert(vi.is_in_setof<const volatile signed int>(), "");
    static_assert(vi.is_in_setof<signed int&>(), "");
    static_assert(vi.is_in_setof<const signed int&>(), "");
    static_assert(vi.is_in_setof<volatile signed int&>(), "");
    static_assert(vi.is_in_setof<const volatile signed int&>(), "");
    static_assert(vi.is_in_setof<signed int&&>(), "");
    static_assert(vi.is_in_setof<const signed int&&>(), "");
    static_assert(vi.is_in_setof<volatile signed int&&>(), "");
    static_assert(vi.is_in_setof<const volatile signed int&&>(), "");
    static_assert(cvi.is_in_setof<int>(), "");
    static_assert(cvi.is_in_setof<const int>(), "");
    static_assert(cvi.is_in_setof<volatile int>(), "");
    static_assert(cvi.is_in_setof<const volatile int>(), "");
    static_assert(cvi.is_in_setof<int&>(), "");
    static_assert(cvi.is_in_setof<const int&>(), "");
    static_assert(cvi.is_in_setof<volatile int&>(), "");
    static_assert(cvi.is_in_setof<const volatile int&>(), "");
    static_assert(cvi.is_in_setof<int&&>(), "");
    static_assert(cvi.is_in_setof<const int&&>(), "");
    static_assert(cvi.is_in_setof<volatile int&&>(), "");
    static_assert(cvi.is_in_setof<const volatile int&&>(), "");
    static_assert(cvi.is_in_setof<signed int>(), "");
    static_assert(cvi.is_in_setof<const signed int>(), "");
    static_assert(cvi.is_in_setof<volatile signed int>(), "");
    static_assert(cvi.is_in_setof<const volatile signed int>(), "");
    static_assert(cvi.is_in_setof<signed int&>(), "");
    static_assert(cvi.is_in_setof<const signed int&>(), "");
    static_assert(cvi.is_in_setof<volatile signed int&>(), "");
    static_assert(cvi.is_in_setof<const volatile signed int&>(), "");
    static_assert(cvi.is_in_setof<signed int&&>(), "");
    static_assert(cvi.is_in_setof<const signed int&&>(), "");
    static_assert(cvi.is_in_setof<volatile signed int&&>(), "");
    static_assert(cvi.is_in_setof<const volatile signed int&&>(), "");
    static_assert(ri.is_in_setof<int>(), "");
    static_assert(ri.is_in_setof<const int>(), "");
    static_assert(ri.is_in_setof<volatile int>(), "");
    static_assert(ri.is_in_setof<const volatile int>(), "");
    static_assert(ri.is_in_setof<int&>(), "");
    static_assert(ri.is_in_setof<const int&>(), "");
    static_assert(ri.is_in_setof<volatile int&>(), "");
    static_assert(ri.is_in_setof<const volatile int&>(), "");
    static_assert(ri.is_in_setof<int&&>(), "");
    static_assert(ri.is_in_setof<const int&&>(), "");
    static_assert(ri.is_in_setof<volatile int&&>(), "");
    static_assert(ri.is_in_setof<const volatile int&&>(), "");
    static_assert(ri.is_in_setof<signed int>(), "");
    static_assert(ri.is_in_setof<const signed int>(), "");
    static_assert(ri.is_in_setof<volatile signed int>(), "");
    static_assert(ri.is_in_setof<const volatile signed int>(), "");
    static_assert(ri.is_in_setof<signed int&>(), "");
    static_assert(ri.is_in_setof<const signed int&>(), "");
    static_assert(ri.is_in_setof<volatile signed int&>(), "");
    static_assert(ri.is_in_setof<const volatile signed int&>(), "");
    static_assert(ri.is_in_setof<signed int&&>(), "");
    static_assert(ri.is_in_setof<const signed int&&>(), "");
    static_assert(ri.is_in_setof<volatile signed int&&>(), "");
    static_assert(ri.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rci.is_in_setof<int>(), "");
    static_assert(rci.is_in_setof<const int>(), "");
    static_assert(rci.is_in_setof<volatile int>(), "");
    static_assert(rci.is_in_setof<const volatile int>(), "");
    static_assert(rci.is_in_setof<int&>(), "");
    static_assert(rci.is_in_setof<const int&>(), "");
    static_assert(rci.is_in_setof<volatile int&>(), "");
    static_assert(rci.is_in_setof<const volatile int&>(), "");
    static_assert(rci.is_in_setof<int&&>(), "");
    static_assert(rci.is_in_setof<const int&&>(), "");
    static_assert(rci.is_in_setof<volatile int&&>(), "");
    static_assert(rci.is_in_setof<const volatile int&&>(), "");
    static_assert(rci.is_in_setof<signed int>(), "");
    static_assert(rci.is_in_setof<const signed int>(), "");
    static_assert(rci.is_in_setof<volatile signed int>(), "");
    static_assert(rci.is_in_setof<const volatile signed int>(), "");
    static_assert(rci.is_in_setof<signed int&>(), "");
    static_assert(rci.is_in_setof<const signed int&>(), "");
    static_assert(rci.is_in_setof<volatile signed int&>(), "");
    static_assert(rci.is_in_setof<const volatile signed int&>(), "");
    static_assert(rci.is_in_setof<signed int&&>(), "");
    static_assert(rci.is_in_setof<const signed int&&>(), "");
    static_assert(rci.is_in_setof<volatile signed int&&>(), "");
    static_assert(rci.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rvi.is_in_setof<int>(), "");
    static_assert(rvi.is_in_setof<const int>(), "");
    static_assert(rvi.is_in_setof<volatile int>(), "");
    static_assert(rvi.is_in_setof<const volatile int>(), "");
    static_assert(rvi.is_in_setof<int&>(), "");
    static_assert(rvi.is_in_setof<const int&>(), "");
    static_assert(rvi.is_in_setof<volatile int&>(), "");
    static_assert(rvi.is_in_setof<const volatile int&>(), "");
    static_assert(rvi.is_in_setof<int&&>(), "");
    static_assert(rvi.is_in_setof<const int&&>(), "");
    static_assert(rvi.is_in_setof<volatile int&&>(), "");
    static_assert(rvi.is_in_setof<const volatile int&&>(), "");
    static_assert(rvi.is_in_setof<signed int>(), "");
    static_assert(rvi.is_in_setof<const signed int>(), "");
    static_assert(rvi.is_in_setof<volatile signed int>(), "");
    static_assert(rvi.is_in_setof<const volatile signed int>(), "");
    static_assert(rvi.is_in_setof<signed int&>(), "");
    static_assert(rvi.is_in_setof<const signed int&>(), "");
    static_assert(rvi.is_in_setof<volatile signed int&>(), "");
    static_assert(rvi.is_in_setof<const volatile signed int&>(), "");
    static_assert(rvi.is_in_setof<signed int&&>(), "");
    static_assert(rvi.is_in_setof<const signed int&&>(), "");
    static_assert(rvi.is_in_setof<volatile signed int&&>(), "");
    static_assert(rvi.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rcvi.is_in_setof<int>(), "");
    static_assert(rcvi.is_in_setof<const int>(), "");
    static_assert(rcvi.is_in_setof<volatile int>(), "");
    static_assert(rcvi.is_in_setof<const volatile int>(), "");
    static_assert(rcvi.is_in_setof<int&>(), "");
    static_assert(rcvi.is_in_setof<const int&>(), "");
    static_assert(rcvi.is_in_setof<volatile int&>(), "");
    static_assert(rcvi.is_in_setof<const volatile int&>(), "");
    static_assert(rcvi.is_in_setof<int&&>(), "");
    static_assert(rcvi.is_in_setof<const int&&>(), "");
    static_assert(rcvi.is_in_setof<volatile int&&>(), "");
    static_assert(rcvi.is_in_setof<const volatile int&&>(), "");
    static_assert(rcvi.is_in_setof<signed int>(), "");
    static_assert(rcvi.is_in_setof<const signed int>(), "");
    static_assert(rcvi.is_in_setof<volatile signed int>(), "");
    static_assert(rcvi.is_in_setof<const volatile signed int>(), "");
    static_assert(rcvi.is_in_setof<signed int&>(), "");
    static_assert(rcvi.is_in_setof<const signed int&>(), "");
    static_assert(rcvi.is_in_setof<volatile signed int&>(), "");
    static_assert(rcvi.is_in_setof<const volatile signed int&>(), "");
    static_assert(rcvi.is_in_setof<signed int&&>(), "");
    static_assert(rcvi.is_in_setof<const signed int&&>(), "");
    static_assert(rcvi.is_in_setof<volatile signed int&&>(), "");
    static_assert(rcvi.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rri.is_in_setof<int>(), "");
    static_assert(rri.is_in_setof<const int>(), "");
    static_assert(rri.is_in_setof<volatile int>(), "");
    static_assert(rri.is_in_setof<const volatile int>(), "");
    static_assert(rri.is_in_setof<int&>(), "");
    static_assert(rri.is_in_setof<const int&>(), "");
    static_assert(rri.is_in_setof<volatile int&>(), "");
    static_assert(rri.is_in_setof<const volatile int&>(), "");
    static_assert(rri.is_in_setof<int&&>(), "");
    static_assert(rri.is_in_setof<const int&&>(), "");
    static_assert(rri.is_in_setof<volatile int&&>(), "");
    static_assert(rri.is_in_setof<const volatile int&&>(), "");
    static_assert(rri.is_in_setof<signed int>(), "");
    static_assert(rri.is_in_setof<const signed int>(), "");
    static_assert(rri.is_in_setof<volatile signed int>(), "");
    static_assert(rri.is_in_setof<const volatile signed int>(), "");
    static_assert(rri.is_in_setof<signed int&>(), "");
    static_assert(rri.is_in_setof<const signed int&>(), "");
    static_assert(rri.is_in_setof<volatile signed int&>(), "");
    static_assert(rri.is_in_setof<const volatile signed int&>(), "");
    static_assert(rri.is_in_setof<signed int&&>(), "");
    static_assert(rri.is_in_setof<const signed int&&>(), "");
    static_assert(rri.is_in_setof<volatile signed int&&>(), "");
    static_assert(rri.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rrci.is_in_setof<int>(), "");
    static_assert(rrci.is_in_setof<const int>(), "");
    static_assert(rrci.is_in_setof<volatile int>(), "");
    static_assert(rrci.is_in_setof<const volatile int>(), "");
    static_assert(rrci.is_in_setof<int&>(), "");
    static_assert(rrci.is_in_setof<const int&>(), "");
    static_assert(rrci.is_in_setof<volatile int&>(), "");
    static_assert(rrci.is_in_setof<const volatile int&>(), "");
    static_assert(rrci.is_in_setof<int&&>(), "");
    static_assert(rrci.is_in_setof<const int&&>(), "");
    static_assert(rrci.is_in_setof<volatile int&&>(), "");
    static_assert(rrci.is_in_setof<const volatile int&&>(), "");
    static_assert(rrci.is_in_setof<signed int>(), "");
    static_assert(rrci.is_in_setof<const signed int>(), "");
    static_assert(rrci.is_in_setof<volatile signed int>(), "");
    static_assert(rrci.is_in_setof<const volatile signed int>(), "");
    static_assert(rrci.is_in_setof<signed int&>(), "");
    static_assert(rrci.is_in_setof<const signed int&>(), "");
    static_assert(rrci.is_in_setof<volatile signed int&>(), "");
    static_assert(rrci.is_in_setof<const volatile signed int&>(), "");
    static_assert(rrci.is_in_setof<signed int&&>(), "");
    static_assert(rrci.is_in_setof<const signed int&&>(), "");
    static_assert(rrci.is_in_setof<volatile signed int&&>(), "");
    static_assert(rrci.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rrvi.is_in_setof<int>(), "");
    static_assert(rrvi.is_in_setof<const int>(), "");
    static_assert(rrvi.is_in_setof<volatile int>(), "");
    static_assert(rrvi.is_in_setof<const volatile int>(), "");
    static_assert(rrvi.is_in_setof<int&>(), "");
    static_assert(rrvi.is_in_setof<const int&>(), "");
    static_assert(rrvi.is_in_setof<volatile int&>(), "");
    static_assert(rrvi.is_in_setof<const volatile int&>(), "");
    static_assert(rrvi.is_in_setof<int&&>(), "");
    static_assert(rrvi.is_in_setof<const int&&>(), "");
    static_assert(rrvi.is_in_setof<volatile int&&>(), "");
    static_assert(rrvi.is_in_setof<const volatile int&&>(), "");
    static_assert(rrvi.is_in_setof<signed int>(), "");
    static_assert(rrvi.is_in_setof<const signed int>(), "");
    static_assert(rrvi.is_in_setof<volatile signed int>(), "");
    static_assert(rrvi.is_in_setof<const volatile signed int>(), "");
    static_assert(rrvi.is_in_setof<signed int&>(), "");
    static_assert(rrvi.is_in_setof<const signed int&>(), "");
    static_assert(rrvi.is_in_setof<volatile signed int&>(), "");
    static_assert(rrvi.is_in_setof<const volatile signed int&>(), "");
    static_assert(rrvi.is_in_setof<signed int&&>(), "");
    static_assert(rrvi.is_in_setof<const signed int&&>(), "");
    static_assert(rrvi.is_in_setof<volatile signed int&&>(), "");
    static_assert(rrvi.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rrcvi.is_in_setof<int>(), "");
    static_assert(rrcvi.is_in_setof<const int>(), "");
    static_assert(rrcvi.is_in_setof<volatile int>(), "");
    static_assert(rrcvi.is_in_setof<const volatile int>(), "");
    static_assert(rrcvi.is_in_setof<int&>(), "");
    static_assert(rrcvi.is_in_setof<const int&>(), "");
    static_assert(rrcvi.is_in_setof<volatile int&>(), "");
    static_assert(rrcvi.is_in_setof<const volatile int&>(), "");
    static_assert(rrcvi.is_in_setof<int&&>(), "");
    static_assert(rrcvi.is_in_setof<const int&&>(), "");
    static_assert(rrcvi.is_in_setof<volatile int&&>(), "");
    static_assert(rrcvi.is_in_setof<const volatile int&&>(), "");
    static_assert(rrcvi.is_in_setof<signed int>(), "");
    static_assert(rrcvi.is_in_setof<const signed int>(), "");
    static_assert(rrcvi.is_in_setof<volatile signed int>(), "");
    static_assert(rrcvi.is_in_setof<const volatile signed int>(), "");
    static_assert(rrcvi.is_in_setof<signed int&>(), "");
    static_assert(rrcvi.is_in_setof<const signed int&>(), "");
    static_assert(rrcvi.is_in_setof<volatile signed int&>(), "");
    static_assert(rrcvi.is_in_setof<const volatile signed int&>(), "");
    static_assert(rrcvi.is_in_setof<signed int&&>(), "");
    static_assert(rrcvi.is_in_setof<const signed int&&>(), "");
    static_assert(rrcvi.is_in_setof<volatile signed int&&>(), "");
    static_assert(rrcvi.is_in_setof<const volatile signed int&&>(), "");

    static_assert(not i.is_in_setof<long>(), "");
    static_assert(not ci.is_in_setof<long>(), "");
    static_assert(not i.is_in_setof<unsigned int>(), "");
    static_assert(not rci.is_in_setof<unsigned int>(), "");
    struct Foo;
    static_assert(not i.is_in_setof<Foo>(), "");
    static_assert(not rrvi.is_in_setof<Foo>(), "");

    // all of these are the same set<int, long>, except for ..
    constexpr auto rci_rrl = type_set<const int&, long&&>();
    constexpr auto i_cl = type_set<int, const long>();
    constexpr auto i_rrl = type_set<int, long&&>();
    constexpr auto i_rsl = type_set<int, signed long&>();
    constexpr auto ri_rrcsl = type_set<int&, const signed long&&>();
    constexpr auto rri_rrsl = type_set<int&&, signed long&&>();
    // ... the one here, which is set<int, unsigned long>
    constexpr auto rri_rrul = type_set<int&&, unsigned long&&>();

    static_assert(decltype(rci_rrl)::is_same<decltype(i_cl)>(), "");
    static_assert(decltype(rci_rrl)::is_same<decltype(i_rrl)>(), "");
    static_assert(decltype(rci_rrl)::is_same<decltype(i_rsl)>(), "");
    static_assert(decltype(rci_rrl)::is_same<decltype(ri_rrcsl)>(), "");
    static_assert(decltype(rci_rrl)::is_same<decltype(rri_rrsl)>(), "");
    static_assert(not decltype(rri_rrsl)::is_same<decltype(rri_rrul)>(), "");

    static_assert(not rci_rrl.empty(), "");
    static_assert(not i_cl.empty(), "");
    static_assert(not i_rrl.empty(), "");
    static_assert(not i_rsl.empty(), "");
    static_assert(not ri_rrcsl.empty(), "");
    static_assert(not rri_rrsl.empty(), "");
    static_assert(not rri_rrul.empty(), "");
    static_assert(rci_rrl.size() == 2, "");
    static_assert(i_cl.size() == 2, "");
    static_assert(i_rrl.size() == 2, "");
    static_assert(i_rsl.size() == 2, "");
    static_assert(ri_rrcsl.size() == 2, "");
    static_assert(rri_rrsl.size() == 2, "");
    static_assert(rri_rrul.size() == 2, "");

    static_assert(rci_rrl.is_in_setof<int>(), "");
    static_assert(rci_rrl.is_in_setof<const int>(), "");
    static_assert(rci_rrl.is_in_setof<volatile int>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile int>(), "");
    static_assert(rci_rrl.is_in_setof<int&>(), "");
    static_assert(rci_rrl.is_in_setof<const int&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile int&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile int&>(), "");
    static_assert(rci_rrl.is_in_setof<int&&>(), "");
    static_assert(rci_rrl.is_in_setof<const int&&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile int&&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile int&&>(), "");
    static_assert(rci_rrl.is_in_setof<signed int>(), "");
    static_assert(rci_rrl.is_in_setof<const signed int>(), "");
    static_assert(rci_rrl.is_in_setof<volatile signed int>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile signed int>(), "");
    static_assert(rci_rrl.is_in_setof<signed int&>(), "");
    static_assert(rci_rrl.is_in_setof<const signed int&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile signed int&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile signed int&>(), "");
    static_assert(rci_rrl.is_in_setof<signed int&&>(), "");
    static_assert(rci_rrl.is_in_setof<const signed int&&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile signed int&&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile signed int&&>(), "");
    static_assert(rci_rrl.is_in_setof<long>(), "");
    static_assert(rci_rrl.is_in_setof<const long>(), "");
    static_assert(rci_rrl.is_in_setof<volatile long>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile long>(), "");
    static_assert(rci_rrl.is_in_setof<long&>(), "");
    static_assert(rci_rrl.is_in_setof<const long&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile long&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile long&>(), "");
    static_assert(rci_rrl.is_in_setof<long&&>(), "");
    static_assert(rci_rrl.is_in_setof<const long&&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile long&&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile long&&>(), "");
    static_assert(rci_rrl.is_in_setof<signed long>(), "");
    static_assert(rci_rrl.is_in_setof<const signed long>(), "");
    static_assert(rci_rrl.is_in_setof<volatile signed long>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile signed long>(), "");
    static_assert(rci_rrl.is_in_setof<signed long&>(), "");
    static_assert(rci_rrl.is_in_setof<const signed long&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile signed long&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile signed long&>(), "");
    static_assert(rci_rrl.is_in_setof<signed long&&>(), "");
    static_assert(rci_rrl.is_in_setof<const signed long&&>(), "");
    static_assert(rci_rrl.is_in_setof<volatile signed long&&>(), "");
    static_assert(rci_rrl.is_in_setof<const volatile signed long&&>(), "");
    static_assert(i_cl.is_in_setof<long>(), "");
    static_assert(i_cl.is_in_setof<const long>(), "");
    static_assert(i_cl.is_in_setof<volatile long>(), "");
    static_assert(i_cl.is_in_setof<const volatile long>(), "");
    static_assert(i_cl.is_in_setof<long&>(), "");
    static_assert(i_cl.is_in_setof<const long&>(), "");
    static_assert(i_cl.is_in_setof<volatile long&>(), "");
    static_assert(i_cl.is_in_setof<const volatile long&>(), "");
    static_assert(i_cl.is_in_setof<long&&>(), "");
    static_assert(i_cl.is_in_setof<const long&&>(), "");
    static_assert(i_cl.is_in_setof<volatile long&&>(), "");
    static_assert(i_cl.is_in_setof<const volatile long&&>(), "");
    static_assert(i_cl.is_in_setof<signed long>(), "");
    static_assert(i_cl.is_in_setof<const signed long>(), "");
    static_assert(i_cl.is_in_setof<volatile signed long>(), "");
    static_assert(i_cl.is_in_setof<const volatile signed long>(), "");
    static_assert(i_cl.is_in_setof<signed long&>(), "");
    static_assert(i_cl.is_in_setof<const signed long&>(), "");
    static_assert(i_cl.is_in_setof<volatile signed long&>(), "");
    static_assert(i_cl.is_in_setof<const volatile signed long&>(), "");
    static_assert(i_cl.is_in_setof<signed long&&>(), "");
    static_assert(i_cl.is_in_setof<const signed long&&>(), "");
    static_assert(i_cl.is_in_setof<volatile signed long&&>(), "");
    static_assert(i_cl.is_in_setof<const volatile signed long&&>(), "");

    static_assert(not rci_rrl.is_in_setof<unsigned int>(), "");
    static_assert(not rci_rrl.is_in_setof<Foo>(), "");

    static_assert(rri_rrul.is_in_setof<int>(), "");
    static_assert(rri_rrul.is_in_setof<int&&>(), "");
    static_assert(rri_rrul.is_in_setof<const int>(), "");
    static_assert(rri_rrul.is_in_setof<unsigned long>(), "");
    static_assert(rri_rrul.is_in_setof<const unsigned long>(), "");
    static_assert(rri_rrul.is_in_setof<volatile unsigned long>(), "");
    static_assert(rri_rrul.is_in_setof<const volatile unsigned long>(), "");
    static_assert(rri_rrul.is_in_setof<unsigned long&>(), "");
    static_assert(rri_rrul.is_in_setof<const unsigned long&>(), "");
    static_assert(rri_rrul.is_in_setof<volatile unsigned long&>(), "");
    static_assert(rri_rrul.is_in_setof<const volatile unsigned long&>(), "");
    static_assert(rri_rrul.is_in_setof<unsigned long&&>(), "");
    static_assert(rri_rrul.is_in_setof<const unsigned long&&>(), "");
    static_assert(rri_rrul.is_in_setof<volatile unsigned long&&>(), "");
    static_assert(rri_rrul.is_in_setof<const volatile unsigned long&&>(), "");

    static_assert(not rri_rrul.is_in_setof<unsigned int>(), "");
    static_assert(not rri_rrul.is_in_setof<signed long>(), "");
    static_assert(not rri_rrul.is_in_setof<const Foo&>(), "");
}
