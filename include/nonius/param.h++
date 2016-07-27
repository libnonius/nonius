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

// User-facing param

#ifndef NONIUS_PARAM_HPP
#define NONIUS_PARAM_HPP

#include <boost/lexical_cast.hpp>
#include <unordered_map>

namespace nonius {
namespace detail {

template <typename=void>
struct params_map {
    static std::unordered_map<std::string, std::string> defaults;
};

template <typename T>
std::unordered_map<std::string, std::string> params_map<T>::defaults = {};

struct param_declaration {
    template <typename T>
    param_declaration(std::string p, T v) {
        params_map<>::defaults.emplace(
            std::move(p),
            boost::lexical_cast<std::string>(std::move(v)));
    }
};

} /* namespace detail */
} /* namespace nonius */

#define NONIUS_PARAM(name, default_value)       \
    static ::nonius::detail::param_declaration  \
    name ## __param (#name, default_value)      \
    //

#endif // NONIUS_PARAM_HPP
