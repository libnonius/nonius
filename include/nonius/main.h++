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

// Executable building kit

#ifndef NONIUS_MAIN_HPP
#define NONIUS_MAIN_HPP

#include <nonius/nonius.h++>
#include <nonius/detail/argparse.h++>

#include <vector>
#include <string>
#include <stdexcept>
#include <exception>
#include <iostream>
#include <iomanip>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename T>
        struct parser;
        template <>
        struct parser<int> {
            static int parse(std::string const& s) { return std::stoi(s); }
        };
        template <>
        struct parser<double> {
            static double parse(std::string const& s) { return std::stod(s); }
        };
        template <>
        struct parser<std::string> {
            static std::string parse(std::string const& s) { return s; }
        };
        template <>
        struct parser<bool> {
            static bool parse(std::string const&) { return true; }
        };
        template <typename T, typename Predicate>
        void parse(T& variable, detail::arguments& args, std::string const& option, Predicate&& is_valid) {
            auto it = args.find(option);
            if(it != args.end()) {
                auto value = parser<T>::parse(it->second);
                if(is_valid(value)) {
                    variable = value;
                } else {
                    throw std::out_of_range("invalid command-line argument");
                }
            }
        }
        template <typename T>
        void parse(T& variable, detail::arguments& args, std::string const& option) {
            return parse(variable, args, option, [](T const&) { return true; });
        }

        struct argument_error {
            virtual ~argument_error() = default;
        };

        detail::option_set command_line_options {
            detail::option("help", "h", "show this help message"),
            detail::option("samples", "s", "number of samples to collect (default: 100)", "SAMPLES"),
            detail::option("resamples", "rs", "number of resamples for the bootstrap (default: 100000)", "RESAMPLES"),
            detail::option("confidence-interval", "ci", "confidence interval for the bootstrap (between 0 and 1, default: 0.95)", "INTERVAL"),
            detail::option("output", "o", "output file (default: <stdout>)", "FILE"),
            detail::option("reporter", "r", "reporter to use (default: standard)", "REPORTER"),
            detail::option("list", "l", "list benchmarks"),
            detail::option("list-reporters", "lr", "list available reporters"),
            detail::option("quiet", "q", "suppress text output"),
        };

        template <typename Iterator>
        configuration parse_args(std::string const& name, Iterator first, Iterator last) {
            try {
                auto args = detail::parse_arguments(command_line_options, first, last);

                configuration cfg;

                auto is_positive = [](int x) { return x > 0; };
                auto is_ci = [](double x) { return x > 0 && x < 1; };
                auto is_reporter = [](std::string const x) { return reporter_registry().count(x) > 0; };

                parse(cfg.help, args, "help");
                parse(cfg.list_benchmarks, args, "list");
                parse(cfg.list_reporters, args, "list-reporters");
                parse(cfg.samples, args, "samples", is_positive);
                parse(cfg.resamples, args, "resamples", is_positive);
                parse(cfg.confidence_interval, args, "confidence-interval", is_ci);
                parse(cfg.output_file, args, "output");
                parse(cfg.reporter, args, "reporter", is_reporter);
                parse(cfg.quiet, args, "quiet");

                return cfg;
            } catch(...) {
                std::cout << help_text(name, command_line_options);
                std::throw_with_nested(argument_error());
            }
        }
    } // namespace detail

    template <typename Iterator>
    int main(std::string const& name, Iterator first, Iterator last) {
        configuration cfg;
       
        try {
            cfg = detail::parse_args(name, first, last);
        } catch(detail::argument_error const&) {
            return 17;
        }

        if(cfg.help) {
            std::cout << detail::help_text(name, detail::command_line_options);
            return 0;
        }

        if(cfg.list_benchmarks) {
            for(auto&& b : benchmark_registry()) {
                std::cout << b.name << "\n";
            }
            return 0;
        }

        if(cfg.list_reporters) {
            using reporter_entry_ref = decltype(*reporter_registry().begin());
            auto cmp = [](reporter_entry_ref a, reporter_entry_ref b) { return a.first.size() < b.first.size(); };
            auto width = 2 + std::max_element(reporter_registry().begin(), reporter_registry().end(), cmp)->first.size();

            std::cout << std::left;
            for(auto&& r : reporter_registry()) {
                if(!r.first.empty()) {
                    std::cout << std::setw(width) << r.first << r.second->description() << "\n";
                }
            }
            return 0;
        }

        try {
            nonius::go(cfg, benchmark_registry().begin(), benchmark_registry().end());
        } catch(...) {
            // TODO output failure information
            return 23;
        }

        return 0;
    }
    inline int main(int argc, char** argv) {
        std::string name(argv[0]);
        std::vector<std::string> args(argv+1, argv+argc);
        return main(name, args.begin(), args.end());
    }
}

int main(int argc, char** argv) {
    return nonius::main(argc, argv);
}

#endif // NONIUS_MAIN_HPP

