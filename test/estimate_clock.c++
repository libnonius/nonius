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

// Tests for clock estimation

#include <nonius/detail/estimate_clock.h++>

#include <catch.hpp>

TEST_CASE("warmup") {
    auto start = nonius::default_clock::now();
    auto iterations = nonius::detail::warmup<nonius::default_clock>();
    auto end = nonius::default_clock::now();

    REQUIRE(iterations > 0);
    REQUIRE((end - start) > nonius::detail::warmup_time);
}

