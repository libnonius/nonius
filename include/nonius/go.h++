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

// Runner entry point

#ifndef NONIUS_GO_HPP
#define NONIUS_GO_HPP

#include <nonius/clock.h++>
#include <nonius/benchmark.h++>
#include <nonius/configuration.h++>
#include <nonius/environment.h++>
#include <nonius/reporter.h++>
#include <nonius/reporters/standard_reporter.h++>
#include <nonius/detail/estimate_clock.h++>
#include <nonius/detail/analyse.h++>

namespace nonius {
    namespace detail {
        template <typename Clock>
        environment<FloatDuration<Clock>> measure_environment(reporter& rep) {
            rep.estimate_clock_resolution_start();
            auto iters = detail::warmup<Clock>();
            auto resolution = detail::estimate_clock_resolution<Clock>(iters);
            rep.estimate_clock_resolution_complete(resolution);

            rep.estimate_clock_cost_start();
            auto cost = detail::estimate_clock_cost<Clock>(resolution.mean);
            rep.estimate_clock_cost_complete(cost);

            return { resolution, cost };
        }
    } // namespace detail
    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter& rep) {
        rep.configure(cfg);

        auto env = detail::measure_environment<Clock>(rep);

        rep.suite_start();

        for(; first != last; ++first) {
            rep.benchmark_start(first->name);

            auto plan = first->template prepare<Clock>(cfg, env);
            rep.measurement_start(plan);
            auto samples = first->template run<Clock>(cfg, env, plan);
            rep.measurement_complete(std::vector<fp_seconds>(samples.begin(), samples.end()));

            rep.analysis_start();
            auto analysis = detail::analyse(cfg, env, samples.begin(), samples.end());
            rep.analysis_complete(analysis);

            rep.benchmark_complete();
        }

        rep.suite_complete();
    }
    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter&& rep) {
        go(cfg, first, last, rep);
    }
    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last) {
        go(cfg, first, last, standard_reporter());
    }
} // namespace nonius

#endif // NONIUS_GO_HPP

