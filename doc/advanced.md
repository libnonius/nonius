---
title: Nonius - Advanced features
layout: default
---

## Advanced features

### Custom entry point

Nonius comes with a default `main` function that drives the benchmarking process. However, sometimes it might be necessary
to write your own `main` function. In order to do this, your code should not define `NONIUS_RUNNER` and have a `main`
function instead. That function can then invoke the default benchmark driver by calling `nonius::main` or `nonius::go`.

{% highlight cpp %}
namespace nonius {
    using default_clock = /*unspecified*/;

    int main(int argc, char** argv);

    template <typename Clock = default_clock, typename Iterator>
    void go(configuration cfg, Iterator first, Iterator last, reporter& rep);
}
{% endhighlight %}

`nonius::main` is exactly what you would get by defining `NONIUS_RUNNER`. This is useful if you want to filter
command-line arguments, initialize something before nonius runs, or do something afterwards.

`nonius::go` only runs the benchmarks, without any of the housekeeping tasks like parsing command-line arguments. The
function takes four parameters: a `configuration` object that defines the parameters of the run, a pair of iterators for
`benchmark` objects, and the `reporter` to be used by the run.

