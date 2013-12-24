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

#include <nonius/detail/duration.h++>

#include <wheels/fun/result_of.h++>
#include <wheels/fun/invoke.h++>

#include <chrono>
#include <tuple>
#include <type_traits>

namespace nonius {
    template <typename Duration, typename Result>
    struct timing {
        Duration elapsed;
        Result result;
    };
    template <typename Clock, typename Sig>
    using TimingOf = timing<Duration<Clock>, wheels::fun::ResultOf<Sig>>;
    // TODO fuck void
    template <typename Clock = std::chrono::high_resolution_clock, typename Fun, typename... Args>
    TimingOf<Clock, Fun(Args...)> time(Fun&& fun, Args&&... args) {
        auto start = Clock::now();
        auto&& r = wheels::fun::invoke(fun, std::forward<Args>(args)...);
        auto end = Clock::now();
        auto delta = end - start;
        return { delta, std::forward<decltype(r)>(r) };
    }

    template <typename Clock = std::chrono::high_resolution_clock, typename Fun>
    TimingOf<Clock, Fun(int)> run_for_at_least(Duration<Clock> how_long, int& seed, Fun&& fun) {
        int iters = 0;
        auto start = Clock::now();
        while(true) {
            auto now = Clock::now();
            if(now - start > how_long * 10) {
                throw "took too long to run: seed %d, iters %d"; //fail
            }
            auto r = time(fun, seed);
            if(r.elapsed >= how_long) {
                return r;
            }
            seed *= 2;
            ++iters;
        }
    }
} // namespace nonius

#endif // NONIUS_MEASUREMENT_HPP

