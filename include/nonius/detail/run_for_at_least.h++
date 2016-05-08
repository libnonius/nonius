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

// Run a function for a minimum amount of time

#ifndef NONIUS_RUN_FOR_AT_LEAST_HPP
#define NONIUS_RUN_FOR_AT_LEAST_HPP

#include <nonius/clock.h++>
#include <nonius/chronometer.h++>
#include <nonius/detail/measure.h++>
#include <nonius/detail/complete_invoke.h++>
#include <nonius/detail/timing.h++>

#include <utility>
#include <type_traits>

namespace nonius {
    namespace detail {
        template <typename Clock, typename Fun>
        TimingOf<Clock, Fun(int)> run_for_at_least_impl(Duration<Clock> how_long, int seed, Fun&& fun, std::false_type) {
            auto iters = seed;
            while(true) {
                auto r = detail::measure<Clock>(fun, iters);
                if(r.elapsed >= how_long) {
                    return { r.elapsed, std::move(r.result), iters };
                }
                iters *= 2;
            }
        }
        template <typename Clock, typename Fun>
        TimingOf<Clock, Fun(chronometer)> run_for_at_least_impl(Duration<Clock> how_long, int seed, Fun&& fun, std::true_type) {
            auto iters = seed;
            while(true) {
                detail::chronometer_model<Clock> meter;
                auto&& result = detail::complete_invoke(fun, chronometer(meter, iters));

                if(meter.elapsed() >= how_long) {
                    return { meter.elapsed(), std::move(result), iters };
                }
                iters *= 2;
            }
        }

        template <typename Clock, typename Fun>
        using run_for_at_least_argument_t = typename std::conditional<detail::is_callable<Fun(chronometer)>::value, chronometer, int>::type;

        template <typename Clock = default_clock, typename Fun>
        TimingOf<Clock, Fun(run_for_at_least_argument_t<Clock, Fun>)> run_for_at_least(Duration<Clock> how_long, int seed, Fun&& fun) {
            return run_for_at_least_impl<Clock>(how_long, seed, std::forward<Fun>(fun), detail::is_callable<Fun(chronometer)>());
        }
    } // namespace detail
} // namespace nonius

#endif // NONIUS_RUN_FOR_AT_LEAST_HPP

