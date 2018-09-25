#include <cstdio>
#include <type_traits>
#include <utility>

template <template <typename> typename Check, typename... L> class set;

namespace set_impl {

template <template <typename> typename Check, typename... L> struct contains_detail;
template <template <typename> typename Check, typename U> struct contains_detail<Check, U> {
    using type = typename Check<U>::type;
    constexpr static bool value = std::is_void_v<type>;
};
template <template <typename> typename Check, typename U, typename T, typename... L>
struct contains_detail<Check, U, T, L...> {
    using type = typename Check<U>::type;
    using head = typename Check<T>::type;
    constexpr static bool value
        = std::is_void_v<
              type> || std::is_same_v<type, head> || contains_detail<Check, U, L...>::value;
};

template <template <typename> typename Check, typename... L> struct insert_detail;
template <template <typename> typename Check, typename T, typename... L>
struct insert_detail<Check, T, set<Check, L...>> {
    using head = typename Check<T>::type;
    using type = set<Check, head, L...>;
};

template <typename... L> struct unchecked_list {};

template <typename T> struct cracker_list;
template <template <typename> typename Check, typename... L>
struct cracker_list<set<Check, L...>> {
    using type = unchecked_list<L...>;
};

template <typename T> struct cracker;
template <template <typename> typename Check, typename... L> struct cracker<set<Check, L...>> {
    using list = typename cracker_list<typename set<Check, L...>::unique>::type;
    constexpr static size_t size = set<Check, L...>::unique::size;
};

template <typename T> struct dummy_check {
    using type = T;
};

template <typename... L> struct intersect_detail;
template <typename... T> struct intersect_detail<unchecked_list<>, unchecked_list<T...>> {
    constexpr static size_t size = 0;
};
template <typename V, typename... L, typename... T>
struct intersect_detail<unchecked_list<V, L...>, unchecked_list<T...>> {
    constexpr static size_t size
        = intersect_detail<unchecked_list<L...>, unchecked_list<T...>>::size
          + (not std::is_void_v<V> && contains_detail<dummy_check, V, T...>::value);
};

template <size_t Mine, size_t Cross, size_t Theirs> struct is_cross {
    constexpr static bool value = Cross > 0;
};

template <size_t Mine, size_t Cross, size_t Theirs> struct is_super {
    constexpr static bool value = Cross == Theirs;
};

template <size_t Mine, size_t Cross, size_t Theirs> struct is_same {
    constexpr static bool value = Mine == Cross && Cross == Theirs;
};

template <template <typename> typename Check, typename... L> struct unique;
template <template <typename> typename Check> struct unique<Check> {
    using type = set<Check>;
    constexpr static size_t size = 0;

    template <template <size_t, size_t, size_t> typename How, typename U, size_t Size>
    constexpr static bool compare = How<0, 0, Size>::value;

    template <typename U> constexpr static bool test = contains_detail<Check, U>::value;
};
template <template <typename> typename Check, typename T, typename... L>
struct unique<Check, T, L...> {
    using type = typename std::conditional<
        contains_detail<Check, T, L...>::value, typename unique<Check, L...>::type,
        typename insert_detail<Check, T, typename unique<Check, L...>::type>::type>::type;
    constexpr static size_t size = contains_detail<Check, T, L...>::value
                                       ? unique<Check, L...>::size
                                       : unique<Check, L...>::size + 1;
    template <template <size_t, size_t, size_t> typename How, typename U, size_t Size>
    constexpr static bool compare
        = How<size, intersect_detail<unchecked_list<T, L...>, U>::size, Size>::value;
    template <typename U>
    constexpr static bool test = contains_detail<Check, U, T, L...>::value;
};

}  // namespace set_impl

template <template <typename> typename Check, typename... L> class set {
    using impl = set_impl::unique<Check, L...>;

public:
    constexpr explicit set() = default;
    using unique = typename impl::type;

    constexpr static size_t size = impl::size;
    constexpr static bool   empty = size == 0;

    template <typename T>
    constexpr static bool is_same
        = impl::template compare<set_impl::is_same, typename set_impl::cracker<T>::list,
                                 set_impl::cracker<T>::size>;

    template <typename T>
    constexpr static bool is_cross
        = impl::template compare<set_impl::is_cross, typename set_impl::cracker<T>::list,
                                 set_impl::cracker<T>::size>;

    template <typename T>
    constexpr static bool is_super
        = impl::template compare<set_impl::is_super, typename set_impl::cracker<T>::list,
                                 set_impl::cracker<T>::size>;

    template <typename T> constexpr static bool test = impl::template test<T>;

    template <typename T>
    using type = typename std::enable_if<(not std::is_void_v<T> && test<T>),
                                         typename Check<T>::type>::type;

    template <typename T> using insert = typename set_impl::unique<Check, T, L...>::type;
};

// clang-format off
struct Baz { Baz() = delete; };
struct Bar { Bar() = delete; };
struct Fuz { Fuz() = delete; };
// clang-format on

struct Foo {
    int i = 0;

    template <typename Set> constexpr explicit Foo(Set)
    {
        if constexpr ((bool)Set::template test<Bar>) i += 1;
    }
};

template <typename T> struct PlainTypes {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(not std::is_pointer_v<T>);
    using type = T;
};

int main()
{
    constexpr static set<PlainTypes> s0{};
    // clang-format off
    constexpr static auto s1 = decltype(s0)
        ::insert<Baz>
        ::insert<Fuz>{};
    // clang-format on

    constexpr static Foo foo1{s1};
    std::printf("foo1.i=%d\n", foo1.i);

    // clang-format off
    constexpr static auto s2 = decltype(s1)
        ::insert<Bar>{};
    // clang-format on
    constexpr static Foo foo2{s2};
    std::printf("foo2.i=%d\n", foo2.i);
}
