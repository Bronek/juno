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
    static_assert(set<>::is_same_set<set<>>::value);
    static_assert(set<>::is_same<>::value);
    static_assert(set<>::empty::value);
    static_assert(set<>::size == 0);

    // void can be used to denote a non-element
    static_assert(set<void>::empty::value);
    static_assert(set<void>::size == 0);
    static_assert(set<void, void>::empty::value);
    static_assert(set<void, void>::size == 0);
    static_assert(set<>::is_same_set<set<void>>::value);
    static_assert(set<>::is_same_set<set<void, void>>::value);
    static_assert(set<>::is_same<void, void, void>::value);
    static_assert(set<void>::is_same_set<set<>>::value);
    static_assert(set<void>::is_same_set<set<void>>::value);
    static_assert(set<void, void>::is_same<>::value);
    static_assert(set<int, void>::is_same_set<set<int>>::value);
    static_assert(set<int, void>::size == 1);
    static_assert(not set<int, void>::empty::value);
    static_assert(set<int, void, void>::size == 1);
    static_assert(set<void, void, int, void, void>::size == 1);
    static_assert(set<int>::is_same<void, int, void>::value);
    static_assert(not set<int>::is_same_set<set<void>>::value);
    static_assert(not set<void>::is_same_set<set<int>>::value);

    // empty set can be found in any set
    static_assert(set<>::contains_set<set<>>::value);
    static_assert(set<void>::contains_set<set<>>::value);
    static_assert(set<void>::contains<>::value);
    static_assert(set<>::contains<void>::value);
    static_assert(set<>::contains_set<set<void, void>>::value);
    static_assert(set<>::contains<void, void>::value);
    static_assert(set<void, int>::contains_set<set<void>>::value);
    static_assert(set<void, int>::contains_set<set<>>::value);
    static_assert(set<Foo, int>::contains_set<set<>>::value);
    static_assert(set<int>::contains<>::value);
    static_assert(set<int, Baz>::contains<>::value);
    static_assert(set<int>::contains<void>::value);

    // for "intersects_set" operation, we are looking for a non-empty intersection of two sets
    static_assert(not set<>::intersects_set<set<>>::value);
    static_assert(not set<void>::intersects<>::value);
    static_assert(not set<>::intersects<int, long>::value);
    static_assert(not set<int, long>::intersects<>::value);
    static_assert(not set<int, long>::intersects<Foo, Bar>::value);
    static_assert(set<int, long>::intersects_set<set<Foo, Bar, long>>::value);
    static_assert(set<Fuz, long>::intersects<Fuz, Bar, int>::value);
    static_assert(set<Fuz, long, Bar>::intersects<Bar>::value);
    static_assert(not set<Fuz, long>::intersects_set<set<int, Foo, Bar>>::value);
    static_assert(not set<Fuz, long>::intersects_set<set<void>>::value);
    static_assert(not set<Fuz, long>::intersects<void, int>::value);
    static_assert(set<Fuz, long>::intersects<void, long>::value);

    // ordering of elements is ignored and duplicate elements are ignored
    static_assert(set<int, long>::is_same<int, long>::value);
    static_assert(set<int, long>::is_same<long, int>::value);
    static_assert(set<int, long>::is_same<int, long, int>::value);
    static_assert(set<int, long>::size == 2);
    static_assert(set<long, int, long>::is_same<int, long, int>::value);
    static_assert(set<long, int, long>::size == 2);
    static_assert(not set<long, int, long>::is_same<int>::value);
    static_assert(not set<long, int, long>::is_same<long, long>::value);
    static_assert(set<int, long, void>::is_same<long, long, int, int>::value);
    static_assert(set<int, long, void>::size == 2);

    // elements are stripped of reference and cv-qualifiers before any operation
    typedef set<Foo, Bar, int, const int, int&&> setA;
    static_assert(setA::is_same<Bar, Foo, int>::value);
    static_assert(setA::is_same<Foo, Bar, const int&>::value);
    static_assert(not setA::empty::value);
    static_assert(setA::size == 3);
    typedef set<const Foo, void, const volatile Fuz, Baz&> setB;
    static_assert(setB::is_same<Baz&&, Foo&&, Fuz&&>::value);
    static_assert(not setB::is_same_set<setA>::value);
    static_assert(not setB::empty::value);
    static_assert(setB::size == 3);
    static_assert(set<void, void, int, const int&&, void, void>::size == 1);
    static_assert(set<
            int, const int, volatile int, const volatile int
            , int&, const int&, volatile int&, const volatile int&
            , int&&, const int&&, volatile int&&, const volatile int&&
            , long, const long, volatile long, const volatile long
            , long&, const long&, volatile long&, const volatile long&
            , long&&, const long&&, volatile long&&, const volatile long&&>::size == 2);
    static_assert(set<
            int, const int, volatile int, const volatile int, void
            , int&, const int&, volatile int&, const volatile int&, void, void
            , int&&, const int&&, volatile int&&, const volatile int&&, void, void, void
            , long, const long, volatile long, const volatile long, void, void, void, void
            , long&, const long&, volatile long&, const volatile long&, void, void, void, void, void
            , long&&, const long&&, volatile long&&, const volatile long&&, void, void, void, void, void, void
    >::is_same_set<set<int, long>>::value);

    // "join_set" performs union of sets
    using setAB = typename setA::join_set<setB>::type;
    static_assert(setAB::is_same_set<set<int, Foo, Fuz, Baz, Bar>>::value);
    static_assert(setAB::is_same<Bar, Baz, int, Foo, Fuz>::value);
    static_assert(not setAB::empty::value);
    static_assert(setAB::size == 5);
    static_assert(not setAB::is_same<Foo>::value);
    static_assert(not setAB::is_same_set<setA>::value);
    static_assert(not setAB::is_same_set<setB>::value);
    static_assert(not setAB::is_same_set<set<Bar, Baz, int>>::value);
    static_assert(setAB::is_same_set<typename setA::join<Fuz, Baz>::type>::value);
    // union of sets is commutative
    static_assert(setAB::is_same_set<typename setB::join_set<setA>::type>::value);
    static_assert(setAB::is_same_set<typename setB::join<int, Bar, Foo>::type>::value);

    // more testing of join_set
    static_assert(set<int, long, unsigned long>::is_same_set<
            typename set<int, long>::join<unsigned long>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::join<long>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::join<int>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::join<int, long>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::join<void>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::join<>::type>::value);

    static_assert(set<void>::is_same_set<typename set<>::join_set<set<>>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::join_set<set<void>>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<>::join_set<set<const int, long&&>>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int>::join_set<set<int, long>>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::join_set<set<int, long>>::type>::value);
    static_assert(set<int, long>::is_same_set<typename set<int, long&&>::join_set<set<const int&>>::type>::value);

    // intersection of sets
    static_assert(set<>::is_same_set<typename set<>::cross_set<set<>>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::cross<>::type>::value);
    static_assert(set<>::is_same_set<typename set<int>::cross<>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::cross<int>::type>::value);
    static_assert(set<>::is_same_set<typename set<int, Foo>::cross<>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::cross<int, Foo>::type>::value);
    static_assert(set<>::is_same_set<typename set<int, Foo>::cross<Bar>::type>::value);
    static_assert(set<>::is_same_set<typename set<Bar>::cross<int, Foo>::type>::value);
    static_assert(set<Baz>::is_same_set<typename set<int, Foo, Baz>::cross<Bar, Baz>::type>::value);
    static_assert(set<Baz>::is_same_set<typename set<Bar, Baz>::cross<int, Foo, Baz>::type>::value);
    static_assert(set<Baz, Bar, int>::is_same_set<typename set<Baz, const Bar, int&>::cross<int&&, Baz, Bar>::type>::value);
    static_assert(set<Baz, Bar>::is_same_set<typename set<int, Baz, Foo, void, Bar>::cross<void, long, Fuz, Bar, Baz>::type>::value);

    // difference of sets
    static_assert(set<>::is_same_set<typename set<>::less_set<set<>>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::less<>::type>::value);
    static_assert(set<int>::is_same_set<typename set<int>::less<>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::less<int>::type>::value);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Foo>::less<>::type>::value);
    static_assert(set<>::is_same_set<typename set<>::less<int, Foo>::type>::value);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Foo>::less<Bar>::type>::value);
    static_assert(set<Bar>::is_same_set<typename set<Bar>::less<int, Foo>::type>::value);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Foo, Baz>::less<Bar, Baz>::type>::value);
    static_assert(set<Bar>::is_same_set<typename set<Bar, Baz>::less<int, Foo, Baz>::type>::value);
    static_assert(set<>::is_same_set<typename set<Baz, const Bar, int&>::less<int&&, Baz, Bar>::type>::value);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Baz, Foo, void, Bar>::less<void, long, Fuz, Bar, Baz>::type>::value);

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
    static_assert(set<>::is_same_set<set<void>>::value);
    static_assert(set<>::is_same_set<set<void, void>>::value);
    static_assert(set<>::is_same_set<set<void, void, void>>::value);
    static_assert(set<void>::is_same_set<set<>>::value);
    static_assert(set<void, void>::is_same_set<set<>>::value);
    static_assert(set<void, void, void>::is_same_set<set<>>::value);
    static_assert(set<void, void, void>::is_same_set<set<void>>::value);
    static_assert(set<void, void, void>::is_same_set<set<void, void>>::value);
    static_assert(set<void, void>::is_same_set<set<void, void>>::value);
    static_assert(set<void, void>::is_same_set<set<void>>::value);
    static_assert(set<int>::is_same_set<set<void, int, void>>::value);
    static_assert(set<void, int>::is_same_set<set<void, int, void>>::value);
    static_assert(set<int>::is_same_set<set<void, const int, void, volatile int>>::value);
    static_assert(set<int>::is_same_set<set<void, int&&, void, const int&, void, void>>::value);
    static_assert(set<int>::is_same_set<set<void, volatile int, void, void>>::value);
    static_assert(set<int>::is_same_set<set<void, void, void, const int&>>::value);
    static_assert(set<void, void, int, void, const int&&, void>::is_same_set<set<void, void, void, const int&>>::value);

    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void, void>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<int>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<long&&>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const unsigned long&>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<volatile long, int>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void, int&, long&>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long&&, const unsigned long&>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long&, unsigned long&&, int&&>>::value);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long, unsigned long, int, void, void>>::value);
    static_assert(set<int, long, unsigned long>::contains_set<set<const long&, unsigned long&&, int&&>>::value);
    static_assert(set<int, long, unsigned long>::contains_set<set<long, unsigned long, int>>::value);
    static_assert(not set<int, long, unsigned long>::contains_set<set<long, unsigned long, int, char>>::value);
    static_assert(not set<int, long, unsigned long>::contains_set<set<void, unsigned int, void>>::value);

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

    static_assert(decltype(i)::is_same_set<decltype(ci)>::value);
    static_assert(decltype(i)::is_same_set<decltype(vi)>::value);
    static_assert(decltype(i)::is_same_set<decltype(cvi)>::value);
    static_assert(decltype(i)::is_same_set<decltype(ri)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rci)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rvi)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rcvi)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rri)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rrci)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rrvi)>::value);
    static_assert(decltype(i)::is_same_set<decltype(rrcvi)>::value);
    static_assert(decltype(i)::contains_set<decltype(ci)>::value);
    static_assert(decltype(i)::contains_set<decltype(vi)>::value);
    static_assert(decltype(i)::contains_set<decltype(cvi)>::value);
    static_assert(decltype(i)::contains_set<decltype(ri)>::value);
    static_assert(decltype(i)::contains_set<decltype(rci)>::value);
    static_assert(decltype(i)::contains_set<decltype(rvi)>::value);
    static_assert(decltype(i)::contains_set<decltype(rcvi)>::value);
    static_assert(decltype(i)::contains_set<decltype(rri)>::value);
    static_assert(decltype(i)::contains_set<decltype(rrci)>::value);
    static_assert(decltype(i)::contains_set<decltype(rrvi)>::value);
    static_assert(decltype(i)::contains_set<decltype(rrcvi)>::value);

    static_assert(not decltype(i)::empty::value);
    static_assert(not decltype(ci)::empty::value);
    static_assert(not decltype(vi)::empty::value);
    static_assert(not decltype(cvi)::empty::value);
    static_assert(not decltype(ri)::empty::value);
    static_assert(not decltype(rci)::empty::value);
    static_assert(not decltype(rvi)::empty::value);
    static_assert(not decltype(rcvi)::empty::value);
    static_assert(not decltype(rri)::empty::value);
    static_assert(not decltype(rrci)::empty::value);
    static_assert(not decltype(rrvi)::empty::value);
    static_assert(not decltype(rrcvi)::empty::value);
    static_assert(i.size == 1);
    static_assert(ci.size == 1);
    static_assert(vi.size == 1);
    static_assert(cvi.size == 1);
    static_assert(ri.size == 1);
    static_assert(rci.size == 1);
    static_assert(rvi.size == 1);
    static_assert(rcvi.size == 1);
    static_assert(rri.size == 1);
    static_assert(rrci.size == 1);
    static_assert(rrvi.size == 1);
    static_assert(rrcvi.size == 1);

    static_assert(decltype(i)::contains<int>::value);
    static_assert(decltype(i)::contains<const int>::value);
    static_assert(decltype(i)::contains<volatile int>::value);
    static_assert(decltype(i)::contains<const volatile int>::value);
    static_assert(decltype(i)::contains<int&>::value);
    static_assert(decltype(i)::contains<const int&>::value);
    static_assert(decltype(i)::contains<volatile int&>::value);
    static_assert(decltype(i)::contains<const volatile int&>::value);
    static_assert(decltype(i)::contains<int&&>::value);
    static_assert(decltype(i)::contains<const int&&>::value);
    static_assert(decltype(i)::contains<volatile int&&>::value);
    static_assert(decltype(i)::contains<const volatile int&&>::value);
    static_assert(decltype(i)::contains<signed int>::value);
    static_assert(decltype(i)::contains<const signed int>::value);
    static_assert(decltype(i)::contains<volatile signed int>::value);
    static_assert(decltype(i)::contains<const volatile signed int>::value);
    static_assert(decltype(i)::contains<signed int&>::value);
    static_assert(decltype(i)::contains<const signed int&>::value);
    static_assert(decltype(i)::contains<volatile signed int&>::value);
    static_assert(decltype(i)::contains<const volatile signed int&>::value);
    static_assert(decltype(i)::contains<signed int&&>::value);
    static_assert(decltype(i)::contains<const signed int&&>::value);
    static_assert(decltype(i)::contains<volatile signed int&&>::value);
    static_assert(decltype(i)::contains<const volatile signed int&&>::value);
    static_assert(decltype(ci)::contains<int>::value);
    static_assert(decltype(ci)::contains<const int>::value);
    static_assert(decltype(ci)::contains<volatile int>::value);
    static_assert(decltype(ci)::contains<const volatile int>::value);
    static_assert(decltype(ci)::contains<int&>::value);
    static_assert(decltype(ci)::contains<const int&>::value);
    static_assert(decltype(ci)::contains<volatile int&>::value);
    static_assert(decltype(ci)::contains<const volatile int&>::value);
    static_assert(decltype(ci)::contains<int&&>::value);
    static_assert(decltype(ci)::contains<const int&&>::value);
    static_assert(decltype(ci)::contains<volatile int&&>::value);
    static_assert(decltype(ci)::contains<const volatile int&&>::value);
    static_assert(decltype(ci)::contains<signed int>::value);
    static_assert(decltype(ci)::contains<const signed int>::value);
    static_assert(decltype(ci)::contains<volatile signed int>::value);
    static_assert(decltype(ci)::contains<const volatile signed int>::value);
    static_assert(decltype(ci)::contains<signed int&>::value);
    static_assert(decltype(ci)::contains<const signed int&>::value);
    static_assert(decltype(ci)::contains<volatile signed int&>::value);
    static_assert(decltype(ci)::contains<const volatile signed int&>::value);
    static_assert(decltype(ci)::contains<signed int&&>::value);
    static_assert(decltype(ci)::contains<const signed int&&>::value);
    static_assert(decltype(ci)::contains<volatile signed int&&>::value);
    static_assert(decltype(ci)::contains<const volatile signed int&&>::value);
    static_assert(decltype(vi)::contains<int>::value);
    static_assert(decltype(vi)::contains<const int>::value);
    static_assert(decltype(vi)::contains<volatile int>::value);
    static_assert(decltype(vi)::contains<const volatile int>::value);
    static_assert(decltype(vi)::contains<int&>::value);
    static_assert(decltype(vi)::contains<const int&>::value);
    static_assert(decltype(vi)::contains<volatile int&>::value);
    static_assert(decltype(vi)::contains<const volatile int&>::value);
    static_assert(decltype(vi)::contains<int&&>::value);
    static_assert(decltype(vi)::contains<const int&&>::value);
    static_assert(decltype(vi)::contains<volatile int&&>::value);
    static_assert(decltype(vi)::contains<const volatile int&&>::value);
    static_assert(decltype(vi)::contains<signed int>::value);
    static_assert(decltype(vi)::contains<const signed int>::value);
    static_assert(decltype(vi)::contains<volatile signed int>::value);
    static_assert(decltype(vi)::contains<const volatile signed int>::value);
    static_assert(decltype(vi)::contains<signed int&>::value);
    static_assert(decltype(vi)::contains<const signed int&>::value);
    static_assert(decltype(vi)::contains<volatile signed int&>::value);
    static_assert(decltype(vi)::contains<const volatile signed int&>::value);
    static_assert(decltype(vi)::contains<signed int&&>::value);
    static_assert(decltype(vi)::contains<const signed int&&>::value);
    static_assert(decltype(vi)::contains<volatile signed int&&>::value);
    static_assert(decltype(vi)::contains<const volatile signed int&&>::value);
    static_assert(decltype(cvi)::contains<int>::value);
    static_assert(decltype(cvi)::contains<const int>::value);
    static_assert(decltype(cvi)::contains<volatile int>::value);
    static_assert(decltype(cvi)::contains<const volatile int>::value);
    static_assert(decltype(cvi)::contains<int&>::value);
    static_assert(decltype(cvi)::contains<const int&>::value);
    static_assert(decltype(cvi)::contains<volatile int&>::value);
    static_assert(decltype(cvi)::contains<const volatile int&>::value);
    static_assert(decltype(cvi)::contains<int&&>::value);
    static_assert(decltype(cvi)::contains<const int&&>::value);
    static_assert(decltype(cvi)::contains<volatile int&&>::value);
    static_assert(decltype(cvi)::contains<const volatile int&&>::value);
    static_assert(decltype(cvi)::contains<signed int>::value);
    static_assert(decltype(cvi)::contains<const signed int>::value);
    static_assert(decltype(cvi)::contains<volatile signed int>::value);
    static_assert(decltype(cvi)::contains<const volatile signed int>::value);
    static_assert(decltype(cvi)::contains<signed int&>::value);
    static_assert(decltype(cvi)::contains<const signed int&>::value);
    static_assert(decltype(cvi)::contains<volatile signed int&>::value);
    static_assert(decltype(cvi)::contains<const volatile signed int&>::value);
    static_assert(decltype(cvi)::contains<signed int&&>::value);
    static_assert(decltype(cvi)::contains<const signed int&&>::value);
    static_assert(decltype(cvi)::contains<volatile signed int&&>::value);
    static_assert(decltype(cvi)::contains<const volatile signed int&&>::value);
    static_assert(decltype(ri)::contains<int>::value);
    static_assert(decltype(ri)::contains<const int>::value);
    static_assert(decltype(ri)::contains<volatile int>::value);
    static_assert(decltype(ri)::contains<const volatile int>::value);
    static_assert(decltype(ri)::contains<int&>::value);
    static_assert(decltype(ri)::contains<const int&>::value);
    static_assert(decltype(ri)::contains<volatile int&>::value);
    static_assert(decltype(ri)::contains<const volatile int&>::value);
    static_assert(decltype(ri)::contains<int&&>::value);
    static_assert(decltype(ri)::contains<const int&&>::value);
    static_assert(decltype(ri)::contains<volatile int&&>::value);
    static_assert(decltype(ri)::contains<const volatile int&&>::value);
    static_assert(decltype(ri)::contains<signed int>::value);
    static_assert(decltype(ri)::contains<const signed int>::value);
    static_assert(decltype(ri)::contains<volatile signed int>::value);
    static_assert(decltype(ri)::contains<const volatile signed int>::value);
    static_assert(decltype(ri)::contains<signed int&>::value);
    static_assert(decltype(ri)::contains<const signed int&>::value);
    static_assert(decltype(ri)::contains<volatile signed int&>::value);
    static_assert(decltype(ri)::contains<const volatile signed int&>::value);
    static_assert(decltype(ri)::contains<signed int&&>::value);
    static_assert(decltype(ri)::contains<const signed int&&>::value);
    static_assert(decltype(ri)::contains<volatile signed int&&>::value);
    static_assert(decltype(ri)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rci)::contains<int>::value);
    static_assert(decltype(rci)::contains<const int>::value);
    static_assert(decltype(rci)::contains<volatile int>::value);
    static_assert(decltype(rci)::contains<const volatile int>::value);
    static_assert(decltype(rci)::contains<int&>::value);
    static_assert(decltype(rci)::contains<const int&>::value);
    static_assert(decltype(rci)::contains<volatile int&>::value);
    static_assert(decltype(rci)::contains<const volatile int&>::value);
    static_assert(decltype(rci)::contains<int&&>::value);
    static_assert(decltype(rci)::contains<const int&&>::value);
    static_assert(decltype(rci)::contains<volatile int&&>::value);
    static_assert(decltype(rci)::contains<const volatile int&&>::value);
    static_assert(decltype(rci)::contains<signed int>::value);
    static_assert(decltype(rci)::contains<const signed int>::value);
    static_assert(decltype(rci)::contains<volatile signed int>::value);
    static_assert(decltype(rci)::contains<const volatile signed int>::value);
    static_assert(decltype(rci)::contains<signed int&>::value);
    static_assert(decltype(rci)::contains<const signed int&>::value);
    static_assert(decltype(rci)::contains<volatile signed int&>::value);
    static_assert(decltype(rci)::contains<const volatile signed int&>::value);
    static_assert(decltype(rci)::contains<signed int&&>::value);
    static_assert(decltype(rci)::contains<const signed int&&>::value);
    static_assert(decltype(rci)::contains<volatile signed int&&>::value);
    static_assert(decltype(rci)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rvi)::contains<int>::value);
    static_assert(decltype(rvi)::contains<const int>::value);
    static_assert(decltype(rvi)::contains<volatile int>::value);
    static_assert(decltype(rvi)::contains<const volatile int>::value);
    static_assert(decltype(rvi)::contains<int&>::value);
    static_assert(decltype(rvi)::contains<const int&>::value);
    static_assert(decltype(rvi)::contains<volatile int&>::value);
    static_assert(decltype(rvi)::contains<const volatile int&>::value);
    static_assert(decltype(rvi)::contains<int&&>::value);
    static_assert(decltype(rvi)::contains<const int&&>::value);
    static_assert(decltype(rvi)::contains<volatile int&&>::value);
    static_assert(decltype(rvi)::contains<const volatile int&&>::value);
    static_assert(decltype(rvi)::contains<signed int>::value);
    static_assert(decltype(rvi)::contains<const signed int>::value);
    static_assert(decltype(rvi)::contains<volatile signed int>::value);
    static_assert(decltype(rvi)::contains<const volatile signed int>::value);
    static_assert(decltype(rvi)::contains<signed int&>::value);
    static_assert(decltype(rvi)::contains<const signed int&>::value);
    static_assert(decltype(rvi)::contains<volatile signed int&>::value);
    static_assert(decltype(rvi)::contains<const volatile signed int&>::value);
    static_assert(decltype(rvi)::contains<signed int&&>::value);
    static_assert(decltype(rvi)::contains<const signed int&&>::value);
    static_assert(decltype(rvi)::contains<volatile signed int&&>::value);
    static_assert(decltype(rvi)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rcvi)::contains<int>::value);
    static_assert(decltype(rcvi)::contains<const int>::value);
    static_assert(decltype(rcvi)::contains<volatile int>::value);
    static_assert(decltype(rcvi)::contains<const volatile int>::value);
    static_assert(decltype(rcvi)::contains<int&>::value);
    static_assert(decltype(rcvi)::contains<const int&>::value);
    static_assert(decltype(rcvi)::contains<volatile int&>::value);
    static_assert(decltype(rcvi)::contains<const volatile int&>::value);
    static_assert(decltype(rcvi)::contains<int&&>::value);
    static_assert(decltype(rcvi)::contains<const int&&>::value);
    static_assert(decltype(rcvi)::contains<volatile int&&>::value);
    static_assert(decltype(rcvi)::contains<const volatile int&&>::value);
    static_assert(decltype(rcvi)::contains<signed int>::value);
    static_assert(decltype(rcvi)::contains<const signed int>::value);
    static_assert(decltype(rcvi)::contains<volatile signed int>::value);
    static_assert(decltype(rcvi)::contains<const volatile signed int>::value);
    static_assert(decltype(rcvi)::contains<signed int&>::value);
    static_assert(decltype(rcvi)::contains<const signed int&>::value);
    static_assert(decltype(rcvi)::contains<volatile signed int&>::value);
    static_assert(decltype(rcvi)::contains<const volatile signed int&>::value);
    static_assert(decltype(rcvi)::contains<signed int&&>::value);
    static_assert(decltype(rcvi)::contains<const signed int&&>::value);
    static_assert(decltype(rcvi)::contains<volatile signed int&&>::value);
    static_assert(decltype(rcvi)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rri)::contains<int>::value);
    static_assert(decltype(rri)::contains<const int>::value);
    static_assert(decltype(rri)::contains<volatile int>::value);
    static_assert(decltype(rri)::contains<const volatile int>::value);
    static_assert(decltype(rri)::contains<int&>::value);
    static_assert(decltype(rri)::contains<const int&>::value);
    static_assert(decltype(rri)::contains<volatile int&>::value);
    static_assert(decltype(rri)::contains<const volatile int&>::value);
    static_assert(decltype(rri)::contains<int&&>::value);
    static_assert(decltype(rri)::contains<const int&&>::value);
    static_assert(decltype(rri)::contains<volatile int&&>::value);
    static_assert(decltype(rri)::contains<const volatile int&&>::value);
    static_assert(decltype(rri)::contains<signed int>::value);
    static_assert(decltype(rri)::contains<const signed int>::value);
    static_assert(decltype(rri)::contains<volatile signed int>::value);
    static_assert(decltype(rri)::contains<const volatile signed int>::value);
    static_assert(decltype(rri)::contains<signed int&>::value);
    static_assert(decltype(rri)::contains<const signed int&>::value);
    static_assert(decltype(rri)::contains<volatile signed int&>::value);
    static_assert(decltype(rri)::contains<const volatile signed int&>::value);
    static_assert(decltype(rri)::contains<signed int&&>::value);
    static_assert(decltype(rri)::contains<const signed int&&>::value);
    static_assert(decltype(rri)::contains<volatile signed int&&>::value);
    static_assert(decltype(rri)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rrci)::contains<int>::value);
    static_assert(decltype(rrci)::contains<const int>::value);
    static_assert(decltype(rrci)::contains<volatile int>::value);
    static_assert(decltype(rrci)::contains<const volatile int>::value);
    static_assert(decltype(rrci)::contains<int&>::value);
    static_assert(decltype(rrci)::contains<const int&>::value);
    static_assert(decltype(rrci)::contains<volatile int&>::value);
    static_assert(decltype(rrci)::contains<const volatile int&>::value);
    static_assert(decltype(rrci)::contains<int&&>::value);
    static_assert(decltype(rrci)::contains<const int&&>::value);
    static_assert(decltype(rrci)::contains<volatile int&&>::value);
    static_assert(decltype(rrci)::contains<const volatile int&&>::value);
    static_assert(decltype(rrci)::contains<signed int>::value);
    static_assert(decltype(rrci)::contains<const signed int>::value);
    static_assert(decltype(rrci)::contains<volatile signed int>::value);
    static_assert(decltype(rrci)::contains<const volatile signed int>::value);
    static_assert(decltype(rrci)::contains<signed int&>::value);
    static_assert(decltype(rrci)::contains<const signed int&>::value);
    static_assert(decltype(rrci)::contains<volatile signed int&>::value);
    static_assert(decltype(rrci)::contains<const volatile signed int&>::value);
    static_assert(decltype(rrci)::contains<signed int&&>::value);
    static_assert(decltype(rrci)::contains<const signed int&&>::value);
    static_assert(decltype(rrci)::contains<volatile signed int&&>::value);
    static_assert(decltype(rrci)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rrvi)::contains<int>::value);
    static_assert(decltype(rrvi)::contains<const int>::value);
    static_assert(decltype(rrvi)::contains<volatile int>::value);
    static_assert(decltype(rrvi)::contains<const volatile int>::value);
    static_assert(decltype(rrvi)::contains<int&>::value);
    static_assert(decltype(rrvi)::contains<const int&>::value);
    static_assert(decltype(rrvi)::contains<volatile int&>::value);
    static_assert(decltype(rrvi)::contains<const volatile int&>::value);
    static_assert(decltype(rrvi)::contains<int&&>::value);
    static_assert(decltype(rrvi)::contains<const int&&>::value);
    static_assert(decltype(rrvi)::contains<volatile int&&>::value);
    static_assert(decltype(rrvi)::contains<const volatile int&&>::value);
    static_assert(decltype(rrvi)::contains<signed int>::value);
    static_assert(decltype(rrvi)::contains<const signed int>::value);
    static_assert(decltype(rrvi)::contains<volatile signed int>::value);
    static_assert(decltype(rrvi)::contains<const volatile signed int>::value);
    static_assert(decltype(rrvi)::contains<signed int&>::value);
    static_assert(decltype(rrvi)::contains<const signed int&>::value);
    static_assert(decltype(rrvi)::contains<volatile signed int&>::value);
    static_assert(decltype(rrvi)::contains<const volatile signed int&>::value);
    static_assert(decltype(rrvi)::contains<signed int&&>::value);
    static_assert(decltype(rrvi)::contains<const signed int&&>::value);
    static_assert(decltype(rrvi)::contains<volatile signed int&&>::value);
    static_assert(decltype(rrvi)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rrcvi)::contains<int>::value);
    static_assert(decltype(rrcvi)::contains<const int>::value);
    static_assert(decltype(rrcvi)::contains<volatile int>::value);
    static_assert(decltype(rrcvi)::contains<const volatile int>::value);
    static_assert(decltype(rrcvi)::contains<int&>::value);
    static_assert(decltype(rrcvi)::contains<const int&>::value);
    static_assert(decltype(rrcvi)::contains<volatile int&>::value);
    static_assert(decltype(rrcvi)::contains<const volatile int&>::value);
    static_assert(decltype(rrcvi)::contains<int&&>::value);
    static_assert(decltype(rrcvi)::contains<const int&&>::value);
    static_assert(decltype(rrcvi)::contains<volatile int&&>::value);
    static_assert(decltype(rrcvi)::contains<const volatile int&&>::value);
    static_assert(decltype(rrcvi)::contains<signed int>::value);
    static_assert(decltype(rrcvi)::contains<const signed int>::value);
    static_assert(decltype(rrcvi)::contains<volatile signed int>::value);
    static_assert(decltype(rrcvi)::contains<const volatile signed int>::value);
    static_assert(decltype(rrcvi)::contains<signed int&>::value);
    static_assert(decltype(rrcvi)::contains<const signed int&>::value);
    static_assert(decltype(rrcvi)::contains<volatile signed int&>::value);
    static_assert(decltype(rrcvi)::contains<const volatile signed int&>::value);
    static_assert(decltype(rrcvi)::contains<signed int&&>::value);
    static_assert(decltype(rrcvi)::contains<const signed int&&>::value);
    static_assert(decltype(rrcvi)::contains<volatile signed int&&>::value);
    static_assert(decltype(rrcvi)::contains<const volatile signed int&&>::value);

    static_assert(not decltype(i)::contains<long>::value);
    static_assert(not decltype(ci)::contains<long>::value);
    static_assert(not decltype(i)::contains<unsigned int>::value);
    static_assert(not decltype(rci)::contains<unsigned int>::value);
    struct Foo;
    static_assert(not decltype(i)::contains<Foo>::value);
    static_assert(not decltype(rrvi)::contains<Foo>::value);

    // all of these are the same set_impl<int, long>, except for ..
    constexpr auto rci_rrl = set<const int&, long&&>();
    constexpr auto i_cl = set<int, const long>();
    constexpr auto i_rrl = set<int, long&&>();
    constexpr auto i_rsl = set<int, signed long&>();
    constexpr auto ri_rrcsl = set<int&, const signed long&&>();
    constexpr auto rri_rrsl = set<int&&, signed long&&>();
    // ... the one here, which is set_impl<int, unsigned long>
    constexpr auto rri_rrul = set<int&&, unsigned long&&>();

    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_cl)>::value);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_rrl)>::value);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_rsl)>::value);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(ri_rrcsl)>::value);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(rri_rrsl)>::value);
    static_assert(not decltype(rri_rrsl)::is_same_set<decltype(rri_rrul)>::value);

    static_assert(not decltype(rci_rrl)::empty::value);
    static_assert(not decltype(i_cl)::empty::value);
    static_assert(not decltype(i_rrl)::empty::value);
    static_assert(not decltype(i_rsl)::empty::value);
    static_assert(not decltype(ri_rrcsl)::empty::value);
    static_assert(not decltype(rri_rrsl)::empty::value);
    static_assert(not decltype(rri_rrul)::empty::value);
    static_assert(rci_rrl.size == 2);
    static_assert(i_cl.size == 2);
    static_assert(i_rrl.size == 2);
    static_assert(i_rsl.size == 2);
    static_assert(ri_rrcsl.size == 2);
    static_assert(rri_rrsl.size == 2);
    static_assert(rri_rrul.size == 2);

    static_assert(decltype(rci_rrl)::contains<int>::value);
    static_assert(decltype(rci_rrl)::contains<const int>::value);
    static_assert(decltype(rci_rrl)::contains<volatile int>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile int>::value);
    static_assert(decltype(rci_rrl)::contains<int&>::value);
    static_assert(decltype(rci_rrl)::contains<const int&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile int&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile int&>::value);
    static_assert(decltype(rci_rrl)::contains<int&&>::value);
    static_assert(decltype(rci_rrl)::contains<const int&&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile int&&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile int&&>::value);
    static_assert(decltype(rci_rrl)::contains<signed int>::value);
    static_assert(decltype(rci_rrl)::contains<const signed int>::value);
    static_assert(decltype(rci_rrl)::contains<volatile signed int>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile signed int>::value);
    static_assert(decltype(rci_rrl)::contains<signed int&>::value);
    static_assert(decltype(rci_rrl)::contains<const signed int&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile signed int&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile signed int&>::value);
    static_assert(decltype(rci_rrl)::contains<signed int&&>::value);
    static_assert(decltype(rci_rrl)::contains<const signed int&&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile signed int&&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile signed int&&>::value);
    static_assert(decltype(rci_rrl)::contains<long>::value);
    static_assert(decltype(rci_rrl)::contains<const long>::value);
    static_assert(decltype(rci_rrl)::contains<volatile long>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile long>::value);
    static_assert(decltype(rci_rrl)::contains<long&>::value);
    static_assert(decltype(rci_rrl)::contains<const long&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile long&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile long&>::value);
    static_assert(decltype(rci_rrl)::contains<long&&>::value);
    static_assert(decltype(rci_rrl)::contains<const long&&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile long&&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile long&&>::value);
    static_assert(decltype(rci_rrl)::contains<signed long>::value);
    static_assert(decltype(rci_rrl)::contains<const signed long>::value);
    static_assert(decltype(rci_rrl)::contains<volatile signed long>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile signed long>::value);
    static_assert(decltype(rci_rrl)::contains<signed long&>::value);
    static_assert(decltype(rci_rrl)::contains<const signed long&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile signed long&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile signed long&>::value);
    static_assert(decltype(rci_rrl)::contains<signed long&&>::value);
    static_assert(decltype(rci_rrl)::contains<const signed long&&>::value);
    static_assert(decltype(rci_rrl)::contains<volatile signed long&&>::value);
    static_assert(decltype(rci_rrl)::contains<const volatile signed long&&>::value);
    static_assert(decltype(i_cl)::contains<long>::value);
    static_assert(decltype(i_cl)::contains<const long>::value);
    static_assert(decltype(i_cl)::contains<volatile long>::value);
    static_assert(decltype(i_cl)::contains<const volatile long>::value);
    static_assert(decltype(i_cl)::contains<long&>::value);
    static_assert(decltype(i_cl)::contains<const long&>::value);
    static_assert(decltype(i_cl)::contains<volatile long&>::value);
    static_assert(decltype(i_cl)::contains<const volatile long&>::value);
    static_assert(decltype(i_cl)::contains<long&&>::value);
    static_assert(decltype(i_cl)::contains<const long&&>::value);
    static_assert(decltype(i_cl)::contains<volatile long&&>::value);
    static_assert(decltype(i_cl)::contains<const volatile long&&>::value);
    static_assert(decltype(i_cl)::contains<signed long>::value);
    static_assert(decltype(i_cl)::contains<const signed long>::value);
    static_assert(decltype(i_cl)::contains<volatile signed long>::value);
    static_assert(decltype(i_cl)::contains<const volatile signed long>::value);
    static_assert(decltype(i_cl)::contains<signed long&>::value);
    static_assert(decltype(i_cl)::contains<const signed long&>::value);
    static_assert(decltype(i_cl)::contains<volatile signed long&>::value);
    static_assert(decltype(i_cl)::contains<const volatile signed long&>::value);
    static_assert(decltype(i_cl)::contains<signed long&&>::value);
    static_assert(decltype(i_cl)::contains<const signed long&&>::value);
    static_assert(decltype(i_cl)::contains<volatile signed long&&>::value);
    static_assert(decltype(i_cl)::contains<const volatile signed long&&>::value);

    static_assert(not decltype(rci_rrl)::contains<unsigned int>::value);
    static_assert(not decltype(rci_rrl)::contains<Foo>::value);

    static_assert(decltype(rri_rrul)::contains<int>::value);
    static_assert(decltype(rri_rrul)::contains<int&&>::value);
    static_assert(decltype(rri_rrul)::contains<const int>::value);
    static_assert(decltype(rri_rrul)::contains<unsigned long>::value);
    static_assert(decltype(rri_rrul)::contains<const unsigned long>::value);
    static_assert(decltype(rri_rrul)::contains<volatile unsigned long>::value);
    static_assert(decltype(rri_rrul)::contains<const volatile unsigned long>::value);
    static_assert(decltype(rri_rrul)::contains<unsigned long&>::value);
    static_assert(decltype(rri_rrul)::contains<const unsigned long&>::value);
    static_assert(decltype(rri_rrul)::contains<volatile unsigned long&>::value);
    static_assert(decltype(rri_rrul)::contains<const volatile unsigned long&>::value);
    static_assert(decltype(rri_rrul)::contains<unsigned long&&>::value);
    static_assert(decltype(rri_rrul)::contains<const unsigned long&&>::value);
    static_assert(decltype(rri_rrul)::contains<volatile unsigned long&&>::value);
    static_assert(decltype(rri_rrul)::contains<const volatile unsigned long&&>::value);

    static_assert(not decltype(rri_rrul)::contains<unsigned int>::value);
    static_assert(not decltype(rri_rrul)::contains<signed long>::value);
    static_assert(not decltype(rri_rrul)::contains<const Foo&>::value);
}
