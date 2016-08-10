#define NONIUS_RUNNER
#include <nonius.h++>

#include <string>
#include <vector>
#include <cmath>

NONIUS_PARAM(X, 1.0)

template <typename Fn>
struct volatilize_fn {
    Fn fn;
    auto operator()() const -> decltype(fn()) {
        volatile auto x = fn();
        return x;
    }
};

template <typename Fn>
auto volatilize(Fn&& fn) -> volatilize_fn<typename std::decay<Fn>::type> {
    return { std::forward<Fn>(fn) };
}

NONIUS_BENCHMARK("sin", [](nonius::chronometer meter)
{
    auto x = meter.param<X>();
    meter.measure(volatilize([&]() {
        return std::sin(x);
    }));
})

NONIUS_BENCHMARK("cos", [](nonius::chronometer meter)
{
    auto v = meter.param<X>();
    meter.measure(volatilize([&]() {
        return std::cos(v);
    }));
})

NONIUS_BENCHMARK("asin", [](nonius::chronometer meter)
{
    auto v = meter.param<X>();
    meter.measure(volatilize([&]() {
        return std::asin(v);
    }));
})

NONIUS_BENCHMARK("acos", [](nonius::chronometer meter)
{
    auto v = meter.param<X>();
    meter.measure(volatilize([&]() {
        return std::asin(v);
    }));
})

NONIUS_BENCHMARK("atan", [](nonius::chronometer meter)
{
    auto v = meter.param<X>();
    meter.measure(volatilize([&]() {
        return std::atan(v);
    }));
})

NONIUS_BENCHMARK("log2", [](nonius::chronometer meter)
{
    auto v = meter.param<X>();
    meter.measure(volatilize([&]() {
        return std::log2(v);
    }));
})

// alternative syntax
NONIUS_BENCHMARK("log", [](nonius::parameters params)
{
    auto v = params.get<X>();
    return volatilize([=]() {
        return std::log(v);
    });
})
