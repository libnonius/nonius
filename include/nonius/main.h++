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

#include <vector>
#include <string>
#include <stdexcept>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename T, typename Fun, typename Iterator>
        T parse_next(Fun&& fun, Iterator& first, Iterator last) {
            if(first == last) { throw std::invalid_argument(""); }
            ++first;
            auto&& s = *first;
            size_t i;
            T n;
            try {
                n = fun(s, &i);
            } catch(std::out_of_range const&) { throw std::invalid_argument(s); }
            if(i != s.size()) { throw std::invalid_argument(s); }

            return n;
        }
        template <typename Iterator>
        int parse_next_int(Iterator& first, Iterator last) {
            return parse_next<int>([](std::string const& s, size_t* ip) { return std::stoi(s, ip); }, first, last);
        }
        template <typename Iterator>
        double parse_next_double(Iterator& first, Iterator last) {
            return parse_next<double>([](std::string const& s, size_t* ip) { return std::stod(s, ip); }, first, last);
        }
        template <typename Iterator>
        std::string parse_next_string(Iterator& first, Iterator last) {
            return parse_next<std::string>([](std::string const& s, size_t* ip) { return (*ip = s.size(), s); }, first, last);
        }

        template <typename Iterator>
        configuration do_parse_args(Iterator first, Iterator last) {
            configuration cfg;
            for(; first != last; ++first) {
                auto&& arg = *first;
                if(arg == "--samples" || arg == "-s") {
                    cfg.samples = parse_next_int(first, last);
                } else if(arg == "--resamples" || arg == "-rs") {
                    cfg.resamples = parse_next_int(first, last);
                } else if(arg == "--confidence-interval" || arg == "-ci") {
                    cfg.confidence_interval = parse_next_double(first, last);
                } else if(arg == "--output" || arg == "-o") {
                    cfg.output_file = parse_next_string(first, last);
                } else if(arg == "--reporter" || arg == "-r") {
                    cfg.reporter = parse_next_string(first, last);
                } else if(arg == "--quiet" || arg == "-q") {
                    cfg.quiet = true;
                } else if(arg == "--help" || arg == "-h") {
                    throw std::invalid_argument(arg); // HACK
                } else {
                    throw std::invalid_argument(arg);
                }
            }
            return cfg;
        }
        template <typename Iterator>
        configuration parse_args(Iterator first, Iterator last) {
            try {
                return do_parse_args(first, last);
            } catch(std::invalid_argument const&) {
                std::cout << "help text here\n";
                throw;
            }
        }
    } // namespace detail

    template <typename Iterator>
    int main(Iterator first, Iterator last) {
        configuration cfg = detail::parse_args(first, last);

        nonius::go(cfg, benchmark_registry().begin(), benchmark_registry().end());

        return 0;
    }
    inline int main(int argc, char** argv) {
        std::vector<std::string> args(argv+1, argv+argc);
        return main(args.begin(), args.end());
    }
}

int main(int argc, char** argv) {
    return nonius::main(argc, argv);
}

#endif // NONIUS_MAIN_HPP

