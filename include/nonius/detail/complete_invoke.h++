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

// Invoke with a special case for void

#ifndef NONIUS_DETAIL_COMPLETE_INVOKE_HPP
#define NONIUS_DETAIL_COMPLETE_INVOKE_HPP

#include <wheels/fun/result_of.h++>
#include <wheels/fun/invoke.h++>

#include <utility>

namespace nonius {
    namespace detail {
        template <typename T>
        struct complete_type { using type = T; };
        template <>
        struct complete_type<void> { struct type {}; };

        template <typename T>
        using CompleteType = typename complete_type<T>::type;

        template <typename Result>
        struct complete_invoker {
            template <typename... Args>
            static Result invoke(Args&&... args) {
                return wheels::fun::invoke(std::forward<Args>(args)...);
            }
        };
        template <>
        struct complete_invoker<void> {
            template <typename... Args>
            static CompleteType<void> invoke(Args&&... args) {
                wheels::fun::invoke(std::forward<Args>(args)...);
                return {};
            }
        };
        template <typename Sig>
        using CompleteResultOf = detail::CompleteType<wheels::fun::ResultOf<Sig>>;

        // invoke and not return void :(
        template <typename Fun, typename... Args>
        CompleteResultOf<Fun(Args...)> complete_invoke(Fun&& fun, Args&&... args) {
            return complete_invoker<wheels::fun::ResultOf<Fun(Args...)>>::invoke(std::forward<Fun>(fun), std::forward<Args>(args)...);
        }
    } // namespace detail
} // namespace nonius

#endif // NONIUS_DETAIL_COMPLETE_INVOKE_HPP

