// Copyright (c) 2018 Bronek (Bronislaw) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "juno/map.hpp"
#include "juno/val.hpp"

using namespace juno;

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
