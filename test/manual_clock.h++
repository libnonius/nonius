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

// Clock that advances manually

#ifndef NONIUS_TEST_MANUAL_CLOCK_HPP
#define NONIUS_TEST_MANUAL_CLOCK_HPP

#include <nonius/clock.h++>

namespace nonius {
    struct manual_clock {
    public:
        using duration = chrono::nanoseconds;
        using time_point = chrono::time_point<manual_clock, duration>;
        using rep = duration::rep;
        using period = duration::period;
        enum { is_steady = true };

        static time_point now() {
            return time_point(duration(tick()));
        }

        static void advance(int ticks = 1) {
            tick() += ticks;
        }

    private:
        static rep& tick() {
            static rep the_tick = 0;
            return the_tick;
        }
    };
} // namespace nonius

#endif // NONIUS_TEST_MANUAL_CLOCK_HPP
