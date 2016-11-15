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

// Tests for statistical stuff

#include <nonius/detail/stats.h++>

#include <catch.hpp>

#include <vector>

TEST_CASE("normal_cdf") {
    using nonius::detail::normal_cdf;
    CHECK(normal_cdf(0.000000) == Approx(0.50000000000000000));
    CHECK(normal_cdf(1.000000) == Approx(0.84134474606854293));
    CHECK(normal_cdf(-1.000000) == Approx(0.15865525393145705));
    CHECK(normal_cdf(2.809729) == Approx(0.99752083845315409));
    CHECK(normal_cdf(-1.352570) == Approx(0.08809652095066035));
}

TEST_CASE("erfc_inv") {
    using nonius::detail::erfc_inv;
    CHECK(erfc_inv(1.103560) == Approx(-0.09203687623843015));
    CHECK(erfc_inv(1.067400) == Approx(-0.05980291115763361));
    CHECK(erfc_inv(0.050000) == Approx(1.38590382434967796));
}

TEST_CASE("normal_quantile") {
    using nonius::detail::normal_quantile;
    CHECK(normal_quantile(0.551780) == Approx(0.13015979861484198));
    CHECK(normal_quantile(0.533700) == Approx(0.08457408802851875));
    CHECK(normal_quantile(0.025000) == Approx(-1.95996398454005449));
}


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

