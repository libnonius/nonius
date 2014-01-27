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
#include <boost/variant.hpp>

#include <ratio>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>

namespace nonius {
    struct standard_reporter : reporter {
    public:
        void configure(configuration cfg) override {
            n_samples = cfg.samples;
            n_resamples = cfg.resamples;
            if(cfg.output_file.empty()) {
                fs = &std::cout;
            } else {
                fs = std::unique_ptr<std::ofstream>(new std::ofstream(cfg.output_file));
            }
        }

        void estimate_clock_resolution_start() override {
            stream() << "estimating clock resolution...\n";
        }
        void estimate_clock_resolution_complete(environment_estimate<fp_seconds> estimate) override {
            print_environment_estimate(estimate, estimate.outliers.samples_seen + 2);
        }

        void estimate_clock_cost_start() override {
            stream() << "estimating cost of a clock call...\n";
        }
        void estimate_clock_cost_complete(environment_estimate<fp_seconds> estimate) override {
            print_environment_estimate(estimate, estimate.outliers.samples_seen);
        }

        void benchmark_start(std::string const& name) override {
            stream() << "\nbenchmarking " << name << "\n";
        }

        void measurement_start(execution_plan<fp_seconds> plan) override {
            stream() << std::setprecision(7);
            stream().unsetf(std::ios::floatfield);
            stream() << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << pretty_duration(plan.estimated_duration) << "\n";
        }
        void analysis_start() override {
            stream() << "bootstrapping with " << n_resamples << " resamples\n";
        }
        void analysis_complete(sample_analysis<fp_seconds> const& analysis) override {
            print_statistic_estimate("mean", analysis.mean);
            print_statistic_estimate("std dev", analysis.standard_deviation);
            print_outliers(analysis.outliers);
            stream() << "variance introduced by outliers: " << percentage(analysis.outlier_variance) << "\n";
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
            stream() << "variance is " << effect << " by outliers\n";
        }

    private:
        static std::string pretty_duration(fp_seconds secs) {
            using fp_millis = boost::chrono::duration<double, boost::milli>;
            using fp_micros = boost::chrono::duration<double, boost::micro>;
            using fp_nanos = boost::chrono::duration<double, boost::nano>;

            std::ostringstream ss;
            ss << std::setprecision(ss.precision());
            if(secs.count() >= 2.) {
                ss << secs.count() << " s";
            } else if(secs.count() >= .002) {
                ss << fp_millis(secs).count() << " ms";
            } else if(secs.count() >= .000002) {
                ss << fp_micros(secs).count() << u8" μs";
            } else {
                ss << fp_nanos(secs).count() << " ns";
            }
            return ss.str();
        }
        static std::string percentage(double d) {
            std::ostringstream ss;
            ss << std::setprecision(3);
            ss.unsetf(std::ios::floatfield);
            ss << (100. * d) << "%";
            return ss.str();
        }
        static std::string percentage_ratio(double part, double whole) {
            return percentage(part / whole);
        }
        void print_environment_estimate(environment_estimate<fp_seconds> e, int iterations) {
            stream() << std::setprecision(7);
            stream().unsetf(std::ios::floatfield);
            stream() << "mean is " << pretty_duration(e.mean) << " (" << iterations << " iterations)\n";
            print_outliers(e.outliers);
        }
        void print_outlier_count(const char* description, int count, int total) {
            if(count > 0) stream() << "  " << count << " (" << percentage_ratio(count, total) << ") " << description << "\n";
        }
        void print_outliers(outlier_classification o) {
            stream() << "found " << o.total() << " outliers among " << o.samples_seen << " samples (" << percentage_ratio(o.total(), o.samples_seen) << ")\n";
            print_outlier_count("low severe", o.low_severe, o.samples_seen);
            print_outlier_count("low mild", o.low_mild, o.samples_seen);
            print_outlier_count("high mild", o.high_mild, o.samples_seen);
            print_outlier_count("high severe", o.high_severe, o.samples_seen);
        }
        void print_statistic_estimate(const char* name, estimate<fp_seconds> estimate) {
            stream() << std::setprecision(7);
            stream().unsetf(std::ios::floatfield);
            stream() << name << ": " << pretty_duration(estimate.point)
                      << ", lb " << pretty_duration(estimate.lower_bound)
                      << ", ub " << pretty_duration(estimate.upper_bound)
                      << ", ci " << std::setprecision(3) << estimate.confidence_interval
                      << "\n";
        }

        struct os_visitor : boost::static_visitor<std::ostream&> {
            std::ostream& operator()(std::ostream* os) const {
                return *os;
            }
            std::ostream& operator()(std::unique_ptr<std::ofstream>& fs) const {
                return *fs;
            }
        };
        std::ostream& stream() {
            return boost::apply_visitor(os_visitor(), fs);
        }

        int n_samples = 0;
        int n_resamples = 0;
        boost::variant<std::ostream*, std::unique_ptr<std::ofstream>> fs;
    };
} // namespace nonius

#endif // NONIUS_STDOUT_REPORTER_HPP
