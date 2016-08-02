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

// User-facing chronometer

#ifndef NONIUS_PARAM_MAP_HPP
#define NONIUS_PARAM_MAP_HPP

#include <unordered_map>
#include <iterator>
#include <iosfwd>

#include <boost/lexical_cast.hpp>

namespace nonius {
namespace detail {

struct param_map : std::unordered_map<std::string, std::string> {
    using base_t = std::unordered_map<std::string, std::string>;
    using base_t::base_t;

    template <typename T=int>
    T param(const std::string& name) const {
        return boost::lexical_cast<T>(at(name));
    }

    param_map merged(param_map m) const& {
        m.insert(begin(), end());
        return m;
    }
    param_map merged(param_map m) && {
        m.insert(std::make_move_iterator(begin()), std::make_move_iterator(end()));
        return m;
    }

    friend std::ostream& operator<< (std::ostream& os, const param_map& m) {
        for(auto&& p : m) os << "  " << p.first << " = " << p.second << "\n";
        return os;
    }
};

} /* namespace detail */
} /* namespace nonius */

#endif // NONIUS_PARAM_MAP_HPP
