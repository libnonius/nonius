// Nonius - C++ benchmarking tool
//
// Written in 2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>

// Duration helpers

#ifndef NONIUS_DETAIL_DURATION_HPP
#define NONIUS_DETAIL_DURATION_HPP

#include <chrono>

namespace nonius {
    template <typename Clock>
    using Duration = typename Clock::duration;
    template <typename Clock>
    using FloatDuration = std::chrono::duration<double, typename Clock::period>;
} // namespace nonius

#endif // NONIUS_DETAIL_DURATION_HPP

