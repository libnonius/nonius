---
title: foo
layout: test
---
Thingies!

{% highlight console %}
$ bin/examples/example5 -h
Usage: bin/examples/example5 [OPTIONS]

--help                          -h             show this help message
--samples=SAMPLES               -s SAMPLES     number of samples to collect (default: 100)
--resamples=RESAMPLES           -rs RESAMPLES  number of resamples for the bootstrap (default: 100000)
--confidence-interval=INTERVAL  -ci INTERVAL   confidence interval for the bootstrap (between 0 and 1, default: 0.95)
--output=FILE                   -o FILE        output file (default: <stdout>)
--reporter=REPORTER             -r REPORTER    reporter to use (default: standard)
--title=TITLE                   -t TITLE       set report title
--no-analysis                   -A             perform only measurements; do not perform any analysis
--list                          -l             list benchmarks
--list-reporters                -lr            list available reporters
--verbose                       -v             show verbose output (mutually exclusive with -q)
--summary                       -q             show summary output (mutually exclusive with -v)
$
{% endhighlight %}

{% highlight console %}
$ bin/examples/example5
clock resolution: mean is 24.4967 ns (20480002 iterations)

benchmarking construct small
collecting 100 samples, 403 iterations each, in estimated 2.418 ms
mean: 40.3394 ns, lb 40.3318 ns, ub 40.365 ns, ci 0.95
std dev: 0.0629761 ns, lb 0.0186515 ns, ub 0.139918 ns, ci 0.95
found 3 outliers among 100 samples (3%)
variance is unaffected by outliers

benchmarking construct large
collecting 100 samples, 272 iterations each, in estimated 2.448 ms
mean: 51.5876 ns, lb 51.5589 ns, ub 51.7001 ns, ci 0.95
std dev: 0.251572 ns, lb 0.0545789 ns, ub 0.581709 ns, ci 0.95
found 5 outliers among 100 samples (5%)
variance is unaffected by outliers

benchmarking destroy small
collecting 100 samples, 324 iterations each, in estimated 2.43 ms
mean: 27.4421 ns, lb 27.4391 ns, ub 27.456 ns, ci 0.95
std dev: 0.028566 ns, lb 0.00233815 ns, ub 0.0679242 ns, ci 0.95
found 1 outliers among 100 samples (1%)
variance is unaffected by outliers

benchmarking destroy large
collecting 100 samples, 269 iterations each, in estimated 2.4479 ms
mean: 31.5494 ns, lb 31.5042 ns, ub 31.7417 ns, ci 0.95
std dev: 0.406734 ns, lb 0.0355798 ns, ub 0.954117 ns, ci 0.95
found 23 outliers among 100 samples (23%)
variance is slightly inflated by outliers
$
{% endhighlight %}

{% highlight console %}
$ bin/examples/example5 -q

construct small
mean: 40.3974 ns
std dev: 0.493846 ns
variance is unaffected by outliers

construct large
mean: 51.6843 ns
std dev: 0.619526 ns
variance is unaffected by outliers

destroy small
mean: 27.4425 ns
std dev: 0.0689035 ns
variance is unaffected by outliers

destroy large
mean: 31.1184 ns
std dev: 0.140576 ns
variance is unaffected by outliers
{% endhighlight %}

