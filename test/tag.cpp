// Copyright (c) 2017 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <gtest/gtest.h>

#include <type_traits>

#include <tag.hpp>

TEST(type_tag, test_type_tag)
{
    using namespace juno;

    const auto i = tag<int>::value;
    const auto ci = tag<const int>::value;
    const auto vi = tag<volatile int>::value;
    const auto cvi = tag<const volatile int>::value;
    const auto ri = tag<int&>::value;
    const auto rci = tag<const int&>::value;
    const auto rvi = tag<volatile int&>::value;
    const auto rcvi = tag<const volatile int&>::value;
    const auto rri = tag<int&&>::value;
    const auto rrci = tag<const int&&>::value;
    const auto rrvi = tag<volatile int&&>::value;
    const auto rrcvi = tag<const volatile int&&>::value;
    EXPECT_EQ(i, ci);
    static_assert(std::is_same<tag<int>::type, tag<const int>::type>::value, "");
    EXPECT_EQ(i, vi);
    static_assert(std::is_same<tag<int>::type, tag<volatile int>::type>::value, "");
    EXPECT_EQ(i, cvi);
    static_assert(std::is_same<tag<int>::type, tag<const volatile int>::type>::value, "");
    EXPECT_EQ(i, ri);
    static_assert(std::is_same<tag<int>::type, tag<int&>::type>::value, "");
    EXPECT_EQ(i, rci);
    static_assert(std::is_same<tag<int>::type, tag<const int&>::type>::value, "");
    EXPECT_EQ(i, rvi);
    static_assert(std::is_same<tag<int>::type, tag<volatile int&>::type>::value, "");
    EXPECT_EQ(i, rcvi);
    static_assert(std::is_same<tag<int>::type, tag<const volatile int&>::type>::value, "");
    EXPECT_EQ(i, rri);
    static_assert(std::is_same<tag<int>::type, tag<int&&>::type>::value, "");
    EXPECT_EQ(i, rrci);
    static_assert(std::is_same<tag<int>::type, tag<const int&&>::type>::value, "");
    EXPECT_EQ(i, rrvi);
    static_assert(std::is_same<tag<int>::type, tag<volatile int&&>::type>::value, "");
    EXPECT_EQ(i, rrcvi);
    static_assert(std::is_same<tag<int>::type, tag<const volatile int&&>::type>::value, "");
    static_assert(std::is_same<tag<int>::type, tag<decltype(1)>::type>::value, "");

    const auto pi = tag<int*>::value;
    const auto cpi = tag<const int*>::value;
    const auto pic = tag<int* const>::value;
    const auto cpic = tag<const int* const>::value;
    const auto rcpic = tag<const int* const&>::value;
    const auto rrcpi = tag<const int* &&>::value;
    EXPECT_NE(i, pi);
    static_assert(not std::is_same<tag<int>::type, tag<int*>::type>::value, "");
    EXPECT_NE(pi, cpi);
    static_assert(not std::is_same<tag<int*>::type, tag<const int*>::type>::value, "");
    EXPECT_EQ(pi, pic);
    static_assert(std::is_same<tag<int*>::type, tag<int* const>::type>::value, "");
    EXPECT_EQ(cpi, cpic);
    static_assert(std::is_same<tag<int const*>::type, tag<const int* const>::type>::value, "");
    EXPECT_EQ(cpi, rcpic);
    static_assert(std::is_same<tag<int const*>::type, tag<const int* const&>::type>::value, "");
    EXPECT_EQ(cpi, rrcpi);
    static_assert(std::is_same<tag<int const*>::type, tag<const int* &&>::type>::value, "");

    const auto l = tag<long>::value;
    EXPECT_NE(i, l);
    static_assert(not std::is_same<tag<int>::type, tag<long>::type>::value, "");
    EXPECT_NE(pi, l);
    static_assert(not std::is_same<tag<int*>::type, tag<long>::type>::value, "");
    EXPECT_NE(cpi, l);
    static_assert(not std::is_same<tag<const int*>::type, tag<long>::type>::value, "");

    const auto rsl = tag<signed long&>::value;
    EXPECT_EQ(l, rsl);
    static_assert(std::is_same<tag<long>::type, tag<signed long&>::type>::value, "");

    const auto ul = tag<unsigned long>::value;
    EXPECT_NE(i, ul);
    static_assert(not std::is_same<tag<int>::type, tag<unsigned long>::type>::value, "");
    EXPECT_NE(pi, ul);
    static_assert(not std::is_same<tag<int*>::type, tag<unsigned long>::type>::value, "");
    EXPECT_NE(cpi, ul);
    static_assert(not std::is_same<tag<const int*>::type, tag<unsigned long>::type>::value, "");
    EXPECT_NE(l, ul);
    static_assert(not std::is_same<tag<long>::type, tag<unsigned long>::type>::value, "");

    struct Foo;
    const auto f = tag<Foo>::value;
    EXPECT_NE(i, f);
    static_assert(not std::is_same<tag<int>::type, tag<Foo>::type>::value, "");
    EXPECT_NE(pi, f);
    static_assert(not std::is_same<tag<int*>::type, tag<Foo>::type>::value, "");
    EXPECT_NE(cpi, f);
    static_assert(not std::is_same<tag<const int*>::type, tag<Foo>::type>::value, "");
    EXPECT_NE(l, f);
    static_assert(not std::is_same<tag<long>::type, tag<Foo>::type>::value, "");
    EXPECT_NE(ul, f);
    static_assert(not std::is_same<tag<unsigned long>::type, tag<Foo>::type>::value, "");

    enum Baz { };
    const auto b = tag<Baz>::value;
    const auto rrb = tag<Baz&&>::value;
    const auto rcb = tag<const Baz&>::value;
    EXPECT_NE(i, b);
    static_assert(not std::is_same<tag<int>::type, tag<Baz>::type>::value, "");
    EXPECT_NE(pi, b);
    static_assert(not std::is_same<tag<int*>::type, tag<Baz>::type>::value, "");
    EXPECT_NE(cpi, b);
    static_assert(not std::is_same<tag<const int*>::type, tag<Baz>::type>::value, "");
    EXPECT_NE(l, b);
    static_assert(not std::is_same<tag<long>::type, tag<Baz>::type>::value, "");
    EXPECT_NE(ul, b);
    static_assert(not std::is_same<tag<unsigned long>::type, tag<Baz>::type>::value, "");
    EXPECT_NE(f, b);
    static_assert(not std::is_same<tag<Foo>::type, tag<Baz>::type>::value, "");
    EXPECT_EQ(b, rrb);
    static_assert(std::is_same<tag<Baz>::type, tag<Baz&&>::type>::value, "");
    EXPECT_EQ(b, rcb);
    static_assert(std::is_same<tag<Baz>::type, tag<const Baz&>::type>::value, "");
}
