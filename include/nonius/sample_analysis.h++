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

// Benchmark results

#ifndef NONIUS_BENCHMARK_RESULTS_HPP
#define NONIUS_BENCHMARK_RESULTS_HPP

#include <nonius/clock.h++>
#include <nonius/estimate.h++>

#include <vector>
#include <string>

namespace nonius {
    template <typename Duration>
    struct sample_analysis {
        std::vector<Duration> samples;
        estimate<Duration> mean;
        estimate<Duration> standard_deviation;
        double outlier_variance;
    };
} // namespace nonius

#endif // NONIUS_BENCHMARK_RESULTS_HPP

