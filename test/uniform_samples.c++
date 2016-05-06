// Nonius - C++ benchmarking tool
//
// Written in 2014-2015 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Tests for the uniform sample degenerate case

#include <nonius/detail/stats.h++>

#include <catch.hpp>

TEST_CASE("uniform samples") {
    std::vector<double> samples(100);
    std::fill(samples.begin(), samples.end(), 23);

    using it = std::vector<double>::iterator;
    auto e = nonius::detail::bootstrap(0.95, samples.begin(), samples.end(), samples, [](it a, it b) {
        auto sum = std::accumulate(a, b, 0.);
        return sum / (b - a);
    });
    CHECK(e.point == 23);
    CHECK(e.upper_bound == 23);
    CHECK(e.lower_bound == 23);
    CHECK(e.confidence_interval == 0.95);
}
