// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <gtest/gtest.h>

#include <type_traits>

#include <set.hpp>
#include <tag.hpp>

TEST(set, type_set__basic)
{
    // sanity checks of basic use cases
    using namespace juno;

    // incomplete types are OK
    struct Foo; struct Bar; struct Fuz; union Baz;

    // empty set_impl is an empty set_impl
    static_assert(set<>::is_same_set<set<>>);
    static_assert(set<>::is_same<>);
    static_assert(set<>::empty);
    static_assert(set<>::size == 0);

    // void can be used to denote a non-element
    static_assert(set<void>::empty);
    static_assert(set<void>::size == 0);
    static_assert(set<void, void>::empty);
    static_assert(set<void, void>::size == 0);
    static_assert(set<>::is_same_set<set<void>>);
    static_assert(set<>::is_same_set<set<void, void>>);
    static_assert(set<>::is_same<void, void, void>);
    static_assert(set<void>::is_same_set<set<>>);
    static_assert(set<void>::is_same_set<set<void>>);
    static_assert(set<void, void>::is_same<>);
    static_assert(set<int, void>::is_same_set<set<int>>);
    static_assert(set<int, void>::size == 1);
    static_assert(not set<int, void>::empty);
    static_assert(set<int, void, void>::size == 1);
    static_assert(set<void, void, int, void, void>::size == 1);
    static_assert(set<int>::is_same<void, int, void>);
    static_assert(not set<int>::is_same_set<set<void>>);
    static_assert(not set<void>::is_same_set<set<int>>);

    // empty set can be found in any set
    static_assert(set<>::contains_set<set<>>);
    static_assert(set<void>::contains_set<set<>>);
    static_assert(set<void>::contains<>);
    static_assert(set<>::contains<void>);
    static_assert(set<>::contains_set<set<void, void>>);
    static_assert(set<>::contains<void, void>);
    static_assert(set<void, int>::contains_set<set<void>>);
    static_assert(set<void, int>::contains_set<set<>>);
    static_assert(set<Foo, int>::contains_set<set<>>);
    static_assert(set<int>::contains<>);
    static_assert(set<int, Baz>::contains<>);
    static_assert(set<int>::contains<void>);

    // for "intersects_set" operation, we are looking for a non-empty intersection of two sets
    static_assert(not set<>::intersects_set<set<>>);
    static_assert(not set<void>::intersects<>);
    static_assert(not set<>::intersects<int, long>);
    static_assert(not set<int, long>::intersects<>);
    static_assert(not set<int, long>::intersects<Foo, Bar>);
    static_assert(set<int, long>::intersects_set<set<Foo, Bar, long>>);
    static_assert(set<Fuz, long>::intersects<Fuz, Bar, int>);
    static_assert(set<Fuz, long, Bar>::intersects<Bar>);
    static_assert(not set<Fuz, long>::intersects_set<set<int, Foo, Bar>>);
    static_assert(not set<Fuz, long>::intersects_set<set<void>>);
    static_assert(not set<Fuz, long>::intersects<void, int>);
    static_assert(set<Fuz, long>::intersects<void, long>);

    // ordering of elements is ignored and duplicate elements are ignored
    static_assert(set<int, long>::is_same<int, long>);
    static_assert(set<int, long>::is_same<long, int>);
    static_assert(set<int, long>::is_same<int, long, int>);
    static_assert(set<int, long>::size == 2);
    static_assert(set<long, int, long>::is_same<int, long, int>);
    static_assert(set<long, int, long>::size == 2);
    static_assert(not set<long, int, long>::is_same<int>);
    static_assert(not set<long, int, long>::is_same<long, long>);
    static_assert(set<int, long, void>::is_same<long, long, int, int>);
    static_assert(set<int, long, void>::size == 2);

    // elements are stripped of reference and cv-qualifiers before any operation
    typedef set<Foo, Bar, int, const int, int&&> setA;
    static_assert(setA::is_same<Bar, Foo, int>);
    static_assert(setA::is_same<Foo, Bar, const int&>);
    static_assert(not setA::empty);
    static_assert(setA::size == 3);
    typedef set<const Foo, void, const volatile Fuz, Baz&> setB;
    static_assert(setB::is_same<Baz&&, Foo&&, Fuz&&>);
    static_assert(not setB::is_same_set<setA>);
    static_assert(not setB::empty);
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
    >::is_same_set<set<int, long>>);

    // "join_set" performs union of sets
    using setAB = typename setA::insert_set<setB>::type;
    static_assert(setAB::is_same_set<set<int, Foo, Fuz, Baz, Bar>>);
    static_assert(setAB::is_same<Bar, Baz, int, Foo, Fuz>);
    static_assert(not setAB::empty);
    static_assert(setAB::size == 5);
    static_assert(not setAB::is_same<Foo>);
    static_assert(not setAB::is_same_set<setA>);
    static_assert(not setAB::is_same_set<setB>);
    static_assert(not setAB::is_same_set<set<Bar, Baz, int>>);
    static_assert(setAB::is_same_set<typename setA::insert<Fuz, Baz>::type>);
    // union of sets is commutative
    static_assert(setAB::is_same_set<typename setB::insert_set<setA>::type>);
    static_assert(setAB::is_same_set<typename setB::insert<int, Bar, Foo>::type>);

    // more testing of insert_set
    static_assert(set<int, long, unsigned long>::is_same_set<
            typename set<int, long>::insert<unsigned long>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::insert<long>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::insert<int>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::insert<int, long>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::insert<void>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::insert<>::type>);

    static_assert(set<void>::is_same_set<typename set<>::insert_set<set<>>::type>);
    static_assert(set<>::is_same_set<typename set<>::insert_set<set<void>>::type>);
    static_assert(set<int, long>::is_same_set<typename set<>::insert_set<set<const int, long&&>>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int>::insert_set<set<int, long>>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long>::insert_set<set<int, long>>::type>);
    static_assert(set<int, long>::is_same_set<typename set<int, long&&>::insert_set<set<const int&>>::type>);

    // intersection of sets
    static_assert(set<>::is_same_set<typename set<>::cross_set<set<>>::type>);
    static_assert(set<>::is_same_set<typename set<>::cross<>::type>);
    static_assert(set<>::is_same_set<typename set<int>::cross<>::type>);
    static_assert(set<>::is_same_set<typename set<>::cross<int>::type>);
    static_assert(set<>::is_same_set<typename set<int, Foo>::cross<>::type>);
    static_assert(set<>::is_same_set<typename set<>::cross<int, Foo>::type>);
    static_assert(set<>::is_same_set<typename set<int, Foo>::cross<Bar>::type>);
    static_assert(set<>::is_same_set<typename set<Bar>::cross<int, Foo>::type>);
    static_assert(set<Baz>::is_same_set<typename set<int, Foo, Baz>::cross<Bar, Baz>::type>);
    static_assert(set<Baz>::is_same_set<typename set<Bar, Baz>::cross<int, Foo, Baz>::type>);
    static_assert(set<Baz, Bar, int>::is_same_set<typename set<Baz, const Bar, int&>::cross<int&&, Baz, Bar>::type>);
    static_assert(set<Baz, Bar>::is_same_set<typename set<int, Baz, Foo, void, Bar>::cross<void, long, Fuz, Bar, Baz>::type>);

    // difference of sets
    static_assert(set<>::is_same_set<typename set<>::remove_set<set<>>::type>);
    static_assert(set<>::is_same_set<typename set<>::remove<>::type>);
    static_assert(set<int>::is_same_set<typename set<int>::remove<>::type>);
    static_assert(set<>::is_same_set<typename set<>::remove<int>::type>);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Foo>::remove<>::type>);
    static_assert(set<>::is_same_set<typename set<>::remove<int, Foo>::type>);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Foo>::remove<Bar>::type>);
    static_assert(set<Bar>::is_same_set<typename set<Bar>::remove<int, Foo>::type>);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Foo, Baz>::remove<Bar, Baz>::type>);
    static_assert(set<Bar>::is_same_set<typename set<Bar, Baz>::remove<int, Foo, Baz>::type>);
    static_assert(set<>::is_same_set<typename set<Baz, const Bar, int&>::remove<int&&, Baz, Bar>::type>);
    static_assert(set<int, Foo>::is_same_set<typename set<int, Baz, Foo, void, Bar>::remove<void, long, Fuz, Bar, Baz>::type>);

    SUCCEED();
}

TEST(set, type_set__for_each)
{
    const auto fn1 = [](auto*  , int& i) -> bool { ++i; return true; };

    using namespace juno;
    int count = 0;
    ASSERT_TRUE((set<>::for_each(fn1, count)));
    ASSERT_TRUE((set<void>::for_each(fn1, count)));
    ASSERT_TRUE((set<void, void>::for_each(fn1, count)));
    ASSERT_EQ(count, 0);
    ASSERT_TRUE((set<int, void, void>::for_each(fn1, count)));
    ASSERT_EQ(count, 1);
    count = 0;
    ASSERT_TRUE((set<int, long>::for_each(fn1, count)));
    ASSERT_EQ(count, 2);
    count = 0;
    ASSERT_TRUE((set<int, long, const int>::for_each(fn1, count)));
    ASSERT_EQ(count, 2); // "const int" is same as "int", see ref_cv_stripping below

    std::set<size_t> tags;
    auto fn2 = [](auto* p, std::set<size_t>& d) -> bool {
        if (not d.insert(tag<typename std::remove_pointer<decltype(p)>::type>::value).second)
            throw std::runtime_error("unexpected duplicate");
        return true;
    };
    class Foo; class Bar;
    ASSERT_TRUE((set<int, int, Bar&, long, Foo&&, long, const Bar, const Foo&>::for_each(std::move(fn2), tags)));
    ASSERT_EQ(tags.size(), 4);
    ASSERT_EQ(tags.count(tag<int>::value), 1);
    ASSERT_EQ(tags.count(tag<long>::value), 1);
    ASSERT_EQ(tags.count(tag<Foo>::value), 1);
    ASSERT_EQ(tags.count(tag<Bar>::value), 1);
    ASSERT_EQ(tags.count(tag<void*>::value), 0);

    int c = 0;
    auto fn3 = [&c](auto* ) -> bool {
        // Visit 3 types (counting from 0), then abort
        return (++c <= 2);
    };

    ASSERT_FALSE((set<Foo, Bar, int, double>::for_each(fn3)));
    ASSERT_EQ(c, 3);
}

TEST(set, type_set__details_unique)
{
    // test that duplicate elements are ignored and void is used to denote non-element
    // these are implementation details of set_impl::unique which itself is implementation
    // detail of set
    using namespace juno;
    struct Foo; struct Bar;

    // can't find actual type in an empty list
    static_assert(not impl_set::contains<int>::value);
    static_assert(not impl_set::contains<Foo>::value);
    static_assert(not impl_set::contains<int, void>::value);
    static_assert(not impl_set::contains<Foo, void, void>::value);
    static_assert(not impl_set::contains<Foo, void, void, void>::value);

    // can find void, i.e. no-element, in any list including empty one
    static_assert(impl_set::contains<void>::value);
    static_assert(impl_set::contains<void, void>::value);
    static_assert(impl_set::contains<void, Foo>::value);
    static_assert(impl_set::contains<void, Foo, void, bool>::value);
    static_assert(impl_set::contains<void, Foo, Bar, int, bool>::value);

    // look for actual type where we expect to find it
    static_assert(impl_set::contains<int, int>::value);
    static_assert(impl_set::contains<int, int, void>::value);
    static_assert(impl_set::contains<int, void, int>::value);
    static_assert(impl_set::contains<int, void, int, void>::value);
    static_assert(impl_set::contains<int, void, void, int>::value);
    static_assert(impl_set::contains<int, int, long, Foo>::value);
    static_assert(impl_set::contains<int, Foo, Bar, long, int>::value);
    static_assert(impl_set::contains<int, Foo, void, void, int, void>::value);

    // look for actual type where we do not expect to find it
    static_assert(not impl_set::contains<Bar, int>::value);
    static_assert(not impl_set::contains<Bar, int, void>::value);
    static_assert(not impl_set::contains<Bar, void, int>::value);
    static_assert(not impl_set::contains<Bar, void, int, void>::value);
    static_assert(not impl_set::contains<Bar, void, void, int>::value);
    static_assert(not impl_set::contains<Bar, int, long, Foo>::value);
    static_assert(not impl_set::contains<Bar, Foo, bool, long, int>::value);
    static_assert(not impl_set::contains<Bar, Foo, void, void, int, void>::value);

    // unique means no duplicate types, no voids
    static_assert(std::is_same<
            impl_set::set_impl<>::unique
            , impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void>::unique
            , impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void, void>::unique
            , impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void, void, void>::unique
            , impl_set::set_impl<>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<int>::unique
            , impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<int, void, int>::unique
            , impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<int, int, int>::unique
            , impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void, int, int, int, int, void, void>::unique
            , impl_set::set_impl<int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<long, int, void>::unique
            , impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<long, int, int>::unique
            , impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<long, int, int, void, void, int>::unique
            , impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void, int, int, long>::unique
            , impl_set::set_impl<int, long>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<long, int, int, long, int>::unique
            , impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<int, int, long, long, long ,int, int, int, int>::unique
            , impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void, void, long, int, void, int, long, long, long, int, int, long, int>::unique
            , impl_set::set_impl<long, int>>::value);
    static_assert(std::is_same<
            impl_set::set_impl<void, long, void, int, long, int, int, long, int, long, long, long, long, void>::unique
            , impl_set::set_impl<int, long>>::value);

    SUCCEED();
}

TEST(set, type_set__extensive__void_and_ref_cv_stripping)
{
    using namespace juno;

    // test removal of duplicate types and "void" from set
    static_assert(set<>::is_same_set<set<void>>);
    static_assert(set<>::is_same_set<set<void, void>>);
    static_assert(set<>::is_same_set<set<void, void, void>>);
    static_assert(set<void>::is_same_set<set<>>);
    static_assert(set<void, void>::is_same_set<set<>>);
    static_assert(set<void, void, void>::is_same_set<set<>>);
    static_assert(set<void, void, void>::is_same_set<set<void>>);
    static_assert(set<void, void, void>::is_same_set<set<void, void>>);
    static_assert(set<void, void>::is_same_set<set<void, void>>);
    static_assert(set<void, void>::is_same_set<set<void>>);
    static_assert(set<int>::is_same_set<set<void, int, void>>);
    static_assert(set<void, int>::is_same_set<set<void, int, void>>);
    static_assert(set<int>::is_same_set<set<void, const int, void, volatile int>>);
    static_assert(set<int>::is_same_set<set<void, int&&, void, const int&, void, void>>);
    static_assert(set<int>::is_same_set<set<void, volatile int, void, void>>);
    static_assert(set<int>::is_same_set<set<void, void, void, const int&>>);
    static_assert(set<void, void, int, void, const int&&, void>::is_same_set<set<void, void, void, const int&>>);

    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void, void>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<int>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<long&&>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const unsigned long&>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<volatile long, int>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<void, int&, long&>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long&&, const unsigned long&>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long&, unsigned long&&, int&&>>);
    static_assert(set<const int, const long, unsigned long&&>::contains_set<set<const long, unsigned long, int, void, void>>);
    static_assert(set<int, long, unsigned long>::contains_set<set<const long&, unsigned long&&, int&&>>);
    static_assert(set<int, long, unsigned long>::contains_set<set<long, unsigned long, int>>);
    static_assert(not set<int, long, unsigned long>::contains_set<set<long, unsigned long, int, char>>);
    static_assert(not set<int, long, unsigned long>::contains_set<set<void, unsigned int, void>>);

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

    static_assert(decltype(i)::is_same_set<decltype(ci)>);
    static_assert(decltype(i)::is_same_set<decltype(vi)>);
    static_assert(decltype(i)::is_same_set<decltype(cvi)>);
    static_assert(decltype(i)::is_same_set<decltype(ri)>);
    static_assert(decltype(i)::is_same_set<decltype(rci)>);
    static_assert(decltype(i)::is_same_set<decltype(rvi)>);
    static_assert(decltype(i)::is_same_set<decltype(rcvi)>);
    static_assert(decltype(i)::is_same_set<decltype(rri)>);
    static_assert(decltype(i)::is_same_set<decltype(rrci)>);
    static_assert(decltype(i)::is_same_set<decltype(rrvi)>);
    static_assert(decltype(i)::is_same_set<decltype(rrcvi)>);
    static_assert(decltype(i)::contains_set<decltype(ci)>);
    static_assert(decltype(i)::contains_set<decltype(vi)>);
    static_assert(decltype(i)::contains_set<decltype(cvi)>);
    static_assert(decltype(i)::contains_set<decltype(ri)>);
    static_assert(decltype(i)::contains_set<decltype(rci)>);
    static_assert(decltype(i)::contains_set<decltype(rvi)>);
    static_assert(decltype(i)::contains_set<decltype(rcvi)>);
    static_assert(decltype(i)::contains_set<decltype(rri)>);
    static_assert(decltype(i)::contains_set<decltype(rrci)>);
    static_assert(decltype(i)::contains_set<decltype(rrvi)>);
    static_assert(decltype(i)::contains_set<decltype(rrcvi)>);

    static_assert(not decltype(i)::empty);
    static_assert(not decltype(ci)::empty);
    static_assert(not decltype(vi)::empty);
    static_assert(not decltype(cvi)::empty);
    static_assert(not decltype(ri)::empty);
    static_assert(not decltype(rci)::empty);
    static_assert(not decltype(rvi)::empty);
    static_assert(not decltype(rcvi)::empty);
    static_assert(not decltype(rri)::empty);
    static_assert(not decltype(rrci)::empty);
    static_assert(not decltype(rrvi)::empty);
    static_assert(not decltype(rrcvi)::empty);
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

    static_assert(decltype(i)::contains<int>);
    static_assert(decltype(i)::contains<const int>);
    static_assert(decltype(i)::contains<volatile int>);
    static_assert(decltype(i)::contains<const volatile int>);
    static_assert(decltype(i)::contains<int&>);
    static_assert(decltype(i)::contains<const int&>);
    static_assert(decltype(i)::contains<volatile int&>);
    static_assert(decltype(i)::contains<const volatile int&>);
    static_assert(decltype(i)::contains<int&&>);
    static_assert(decltype(i)::contains<const int&&>);
    static_assert(decltype(i)::contains<volatile int&&>);
    static_assert(decltype(i)::contains<const volatile int&&>);
    static_assert(decltype(i)::contains<signed int>);
    static_assert(decltype(i)::contains<const signed int>);
    static_assert(decltype(i)::contains<volatile signed int>);
    static_assert(decltype(i)::contains<const volatile signed int>);
    static_assert(decltype(i)::contains<signed int&>);
    static_assert(decltype(i)::contains<const signed int&>);
    static_assert(decltype(i)::contains<volatile signed int&>);
    static_assert(decltype(i)::contains<const volatile signed int&>);
    static_assert(decltype(i)::contains<signed int&&>);
    static_assert(decltype(i)::contains<const signed int&&>);
    static_assert(decltype(i)::contains<volatile signed int&&>);
    static_assert(decltype(i)::contains<const volatile signed int&&>);
    static_assert(decltype(ci)::contains<int>);
    static_assert(decltype(ci)::contains<const int>);
    static_assert(decltype(ci)::contains<volatile int>);
    static_assert(decltype(ci)::contains<const volatile int>);
    static_assert(decltype(ci)::contains<int&>);
    static_assert(decltype(ci)::contains<const int&>);
    static_assert(decltype(ci)::contains<volatile int&>);
    static_assert(decltype(ci)::contains<const volatile int&>);
    static_assert(decltype(ci)::contains<int&&>);
    static_assert(decltype(ci)::contains<const int&&>);
    static_assert(decltype(ci)::contains<volatile int&&>);
    static_assert(decltype(ci)::contains<const volatile int&&>);
    static_assert(decltype(ci)::contains<signed int>);
    static_assert(decltype(ci)::contains<const signed int>);
    static_assert(decltype(ci)::contains<volatile signed int>);
    static_assert(decltype(ci)::contains<const volatile signed int>);
    static_assert(decltype(ci)::contains<signed int&>);
    static_assert(decltype(ci)::contains<const signed int&>);
    static_assert(decltype(ci)::contains<volatile signed int&>);
    static_assert(decltype(ci)::contains<const volatile signed int&>);
    static_assert(decltype(ci)::contains<signed int&&>);
    static_assert(decltype(ci)::contains<const signed int&&>);
    static_assert(decltype(ci)::contains<volatile signed int&&>);
    static_assert(decltype(ci)::contains<const volatile signed int&&>);
    static_assert(decltype(vi)::contains<int>);
    static_assert(decltype(vi)::contains<const int>);
    static_assert(decltype(vi)::contains<volatile int>);
    static_assert(decltype(vi)::contains<const volatile int>);
    static_assert(decltype(vi)::contains<int&>);
    static_assert(decltype(vi)::contains<const int&>);
    static_assert(decltype(vi)::contains<volatile int&>);
    static_assert(decltype(vi)::contains<const volatile int&>);
    static_assert(decltype(vi)::contains<int&&>);
    static_assert(decltype(vi)::contains<const int&&>);
    static_assert(decltype(vi)::contains<volatile int&&>);
    static_assert(decltype(vi)::contains<const volatile int&&>);
    static_assert(decltype(vi)::contains<signed int>);
    static_assert(decltype(vi)::contains<const signed int>);
    static_assert(decltype(vi)::contains<volatile signed int>);
    static_assert(decltype(vi)::contains<const volatile signed int>);
    static_assert(decltype(vi)::contains<signed int&>);
    static_assert(decltype(vi)::contains<const signed int&>);
    static_assert(decltype(vi)::contains<volatile signed int&>);
    static_assert(decltype(vi)::contains<const volatile signed int&>);
    static_assert(decltype(vi)::contains<signed int&&>);
    static_assert(decltype(vi)::contains<const signed int&&>);
    static_assert(decltype(vi)::contains<volatile signed int&&>);
    static_assert(decltype(vi)::contains<const volatile signed int&&>);
    static_assert(decltype(cvi)::contains<int>);
    static_assert(decltype(cvi)::contains<const int>);
    static_assert(decltype(cvi)::contains<volatile int>);
    static_assert(decltype(cvi)::contains<const volatile int>);
    static_assert(decltype(cvi)::contains<int&>);
    static_assert(decltype(cvi)::contains<const int&>);
    static_assert(decltype(cvi)::contains<volatile int&>);
    static_assert(decltype(cvi)::contains<const volatile int&>);
    static_assert(decltype(cvi)::contains<int&&>);
    static_assert(decltype(cvi)::contains<const int&&>);
    static_assert(decltype(cvi)::contains<volatile int&&>);
    static_assert(decltype(cvi)::contains<const volatile int&&>);
    static_assert(decltype(cvi)::contains<signed int>);
    static_assert(decltype(cvi)::contains<const signed int>);
    static_assert(decltype(cvi)::contains<volatile signed int>);
    static_assert(decltype(cvi)::contains<const volatile signed int>);
    static_assert(decltype(cvi)::contains<signed int&>);
    static_assert(decltype(cvi)::contains<const signed int&>);
    static_assert(decltype(cvi)::contains<volatile signed int&>);
    static_assert(decltype(cvi)::contains<const volatile signed int&>);
    static_assert(decltype(cvi)::contains<signed int&&>);
    static_assert(decltype(cvi)::contains<const signed int&&>);
    static_assert(decltype(cvi)::contains<volatile signed int&&>);
    static_assert(decltype(cvi)::contains<const volatile signed int&&>);
    static_assert(decltype(ri)::contains<int>);
    static_assert(decltype(ri)::contains<const int>);
    static_assert(decltype(ri)::contains<volatile int>);
    static_assert(decltype(ri)::contains<const volatile int>);
    static_assert(decltype(ri)::contains<int&>);
    static_assert(decltype(ri)::contains<const int&>);
    static_assert(decltype(ri)::contains<volatile int&>);
    static_assert(decltype(ri)::contains<const volatile int&>);
    static_assert(decltype(ri)::contains<int&&>);
    static_assert(decltype(ri)::contains<const int&&>);
    static_assert(decltype(ri)::contains<volatile int&&>);
    static_assert(decltype(ri)::contains<const volatile int&&>);
    static_assert(decltype(ri)::contains<signed int>);
    static_assert(decltype(ri)::contains<const signed int>);
    static_assert(decltype(ri)::contains<volatile signed int>);
    static_assert(decltype(ri)::contains<const volatile signed int>);
    static_assert(decltype(ri)::contains<signed int&>);
    static_assert(decltype(ri)::contains<const signed int&>);
    static_assert(decltype(ri)::contains<volatile signed int&>);
    static_assert(decltype(ri)::contains<const volatile signed int&>);
    static_assert(decltype(ri)::contains<signed int&&>);
    static_assert(decltype(ri)::contains<const signed int&&>);
    static_assert(decltype(ri)::contains<volatile signed int&&>);
    static_assert(decltype(ri)::contains<const volatile signed int&&>);
    static_assert(decltype(rci)::contains<int>);
    static_assert(decltype(rci)::contains<const int>);
    static_assert(decltype(rci)::contains<volatile int>);
    static_assert(decltype(rci)::contains<const volatile int>);
    static_assert(decltype(rci)::contains<int&>);
    static_assert(decltype(rci)::contains<const int&>);
    static_assert(decltype(rci)::contains<volatile int&>);
    static_assert(decltype(rci)::contains<const volatile int&>);
    static_assert(decltype(rci)::contains<int&&>);
    static_assert(decltype(rci)::contains<const int&&>);
    static_assert(decltype(rci)::contains<volatile int&&>);
    static_assert(decltype(rci)::contains<const volatile int&&>);
    static_assert(decltype(rci)::contains<signed int>);
    static_assert(decltype(rci)::contains<const signed int>);
    static_assert(decltype(rci)::contains<volatile signed int>);
    static_assert(decltype(rci)::contains<const volatile signed int>);
    static_assert(decltype(rci)::contains<signed int&>);
    static_assert(decltype(rci)::contains<const signed int&>);
    static_assert(decltype(rci)::contains<volatile signed int&>);
    static_assert(decltype(rci)::contains<const volatile signed int&>);
    static_assert(decltype(rci)::contains<signed int&&>);
    static_assert(decltype(rci)::contains<const signed int&&>);
    static_assert(decltype(rci)::contains<volatile signed int&&>);
    static_assert(decltype(rci)::contains<const volatile signed int&&>);
    static_assert(decltype(rvi)::contains<int>);
    static_assert(decltype(rvi)::contains<const int>);
    static_assert(decltype(rvi)::contains<volatile int>);
    static_assert(decltype(rvi)::contains<const volatile int>);
    static_assert(decltype(rvi)::contains<int&>);
    static_assert(decltype(rvi)::contains<const int&>);
    static_assert(decltype(rvi)::contains<volatile int&>);
    static_assert(decltype(rvi)::contains<const volatile int&>);
    static_assert(decltype(rvi)::contains<int&&>);
    static_assert(decltype(rvi)::contains<const int&&>);
    static_assert(decltype(rvi)::contains<volatile int&&>);
    static_assert(decltype(rvi)::contains<const volatile int&&>);
    static_assert(decltype(rvi)::contains<signed int>);
    static_assert(decltype(rvi)::contains<const signed int>);
    static_assert(decltype(rvi)::contains<volatile signed int>);
    static_assert(decltype(rvi)::contains<const volatile signed int>);
    static_assert(decltype(rvi)::contains<signed int&>);
    static_assert(decltype(rvi)::contains<const signed int&>);
    static_assert(decltype(rvi)::contains<volatile signed int&>);
    static_assert(decltype(rvi)::contains<const volatile signed int&>);
    static_assert(decltype(rvi)::contains<signed int&&>);
    static_assert(decltype(rvi)::contains<const signed int&&>);
    static_assert(decltype(rvi)::contains<volatile signed int&&>);
    static_assert(decltype(rvi)::contains<const volatile signed int&&>);
    static_assert(decltype(rcvi)::contains<int>);
    static_assert(decltype(rcvi)::contains<const int>);
    static_assert(decltype(rcvi)::contains<volatile int>);
    static_assert(decltype(rcvi)::contains<const volatile int>);
    static_assert(decltype(rcvi)::contains<int&>);
    static_assert(decltype(rcvi)::contains<const int&>);
    static_assert(decltype(rcvi)::contains<volatile int&>);
    static_assert(decltype(rcvi)::contains<const volatile int&>);
    static_assert(decltype(rcvi)::contains<int&&>);
    static_assert(decltype(rcvi)::contains<const int&&>);
    static_assert(decltype(rcvi)::contains<volatile int&&>);
    static_assert(decltype(rcvi)::contains<const volatile int&&>);
    static_assert(decltype(rcvi)::contains<signed int>);
    static_assert(decltype(rcvi)::contains<const signed int>);
    static_assert(decltype(rcvi)::contains<volatile signed int>);
    static_assert(decltype(rcvi)::contains<const volatile signed int>);
    static_assert(decltype(rcvi)::contains<signed int&>);
    static_assert(decltype(rcvi)::contains<const signed int&>);
    static_assert(decltype(rcvi)::contains<volatile signed int&>);
    static_assert(decltype(rcvi)::contains<const volatile signed int&>);
    static_assert(decltype(rcvi)::contains<signed int&&>);
    static_assert(decltype(rcvi)::contains<const signed int&&>);
    static_assert(decltype(rcvi)::contains<volatile signed int&&>);
    static_assert(decltype(rcvi)::contains<const volatile signed int&&>);
    static_assert(decltype(rri)::contains<int>);
    static_assert(decltype(rri)::contains<const int>);
    static_assert(decltype(rri)::contains<volatile int>);
    static_assert(decltype(rri)::contains<const volatile int>);
    static_assert(decltype(rri)::contains<int&>);
    static_assert(decltype(rri)::contains<const int&>);
    static_assert(decltype(rri)::contains<volatile int&>);
    static_assert(decltype(rri)::contains<const volatile int&>);
    static_assert(decltype(rri)::contains<int&&>);
    static_assert(decltype(rri)::contains<const int&&>);
    static_assert(decltype(rri)::contains<volatile int&&>);
    static_assert(decltype(rri)::contains<const volatile int&&>);
    static_assert(decltype(rri)::contains<signed int>);
    static_assert(decltype(rri)::contains<const signed int>);
    static_assert(decltype(rri)::contains<volatile signed int>);
    static_assert(decltype(rri)::contains<const volatile signed int>);
    static_assert(decltype(rri)::contains<signed int&>);
    static_assert(decltype(rri)::contains<const signed int&>);
    static_assert(decltype(rri)::contains<volatile signed int&>);
    static_assert(decltype(rri)::contains<const volatile signed int&>);
    static_assert(decltype(rri)::contains<signed int&&>);
    static_assert(decltype(rri)::contains<const signed int&&>);
    static_assert(decltype(rri)::contains<volatile signed int&&>);
    static_assert(decltype(rri)::contains<const volatile signed int&&>);
    static_assert(decltype(rrci)::contains<int>);
    static_assert(decltype(rrci)::contains<const int>);
    static_assert(decltype(rrci)::contains<volatile int>);
    static_assert(decltype(rrci)::contains<const volatile int>);
    static_assert(decltype(rrci)::contains<int&>);
    static_assert(decltype(rrci)::contains<const int&>);
    static_assert(decltype(rrci)::contains<volatile int&>);
    static_assert(decltype(rrci)::contains<const volatile int&>);
    static_assert(decltype(rrci)::contains<int&&>);
    static_assert(decltype(rrci)::contains<const int&&>);
    static_assert(decltype(rrci)::contains<volatile int&&>);
    static_assert(decltype(rrci)::contains<const volatile int&&>);
    static_assert(decltype(rrci)::contains<signed int>);
    static_assert(decltype(rrci)::contains<const signed int>);
    static_assert(decltype(rrci)::contains<volatile signed int>);
    static_assert(decltype(rrci)::contains<const volatile signed int>);
    static_assert(decltype(rrci)::contains<signed int&>);
    static_assert(decltype(rrci)::contains<const signed int&>);
    static_assert(decltype(rrci)::contains<volatile signed int&>);
    static_assert(decltype(rrci)::contains<const volatile signed int&>);
    static_assert(decltype(rrci)::contains<signed int&&>);
    static_assert(decltype(rrci)::contains<const signed int&&>);
    static_assert(decltype(rrci)::contains<volatile signed int&&>);
    static_assert(decltype(rrci)::contains<const volatile signed int&&>);
    static_assert(decltype(rrvi)::contains<int>);
    static_assert(decltype(rrvi)::contains<const int>);
    static_assert(decltype(rrvi)::contains<volatile int>);
    static_assert(decltype(rrvi)::contains<const volatile int>);
    static_assert(decltype(rrvi)::contains<int&>);
    static_assert(decltype(rrvi)::contains<const int&>);
    static_assert(decltype(rrvi)::contains<volatile int&>);
    static_assert(decltype(rrvi)::contains<const volatile int&>);
    static_assert(decltype(rrvi)::contains<int&&>);
    static_assert(decltype(rrvi)::contains<const int&&>);
    static_assert(decltype(rrvi)::contains<volatile int&&>);
    static_assert(decltype(rrvi)::contains<const volatile int&&>);
    static_assert(decltype(rrvi)::contains<signed int>);
    static_assert(decltype(rrvi)::contains<const signed int>);
    static_assert(decltype(rrvi)::contains<volatile signed int>);
    static_assert(decltype(rrvi)::contains<const volatile signed int>);
    static_assert(decltype(rrvi)::contains<signed int&>);
    static_assert(decltype(rrvi)::contains<const signed int&>);
    static_assert(decltype(rrvi)::contains<volatile signed int&>);
    static_assert(decltype(rrvi)::contains<const volatile signed int&>);
    static_assert(decltype(rrvi)::contains<signed int&&>);
    static_assert(decltype(rrvi)::contains<const signed int&&>);
    static_assert(decltype(rrvi)::contains<volatile signed int&&>);
    static_assert(decltype(rrvi)::contains<const volatile signed int&&>);
    static_assert(decltype(rrcvi)::contains<int>);
    static_assert(decltype(rrcvi)::contains<const int>);
    static_assert(decltype(rrcvi)::contains<volatile int>);
    static_assert(decltype(rrcvi)::contains<const volatile int>);
    static_assert(decltype(rrcvi)::contains<int&>);
    static_assert(decltype(rrcvi)::contains<const int&>);
    static_assert(decltype(rrcvi)::contains<volatile int&>);
    static_assert(decltype(rrcvi)::contains<const volatile int&>);
    static_assert(decltype(rrcvi)::contains<int&&>);
    static_assert(decltype(rrcvi)::contains<const int&&>);
    static_assert(decltype(rrcvi)::contains<volatile int&&>);
    static_assert(decltype(rrcvi)::contains<const volatile int&&>);
    static_assert(decltype(rrcvi)::contains<signed int>);
    static_assert(decltype(rrcvi)::contains<const signed int>);
    static_assert(decltype(rrcvi)::contains<volatile signed int>);
    static_assert(decltype(rrcvi)::contains<const volatile signed int>);
    static_assert(decltype(rrcvi)::contains<signed int&>);
    static_assert(decltype(rrcvi)::contains<const signed int&>);
    static_assert(decltype(rrcvi)::contains<volatile signed int&>);
    static_assert(decltype(rrcvi)::contains<const volatile signed int&>);
    static_assert(decltype(rrcvi)::contains<signed int&&>);
    static_assert(decltype(rrcvi)::contains<const signed int&&>);
    static_assert(decltype(rrcvi)::contains<volatile signed int&&>);
    static_assert(decltype(rrcvi)::contains<const volatile signed int&&>);

    static_assert(not decltype(i)::contains<long>);
    static_assert(not decltype(ci)::contains<long>);
    static_assert(not decltype(i)::contains<unsigned int>);
    static_assert(not decltype(rci)::contains<unsigned int>);
    struct Foo;
    static_assert(not decltype(i)::contains<Foo>);
    static_assert(not decltype(rrvi)::contains<Foo>);

    // all of these are the same set_impl<int, long>, except for ..
    constexpr auto rci_rrl = set<const int&, long&&>();
    constexpr auto i_cl = set<int, const long>();
    constexpr auto i_rrl = set<int, long&&>();
    constexpr auto i_rsl = set<int, signed long&>();
    constexpr auto ri_rrcsl = set<int&, const signed long&&>();
    constexpr auto rri_rrsl = set<int&&, signed long&&>();
    // ... the one here, which is set_impl<int, unsigned long>
    constexpr auto rri_rrul = set<int&&, unsigned long&&>();

    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_cl)>);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_rrl)>);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(i_rsl)>);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(ri_rrcsl)>);
    static_assert(decltype(rci_rrl)::is_same_set<decltype(rri_rrsl)>);
    static_assert(not decltype(rri_rrsl)::is_same_set<decltype(rri_rrul)>);

    static_assert(not decltype(rci_rrl)::empty);
    static_assert(not decltype(i_cl)::empty);
    static_assert(not decltype(i_rrl)::empty);
    static_assert(not decltype(i_rsl)::empty);
    static_assert(not decltype(ri_rrcsl)::empty);
    static_assert(not decltype(rri_rrsl)::empty);
    static_assert(not decltype(rri_rrul)::empty);
    static_assert(rci_rrl.size == 2);
    static_assert(i_cl.size == 2);
    static_assert(i_rrl.size == 2);
    static_assert(i_rsl.size == 2);
    static_assert(ri_rrcsl.size == 2);
    static_assert(rri_rrsl.size == 2);
    static_assert(rri_rrul.size == 2);

    static_assert(decltype(rci_rrl)::contains<int>);
    static_assert(decltype(rci_rrl)::contains<const int>);
    static_assert(decltype(rci_rrl)::contains<volatile int>);
    static_assert(decltype(rci_rrl)::contains<const volatile int>);
    static_assert(decltype(rci_rrl)::contains<int&>);
    static_assert(decltype(rci_rrl)::contains<const int&>);
    static_assert(decltype(rci_rrl)::contains<volatile int&>);
    static_assert(decltype(rci_rrl)::contains<const volatile int&>);
    static_assert(decltype(rci_rrl)::contains<int&&>);
    static_assert(decltype(rci_rrl)::contains<const int&&>);
    static_assert(decltype(rci_rrl)::contains<volatile int&&>);
    static_assert(decltype(rci_rrl)::contains<const volatile int&&>);
    static_assert(decltype(rci_rrl)::contains<signed int>);
    static_assert(decltype(rci_rrl)::contains<const signed int>);
    static_assert(decltype(rci_rrl)::contains<volatile signed int>);
    static_assert(decltype(rci_rrl)::contains<const volatile signed int>);
    static_assert(decltype(rci_rrl)::contains<signed int&>);
    static_assert(decltype(rci_rrl)::contains<const signed int&>);
    static_assert(decltype(rci_rrl)::contains<volatile signed int&>);
    static_assert(decltype(rci_rrl)::contains<const volatile signed int&>);
    static_assert(decltype(rci_rrl)::contains<signed int&&>);
    static_assert(decltype(rci_rrl)::contains<const signed int&&>);
    static_assert(decltype(rci_rrl)::contains<volatile signed int&&>);
    static_assert(decltype(rci_rrl)::contains<const volatile signed int&&>);
    static_assert(decltype(rci_rrl)::contains<long>);
    static_assert(decltype(rci_rrl)::contains<const long>);
    static_assert(decltype(rci_rrl)::contains<volatile long>);
    static_assert(decltype(rci_rrl)::contains<const volatile long>);
    static_assert(decltype(rci_rrl)::contains<long&>);
    static_assert(decltype(rci_rrl)::contains<const long&>);
    static_assert(decltype(rci_rrl)::contains<volatile long&>);
    static_assert(decltype(rci_rrl)::contains<const volatile long&>);
    static_assert(decltype(rci_rrl)::contains<long&&>);
    static_assert(decltype(rci_rrl)::contains<const long&&>);
    static_assert(decltype(rci_rrl)::contains<volatile long&&>);
    static_assert(decltype(rci_rrl)::contains<const volatile long&&>);
    static_assert(decltype(rci_rrl)::contains<signed long>);
    static_assert(decltype(rci_rrl)::contains<const signed long>);
    static_assert(decltype(rci_rrl)::contains<volatile signed long>);
    static_assert(decltype(rci_rrl)::contains<const volatile signed long>);
    static_assert(decltype(rci_rrl)::contains<signed long&>);
    static_assert(decltype(rci_rrl)::contains<const signed long&>);
    static_assert(decltype(rci_rrl)::contains<volatile signed long&>);
    static_assert(decltype(rci_rrl)::contains<const volatile signed long&>);
    static_assert(decltype(rci_rrl)::contains<signed long&&>);
    static_assert(decltype(rci_rrl)::contains<const signed long&&>);
    static_assert(decltype(rci_rrl)::contains<volatile signed long&&>);
    static_assert(decltype(rci_rrl)::contains<const volatile signed long&&>);
    static_assert(decltype(i_cl)::contains<long>);
    static_assert(decltype(i_cl)::contains<const long>);
    static_assert(decltype(i_cl)::contains<volatile long>);
    static_assert(decltype(i_cl)::contains<const volatile long>);
    static_assert(decltype(i_cl)::contains<long&>);
    static_assert(decltype(i_cl)::contains<const long&>);
    static_assert(decltype(i_cl)::contains<volatile long&>);
    static_assert(decltype(i_cl)::contains<const volatile long&>);
    static_assert(decltype(i_cl)::contains<long&&>);
    static_assert(decltype(i_cl)::contains<const long&&>);
    static_assert(decltype(i_cl)::contains<volatile long&&>);
    static_assert(decltype(i_cl)::contains<const volatile long&&>);
    static_assert(decltype(i_cl)::contains<signed long>);
    static_assert(decltype(i_cl)::contains<const signed long>);
    static_assert(decltype(i_cl)::contains<volatile signed long>);
    static_assert(decltype(i_cl)::contains<const volatile signed long>);
    static_assert(decltype(i_cl)::contains<signed long&>);
    static_assert(decltype(i_cl)::contains<const signed long&>);
    static_assert(decltype(i_cl)::contains<volatile signed long&>);
    static_assert(decltype(i_cl)::contains<const volatile signed long&>);
    static_assert(decltype(i_cl)::contains<signed long&&>);
    static_assert(decltype(i_cl)::contains<const signed long&&>);
    static_assert(decltype(i_cl)::contains<volatile signed long&&>);
    static_assert(decltype(i_cl)::contains<const volatile signed long&&>);

    static_assert(not decltype(rci_rrl)::contains<unsigned int>);
    static_assert(not decltype(rci_rrl)::contains<Foo>);

    static_assert(decltype(rri_rrul)::contains<int>);
    static_assert(decltype(rri_rrul)::contains<int&&>);
    static_assert(decltype(rri_rrul)::contains<const int>);
    static_assert(decltype(rri_rrul)::contains<unsigned long>);
    static_assert(decltype(rri_rrul)::contains<const unsigned long>);
    static_assert(decltype(rri_rrul)::contains<volatile unsigned long>);
    static_assert(decltype(rri_rrul)::contains<const volatile unsigned long>);
    static_assert(decltype(rri_rrul)::contains<unsigned long&>);
    static_assert(decltype(rri_rrul)::contains<const unsigned long&>);
    static_assert(decltype(rri_rrul)::contains<volatile unsigned long&>);
    static_assert(decltype(rri_rrul)::contains<const volatile unsigned long&>);
    static_assert(decltype(rri_rrul)::contains<unsigned long&&>);
    static_assert(decltype(rri_rrul)::contains<const unsigned long&&>);
    static_assert(decltype(rri_rrul)::contains<volatile unsigned long&&>);
    static_assert(decltype(rri_rrul)::contains<const volatile unsigned long&&>);

    static_assert(not decltype(rri_rrul)::contains<unsigned int>);
    static_assert(not decltype(rri_rrul)::contains<signed long>);
    static_assert(not decltype(rri_rrul)::contains<const Foo&>);
}
