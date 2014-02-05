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

#include "counting_clock.h++"

#include <catch.hpp>

TEST_CASE("warmup") {
    auto rate = 1000;
    nonius::counting_clock::set_rate(rate);

    auto start = nonius::counting_clock::now();
    auto iterations = nonius::detail::warmup<nonius::counting_clock>();
    auto end = nonius::counting_clock::now();

    REQUIRE((iterations * rate) > nonius::detail::warmup_time.count());
    REQUIRE((end - start) > nonius::detail::warmup_time);
}

TEST_CASE("resolution") {
    auto rate = 1000;
    nonius::counting_clock::set_rate(rate);

    int count = 10;
    auto res = nonius::detail::resolution<nonius::counting_clock>(count);

    REQUIRE(res.size() == count);

    for(int i = 1; i < count; ++i) {
        REQUIRE(res[i] == rate);
    }
}

TEST_CASE("estimate_clock_resolution") {
    auto rate = 1000;
    nonius::counting_clock::set_rate(rate);

    int iters = 160000;
    auto res = nonius::detail::estimate_clock_resolution<nonius::counting_clock>(iters);

    REQUIRE(res.mean.count() == rate);
    REQUIRE(res.outliers.total() == 0);
}

