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

// Tests for statistical stuff

#include <nonius/detail/stats.h++>

#include <catch.hpp>

#include <vector>

TEST_CASE("mean") {
    std::vector<double> x { 10., 20., 14., 16., 30., 24. };

    auto m = nonius::detail::mean(x.begin(), x.end());

    REQUIRE(m == 19.);
}

TEST_CASE("weighted_average_quantile") {
    std::vector<double> x { 10., 20., 14., 16., 30., 24. };

    auto q1 = nonius::detail::weighted_average_quantile(1, 4, x.begin(), x.end());
    auto med = nonius::detail::weighted_average_quantile(1, 2, x.begin(), x.end());
    auto q3 = nonius::detail::weighted_average_quantile(3, 4, x.begin(), x.end());

    REQUIRE(q1 == 14.5);
    REQUIRE(med == 18.);
    REQUIRE(q3 == 23.);
}

TEST_CASE("classify_outliers") {
    auto require_outliers = [](nonius::outlier_classification o, int los, int lom, int him, int his) {
        REQUIRE(o.low_severe == los);
        REQUIRE(o.low_mild == lom);
        REQUIRE(o.high_mild == him);
        REQUIRE(o.high_severe == his);
        REQUIRE(o.total() == los + lom + him + his);
    };

    SECTION("none") {
        std::vector<double> x { 10., 20., 14., 16., 30., 24. };

        auto o = nonius::detail::classify_outliers(x.begin(), x.end());

        REQUIRE(o.samples_seen == x.size());
        require_outliers(o, 0, 0, 0, 0);
    }
    SECTION("low severe") {
        std::vector<double> x { -12., 20., 14., 16., 30., 24. };

        auto o = nonius::detail::classify_outliers(x.begin(), x.end());

        REQUIRE(o.samples_seen == x.size());
        require_outliers(o, 1, 0, 0, 0);
    }
    SECTION("low mild") {
        std::vector<double> x { 1., 20., 14., 16., 30., 24. };

        auto o = nonius::detail::classify_outliers(x.begin(), x.end());

        REQUIRE(o.samples_seen == x.size());
        require_outliers(o, 0, 1, 0, 0);
    }
    SECTION("high mild") {
        std::vector<double> x { 10., 20., 14., 16., 36., 24. };

        auto o = nonius::detail::classify_outliers(x.begin(), x.end());

        REQUIRE(o.samples_seen == x.size());
        require_outliers(o, 0, 0, 1, 0);
    }
    SECTION("high severe") {
        std::vector<double> x { 10., 20., 14., 16., 49., 24. };

        auto o = nonius::detail::classify_outliers(x.begin(), x.end());

        REQUIRE(o.samples_seen == x.size());
        require_outliers(o, 0, 0, 0, 1);
    }
    SECTION("mixed") {
        std::vector<double> x { -20., 20., 14., 16., 39., 24. };

        auto o = nonius::detail::classify_outliers(x.begin(), x.end());

        REQUIRE(o.samples_seen == x.size());
        require_outliers(o, 1, 0, 1, 0);
    }
}

