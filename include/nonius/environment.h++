// Nonius - C++ benchmarking tool
//
// Written in 2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Environment module

#ifndef NONIUS_ENVIRONMENT_HPP
#define NONIUS_ENVIRONMENT_HPP

#include <nonius/measurement.h++>
#include <nonius/detail/duration.h++>

#include <chrono>
#include <iterator>
#include <algorithm>

namespace nonius {
    namespace detail {
        template <typename Clock = std::chrono::high_resolution_clock>
        std::vector<Duration<Clock>> resolution(int k) {
            std::vector<typename Clock::time_point> times;
            times.reserve(k+1);
            for(int i = 0; i < k+1; ++i) {
                times.push_back(Clock::now());
            }
            std::vector<Duration<Clock>> deltas;
            deltas.reserve(k);
            for(int i = 0; i < k; ++i) {
                deltas.push_back(times[i+1] - times[i]);
            }
            return deltas;
        }
    } // namespace detail
    template <typename Clock = std::chrono::high_resolution_clock>
    int warmup(int seed) {
        return std::get<1>(run_for_at_least<Clock>(std::chrono::milliseconds(100), seed, detail::resolution<Clock>));
    }
} // namespace nonius

#endif // NONIUS_ENVIRONMENT_HPP

