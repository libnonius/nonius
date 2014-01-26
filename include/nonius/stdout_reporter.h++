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

// Standard output stream reporter

#ifndef NONIUS_STDOUT_REPORTER_HPP
#define NONIUS_STDOUT_REPORTER_HPP

#include <nonius/reporter.h++>
#include <nonius/configuration.h++>
#include <nonius/sample_analysis.h++>
#include <nonius/execution_plan.h++>
#include <nonius/environment.h++>

#include <boost/chrono.hpp>

#include <ratio>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

namespace nonius {
    struct stdout_reporter : reporter {
    public:
        void configure(configuration cfg) override {
            n_samples = cfg.samples;
            n_resamples = cfg.resamples;
        }

        void estimate_clock_resolution_start() override {
            std::cout << "estimating clock resolution...\n";
        }
        void estimate_clock_resolution_complete(environment_estimate<fp_seconds> estimate) override {
            print_environment_estimate(estimate, estimate.outliers.samples_seen + 2);
        }

        void estimate_clock_cost_start() override {
            std::cout << "estimating cost of a clock call...\n";
        }
        void estimate_clock_cost_complete(environment_estimate<fp_seconds> estimate) override {
            print_environment_estimate(estimate, estimate.outliers.samples_seen);
        }

        void benchmark_start(std::string const& name) override {
            std::cout << "\nbenchmarking " << name << "\n";
        }

        void measurement_start(execution_plan<fp_seconds> plan) override {
            std::cout << std::setprecision(7);
            std::cout.unsetf(std::ios::floatfield);
            std::cout << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << pretty_duration(plan.estimated_duration) << "\n";
        }
        void analysis_start() override {
            std::cout << "bootstrapping with " << n_resamples << " resamples\n";
        }
        void analysis_complete(sample_analysis<fp_seconds> const& analysis) override {
            print_statistic_estimate("mean", analysis.mean);
            print_statistic_estimate("std dev", analysis.standard_deviation);
            print_outliers(analysis.outliers);
            std::cout << "variance introduced by outliers: " << percentage(analysis.outlier_variance) << "\n";
            const char* effect;
            if(analysis.outlier_variance < 0.01) {
                effect = "unaffected";
            } else if(analysis.outlier_variance < 0.1) {
                effect = "slightly inflated";
            } else if(analysis.outlier_variance < 0.5) {
                effect = "moderately inflated";
            } else {
                effect = "severely inflated";
            }
            std::cout << "variance is " << effect << " by outliers\n";
        }

    private:
        static std::string pretty_duration(fp_seconds secs) {
            using fp_millis = boost::chrono::duration<double, boost::milli>;
            using fp_micros = boost::chrono::duration<double, boost::micro>;
            using fp_nanos = boost::chrono::duration<double, boost::nano>;

            std::ostringstream os;
            os << std::setprecision(std::cout.precision());
            if(secs.count() >= 2.) {
                os << secs.count() << " s";
            } else if(secs.count() >= .002) {
                os << fp_millis(secs).count() << " ms";
            } else if(secs.count() >= .000002) {
                os << fp_micros(secs).count() << u8" μs";
            } else {
                os << fp_nanos(secs).count() << " ns";
            }
            return os.str();
        }
        static std::string percentage(double d) {
            std::ostringstream os;
            os << std::setprecision(3);
            os.unsetf(std::ios::floatfield);
            os << (100. * d) << "%";
            return os.str();
        }
        static std::string percentage_ratio(double part, double whole) {
            return percentage(part / whole);
        }
        void print_environment_estimate(environment_estimate<fp_seconds> e, int iterations) {
            std::cout << std::setprecision(7);
            std::cout.unsetf(std::ios::floatfield);
            std::cout << "mean is " << pretty_duration(e.mean) << " (" << iterations << " iterations)\n";
            print_outliers(e.outliers);
        }
        void print_outlier_count(const char* description, int count, int total) {
            if(count > 0) std::cout << "  " << count << " (" << percentage_ratio(count, total) << ") " << description << "\n";
        }
        void print_outliers(outlier_classification o) {
            std::cout << "found " << o.total() << " outliers among " << o.samples_seen << " samples (" << percentage_ratio(o.total(), o.samples_seen) << ")\n";
            print_outlier_count("low severe", o.low_severe, o.samples_seen);
            print_outlier_count("low mild", o.low_mild, o.samples_seen);
            print_outlier_count("high mild", o.high_mild, o.samples_seen);
            print_outlier_count("high severe", o.high_severe, o.samples_seen);
        }
        void print_statistic_estimate(const char* name, estimate<fp_seconds> estimate) {
            std::cout << std::setprecision(7);
            std::cout.unsetf(std::ios::floatfield);
            std::cout << name << ": " << pretty_duration(estimate.point)
                      << ", lb " << pretty_duration(estimate.lower_bound)
                      << ", ub " << pretty_duration(estimate.upper_bound)
                      << ", ci " << std::setprecision(3) << estimate.confidence_interval
                      << "\n";
        }

        int n_samples = 0;
        int n_resamples = 0;
    };
} // namespace nonius

#endif // NONIUS_STDOUT_REPORTER_HPP
