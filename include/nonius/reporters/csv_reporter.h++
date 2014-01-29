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

// CSV raw data reporter

#ifndef NONIUS_REPORTERS_CSV_REPORTER_HPP
#define NONIUS_REPORTERS_CSV_REPORTER_HPP

#include <nonius/reporter.h++>
#include <nonius/configuration.h++>
#include <nonius/sample_analysis.h++>
#include <nonius/execution_plan.h++>
#include <nonius/environment.h++>

#include <ios>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <vector>

namespace nonius {
    struct csv_reporter : reporter {
    private:
        void do_configure(configuration cfg) override {
            n_samples = cfg.samples;
        }

        void do_benchmark_start(std::string const& name) override {
            current = name;
        }

        void do_measurement_complete(std::vector<fp_seconds> const& samples) override {
            data[current] = samples;
        }

        void do_suite_complete() override {
            stream() << std::fixed;
            stream().precision(std::numeric_limits<double>::digits10);
            bool first = true;
            for(auto&& kv : data) {
                if(!first) stream() << ", ";
                stream() << "\"" << escape(kv.first) << "\""; // TODO escape
                first = false;
            }
            stream() << "\n";
            for(int i = 0; i < n_samples; ++i) {
                first = true;
                for(auto&& kv : data) {
                    if(!first) stream() << ", ";
                    stream() << kv.second[i].count();
                    first = false;
                }
                stream() << "\n";
            }
        }

    private:
        static std::string escape(std::string const& source) {
            auto first = source.begin();
            auto last = source.end();

            int quotes = std::count(first, last, '"');
            if(quotes == 0) return source;

            std::string escaped;
            escaped.reserve(source.size() + quotes);

            while(first != last) {
                auto next_quote = std::find(first, last, '"');
                std::copy(first, next_quote, std::back_inserter(escaped));
                first = next_quote;
                if(first != last) {
                    ++first;
                    escaped.push_back('"');
                    escaped.push_back('"');
                }
            }

            return escaped;
        }

        int n_samples;
        std::string current;
        std::unordered_map<std::string, std::vector<fp_seconds>> data;
    };

    NONIUS_REPORTER("csv", csv_reporter);
} // namespace nonius

#endif // NONIUS_REPORTERS_CSV_REPORTER_HPP

