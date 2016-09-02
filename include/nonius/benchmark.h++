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

#define NONIUS_BENCHMARK_CHOOSE_(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_100,_101,_102,_103,_104,_105,_106,_107,_108,_109,_110,_111,_112,_113,_114,_115,_116,_117,_118,_119,_120,_121,_122,_123,_124,_125,_126,_127,_128,N,...) N
#define NONIUS_BENCHMARK_CAT__(X1, X2) X1 ## X2
#define NONIUS_BENCHMARK_CAT_(X1, X2) NONIUS_BENCHMARK_CAT__(X1, X2)

#define NONIUS_BENCHMARK(...) NONIUS_BENCHMARK_CAT_(NONIUS_BENCHMARK_,NONIUS_BENCHMARK_CHOOSE_(__VA_ARGS__,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,OLD_,NEW_,ERR_))(__VA_ARGS__)

#endif // NONIUS_BENCHMARK_HPP
