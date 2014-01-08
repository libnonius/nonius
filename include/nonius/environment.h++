// Nonius - C++ benchmarking tool
//
// Written in 2013-2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Environment module

#ifndef NONIUS_ENVIRONMENT_HPP
#define NONIUS_ENVIRONMENT_HPP

#include <nonius/clock.h++>
#include <nonius/measurement.h++>
#include <nonius/analysis.h++>
#include <nonius/detail/duration.h++>
#include <nonius/detail/transform_if.h++>

#include <wheels/op/minus.h++>

#include <chrono>
#include <iterator>
#include <algorithm>
#include <tuple>
#include <vector>
#include <cmath>

namespace nonius {
    namespace detail {
        template <typename Clock>
        std::tuple<std::vector<Duration<Clock>>, int> resolution(int k) {
            std::vector<TimePoint<Clock>> times;
            times.reserve(k+1);
            std::generate_n(std::back_inserter(times), k+1, now<Clock>{});

            std::vector<Duration<Clock>> deltas;
            deltas.reserve(k);
            detail::transform_if(std::next(times.begin()), times.end(), times.begin(),
                              std::back_inserter(deltas),
                              wheels::op::minus{},
                              [](Duration<Clock> d) { return d > Duration<Clock>::zero(); });

            return std::make_tuple(std::move(deltas), times.size());
        }

        auto warmup_seed = 10000;
        auto warmup_time = std::chrono::milliseconds(100);
        auto clock_resolution_estimation_time = std::chrono::milliseconds(500);
        auto clock_cost_estimation_time_limit = std::chrono::seconds(1);
        auto clock_cost_estimation_tick_limit = 100000;
        auto clock_cost_estimation_time = std::chrono::milliseconds(10);
        auto clock_cost_estimation_iterations = 10000;
    } // namespace detail

    template <typename Clock>
    int warmup() {
        return run_for_at_least<Clock>(Duration<Clock>(detail::warmup_time), detail::warmup_seed, detail::resolution<Clock>)
                .iterations;
    }
    template <typename Clock>
    FloatDuration<Clock> estimate_clock_resolution(int iterations) {
        auto r = run_for_at_least<Clock>(Duration<Clock>(detail::clock_resolution_estimation_time), iterations, detail::resolution<Clock>)
                .result;
        return analyse_mean<Clock>(std::get<0>(r).begin(), std::get<0>(r).end());
    }
    template <typename Clock>
    FloatDuration<Clock> estimate_clock_cost(FloatDuration<Clock> resolution) {
        auto time_limit = std::min(resolution * detail::clock_cost_estimation_tick_limit, FloatDuration<Clock>(detail::clock_cost_estimation_time_limit));
        auto time_clock = [](int k) {
            return time<Clock>([k]{
                for(int i = 0; i < k; ++i) {
                    volatile auto ignored = Clock::now();
                    (void)ignored;
                }
            }).elapsed;
        };
        time_clock(1);
        int iters = detail::clock_cost_estimation_iterations;
        auto&& r = run_for_at_least<Clock>(Duration<Clock>(detail::clock_cost_estimation_time), iters, time_clock);
        std::vector<FloatDuration<Clock>> times;
        int nsamples = std::ceil(time_limit / r.elapsed);
        times.reserve(nsamples);
        std::generate_n(std::back_inserter(times), nsamples, [time_clock, &r]{ return time_clock(r.iterations) / r.iterations; });
        return analyse_mean<Clock>(times.begin(), times.end());
    }

    template <typename Clock>
    struct environment {
        FloatDuration<Clock> clock_resolution;
        FloatDuration<Clock> clock_cost;
    };

    template <typename Clock>
    environment<Clock> measure_environment() {
        auto iters = warmup<Clock>();
        auto resolution = estimate_clock_resolution<Clock>(iters);
        auto cost = estimate_clock_cost<Clock>(resolution);
        return { resolution, cost };
    }
} // namespace nonius

#endif // NONIUS_ENVIRONMENT_HPP

