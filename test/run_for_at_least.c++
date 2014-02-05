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

// Tests for nonius::detail::run_for_at_least

#include <nonius/detail/run_for_at_least.h++>

#include "manual_clock.h++"

#include <catch.hpp>

TEST_CASE("run_for_at_least") {
    nonius::manual_clock::duration time(100);

    auto timing = nonius::detail::run_for_at_least<nonius::manual_clock>(time, 1, [](int x) -> int {
                CHECK(x >= 1);
                nonius::manual_clock::advance(x);
                return x + 17;
            });

    REQUIRE(timing.elapsed >= time);
    REQUIRE(timing.result == timing.iterations + 17);
    REQUIRE(timing.iterations >= time.count());
}

