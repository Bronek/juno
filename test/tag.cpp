// Copyright (c) 2017-2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <gtest/gtest.h>

#include <type_traits>

#include <tag.hpp>

TEST(type_tag, test_type_tag)
{
    using namespace juno;

    const auto i = tag<int>{};
    const auto ci = tag<const int>{};
    const auto vi = tag<volatile int>{};
    const auto cvi = tag<const volatile int>{};
    const auto ri = tag<int&>{};
    const auto rci = tag<const int&>{};
    const auto rvi = tag<volatile int&>{};
    const auto rcvi = tag<const volatile int&>{};
    const auto rri = tag<int&&>{};
    const auto rrci = tag<const int&&>{};
    const auto rrvi = tag<volatile int&&>{};
    const auto rrcvi = tag<const volatile int&&>{};
    EXPECT_EQ(decltype(i)::value, decltype(ci)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(ci)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(vi)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(vi)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(cvi)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(cvi)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(ri)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(ri)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rci)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rci)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rvi)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rvi)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rcvi)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rcvi)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rri)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rri)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rrci)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rrci)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rrvi)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rrvi)::type>::value);
    EXPECT_EQ(decltype(i)::value, decltype(rrcvi)::value);
    static_assert(std::is_same<decltype(i)::type, decltype(rrcvi)::type>::value);
    static_assert(std::is_same<decltype(i)::type, tag<decltype(1)>::type>::value);

    const auto pi = tag<int*>{};
    const auto cpi = tag<const int*>{};
    const auto pic = tag<int* const>{};
    const auto cpic = tag<const int* const>{};
    const auto rcpic = tag<const int* const&>{};
    const auto rrcpi = tag<const int* &&>{};
    EXPECT_NE(decltype(i)::value, decltype(pi)::value);
    static_assert(not std::is_same<decltype(i)::type, decltype(pi)::type>::value);
    EXPECT_NE(decltype(pi)::value, decltype(cpi)::value);
    static_assert(not std::is_same<decltype(pi)::type, decltype(cpi)::type>::value);
    EXPECT_EQ(decltype(pi)::value, decltype(pic)::value);
    static_assert(std::is_same<decltype(pi)::type, decltype(pic)::type>::value);
    EXPECT_EQ(decltype(cpi)::value, decltype(cpic)::value);
    static_assert(std::is_same<tag<int const*>::type, decltype(cpic)::type>::value);
    EXPECT_EQ(decltype(cpi)::value, decltype(rcpic)::value);
    static_assert(std::is_same<tag<int const*>::type, decltype(rcpic)::type>::value);
    EXPECT_EQ(decltype(cpi)::value, decltype(rrcpi)::value);
    static_assert(std::is_same<tag<int const*>::type, decltype(rrcpi)::type>::value);

    const auto l = tag<long>{};
    EXPECT_NE(decltype(i)::value, decltype(l)::value);
    static_assert(not std::is_same<decltype(i)::type, decltype(l)::type>::value);
    EXPECT_NE(decltype(pi)::value, decltype(l)::value);
    static_assert(not std::is_same<decltype(pi)::type, decltype(l)::type>::value);
    EXPECT_NE(decltype(cpi)::value, decltype(l)::value);
    static_assert(not std::is_same<decltype(cpi)::type, decltype(l)::type>::value);

    const auto rsl = tag<signed long&>{};
    EXPECT_EQ(decltype(l)::value, decltype(rsl)::value);
    static_assert(std::is_same<decltype(l)::type, decltype(rsl)::type>::value);

    const auto ul = tag<unsigned long>{};
    EXPECT_NE(decltype(i)::value, decltype(ul)::value);
    static_assert(not std::is_same<decltype(i)::type, decltype(ul)::type>::value);
    EXPECT_NE(decltype(pi)::value, decltype(ul)::value);
    static_assert(not std::is_same<decltype(pi)::type, decltype(ul)::type>::value);
    EXPECT_NE(decltype(cpi)::value, decltype(ul)::value);
    static_assert(not std::is_same<decltype(cpi)::type, decltype(ul)::type>::value);
    EXPECT_NE(decltype(l)::value, decltype(ul)::value);
    static_assert(not std::is_same<decltype(l)::type, decltype(ul)::type>::value);

    struct Foo;
    const auto f = tag<Foo>{};
    EXPECT_NE(decltype(i)::value, decltype(f)::value);
    static_assert(not std::is_same<decltype(i)::type, decltype(f)::type>::value);
    EXPECT_NE(decltype(pi)::value, decltype(f)::value);
    static_assert(not std::is_same<decltype(pi)::type, decltype(f)::type>::value);
    EXPECT_NE(decltype(cpi)::value, decltype(f)::value);
    static_assert(not std::is_same<decltype(cpi)::type, decltype(f)::type>::value);
    EXPECT_NE(decltype(l)::value, decltype(f)::value);
    static_assert(not std::is_same<decltype(l)::type, decltype(f)::type>::value);
    EXPECT_NE(decltype(ul)::value, decltype(f)::value);
    static_assert(not std::is_same<decltype(ul)::type, decltype(f)::type>::value);

    enum Baz { };
    const auto b = tag<Baz>{};
    const auto rrb = tag<Baz&&>{};
    const auto rcb = tag<const Baz&>{};
    EXPECT_NE(decltype(i)::value, decltype(b)::value);
    static_assert(not std::is_same<decltype(i)::type, decltype(b)::type>::value);
    EXPECT_NE(decltype(pi)::value, decltype(b)::value);
    static_assert(not std::is_same<decltype(pi)::type, decltype(b)::type>::value);
    EXPECT_NE(decltype(cpi)::value, decltype(b)::value);
    static_assert(not std::is_same<decltype(cpi)::type, decltype(b)::type>::value);
    EXPECT_NE(decltype(l)::value, decltype(b)::value);
    static_assert(not std::is_same<decltype(l)::type, decltype(b)::type>::value);
    EXPECT_NE(decltype(ul)::value, decltype(b)::value);
    static_assert(not std::is_same<decltype(ul)::type, decltype(b)::type>::value);
    EXPECT_NE(decltype(f)::value, decltype(b)::value);
    static_assert(not std::is_same<decltype(f)::type, decltype(b)::type>::value);
    EXPECT_EQ(decltype(b)::value, decltype(rrb)::value);
    static_assert(std::is_same<decltype(b)::type, decltype(rrb)::type>::value);
    EXPECT_EQ(decltype(b)::value, decltype(rcb)::value);
    static_assert(std::is_same<decltype(b)::type, decltype(rcb)::type>::value);
}
