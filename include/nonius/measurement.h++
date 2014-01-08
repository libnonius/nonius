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

// Measurement module

#ifndef NONIUS_MEASUREMENT_HPP
#define NONIUS_MEASUREMENT_HPP

#include <nonius/clock.h++>
#include <nonius/detail/duration.h++>

#include <wheels/fun/result_of.h++>
#include <wheels/fun/invoke.h++>

#include <tuple>
#include <type_traits>

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

    template <typename Duration, typename Result>
    struct timing {
        Duration elapsed;
        Result result;
        int iterations;
    };
    template <typename Clock, typename Sig>
    using TimingOf = timing<Duration<Clock>, detail::CompleteResultOf<Sig>>;
    // TODO fuck void
    template <typename Clock = default_clock, typename Fun, typename... Args>
    TimingOf<Clock, Fun(Args...)> time(Fun&& fun, Args&&... args) {
        auto start = Clock::now();
        auto&& r = detail::complete_invoke(fun, std::forward<Args>(args)...);
        auto end = Clock::now();
        auto delta = end - start;
        return { delta, std::forward<decltype(r)>(r), 1 };
    }

    template <typename Clock = default_clock, typename Fun>
    TimingOf<Clock, Fun(int)> run_for_at_least(Duration<Clock> how_long, int seed, Fun&& fun) {
        auto start = Clock::now();
        while(true) {
            auto now = Clock::now();
            if(now - start > how_long * 10) {
                throw "took too long to run: seed %d, iters %d"; //fail
            }
            auto r = time(fun, seed);
            if(r.elapsed >= how_long) {
                return { r.elapsed, std::move(r.result), seed };
            }
            seed *= 2;
        }
    }
} // namespace nonius

#endif // NONIUS_MEASUREMENT_HPP

