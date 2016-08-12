---
# vim: ft=jekyll tw=120:
title: Nonius - Contributor guide
layout: default
---

## Contributor guide

Contributions to nonius are very welcome, whether by filing bugs, suggesting features, contributing code, or helping
with documentation. This page provides some info that might be helpful when doing so.

### Reporting bugs and suggesting features

If you find a bug in nonius, please open an issue in the [issue tracker]. Provide as much detail as you can. Suggested
features also go on the issue tracker.

 [issue tracker]: https://github.com/rmartinho/nonius/issues

### Building

To build the nonius single-header file, the examples, and the tests, you need to have a Python installation and the
[ninja] build tool.

Start by running the boostrap script from the repository root:

{% highlight console %}
$ tools/bootstrap.py
{% endhighlight %}

This script accepts a few flags that you can checkout with `--help`.

After running the bootstrapper you can use ninja to perform build tasks.

{% highlight console %}
# generate the single-header file in dist/nonius.h++
$ ninja header

# build the documentation in dist/doc/ (this requires jekyll and pygments to be installed)
$ ninja docs

# build the examples in bin/examples/
$ ninja examples

# build the test runner in bin/test
$ ninja test
{% endhighlight %}

Currently, if you create new files, ninja won't know about them until the bootstrap script is re-run. You can force this
by simply `touch`ing the bootstrap script before running ninja. It will then pick up all the new files.

{% highlight console %}
$ touch tools/bootstrap.py
$ ninja examples
[1/1] BOOTSTRAP
[1/2] C++ examples/example6.c++
{% endhighlight %}

 [ninja]: https://ninja-build.org

### Writing code

Please try to keep coding style consistent throughout the repository.

All macros starts with `NONIUS_` for namespacing purposes, and everything else goes inside the `nonius` namespace.
Please hide implementation details in the `nonius::detail` namespace.

Nonius should work with GCC, clang, and Visual C++. The versions supported are as follows:

- for GCC, the latest minor and patch of each of the last three major versions (as of August 12, 2016, that means
  versions 4.9.4, 5.4, and 6.1 are supported);
- for clang, the latest patch of each of the last three minor versions, according to the LLVM project versioning scheme,
  not according to Apple's (as of August 12, 2016, that means versions 3.6.2, 3.7.1, and 3.8.1 are supported);
- for Visual C++ the latest minor and patch of each of the last three major versions, starting with Visual Studio 2015
  (as of August 12, 2016, that means only the compiler&emdash;cl.exe, not the IDE&emdash;version 19.00.24213.1 is
  supported);

C++ feature use should be limited in such a way that all these versions are supported.

Currently nonius depends on Boost for some functionality. The versions of Boost that are supported are the latest
patches of each of the last three minor versions (as of August 12, 2016, that means versions 1.59.0, 1.60.0, and 1.61.0
are supported).

#### Branches, tags, releases

If you want to contribute code, make a pull request against the `devel` branch in the [main repository]. Once it has
been reviewed and approved it will be merged. The `devel` branch tracks all code that is pending for the next release.

Once properly tested on all supported platforms, the `devel` branch is merged into the `stable` branch, tagged with a
release number, and a new release entry is added on GitHub.

 [main repository]: https://github.com/rmartinho/nonius

#### Writing documentation

Documentation is kept in the `doc/` folder, in the form of a Jekyll website.

All interfaces intended for public consumption should be described in the documentation. Currently there is no
reference-style documentation as the number of interfaces exposed is low; descriptive prose is preferred.

Pull requests that introduce new features will not be merged until the documentation is written. This ensures that on
every merge to the `devel` branch, the documentation in the repo reflects the current state of the code.

Documentation is automatically picked up every day and served at https://nonius.io

- documentation for the latest release is served from https://nonius.io/;
- documentation for older releases are served from https://nonius.io/vM.m/, where `M` is the major number and `m` is the minor number (e.g.
  https://nonius.io/v1.1/);
- documentation for the latest `devel` branch is served from https://nonius.io/devel/;

### Licensing

**[rmf]**: When I started nonius, I put the code in the public domain using [CC0]. I would like it to continue as such,
so I ask that you license your contributions under CC0 as well. Files in the repository have a notice stating that
nonius is dedicated to the public domain worldwide, and that the authors are all the contributors. Pl

 [rmf]: https://rmf.io/robot
 [CC0]: http://creativecommons.org/publicdomain/zero/1.0/
