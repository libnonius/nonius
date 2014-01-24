# Nonius

## What is nonius?

Nonius is a framework for benchmarking small snippets of C++ code. It is very
heavily inspired by [Criterion][criterion] a similar Haskell-based tool. It runs
your code, measures the time it takes to run, and then performs some statistical
analysis on those measurements.

 [criterion]: http://www.serpentine.com/blog/2009/09/29/criterion-a-new-benchmarking-library-for-haskell/

## How do I use it?

The library itself is header-only, so you don't have to build anything. It
requires a C++11 capable compiler (like GCC 4.8) and it depends on my other
library [wheels] for some small tools, and on [Boost] for some statistical
functions.

You can grab a distribution zip from the [CI server] and just drop it in your
favourite include folder. That includes any needed headers from wheels.

You can build the examples provided by running `bootstrap.py` first and then
`ninja examples`. There will be an executable file for each example in the
`bin/examples` directory.

 [wheels]: http://github.com/rmartinho/wheels
 [Boost]: http://www.boost.org
 [CI server]: http://teamcity.loungecpp.net/project.html?projectId=Nonius

## Woah, what do all these numbers mean?

The first thing that nonius does when benchmarking is to find out where it is
running. It estimates the resolution and the cost of using the clock. It will
print out the mean of the samples it took, and also some information about the
spread of those values, namely any outliers seen.

Outliers are classified as "low" or "high" depending on whether they are above
or below the mean. They can be "mild" or "severe" if they are relatively far
from the rest of the measurements.

After ascertaining the characteristics of the environment, the benchmarks are
run in sequence. Each one consists of taking a number of samples determined by
the configuration (defaults to 100). Each sample consists of running the code
being measured for a number of times that makes sure it takes enough time that
the clock resolution does not affect the measurement.

After the measurements are performed, a statistical bootstrapping is performed
on the data. The number of resamples is configurable but defaults to 100000.
After the bootstrapping is done, the mean and standard deviation estimates are
printed out, along with their confidence interval, followed by classification of
the outliers. The very last information is tells us if the outliers are
important: if they affect the variance greatly, our measurements might not be
very significative. It could be that there is another factor affecting our
measurements (say, some other application that was doing some heavy task at the
same time), or maybe the code being measure varies wildly in performance. Nonius
will provide the data; it's up to you to make sense of it.

 [bootstrapping]: http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29

## What does nonius mean?

[Nonius][wikipedia] is a device created in 1542 by the Portuguese inventor
Pedro Nunes (Petrus Nonius in Latin) that improved the accuracy of the
astrolabe. It was adapted in 1631 by the French mathematician Pierre Vernier to
created the vernier scale.

 [wikipedia]: http://en.wikipedia.org/wiki/Nonius_%28device%29

