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

// transform_if algorithm

#ifndef NONIUS_DETAIL_TRANSFORM_IF_HPP
#define NONIUS_DETAIL_TRANSFORM_IF_HPP

#include <iterator>
#include <utility>

namespace nonius {
    namespace detail {
        template <typename InputIt1, typename InputIt2, typename OutputIt,
                  typename BinaryOp, typename UnaryPred>
        OutputIt transform_if(InputIt1 first1, InputIt1 last1, InputIt2 first2, OutputIt d_first,
                              BinaryOp op, UnaryPred pred) {
            for(; first1 != last1; ++first1, ++first2) {
                auto&& r = op(*first1, *first2);
                if(pred(r)) {
                    *d_first++ = std::forward<decltype(r)>(r);
                }
            }
            return d_first;
        }
    } // namespace detail
} // namespace nonius

#endif // NONIUS_DETAIL_TRANSFORM_IF_HPP

