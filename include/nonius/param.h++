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

struct param_registry {
    param_map defaults;
};

inline param_registry& global_param_registry() {
    static param_registry instance;
    return instance;
}

struct param_declaration {
    template <typename T>
    param_declaration(std::string p, T v,
                      param_registry& registry = global_param_registry())
    {
        registry.defaults.emplace(
            std::move(p),
            boost::lexical_cast<std::string>(std::move(v)));
    }
};

} /* namespace detail */
} /* namespace nonius */

#define NONIUS_PARAM(name, default_value)                               \
    namespace {                                                         \
    static auto NONIUS_DETAIL_UNIQUE_NAME(param_declaration) =          \
        ::nonius::detail::param_declaration{ name, default_value };     \
    }                                                                   \
    //

#endif // NONIUS_PARAM_HPP
