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

// Main header

#ifndef NONIUS_HPP
#define NONIUS_HPP

#include <nonius/clock.h++>
#include <nonius/benchmark.h++>
#include <nonius/constructor.h++>
#include <nonius/configuration.h++>
#include <nonius/chronometer.h++>
#include <nonius/optimizer.h++>
#include <nonius/go.h++>
#include <nonius/param.h++>

#include <nonius/reporters/standard_reporter.h++>

#ifndef NONIUS_DISABLE_EXTRA_REPORTERS
#ifndef NONIUS_DISABLE_CSV_REPORTER
#include <nonius/reporters/csv_reporter.h++>
#endif // NONIUS_DISABLE_CSV_REPORTER
#ifndef NONIUS_DISABLE_JUNIT_REPORTER
#include <nonius/reporters/junit_reporter.h++>
#endif // NONIUS_DISABLE_JUNIT_REPORTER
#ifndef NONIUS_DISABLE_HTML_REPORTER
#include <nonius/reporters/html_reporter.h++>
#endif // NONIUS_DISABLE_HTML_REPORTER
#endif // NONIUS_DISABLE_EXTRA_REPORTERS

#ifdef NONIUS_RUNNER
#include <nonius/main.h++>
#endif // NONIUS_RUNNER

#endif // NONIUS_HPP
