// Nonius - C++ benchmarking tool
//
// Written in 2014- by the nonius contributors <nonius@rmf.io>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// HTML single-chart reporter

#ifndef NONIUS_REPORTERS_HTML_ALL_REPORTER_HPP
#define NONIUS_REPORTERS_HTML_ALL_REPORTER_HPP

#include <nonius/reporter.h++>
#include <nonius/configuration.h++>
#include <nonius/sample_analysis.h++>
#include <nonius/execution_plan.h++>
#include <nonius/environment.h++>
#include <nonius/detail/pretty_print.h++>
#include <nonius/detail/escape.h++>
#include <nonius/detail/cpptempl.h>

#include <ios>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <vector>

#include <fstream>

namespace nonius {
    struct html_reporter : reporter {
    private:
        static std::string const& template_string() {
            static char const* template_parts[] = {
// generated content broken into pieces because MSVC is in the 1990s.
#include <nonius/detail/html_report_template.g.h++>
            };
            static std::string const the_template = []() -> std::string {
                std::string s;
                for(auto part : template_parts) {
                    s += part;
                }
                return s;
            }();
            return the_template;
        }

        std::string description() override {
            return "outputs an HTML file with a single interactive chart of all benchmarks";
        }

        void do_configure(configuration& cfg) override {
            cfg.no_analysis = false;
            title = cfg.title;
            n_samples = cfg.samples;
            verbose = cfg.verbose;
            logarithmic = cfg.params.run && cfg.params.run->op == "*";
            run_param = cfg.params.run ? cfg.params.run->name : "";
        }

        void do_warmup_start() override {
            if(verbose) progress_stream() << "warming up\n";
        }
        void do_estimate_clock_resolution_start() override {
            if(verbose) progress_stream() << "estimating clock resolution\n";
        }
        void do_estimate_clock_cost_start() override {
            if(verbose) progress_stream() << "estimating cost of a clock call\n";
        }

        void do_params_start(parameters const& params) override {
            if(verbose) progress_stream() << "\n\nnew parameter round\n" << params;
            runs.emplace_back();
            runs.back().params = params;
        }
        void do_benchmark_start(std::string const& name) override {
            if(verbose) progress_stream() << "\nbenchmarking " << name << "\n";
            runs.back().data.push_back({name, {}, {}});
        }

        void do_measurement_start(execution_plan<fp_seconds> plan) override {
            progress_stream() << std::setprecision(7);
            progress_stream().unsetf(std::ios::floatfield);
            if(verbose) progress_stream() << "collecting " << n_samples << " samples, " << plan.iterations_per_sample << " iterations each, in estimated " << detail::pretty_duration(plan.estimated_duration) << "\n";
        }
        void do_measurement_complete(std::vector<fp_seconds> const& samples) override {
            runs.back().data.back().samples = samples;
        }
        void do_analysis_complete(sample_analysis<fp_seconds> const& analysis) override {
            runs.back().data.back().analysis = analysis;
        }
        void do_benchmark_failure(std::exception_ptr) override {
            error_stream() << runs.back().data.back().name << " failed to run successfully\n";
        }

        void do_suite_complete() override {
            if(verbose) progress_stream() << "\ngenerating HTML report\n";

            auto&& templ = template_string();
            auto magnitude = ideal_magnitude();

            cpptempl::data_map map;
            map["title"] = escape(title);
            map["units"] = detail::units_for_magnitude(magnitude);
            map["logarithmic"] = logarithmic;
            map["runparam"] = run_param;
            for (auto&& r : runs) {
                cpptempl::data_map run_item;
                cpptempl::data_list params;
                for (auto&& p : r.params) {
                    cpptempl::data_map item;
                    item["name"] = p.first;
                    item["value"] = p.second;
                    params.push_back(item);
                }
                run_item["params"] = cpptempl::make_data(params);
                for(auto&& d : r.data) {
                    cpptempl::data_map item;
                    item["name"] = escape(d.name);
                    cpptempl::data_map data;
                    if (!d.samples.empty()) {
                        data["mean"] = truncate(d.analysis.mean.point.count() * magnitude);
                        data["stddev"] = truncate(d.analysis.standard_deviation.point.count() * magnitude);
                        for(auto e : d.samples)
                            data["samples"].push_back(truncate(e.count() * magnitude));
                    }
                    item["data"] = data;
                    run_item["benchmarks"].push_back(item);
                }
                map["runs"].push_back(run_item);
            }

            cpptempl::parse(report_stream(), templ, map);
            report_stream() << std::flush;
            if(verbose) {
                progress_stream() << "\n\nresult summary ("
                                  << detail::units_for_magnitude(magnitude)
                                  << ")\n";
                for (auto&& r : runs) {
                    for (auto&& p : r.params)
                        progress_stream() << "\n  " << p.first << " = " << p.second;
                    progress_stream() << "\n";
                    for(auto&& d : r.data) {
                        progress_stream() << "  " << d.name << "\t "
                                  << truncate(d.analysis.mean.point.count() * magnitude) << "\t "
                                  << truncate(d.analysis.standard_deviation.point.count() * magnitude)
                                  << "\n";
                    }
                }
                progress_stream() << "\ndone\n";
                progress_stream() << std::flush;
            }
        }

        static double truncate(double x) {
            return std::trunc(x * 1000.) / 1000.;
        }

        double ideal_magnitude() const {
            std::vector<fp_seconds> mins;
            mins.reserve(runs.size() * runs.front().data.size());
            for (auto&& r : runs) {
                for(auto&& d : r.data) {
                    if (d.samples.begin() != d.samples.end())
                        mins.push_back(*std::min_element(d.samples.begin(), d.samples.end()));
                }
            }
            auto min = *std::min_element(mins.begin(), mins.end());
            return detail::get_magnitude(min);
        }

        static std::string escape(std::string const& source) {
            static const std::unordered_map<char, std::string> escapes {
                { '\'', "&apos;" },
                { '"',  "&quot;" },
                { '<',  "&lt;"   },
                { '>',  "&gt;"   },
                { '&',  "&amp;"  },
                { '\\',  "\\\\"  },
            };
            return detail::escape(source, escapes);
        }

        struct result_t {
            std::string name;
            std::vector<fp_seconds> samples;
            sample_analysis<fp_seconds> analysis;
        };

        struct run_t {
            parameters params;
            std::vector<result_t> data;
        };

        int n_samples;
        bool verbose;
        std::string title;
        bool logarithmic;
        std::string run_param;
        std::vector<run_t> runs;
    };

    NONIUS_REPORTER("html", html_reporter);
} // namespace nonius

#endif // NONIUS_REPORTERS_HTML_ALL_REPORTER_HPP
