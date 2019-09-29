# Nonius

## What is nonius?

Nonius is an open-source framework for benchmarking small snippets of C++ code.
It is very heavily inspired by [Criterion], a similar Haskell-based tool. It
runs your code, measures the time it takes to run, and then performs some
statistical analysis on those measurements.

The library is header-only so you don’t have to build it. It comes as a single
header that you can drop somewhere and #include it in your code. Grab that
header from the [releases] page.

 [Criterion]: http://www.serpentine.com/blog/2009/09/29/criterion-a-new-benchmarking-library-for-haskell/
 [releases]: https://github.com/rmartinho/nonius/releases
