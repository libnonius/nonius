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

// Tests for nonius::detail::run_for_at_least

#include <nonius/detail/run_for_at_least.h++>
#include <nonius/chronometer.h++>

#include "manual_clock.h++"

#include <catch.hpp>

TEST_CASE("run_for_at_least, int") {
    nonius::manual_clock::duration time(100);

    int old_x = 1;
    auto timing = nonius::detail::run_for_at_least<nonius::manual_clock>(time, 1, [&old_x](int x) -> int {
                CHECK(x >= old_x);
                nonius::manual_clock::advance(x);
                old_x = x;
                return x + 17;
            });

    REQUIRE(timing.elapsed >= time);
    REQUIRE(timing.result == timing.iterations + 17);
    REQUIRE(timing.iterations >= time.count());
}

TEST_CASE("run_for_at_least, chronometer") {
    nonius::manual_clock::duration time(100);

    int old_runs = 1;
    auto timing = nonius::detail::run_for_at_least<nonius::manual_clock>(time, 1, [&old_runs](nonius::chronometer meter) -> int {
                CHECK(meter.runs() >= old_runs);
                nonius::manual_clock::advance(100);
                meter.measure([] {
                    nonius::manual_clock::advance(1);
                });
                old_runs = meter.runs();
                return meter.runs() + 17;
            });

    REQUIRE(timing.elapsed >= time);
    REQUIRE(timing.result == timing.iterations + 17);
    REQUIRE(timing.iterations >= time.count());
}

