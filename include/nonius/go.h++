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
#include <nonius/detail/estimate_clock.h++>

namespace nonius {
    namespace detail {
        template <typename Clock>
        environment<FloatDuration<Clock>> measure_environment(reporter& r) {
            r.estimate_clock_resolution_start();
            auto iters = detail::warmup<Clock>();
            auto resolution = detail::estimate_clock_resolution<Clock>(iters);
            r.estimate_clock_resolution_complete(resolution);

            r.estimate_clock_cost_start();
            auto cost = detail::estimate_clock_cost<Clock>(resolution);
            r.estimate_clock_cost_complete(cost);

            return { resolution, cost };
        }
    } // namespace detail
    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last) {
        auto env = detail::measure_environment<Clock>(*cfg.reporter);

        cfg.reporter->suite_start();

        for(; first != last; ++first) {
            cfg.reporter->benchmark_start(first->name);

            auto plan = first->template prepare<Clock>(cfg, env);
            cfg.reporter->measurement_start(plan);
            auto samples = first->template run<Clock>(cfg, env, plan);
            cfg.reporter->measurement_complete(std::vector<fp_seconds>(samples.begin(), samples.end()));

            cfg.reporter->benchmark_complete();
        }

        cfg.reporter->suite_complete();
    }
} // namespace nonius

#endif // NONIUS_GO_HPP

