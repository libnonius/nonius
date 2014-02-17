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

// Pretty printing routines

#ifndef NONIUS_PRETTY_PRINT_HPP
#define NONIUS_PRETTY_PRINT_HPP

#include <nonius/reporter.h++>
#include <nonius/configuration.h++>
#include <nonius/sample_analysis.h++>
#include <nonius/execution_plan.h++>
#include <nonius/environment.h++>
#include <nonius/clock.h++>

#include <ratio>
#include <ios>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <memory>

namespace nonius {
    namespace detail {
        inline std::string pretty_duration(fp_seconds secs) {
            using fp_millis = chrono::duration<double, milli>;
            using fp_micros = chrono::duration<double, micro>;
            using fp_nanos = chrono::duration<double, nano>;

            std::ostringstream ss;
            ss << std::setprecision(ss.precision());
            if(secs.count() >= 2.) {
                ss << secs.count() << " s";
            } else if(secs.count() >= .002) {
                ss << fp_millis(secs).count() << " ms";
            } else if(secs.count() >= .000002) {
#ifdef _MSC_VER
                ss << fp_micros(secs).count() << " us";
#else
                ss << fp_micros(secs).count() << u8" μs";
#endif
            } else {
                ss << fp_nanos(secs).count() << " ns";
            }
            return ss.str();
        }
        inline std::string percentage(double d) {
            std::ostringstream ss;
            ss << std::setprecision(3);
            if(d != 0 && d < 1e-5) {
                ss << std::fixed;
                ss << 0.0001 << "%";
            } else {
                ss.unsetf(std::ios::floatfield);
                ss << (100. * d) << "%";
            }
            return ss.str();
        }
        inline std::string percentage_ratio(double part, double whole) {
            return percentage(part / whole);
        }
    } // namespace detail
} // namespace nonius

#endif // NONIUS_PRETTY_PRINT_HPP

