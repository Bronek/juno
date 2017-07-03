#include <gtest/gtest.h>

#include <type_traits>

#include <type_tag.hpp>

TEST(type_tag, test_type_tag)
{
    using namespace juno;

    const auto i = type_tag<int>::value;
    const auto ci = type_tag<const int>::value;
    const auto vi = type_tag<volatile int>::value;
    const auto cvi = type_tag<const volatile int>::value;
    const auto ri = type_tag<int&>::value;
    const auto rci = type_tag<const int&>::value;
    const auto rvi = type_tag<volatile int&>::value;
    const auto rcvi = type_tag<const volatile int&>::value;
    const auto rri = type_tag<int&&>::value;
    const auto rrci = type_tag<const int&&>::value;
    const auto rrvi = type_tag<volatile int&&>::value;
    const auto rrcvi = type_tag<const volatile int&&>::value;
    EXPECT_EQ(i, ci);
    static_assert(std::is_same<type_tag<int>::type, type_tag<const int>::type>::value, "");
    EXPECT_EQ(i, vi);
    static_assert(std::is_same<type_tag<int>::type, type_tag<volatile int>::type>::value, "");
    EXPECT_EQ(i, cvi);
    static_assert(std::is_same<type_tag<int>::type, type_tag<const volatile int>::type>::value, "");
    EXPECT_EQ(i, ri);
    static_assert(std::is_same<type_tag<int>::type, type_tag<int&>::type>::value, "");
    EXPECT_EQ(i, rci);
    static_assert(std::is_same<type_tag<int>::type, type_tag<const int&>::type>::value, "");
    EXPECT_EQ(i, rvi);
    static_assert(std::is_same<type_tag<int>::type, type_tag<volatile int&>::type>::value, "");
    EXPECT_EQ(i, rcvi);
    static_assert(std::is_same<type_tag<int>::type, type_tag<const volatile int&>::type>::value, "");
    EXPECT_EQ(i, rri);
    static_assert(std::is_same<type_tag<int>::type, type_tag<int&&>::type>::value, "");
    EXPECT_EQ(i, rrci);
    static_assert(std::is_same<type_tag<int>::type, type_tag<const int&&>::type>::value, "");
    EXPECT_EQ(i, rrvi);
    static_assert(std::is_same<type_tag<int>::type, type_tag<volatile int&&>::type>::value, "");
    EXPECT_EQ(i, rrcvi);
    static_assert(std::is_same<type_tag<int>::type, type_tag<const volatile int&&>::type>::value, "");
    static_assert(std::is_same<type_tag<int>::type, type_tag<decltype(1)>::type>::value, "");

    const auto pi = type_tag<int*>::value;
    const auto cpi = type_tag<const int*>::value;
    const auto pic = type_tag<int* const>::value;
    const auto cpic = type_tag<const int* const>::value;
    const auto rcpic = type_tag<const int* const&>::value;
    const auto rrcpi = type_tag<const int* &&>::value;
    EXPECT_NE(i, pi);
    static_assert(not std::is_same<type_tag<int>::type, type_tag<int*>::type>::value, "");
    EXPECT_NE(pi, cpi);
    static_assert(not std::is_same<type_tag<int*>::type, type_tag<const int*>::type>::value, "");
    EXPECT_EQ(pi, pic);
    static_assert(std::is_same<type_tag<int*>::type, type_tag<int* const>::type>::value, "");
    EXPECT_EQ(cpi, cpic);
    static_assert(std::is_same<type_tag<int const*>::type, type_tag<const int* const>::type>::value, "");
    EXPECT_EQ(cpi, rcpic);
    static_assert(std::is_same<type_tag<int const*>::type, type_tag<const int* const&>::type>::value, "");
    EXPECT_EQ(cpi, rrcpi);
    static_assert(std::is_same<type_tag<int const*>::type, type_tag<const int* &&>::type>::value, "");

    const auto l = type_tag<long>::value;
    EXPECT_NE(i, l);
    static_assert(not std::is_same<type_tag<int>::type, type_tag<long>::type>::value, "");
    EXPECT_NE(pi, l);
    static_assert(not std::is_same<type_tag<int*>::type, type_tag<long>::type>::value, "");
    EXPECT_NE(cpi, l);
    static_assert(not std::is_same<type_tag<const int*>::type, type_tag<long>::type>::value, "");

    const auto rsl = type_tag<signed long&>::value;
    EXPECT_EQ(l, rsl);
    static_assert(std::is_same<type_tag<long>::type, type_tag<signed long&>::type>::value, "");

    const auto ul = type_tag<unsigned long>::value;
    EXPECT_NE(i, ul);
    static_assert(not std::is_same<type_tag<int>::type, type_tag<unsigned long>::type>::value, "");
    EXPECT_NE(pi, ul);
    static_assert(not std::is_same<type_tag<int*>::type, type_tag<unsigned long>::type>::value, "");
    EXPECT_NE(cpi, ul);
    static_assert(not std::is_same<type_tag<const int*>::type, type_tag<unsigned long>::type>::value, "");
    EXPECT_NE(l, ul);
    static_assert(not std::is_same<type_tag<long>::type, type_tag<unsigned long>::type>::value, "");

    struct Foo;
    const auto f = type_tag<Foo>::value;
    EXPECT_NE(i, f);
    static_assert(not std::is_same<type_tag<int>::type, type_tag<Foo>::type>::value, "");
    EXPECT_NE(pi, f);
    static_assert(not std::is_same<type_tag<int*>::type, type_tag<Foo>::type>::value, "");
    EXPECT_NE(cpi, f);
    static_assert(not std::is_same<type_tag<const int*>::type, type_tag<Foo>::type>::value, "");
    EXPECT_NE(l, f);
    static_assert(not std::is_same<type_tag<long>::type, type_tag<Foo>::type>::value, "");
    EXPECT_NE(ul, f);
    static_assert(not std::is_same<type_tag<unsigned long>::type, type_tag<Foo>::type>::value, "");

    enum Baz { };
    const auto b = type_tag<Baz>::value;
    const auto rrb = type_tag<Baz&&>::value;
    const auto rcb = type_tag<const Baz&>::value;
    EXPECT_NE(i, b);
    static_assert(not std::is_same<type_tag<int>::type, type_tag<Baz>::type>::value, "");
    EXPECT_NE(pi, b);
    static_assert(not std::is_same<type_tag<int*>::type, type_tag<Baz>::type>::value, "");
    EXPECT_NE(cpi, b);
    static_assert(not std::is_same<type_tag<const int*>::type, type_tag<Baz>::type>::value, "");
    EXPECT_NE(l, b);
    static_assert(not std::is_same<type_tag<long>::type, type_tag<Baz>::type>::value, "");
    EXPECT_NE(ul, b);
    static_assert(not std::is_same<type_tag<unsigned long>::type, type_tag<Baz>::type>::value, "");
    EXPECT_NE(f, b);
    static_assert(not std::is_same<type_tag<Foo>::type, type_tag<Baz>::type>::value, "");
    EXPECT_EQ(b, rrb);
    static_assert(std::is_same<type_tag<Baz>::type, type_tag<Baz&&>::type>::value, "");
    EXPECT_EQ(b, rcb);
    static_assert(std::is_same<type_tag<Baz>::type, type_tag<const Baz&>::type>::value, "");
}
