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

// Tests for nonius::detail::benchmark_function

#include <nonius/detail/benchmark_function.h++>
#include <catch.hpp>

namespace nonius {

struct test_chronometer_model : detail::chronometer_concept {
    int started  = 0;
    int finished = 0;

    void start() override { ++started; }
    void finish() override { ++finished; }
};

TEST_CASE("benchmark function call") {
    SECTION("without chronometer") {
        auto called = 0;
        auto model = test_chronometer_model{};
        auto meter = chronometer{model, 1, {}};
        auto fn = detail::benchmark_function{[&]{
                CHECK(model.started == 1);
                CHECK(model.finished == 0);
                ++called;
            }};

        fn(meter);

        CHECK(model.started == 1);
        CHECK(model.finished == 1);
        CHECK(called == 1);
    }

    SECTION("with chronometer") {
        auto called = 0;
        auto model = test_chronometer_model{};
        auto meter = chronometer{model, 1, {}};
        auto fn = detail::benchmark_function{[&](chronometer) {
                CHECK(model.started == 0);
                CHECK(model.finished == 0);
                ++called;
            }};

        fn(meter);

        CHECK(model.started == 0);
        CHECK(model.finished == 0);
        CHECK(called == 1);
    }
}

TEST_CASE("benchmark function factory") {
    SECTION("default factory") {
        auto called = 0;
        auto model = test_chronometer_model{};
        auto meter = chronometer{model, 1, {}};
        auto fn = detail::benchmark_function{[&] {
                ++called;
            }};

        auto r = fn(parameters{});
        CHECK(called == 0);

        r(meter);
        CHECK(called == 1);
    }

    SECTION("explicit factory") {
        auto called = 0;
        auto created = 0;
        auto model = test_chronometer_model{};
        auto meter = chronometer{model, 1, {}};
        auto fn = detail::benchmark_function{[&] (parameters) {
                ++ created;
                return [&] { ++called; };
            }};

        auto r = fn(parameters{});
        CHECK(created == 1);
        CHECK(called == 0);

        r(meter);
        CHECK(created == 1);
        CHECK(called == 1);
    }
}

} // namespace nonius
