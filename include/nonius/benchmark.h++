// Nonius - C++ benchmarking tool
//
// Written in 2014- by the nonius contributors <nonius@rmf.io>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Benchmark

#ifndef NONIUS_BENCHMARK_HPP
#define NONIUS_BENCHMARK_HPP

#include <nonius/clock.h++>
#include <nonius/configuration.h++>
#include <nonius/environment.h++>
#include <nonius/execution_plan.h++>
#include <nonius/chronometer.h++>
#include <nonius/param.h++>
#include <nonius/optimizer.h++>
#include <nonius/detail/benchmark_function.h++>
#include <nonius/detail/repeat.h++>
#include <nonius/detail/run_for_at_least.h++>
#include <nonius/detail/unique_name.h++>

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <cmath>

namespace nonius {
    namespace detail {
        const auto warmup_iterations = 10000;
        const auto warmup_time = chrono::milliseconds(100);
        const auto minimum_ticks = 1000;
    } // namespace detail

    struct benchmark {
        benchmark(std::string name, detail::benchmark_function fun)
        : name(std::move(name)), fun(std::move(fun)) {}

        template <typename Clock>
        execution_plan<FloatDuration<Clock>> prepare(configuration cfg, parameters params, environment<FloatDuration<Clock>> env) const {
            auto bench = fun(params);
            auto min_time = env.clock_resolution.mean * detail::minimum_ticks;
            auto run_time = std::max(min_time, chrono::duration_cast<decltype(min_time)>(detail::warmup_time));
            auto&& test = detail::run_for_at_least<Clock>(params, chrono::duration_cast<Duration<Clock>>(run_time), 1, bench);
            int new_iters = static_cast<int>(std::ceil(min_time * test.iterations / test.elapsed));
            return { new_iters, test.elapsed / test.iterations * new_iters * cfg.samples, params, bench, chrono::duration_cast<FloatDuration<Clock>>(detail::warmup_time), detail::warmup_iterations };
        }

        std::string name;
        detail::benchmark_function fun;
    };

    using benchmark_registry = std::vector<benchmark>;

    inline benchmark_registry& global_benchmark_registry() {
        static benchmark_registry registry;
        return registry;
    }

    struct benchmark_registrar {
        benchmark_registry& registry;
        std::string name;

        benchmark_registrar(benchmark_registry& r, std::string n)
            : registry(r)
            , name(n)
        {}

        template <typename Fun>
        benchmark_registrar const& operator+ (Fun&& registrant) const {
            registry.emplace_back(name, std::forward<Fun>(registrant));
            return *this;
        }
    };
} // namespace nonius

#define NONIUS_BENCHMARK_OLD_(name, ...)                                \
    namespace {                                                         \
    static auto NONIUS_DETAIL_UNIQUE_NAME(benchmark_registrar) =        \
        ::nonius::benchmark_registrar{::nonius::global_benchmark_registry(), name} + (__VA_ARGS__); \
    }                                                                   \
    //

#define NONIUS_BENCHMARK_NEW_(name)                                     \
    static auto NONIUS_DETAIL_UNIQUE_NAME(benchmark_registrar) =        \
        ::nonius::benchmark_registrar{::nonius::global_benchmark_registry(), name} + \
        //

#define NONIUS_BENCHMARK_CHOOSE_(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19,p20,p21,p22,p23,p24,p25,p26,p27,p28,p29,p30,p31,p32,p33,p34,p35,p36,p37,p38,p39,p40,p41,p42,p43,p44,p45,p46,p47,p48,p49,p50,p51,p52,p53,p54,p55,p56,p57,p58,p59,p60,p61,p62,p63,p64,p65,p66,p67,p68,p69,p70,p71,p72,p73,p74,p75,p76,p77,p78,p79,p80,p81,p82,p83,p84,p85,p86,p87,p88,p89,p90,p91,p92,p93,p94,p95,p96,p97,p98,p99,p100,p101,p102,p103,p104,p105,p106,p107,p108,p109,p110,p111,p112,p113,p114,p115,p116,p117,p118,p119,p120,p121,p122,p123,p124,p125,p126,p127,p128,N,...) N
#define NONIUS_BENCHMARK_CAT__(X1, X2) X1 ## X2
#define NONIUS_BENCHMARK_CAT_(X1, X2) NONIUS_BENCHMARK_CAT__(X1, X2)

#define NONIUS_BENCHMARK(...) NONIUS_BENCHMARK_CAT_(NONIUS_BENCHMARK_,NONIUS_BENCHMARK_CHOOSE_(__VA_ARGS__,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,NEW_,ERR_))(__VA_ARGS__)

#endif // NONIUS_BENCHMARK_HPP
