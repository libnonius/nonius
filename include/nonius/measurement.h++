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

// Measurement module

#ifndef NONIUS_MEASUREMENT_HPP
#define NONIUS_MEASUREMENT_HPP

#include <chrono>
#include <tuple>
#include <type_traits>

namespace nonius {
    template <typename Clock>
    using Duration = typename Clock::duration;
    template <typename Sig>
    using ResultOf = typename std::result_of<Sig>::type;

    template <typename Clock = std::chrono::high_resolution_clock, typename Fun, typename... Args>
    std::tuple<Duration<Clock>, ResultOf<Fun(Args...)>> time(Fun&& fun, Args&&... args) {
        auto start = Clock::now();
        auto&& r = fun(std::forward<Args>(args)...);
        auto end = Clock::now();
        auto delta = end - start;
        return std::make_tuple(delta, std::forward<decltype(r)>(r));
    }
    template <typename Clock = std::chrono::high_resolution_clock, typename Fun, typename... Args>
    Duration<Clock> time_(Fun&& fun, Args&&... args) {
        return std::get<0>(time<Clock>(std::forward<Fun>(fun), std::forward<Args>(args)...));
    }

    template <typename Clock = std::chrono::high_resolution_clock, typename Fun>
    std::tuple<Duration<Clock>, int, ResultOf<Fun(int)>> run_for_at_least(Duration<Clock> how_long, int init_seed, Fun&& fun) {
        auto seed = init_seed;
        int iters = 0;
        auto start = Clock::now();
        while(true) {
            auto now = Clock::now();
            if(now - start > how_long * 10) {
                throw "took to long to run: seed %d, iters %d"; //fail
            }
            auto r = time(fun, seed);
            if(std::get<0>(r) >= how_long) {
                return std::make_tuple(std::get<0>(r), seed, std::get<1>(r));
            }
            seed += 1; // TODO *2
            ++iters;
        }
    }
} // namespace nonius

#endif // NONIUS_MEASUREMENT_HPP

