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

// Timing

#ifndef NONIUS_STRING_UTILS_HPP
#define NONIUS_STRING_UTILS_HPP

#include <string>

namespace nonius {
    namespace detail {
    struct is_spaceF
    {
        bool operator()(const char c) const
        {
            return std::isspace(c);
        }
    };

    struct is_any_ofF {
        std::string chars;
        is_any_ofF(std::string chars) : chars(chars) {}
        bool operator()(const char c) const
        {
            return chars.find_first_of(c) != std::string::npos;
        }
    };
    } // namespace detail

    detail::is_spaceF is_space() { return detail::is_spaceF {}; }
    detail::is_any_ofF is_any_of(const char* chars) { return detail::is_any_ofF{chars}; }
} // namespace nonius

#endif // NONIUS_STRING_UTILS_HPP

