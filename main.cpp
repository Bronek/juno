#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
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
    template <typename F> constexpr static bool apply(const F&) noexcept { return true; }
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

    template <typename F> constexpr static bool apply(const F& fn) noexcept
    {
        if constexpr (not std::is_void_v<T> && not contains_detail<Check, T, L...>::value) {
            if (not fn((const T*)nullptr)) return false;
        }
        return unique<Check, L...>::apply(fn);
    }
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

    template <typename F> constexpr static bool for_each(const F& fn) noexcept
    {
        return impl::apply(fn);
    }
};

namespace map_impl {

template <typename T> struct wrap {
    constexpr explicit wrap() = default;
    using type = T;
};

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
struct impl;
template <template <typename> typename CheckT, template <typename> typename CheckV>
struct impl<CheckT, CheckV> {
    using selectors = set<CheckT>;
    constexpr explicit impl() = default;
    constexpr static void pair() noexcept;
};
template <template <typename> typename CheckT, template <typename> typename CheckV, typename T,
          typename V, typename... L>
struct impl<CheckT, CheckV, T, V, L...> : impl<CheckT, CheckV, L...> {
    using check = typename CheckV<V>::type;
    using base = impl<CheckT, CheckV, L...>;
    using selectors = typename base::selectors::template insert<T>;
    static_assert(not base::selectors ::template test<T>);
    constexpr explicit impl() = default;
    using base::pair;
    constexpr static auto pair(wrap<T>) noexcept { return wrap<check>(); }
};

}  // namespace map_impl

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
class map {
    using impl = map_impl::impl<CheckT, CheckV, L...>;

public:
    constexpr map() = default;
    template <typename T, typename V> constexpr auto insert() const noexcept
    {
        using result = map<CheckT, CheckV, T, V, L...>;
        return result();
    }

    using set = typename impl::selectors;
    template <typename U> using type = typename decltype(impl::pair(map_impl::wrap<U>()))::type;
};

namespace val_impl {

template <typename T> struct wrap {
    constexpr explicit wrap() = default;
    using type = T;
};

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
struct impl;

template <template <typename> typename CheckT, template <typename> typename CheckV>
struct impl<CheckT, CheckV> {
    using selectors = set<CheckT>;
    constexpr explicit impl() = default;
    constexpr static void pair() noexcept;
    constexpr static void type() noexcept;
};

template <template <typename> typename CheckT, template <typename> typename CheckV, typename T,
          typename V, typename... L>
struct impl<CheckT, CheckV, T, V, L...> : impl<CheckT, CheckV, L...> {
    using check = typename CheckV<V>::type;
    using base = impl<CheckT, CheckV, L...>;
    using selectors = typename base::selectors::template insert<T>;
    static_assert(not base::selectors ::template test<T>);

    constexpr impl(const impl<CheckT, CheckV, L...>& b, const check& v) : base(b), val_(v) {}
    using base::pair;
    constexpr const auto& pair(wrap<T>) const noexcept { return val_; }
    using base::type;
    constexpr auto type(wrap<T>) const noexcept { return wrap<V>{}; }

private:
    check val_;
};

}  // namespace val_impl

template <template <typename> typename CheckT, template <typename> typename CheckV,
          typename... L>
class val {
    template <template <typename> typename, template <typename> typename, typename...>
    friend class val;
    using impl = val_impl::impl<CheckT, CheckV, L...>;
    impl val_;

    constexpr explicit val(const impl& v) : val_(v) {}

public:
    constexpr val() = default;
    template <typename T, typename V> constexpr auto insert(const V& v) const noexcept
    {
        using result = val<CheckT, CheckV, T, V, L...>;
        using rimpl = typename result::impl;
        return result(rimpl(val_, v));
    }

    using set = typename impl::selectors;
    template <typename U> using type = typename decltype(impl::type(val_impl::wrap<U>()))::type;

    template <typename U> constexpr auto get() const noexcept
    {
        return val_.pair(val_impl::wrap<U>());
    }

    template <typename U, typename... A>
    constexpr auto run(A&&... a) const
        -> decltype(val_.pair(val_impl::wrap<U>())(std::forward<A>(a)...))
    {
        return get<U>()(std::forward<A>(a)...);
    }
};

struct IFuzzer {
    virtual ~IFuzzer() = default;
    virtual std::string fuzz(std::string) const = 0;
};

struct Fuz {};
struct Baz {};

template <typename T> struct PlainTypes {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(not std::is_pointer_v<T>);
    using type = T;
};

template <typename T> struct PlainNotVoid {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    static_assert(not std::is_void_v<T>);
    using type = T;
};

struct Foo {
    template <typename... L,
              typename
              = std::enable_if_t<map<PlainTypes, PlainNotVoid, L...>::set::template test<Fuz>>>
    explicit Foo(const val<PlainTypes, PlainNotVoid, L...>& v)
    {
        fuzz = v.template run<Fuz>(v.template get<Baz>());
    }

    std::unique_ptr<IFuzzer> fuzz;
};

struct Fuzzer : IFuzzer {
    const int i = {};
    explicit Fuzzer(int i) : i(i) {}
    std::string fuzz(std::string n) const override { return n + std::to_string(i); }
};

struct Ver1 {};
struct Ver2 {};

template <typename T> struct PlatformSel {
    using type = T;
};
template <typename T> struct MarketDataSel {
    using type = T;
};

template <typename T> void foo(T);

template <typename... L> void foo(const val<PlainTypes, PlainNotVoid, L...>& v)
{
    auto fuz = v.template run<Fuz>(v.template get<Baz>());
    std::cout << fuz.fuzz("Hello ") << std::endl;
    using Val = std::decay_t<decltype(v)>;
    std::cout << "foo " << Val::set::size << ' ' << typeid(Val).name() << std::endl;
}

template <typename... L> void foo(const val<MarketDataSel, PlainNotVoid, L...>& v)
{
    using Val = std::decay_t<decltype(v)>;
    std::cout << "foo " << Val::set::size << ' ' << typeid(Val).name() << std::endl;
}

template <typename... L>
std::enable_if_t<val<PlatformSel, PlainNotVoid, L...>::set::template test<Ver1>> foo(
    const val<PlatformSel, PlainNotVoid, L...>& v)
{
    using Val = std::decay_t<decltype(v)>;
    std::cout << "foo Ver1 " << Val::set::size << ' ' << typeid(Val).name() << std::endl;
}

template <typename... L>
std::enable_if_t<val<PlatformSel, PlainNotVoid, L...>::set::template test<Ver2>> foo(
    const val<PlatformSel, PlainNotVoid, L...>& v)
{
    using Val = std::decay_t<decltype(v)>;
    std::cout << "foo Ver2 " << Val::set::size << ' ' << typeid(Val).name() << std::endl;
}

template <typename Context> void baz(const Context& f)
{
    using Far = std::decay_t<decltype(f.logger)>;
    std::cout << "baz " << typeid(Far).name() << std::endl;
}

struct Logger {};

struct MyContext {
    Logger& logger;
};

struct CommandLine {
    CommandLine(int argv, char* const* argc)
    {  // ...
    }
};

enum ConfigurationSelector {};
struct Configuration {
    template <typename... L>
    explicit Configuration(const val<PlainTypes, PlainNotVoid, L...>&, const CommandLine&)
    {  // ...
    }
};

enum PlatformServicesSelector {};
struct PlatformServices {
    template <typename... L>
    explicit PlatformServices(const val<PlainTypes, PlainNotVoid, L...>& v)
    {
        const auto& conf = v.template run<ConfigurationSelector>(v);
    }
};

enum MarketPriceInputsSelector {};
struct MarketPriceInputs {
    template <typename... L>
    explicit MarketPriceInputs(const val<PlainTypes, PlainNotVoid, L...>& v)
    {
        const auto& conf = v.template run<ConfigurationSelector>(v);
        const auto& plat = v.template run<PlatformServicesSelector>(v);
    }
};

enum ClientPriceCalcSelector {};
struct ClientPriceCalc {
    template <typename... L>
    explicit ClientPriceCalc(const val<PlainTypes, PlainNotVoid, L...>& v)
    {
        const auto& conf = v.template run<ConfigurationSelector>(v);
        const auto& mark = v.template run<MarketPriceInputsSelector>(v);
    }
};

enum NetworkOutputSelector {};
struct NetworkOutput {
    template <typename... L>
    explicit NetworkOutput(const val<PlainTypes, PlainNotVoid, L...>& v)
    {
        const auto& conf = v.template run<ConfigurationSelector>(v);
        const auto& plat = v.template run<PlatformServicesSelector>(v);
    }
};

struct ClientPriceOutput {
    template <typename... L>
    explicit ClientPriceOutput(const val<PlainTypes, PlainNotVoid, L...>& v)
    {
        const auto& conf = v.template run<ConfigurationSelector>(v);
        const auto& price = v.template run<ClientPriceCalcSelector>(v);
        auto&       out = v.template run<NetworkOutputSelector>(v);
    }
};

int main(int argv, char** argc)
{
    constexpr static auto m1 = val<PlainTypes, PlainNotVoid>{}
                                   .insert<Fuz>([](int v) { return Fuzzer(v); })
                                   .insert<Baz>(13);
    foo(m1);

    try {
        const auto cmdln = CommandLine(argv, argc);
        // clang-format off
        const auto v = val<PlainTypes, PlainNotVoid>{}
            .insert<ConfigurationSelector>([cmdln](const auto& v) -> const Configuration& {
                static auto config = Configuration(v, cmdln);
                return config;
            }).insert<PlatformServicesSelector>([](const auto& v) -> const PlatformServices& {
                static auto plsrv = PlatformServices(v);
                return plsrv;
            }).insert<MarketPriceInputsSelector>([](const auto& v) -> const MarketPriceInputs& {
                static auto input = MarketPriceInputs(v);
                return input;
            }).insert<ClientPriceCalcSelector>([](const auto& v) -> const ClientPriceCalc& {
                static auto price = ClientPriceCalc(v);
                return price;
            }).insert<NetworkOutputSelector>([](const auto& v) -> NetworkOutput& {
                static auto output = NetworkOutput(v);
                return output;
            });
        // clang-format on
        const auto clout = ClientPriceOutput(v);
    }
    catch (...) {
        return 1;
    }
    return 0;
}

#define TEST(x) void testFunction(const char* const s = x)

struct MockConfiguration {};
struct MockPlatformServices {
    void push(const char* const) {}
};

TEST("Test MarketPriceInputs")
{
    auto        plsrv = MockPlatformServices();
    const auto* pplsrv = &plsrv;
    // clang-format off
    const auto  v = val<PlainTypes, PlainNotVoid>{}
        .insert<ConfigurationSelector>([](const auto&) {
            static auto config = MockConfiguration();
            return config;
        }).insert<PlatformServicesSelector>([pplsrv](const auto&) { return *pplsrv; });
    // clang-format on
    auto input = MarketPriceInputs(v);
    plsrv.push("Some test inputs");
}
