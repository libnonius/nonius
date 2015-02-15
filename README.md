# Nonius

## What is nonius?

Nonius is a framework for benchmarking small snippets of C++ code. It is very
heavily inspired by [Criterion], a similar Haskell-based tool. It
runs your code, measures the time it takes to run, and then performs some
statistical analysis on those measurements.

 [Criterion]: http://www.serpentine.com/blog/2009/09/29/criterion-a-new-benchmarking-library-for-haskell/

## How do I use it?

The library itself is header-only, so you don't have to build anything. It
requires a C++11 capable compiler; it was tested with GCC 4.8.3, clang 3.5, and
VC++ 19.0. Most development takes place in the `devel` branch with GCC with
clang. The `msvc` branch tracks the latest successfully tested revision on
VC++ and the `stable` branch tracks the latest revision that was tested
successfully on all three compilers.

It depends on [Boost] for a few mathematical functions, for some
string algorithms, and, in VC++, for the timing functions as well. Boost.Chrono
is not a header-only library, but since it is only used with VC++ everything
gets linked automatically without intervention.

In the [releases] page you can grab a single header file with everything, ready to
be #included in your files.

There are examples of both simple and advanced usage in the `examples` folder.

If you just want to run a quick benchmark you can put everything in one file,
as in the examples. If you prefer to separate things into different files, it
is recommended that you create one small file with the runner code by #defining
the macro `NONIUS_RUNNER` and then #including the nonius single header. In other
files you don't #define that macro; just #include the header and write the
benchmarks. Then compile and link everything together.

Nonius standard runner has several command-line options for configuring a run.
Pass the `--help` flag to the compiled runner to see the various flags and a
short description of each. The standard runner includes all your benchmarks and
four reporters: plain text, CSV with raw timings, JUnit-compatible XML, and a
nice HTML file with a scatter plot of the timings.

 [Boost]: http://www.boost.org
 [CI server]: https://github.com/rmartinho/nonius/releases

## Woah, what do all these numbers mean?

If you execute the standard runner without requesting a particular reporter,
nonius will use plain text to report the results.

The first thing that nonius does when benchmarking is to find out where it is
running. It estimates the resolution and the cost of using the clock. It will
print out the mean of the samples it took, and also some information about the
spread of those values, namely any outliers seen.

Outliers are classified as "low" or "high" depending on whether they are above
or below the mean. They can be "mild" or "severe" if they are relatively far
from the rest of the measurements. If you request verbose output the default
reporter will provide outlier classification.

After ascertaining the characteristics of the environment, the benchmarks are
run in sequence. Each one consists of taking a number of samples determined by
the configuration (defaults to 100). Each sample consists of running the code
being measured for a number of times that makes sure it takes enough time that
the clock resolution does not affect the measurement.

After the measurements are performed, a statistical [bootstrapping] is performed
on the data. The number of resamples is configurable but defaults to 100000.
After the bootstrapping is done, the mean and standard deviation estimates are
printed out, along with their confidence interval, followed by information about
the outliers. The very last information tells us if the outliers might be
important: if they affect the variance greatly, our measurements might not be
very trustworthy. It could be that there is another factor affecting our
measurements (say, some other application that was doing some heavy task at the
same time), or maybe the code being measure varies wildly in performance.
Nonius will provide the data; it's up to you to make sense of it.

 [bootstrapping]: http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29

## Are there any restrictions on the use of nonius?

Nonius is released under the [CC0] license, which is essentially a public
domain dedication with legalese to emulate the public domain as much as
possible under jurisdictions that do not have such a concept. That means you
can really do whatever you want with the code in nonius, because I waived as
many of my rights on it as I am allowed.

However, currently nonius makes use of some code distributed under the
[CC-BY-NC] and the [MIT] licenses. The `html` reporter uses the [Highcharts JS]
and [jQuery] libraries for the interactive charts and the [cpptemplate] library
for generating HTML from a template.

 [CC0]: http://creativecommons.org/publicdomain/zero/1.0/
 [CC-BY-NC]: http://creativecommons.org/licenses/by-nc/3.0/
 [MIT]: https://bitbucket.org/ginstrom/cpptemplate/raw/d4263ca998038f7ae18aeb9d2358f0c11f00552d/LICENSE.txt
 [Highcharts JS]: http://www.highcharts.com/
 [jQuery]: http://jquery.org/
 [cpptemplate]: https://bitbucket.org/ginstrom/cpptemplate

## What does "nonius" mean?

[Nonius][wikipedia] is a device created in 1542 by the Portuguese inventor
Pedro Nunes (Petrus Nonius in Latin) that improved the accuracy of the
astrolabe. It was adapted in 1631 by the French mathematician Pierre Vernier to
create the vernier scale.

 [wikipedia]: http://en.wikipedia.org/wiki/Nonius_%28device%29

