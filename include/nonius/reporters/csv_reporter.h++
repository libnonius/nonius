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

// CSV raw data reporter

#ifndef NONIUS_CSV_REPORTER_HPP
#define NONIUS_CSV_REPORTER_HPP

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
#include <fstream>

#include <string>
#include <limits>
#include <unordered_map>
#include <vector>

namespace nonius {
    struct csv_reporter : reporter {
    public:
        void configure(configuration cfg) override {
            n_samples = cfg.samples;
            if(cfg.output_file.empty()) {
                fs = &std::cout;
            } else {
                fs = std::unique_ptr<std::ofstream>(new std::ofstream(cfg.output_file));
            }
        }

        void benchmark_start(std::string const& name) override {
            current = name;
        }

        void measurement_complete(std::vector<fp_seconds> const& samples) override {
            data[current] = samples;
        }

        void suite_complete() override {
            stream() << std::fixed;
            stream().precision(std::numeric_limits<double>::digits10);
            bool first = true;
            for(auto&& kv : data) {
                if(!first) stream() << ", ";
                stream() << "\"" << kv.first << "\""; // TODO escape
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

        int n_samples;
        std::string current;
        std::unordered_map<std::string, std::vector<fp_seconds>> data;
        boost::variant<std::ostream*, std::unique_ptr<std::ofstream>> fs;
    };
} // namespace nonius

#endif // NONIUS_CSV_REPORTER_HPP

