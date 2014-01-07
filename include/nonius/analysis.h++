// Nonius - C++ benchmarking tool
//
// Written in 2013-2014 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Analysis module

#ifndef NONIUS_ANALYSIS_HPP
#define NONIUS_ANALYSIS_HPP

#include <nonius/clock.h++>
#include <nonius/detail/duration.h++>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>

#include <algorithm>

namespace nonius {
    template <typename Clock = default_clock, typename Iterator>
    FloatDuration<Clock> analyse_mean(Iterator first, Iterator last, int ) { // TODO outliers
        namespace acc = boost::accumulators;
        acc::accumulator_set<FloatDuration<Clock>, acc::stats<acc::tag::mean>> a;
        return acc::mean(std::for_each(first, last, a));
    }
} // namespace nonius

#endif // NONIUS_ANALYSIS_HPP

