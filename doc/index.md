---
title: Nonius
layout: default
---

## What is nonius?

Nonius is a framework for benchmarking small snippets of C++ code. It is very
heavily inspired by [Criterion], a similar Haskell-based tool. It runs your
code, measures the time it takes to run, and then performs some statistical
analysis on those measurements. The [source code] can be found on GitHub.

 [Criterion]: http://www.serpentine.com/blog/2009/09/29/criterion-a-new-benchmarking-library-for-haskell/
 [source code]: https://github.com/rmartinho/nonius

## How do I use it?

### Installation and dependencies

The library itself is header-only so you don't have to build it. It comes as a
single header that you can drop somewhere and #include it in your code. You can
grab the header from the [releases] page.

 [releases]: https://github.com/rmartinho/nonius/releases


You will need a C++11 capable compiler; it has been tested with GCC 4.8.3,
clang 3.5, and VC++ 18.0. Older versions of these compilers may work, but there
are no guarantees. Newer versions of these compilers are also supported.

The library depends on [Boost] for a few mathematical functions, for some
string algorithms, and, in some versions of VC++, for the timing functions as
well. Boost.Chrono is not a header-only library, but since it is only used with
VC++ everything gets linked automatically without intervention. If desired,
usage of Boost.Chrono can be forced by #defining the macro
`NONIUS_USE_BOOST_CHRONO`.

 [Boost]: http://www.boost.org

### Authoring benchmarks

Writing benchmarks with nonius is not complicated, but there are several things
to keep in mind when doing so. There is a separate [guide] about the subject,
and there are examples of both simple and advanced usage in the [examples]
folder.

 [guide]: authoring-benchmarks
 [examples]: https://github.com/rmartinho/nonius/tree/devel/examples

### Compiling benchmarks

If you just want to run a quick benchmark you can put everything in one file, as
in the examples. If you have something more complicated and prefer to separate
things into different files, it is recommended that you create one small file
with the runner infrastructure by #defining the macro `NONIUS_RUNNER` and
then #including the nonius header.

You can also write your own `main` function instead, if you need something
fancy, but for now that API is subject to change and not documented.

{% highlight cpp %}
// runner file contents
#define NONIUS_RUNNER
#include "nonius.h++"
{% endhighlight %}

In other files you don't #define that macro; just #include the header and write
the benchmarks.

{% highlight cpp %}
// other files
#include "nonius.h++"

// everything else goes here
{% endhighlight %}

Then you compile and link everything together as normal. Keep in mind that the
statistical analysis is multithreaded so you may need to pass extra flags to
your compiler (like `-pthread` in GCC). That gives you an executable with your
benchmarks and with the nonius standard benchmark runner. And don't forget to
enable optimisations!

### Running benchmarks

Invoking the standard runner with the `--help` flag provides information about
the options available. Here are some examples of common choices:

> Run all benchmarks and provide a simple textual report
>
>     $ runner
>
> Run all benchmarks and provide extra details
>
>     $ runner -v
>
> Run all benchmarks collecting 500 samples instead of the default 100, and
> report extra details
>
>     $ runner -v -s 500
>
> Run all benchmarks and output all samples to a CSV file named `results.csv`
>
>     $ runner -r csv -o results.csv
>
> Run all benchmarks and output a JUnit compatible report named `results.xml`
>
>     $ runner -r junit -o results.xml
>
> Run all benchmarks and output an HTML report named `results.html` with the
> title "Some benchmarks", using 250 samples per benchmark
>
>     $ runner -r html -o results.html -t "Some benchmarks" -s 250
>

The runner includes all your benchmarks and it comes equipped with four
reporters: plain text, CSV with raw timings, JUnit-compatible XML, and an HTML
file with a scatter plot of the timings. If you execute the runner without
requesting a particular reporter, it will use plain text to report the results.
When compiling you can selectively disable any or all of the extra reporters
by #defining some macros before #including the runner.
`NONIUS_DISABLE_EXTRA_REPORTERS` disables everything but plain text;
`NONIUS_DISABLE_X_REPORTER`, where `X` is one of `CSV`, `JUNIT`, or `HTML`
disables a particular reporter.

The first thing that nonius does when running is testing the clock. By default
it uses the clock provided by `std::chrono::high_resolution_clock`. The runner
estimates the resolution and the cost of using the clock and then prints out
that estimate.

{% highlight console %}
clock resolution: mean is 28.1296 ns (20480002 iterations)
{% endhighlight %}

After ascertaining the characteristics of the clock, the benchmarks are run in
sequence. Each benchmark consists of taking a number of samples determined by
the command-line flags (defaults to 100). Each of those samples consists of
running the code being measured for a number of times that makes sure it takes
enough time that the clock resolution does not affect the measurement. If you're
benchmarking code that takes significantly more than the clock resolution to
run, it will probably run it once for each sample. However, if one run of that
code is too fast, nonius will scale it by running the code more than once per
sample. This obviously implies that your benchmarks should be completely
repeatable. There is also the underlying assumption that the time it takes to run
the code does not vary wildly.

{% highlight console %}
benchmarking construct small
collecting 100 samples, 438 iterations each, in estimated 2.8032 ms
{% endhighlight %}

After the measurements are performed, a statistical [bootstrapping] is performed
on the samples. The number of resamples for that bootstrapping is configurable
but defaults to 100000.  After the bootstrapping is done, the runner will print
estimates for the mean and standard deviation. The estimates come with a lower
bound and an upper bound, and the confidence interval (which is configurable but
defaults to 95%).

 [bootstrapping]: http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29

{% highlight console %}
mean: 41.3622 ns, lb 41.3479 ns, ub 41.4251 ns, ci 0.95
std dev: 0.130953 ns, lb 0.0209896 ns, ub 0.309054 ns, ci 0.95
{% endhighlight %}

After all that, the runner will tell you about any samples that are outliers
and whether those might be important: if they affect the variance greatly, our
measurements might not be very trustworthy. It could be that there is another
factor affecting our measurements (say, some other application that was doing
some heavy task at the same time), or maybe the code being measure varies wildly
in performance. Nonius will provide the data; it's up to you to make sense of
it.

{% highlight console %}
found 19 outliers among 100 samples (19%)
variance is unaffected by outliers
{% endhighlight %}

Outliers are classified as "low" or "high" depending on whether they are above
or below the mean. They can be "mild" or "severe" if they are relatively far
from the rest of the measurements. If you request verbose output the default
reporter will give you outlier classification.

{% highlight console %}
found 19 outliers among 100 samples (19%)
  2 (2%) low mild
  3 (3%) high mild
  14 (14%) high severe
variance introduced by outliers: 0.99%
variance is unaffected by outliers
{% endhighlight %}

## Licensing

Nonius is released under the [CC0] license, which is essentially a public domain
dedication with legalese to emulate the public domain as much as possible under
jurisdictions that do not have such a concept. That means you can really do
whatever you want with the code in nonius, because I waived as many of my rights
on it as I am allowed.

 [CC0]: http://creativecommons.org/publicdomain/zero/1.0/

However, currently nonius makes use of some code distributed under the
[CC-BY-NC] and the [MIT] licenses. The `html` reporter uses the [Highcharts JS]
and [jQuery] libraries for the interactive charts and the [cpptemplate] library
for generating HTML from a template. If you want to use only the public domain
code for whatever reason, you can disable the `html` reporter easily.

 [CC-BY-NC]: http://creativecommons.org/licenses/by-nc/3.0/
 [MIT]: https://bitbucket.org/ginstrom/cpptemplate/raw/d4263ca998038f7ae18aeb9d2358f0c11f00552d/LICENSE.txt
 [Highcharts JS]: http://www.highcharts.com/
 [jQuery]: http://jquery.org/
 [cpptemplate]: https://bitbucket.org/ginstrom/cpptemplate

## Trivia

A [nonius] is a device created in 1542 by the Portuguese inventor Pedro Nunes
(Petrus Nonius in Latin) that improved the accuracy of the astrolabe. It was
adapted in 1631 by the French mathematician Pierre Vernier to create the vernier
scale.

 [Nonius]: http://en.wikipedia.org/wiki/Nonius_%28device%29

