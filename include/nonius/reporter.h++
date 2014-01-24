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

// Reporter interface

#ifndef NONIUS_REPORTER_HPP
#define NONIUS_REPORTER_HPP

#include <nonius/clock.h++>
#include <nonius/outlier_classification.h++>
#include <nonius/environment.h++>
#include <nonius/execution_plan.h++>
#include <nonius/sample_analysis.h++>

#include <vector>
#include <string>

namespace nonius {
    struct reporter {
        virtual void configure(configuration /*cfg*/) {}

        virtual void estimate_clock_resolution_start() {}
        virtual void estimate_clock_resolution_complete(environment_estimate<fp_seconds> /*estimate*/) {}

        virtual void estimate_clock_cost_start() {}
        virtual void estimate_clock_cost_complete(environment_estimate<fp_seconds> /*estimate*/) {}

        virtual void suite_start() {}
        virtual void benchmark_start(std::string const& /*name*/) {}

        virtual void measurement_start(execution_plan<fp_seconds> /*measurement conditions*/) {}
        virtual void measurement_complete(std::vector<fp_seconds> const& /*samples*/) {}

        virtual void analysis_start() {} // TODO make generic?
        virtual void analysis_complete(sample_analysis<fp_seconds> const& /*analysis*/) {}

        virtual void benchmark_complete() {}
        virtual void suite_complete() {}

        virtual ~reporter() = default;
    };
} // namespace nonius

#endif // NONIUS_REPORTER_HPP

