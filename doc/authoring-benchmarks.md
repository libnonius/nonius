---
title: Nonius - Authoring benchmarks
layout: default
---

## Authoring benchmarks

Writing benchmarks is not easy. Nonius simplifies certain aspects but you'll
always need to take care about various aspects. Understanding a few things about
the way nonius runs your code will be very helpful when writing your benchmarks.

First off, let's go over some terminology that will be used throughout this
guide.

- *User code*: user code is the code that the user provides to be measured.
- *Run*: one run is one execution of the user code.
- *Sample*: one sample is one data point obtained by measuring the time it takes
  to perform a certain number of runs. One sample can consist of more than one
  run if the clock available does not have enough resolution to accurately
  measure a single run. All samples for a given benchmark execution are obtained
  with the same number of runs.

### Execution procedure

Now I can explain how a benchmark is executed in nonius. There are three main
steps, though the first does not need to be repeated for every benchmark.

1. *Environmental probe*: before any benchmarks can be executed, the clock's
resolution is estimated. A few other environmental artifacts are also estimated
at this point, like the cost of calling the clock function, but they almost
never have any impact in the results.

2. *Estimation*: the user code is executed a few times to obtain an estimate of
the amount of runs that should be in each sample. This also has the potential
effect of bringing relevant code and data into the caches before the actual
measurement starts.

3. *Measurement*: all the samples are collected sequentially by performing the
number of runs estimated in the previous step for each sample.

This already gives us one important rule for writing benchmarks for nonius: the
benchmarks must be repeatable. The user code will be executed several times, and
the number of times it will be executed during the estimation step cannot be
known beforehand since it depends on the time it takes to execute the code.
User code that cannot be executed repeatedly will lead to bogus results or
crashes.

### Benchmark specification

Nonius includes a simple declarative interface to specify benchmarks for
execution. This declarative interface consists of the `NONIUS_BENCHMARK` macro. This macro
expands to some machinery that registers the benchmark in a global registry that
can be accessed by the standard runner.

`NONIUS_BENCHMARK` takes two parameters: a string literal with a unique name to
identify the benchmark, and a callable object with the actual code. This
callable object is usually provided as a lambda expression.

There are two types of callable objects that can be provided. The simplest ones
take no arguments and just run the user code that needs to be measured. However,
if the callable can be called with a `nonius::chronometer` argument, some
advanced features are available. The simple callables are invoked once per run,
while the advanced callables are invoked exactly twice: once during the
estimation phase, and another time during the execution phase.

{% highlight cpp %}
NONIUS_BENCHMARK("simple", [] { return long_computation(); });

NONIUS_BENCHMARK("advanced", [](nonius::chronometer meter) {
    set_up();
    meter.measure([] { return long_computation(); });
});
{% endhighlight %}

These advanced callables no longer consist entirely of user code to be measured.
In these cases, the code to be measured is provided via the
`nonius::chronometer::measure` member function. This allows you to set up any
kind of state that might be required for the benchmark but is not to be included
in the measurements, like making a vector of random integers to feed to a
sorting algorithm.

A single call to `nonius::chronometer::measure` performs the actual measurements
by invoking the callable object passed in as many times as necessary. Anything
that needs to be done outside the measurement can be done outside the call to
`measure`.

The callable object passed in to `measure` can optionally accept an `int`
parameter.

{% highlight cpp %}
meter.measure([](int i) { return long_computation(i); });
{% endhighlight %}

If it accepts an `int` parameter, the sequence number of each run will be passed
in, starting with 0. This is useful if you want to measure some mutating code,
for example. The number of runs can be known beforehand by calling
`nonius::chronometer::runs`; with this one can set up a different instance to be
mutated by each run.

{% highlight cpp %}
std::vector<std::string> v(meter.runs());
std::fill(v.begin(), v.end(), test_string());
meter.measure([&v](int i) { in_place_escape(v[i]); });
{% endhighlight %}

Note that it is not possible to simply use the same instance for different runs
and resetting it between each run since that would pollute the measurements with
the resetting code.

All of these tools give you a lot mileage, but there are two things that still
need special handling: constructors and destructors. The problem is that if you
use automatic objects they get destroyed by the end of the scope, so you end up
measuring the time for construction and destruction together. And if you use
dynamic allocation instead, you end up including the time to allocate memory in
the measurements.

To solve this conundrum, nonius provides class templates that let you manually
construct and destroy objects without dynamic allocation and in a way that lets
you measure construction and destruction separately.

{% highlight cpp %}
NONIUS_BENCHMARK("construct", [](nonius::chronometer meter)
{
    std::vector<nonius::storage_for<std::string>> storage(meter.runs());
    meter.measure([&](int i) { storage[i].construct("thing"); });
})

NONIUS_BENCHMARK("destroy", [](nonius::chronometer meter)
{
    std::vector<nonius::destructable_object<std::string>> storage(meter.runs());
    for(auto&& o : storage)
        o.construct("thing");
    meter.measure([&](int i) { storage[i].destruct(); });
})
{% endhighlight %}

`nonius::storage_for<T>` objects are just pieces of raw storage suitable for `T`
objects. You can use the `nonius::storage_for::construct` member function to call a constructor and
create an object in that storage. So if you want to measure the time it takes
for a certain constructor to run, you can just measure the time it takes to run
this function.

When the lifetime of a `nonius::storage_for<T>` object ends, if an actual object was
constructed there it will be automatically destroyed, so nothing leaks.

If you want to measure a destructor, though, we need to use
`nonius::destructable_object<T>`. These objects are similar to
`nonius::storage_for<T>` in that construction of the `T` object is manual, but
it does not destroy anything automatically. Instead, you are required to call
the `nonius::destructable_object::destruct` member function, which is what you
can use to measure the destruction time.

### The optimizer

Sometimes the optimizer will optimize away the very code that you want to
measure. There are several ways to use results that will prevent the optimiser
from removing them. You can use the `volatile` keyword, or you can output the
value to standard output or to a file, both of which force the program to
actually generate the value somehow.

Nonius adds a third option. The values returned by any function provided as user
code are guaranteed to be evaluated and not optimised out. This means that if
your user code consists of computing a certain value, you don't need to bother
with using `volatile` or forcing output. Just `return` it from the function.
That helps with keeping the code in a natural fashion.

Here's an example:

{% highlight cpp %}
// may measure nothing at all by skipping the long calculation since its
// result is not used
NONIUS_BENCHMARK("no return", [] { long_calculation(); })

// the result of long_calculation() is guaranteed to be computed somehow
NONIUS_BENCHMARK("with return", [] { return long_calculation(); })
{% endhighlight %}

However, there's no other form of control over the optimizer whatsoever. It is
up to you to write a benchmark that actually measures what you want and doesn't
just measure the time to do a whole bunch of nothing.

To sum up, there are two simple rules: whatever you would do in handwritten code
to control optimization still works in nonius; and nonius makes return values
from user code into observable effects that can't be optimized away.

