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

// Tests for nonius::detail::measure

#include <nonius/detail/measure.h++>

#include "manual_clock.h++"

#include <catch.hpp>

TEST_CASE("measure") {
    auto r = nonius::detail::measure<nonius::manual_clock>([](int x) -> int {
            CHECK(x == 17);
            nonius::manual_clock::advance(42);
            return 23;
        }, 17);
    auto s = nonius::detail::measure<nonius::manual_clock>([](int x) -> int {
            CHECK(x == 23);
            nonius::manual_clock::advance(69);
            return 17;
        }, 23);

    CHECK(r.elapsed.count() == 42);
    CHECK(r.result == 23);
    CHECK(r.iterations == 1);

    CHECK(s.elapsed.count() == 69);
    CHECK(s.result == 17);
    CHECK(s.iterations == 1);
}

