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
#include <nonius/detail/complete_invoke.h++>
#include <nonius/detail/noexcept.h++>

#include <set>
#include <exception>
#include <iostream>
#include <utility>
#include <regex>

namespace nonius {
    namespace detail {
        template <typename Clock>
        environment<FloatDuration<Clock>> measure_environment(reporter& rep) {
            rep.warmup_start();
            auto iters = detail::warmup<Clock>();
            rep.warmup_end(iters);

            rep.estimate_clock_resolution_start();
            auto resolution = detail::estimate_clock_resolution<Clock>(iters);
            rep.estimate_clock_resolution_complete(resolution);

            rep.estimate_clock_cost_start();
            auto cost = detail::estimate_clock_cost<Clock>(resolution.mean);
            rep.estimate_clock_cost_complete(cost);

            return { resolution, cost };
        }
    } // namespace detail

    struct benchmark_user_error : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "a benchmark failed to run successfully";
        }
    };

    template <typename Fun>
    detail::CompleteType<detail::ResultOf<Fun()>> user_code(reporter& rep, Fun&& fun) {
        try {
            return detail::complete_invoke(std::forward<Fun>(fun));
        } catch(...) {
            rep.benchmark_failure(std::current_exception());
            throw benchmark_user_error();
        }
    }

    template <typename Clock>
    void go_benchmark(configuration cfg, benchmark& bench, environment<FloatDuration<Clock>>& env, reporter& rep) {
        auto plan = user_code(rep, [&bench, &cfg, &env]{
            return bench.template prepare<Clock>(cfg, env);
        });

        rep.measurement_start(plan);
        auto samples = user_code(rep, [&bench, &cfg, &env, &plan]{
            return bench.template run<Clock>(cfg, env, plan);
        });
        rep.measurement_complete(std::vector<fp_seconds>(samples.begin(), samples.end()));

        if(!cfg.no_analysis) {
            rep.analysis_start();
            auto analysis = detail::analyse(cfg, env, samples.begin(), samples.end());
            rep.analysis_complete(analysis);
        }
    }

    template <typename Clock>
    void go_param(configuration cfg, benchmark& bench, environment<FloatDuration<Clock>>& env, reporter& rep) {
        assert(cfg.params.run);

        using stepper_t = std::function<std::string(std::string const&)>;

        auto&& run = *cfg.params.run;
        auto&& spec = detail::global_param_registry().specs.at(run.name).get();
        auto& value = cfg.params.map[run.name];
        auto step  = run.step;
        auto stepper = std::unordered_map<std::string, stepper_t> {
            {"+", [&] (std::string const& x) { return spec.plus(x, step); }},
            {"*", [&] (std::string const& x) { return spec.times(x, step); }},
        }.at(run.op);

        value = run.init;
        for (auto i = std::size_t{}; i < run.count; ++i) {
            rep.param_step_start(run.name, value);
            go_benchmark<Clock>(cfg, bench, env, rep);
            rep.param_step_complete(run.name, value);
            value = stepper(std::move(value));
        }
    }

    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter& rep) {
        rep.configure(cfg);

        auto env = detail::measure_environment<Clock>(rep);
        rep.suite_start();

        std::regex benchmark_pattern_matcher(cfg.filter_pattern);

        for(; first != last; ++first) {
            try {
                if(std::regex_match(first->name, benchmark_pattern_matcher)) {
                    rep.benchmark_start(first->name);

                    if (cfg.params.run)
                        go_param<Clock>(cfg, *first, env, rep);
                    else
                        go_benchmark<Clock>(cfg, *first, env, rep);

                    rep.benchmark_complete();
                }
            } catch(benchmark_user_error const&) {}
        }

        rep.suite_complete();
    }
    struct duplicate_benchmarks : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "two or more benchmarks with the same name were registered";
        }
    };
    template <typename Clock = default_clock, typename Iterator>
    void validate_benchmarks(Iterator first, Iterator last) {
        struct strings_lt_through_pointer {
            bool operator()(std::string* a, std::string* b) const { return *a < *b; };
        };
        std::set<std::string*, strings_lt_through_pointer> names;
        for(; first != last; ++first) {
            if(!names.insert(&first->name).second)
                throw duplicate_benchmarks();
        }
    }
    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter&& rep) {
        go(cfg, first, last, rep);
    }
    struct no_such_reporter : virtual std::exception {
        char const* what() const NONIUS_NOEXCEPT override {
            return "reporter could not be found";
        }
    };
    template <typename Clock = default_clock>
    void go(configuration cfg, benchmark_registry& benchmarks = global_benchmark_registry(), reporter_registry& reporters = global_reporter_registry()) {
        auto it = reporters.find(cfg.reporter);
        if(it == reporters.end()) throw no_such_reporter();
        validate_benchmarks(benchmarks.begin(), benchmarks.end());
        go(cfg, benchmarks.begin(), benchmarks.end(), *it->second);
    }
} // namespace nonius

#endif // NONIUS_GO_HPP
