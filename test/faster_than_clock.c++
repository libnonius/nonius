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

// Tests for the degenerate case where a run is shorter than the mean clock cost

#include <nonius/detail/stats.h++>
#include <nonius/benchmark.h++>
#include <nonius/execution_plan.h++>

#include "counting_clock.h++"

#include <catch.hpp>

TEST_CASE("faster than clock") {
    nonius::benchmark b("foo", [] {
                nonius::counting_clock::set_rate(100);
            });
    nonius::configuration cfg;
    cfg.samples = 3;
    nonius::environment<nonius::FloatDuration<nonius::counting_clock>> env;
    env.clock_cost.mean = nonius::counting_clock::duration(1000);
    nonius::execution_plan<nonius::counting_clock::duration> plan;
    plan.iterations_per_sample = 2;

    auto samples = b.run<nonius::counting_clock>(cfg, env, plan);
    for(auto&& s : samples) { CHECK(s.count() >= 0); }
}

