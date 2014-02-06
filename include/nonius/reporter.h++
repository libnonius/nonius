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

// Reporter interface

#ifndef NONIUS_REPORTER_HPP
#define NONIUS_REPORTER_HPP

#include <nonius/clock.h++>
#include <nonius/outlier_classification.h++>
#include <nonius/environment.h++>
#include <nonius/execution_plan.h++>
#include <nonius/sample_analysis.h++>

#include <boost/variant.hpp>

#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace nonius {
    struct reporter {
    public:
        virtual ~reporter() = default;

        void configure(configuration& cfg) {
            if(cfg.output_file.empty()) {
                os = &std::cout;
            } else {
                os = std::unique_ptr<std::ostream>(new std::ofstream(cfg.output_file));
            }
            do_configure(cfg);
        }

        void warmup_start() {
            do_warmup_start();
        }
        void warmup_end(int iterations) {
            do_warmup_end(iterations);
        }
        void estimate_clock_resolution_start() {
            do_estimate_clock_resolution_start();
        }
        void estimate_clock_resolution_complete(environment_estimate<fp_seconds> estimate) {
            do_estimate_clock_resolution_complete(estimate);
        }

        void estimate_clock_cost_start() {
            do_estimate_clock_cost_start();
        }
        void estimate_clock_cost_complete(environment_estimate<fp_seconds> estimate) {
            do_estimate_clock_cost_complete(estimate);
        }

        void suite_start() {
            do_suite_start();
        }
        void benchmark_start(std::string const& name) {
            do_benchmark_start(name);
        }

        void measurement_start(execution_plan<fp_seconds> plan) {
            do_measurement_start(plan);
        }
        void measurement_complete(std::vector<fp_seconds> const& samples) {
            do_measurement_complete(samples);
        }

        void analysis_start() {
            do_analysis_start();
        }
        void analysis_complete(sample_analysis<fp_seconds> const& analysis) {
            do_analysis_complete(analysis);
        }

        void benchmark_complete() {
            do_benchmark_complete();
        }
        void suite_complete() {
            do_suite_complete();
        }

        virtual std::string description() = 0;

    private:
        virtual void do_configure(configuration& /*cfg*/) {}

        virtual void do_warmup_start() {}
        virtual void do_warmup_end(int /*iterations*/) {}

        virtual void do_estimate_clock_resolution_start() {}
        virtual void do_estimate_clock_resolution_complete(environment_estimate<fp_seconds> /*estimate*/) {}

        virtual void do_estimate_clock_cost_start() {}
        virtual void do_estimate_clock_cost_complete(environment_estimate<fp_seconds> /*estimate*/) {}

        virtual void do_suite_start() {}
        virtual void do_benchmark_start(std::string const& /*name*/) {}

        virtual void do_measurement_start(execution_plan<fp_seconds> /*plan*/) {}
        virtual void do_measurement_complete(std::vector<fp_seconds> const& /*samples*/) {}

        virtual void do_analysis_start() {} // TODO make generic?
        virtual void do_analysis_complete(sample_analysis<fp_seconds> const& /*analysis*/) {}

        virtual void do_benchmark_complete() {}
        virtual void do_suite_complete() {}

    private:
        struct stream_visitor : boost::static_visitor<std::ostream&> {
            std::ostream& operator()(std::ostream* os) const { return *os; }
            std::ostream& operator()(std::unique_ptr<std::ostream>& os) const { return *os; }
        };

    protected:
        std::ostream& stream() {
            return boost::apply_visitor(stream_visitor(), os);
        }

    private:
        boost::variant<std::ostream*, std::unique_ptr<std::ostream>> os;
    };

    inline std::unordered_map<std::string, std::unique_ptr<reporter>>& reporter_registry() {
        static std::unordered_map<std::string, std::unique_ptr<reporter>> registry;
        return registry;
    }

    struct reporter_registrar {
        reporter_registrar(std::string name, reporter* registrant) {
            reporter_registry().emplace(std::move(name), std::unique_ptr<reporter>(registrant));
        }
    };
} // namespace nonius

#define NONIUS_DETAIL_UNIQUE_NAME_LINE2(name, line) NONIUS_ ## name ## _ ## line
#define NONIUS_DETAIL_UNIQUE_NAME_LINE(name, line) NONIUS_DETAIL_UNIQUE_NAME_LINE2(name, line)
#define NONIUS_DETAIL_UNIQUE_NAME(name) NONIUS_DETAIL_UNIQUE_NAME_LINE(name, __LINE__)

#define NONIUS_REPORTER(name, ...) \
    namespace { static ::nonius::reporter_registrar NONIUS_DETAIL_UNIQUE_NAME(reporter_registrar) (name, new __VA_ARGS__()); } \
    static_assert(true, "")

#endif // NONIUS_REPORTER_HPP

