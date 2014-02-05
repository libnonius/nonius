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

// Clock that ticks every time you call now()

#ifndef NONIUS_TEST_COUNTING_CLOCK_HPP
#define NONIUS_TEST_COUNTING_CLOCK_HPP

#include <nonius/clock.h++>

namespace nonius {
    struct counting_clock {
    public:
        using duration = chrono::nanoseconds;
        using time_point = chrono::time_point<counting_clock, duration>;
        using rep = duration::rep;
        using period = duration::period;
        enum { is_steady = true };

        static time_point now() {
            static rep ticks = 0;
            return time_point(duration(ticks += rate()));
        }

        static void set_rate(rep new_rate) { rate() = new_rate; }

    private:
        static rep& rate() {
            static rep the_rate = 1;
            return the_rate;
        }
    };
} // namespace nonius

#endif // NONIUS_TEST_COUNTING_CLOCK_HPP

