// Nonius - C++ benchmarking tool
//
// Written in 2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Benchmark running

#ifndef NONIUS_RUN_HPP
#define NONIUS_RUN_HPP

#include <nonius/clock.h++>
#include <nonius/environment.h++>
#include <nonius/measurement.h++>
#include <nonius/configuration.h++>
#include <nonius/detail/duration.h++>
#include <nonius/detail/repeat.h++>

#include <chrono>
#include <algorithm>
#include <vector>
#include <iterator>
#include <string>
#include <functional>
#include <utility>

namespace nonius {
    namespace detail {
    } // namespace detail

    template <typename Fun>
    struct benchmark {
        std::string description;
        Fun function;

        void operator()(int k) const {
            detail::repeat(std::ref(function))(k);
        }
    };
    template <typename Fun>
    benchmark<wheels::meta::Decay<Fun>> make_benchmark(std::string description, Fun&& fun) {
        return { std::move(description), std::forward<Fun>(fun) };
    }

    template <typename Clock>
    using sample = std::vector<FloatDuration<Clock>>;

    template <typename Clock, typename Fun>
    sample<Clock> run_benchmark(configuration cfg, environment<Clock> env, benchmark<Fun> const& benchmark) {
        int warmup_iters = 10000;
        run_for_at_least<Clock>(std::chrono::milliseconds(100), warmup_iters, detail::repeat(now<Clock>{}));
        auto min_time = env.clock_resolution * 1000;
        auto run_time = std::min(min_time, decltype(min_time)(std::chrono::milliseconds(100)));
        auto&& test = run_for_at_least<Clock>(std::chrono::duration_cast<Duration<Clock>>(run_time), 1, benchmark);
        auto new_iters = std::ceil(min_time * test.iterations / test.elapsed);
        std::vector<FloatDuration<Clock>> times;
        times.reserve(cfg.samples);
        std::generate_n(std::back_inserter(times), cfg.samples, [&benchmark, env, new_iters]{
                auto t = time<Clock>(benchmark, new_iters).elapsed;
                return (t - env.clock_cost) / new_iters;
        });
        return times;
    }

    //template <typename Clock>
    //void run_analyse1(configuration cfg, environment<Clock> env, benchmark<Fun> const& benchmark) {
    //    auto ci = cfg.confidence_interval;
    //}
} // namespace nonius

#endif // NONIUS_RUN_HPP


