[![Build Status][nlio-travis-svg]][nlio-travis]
[![Coverage Status][nlio-codecov-svg]][nlio-codecov]

Nest Labs Memory Mapped I/O
===========================

# Introduction

Nest Labs Memory Mapped I/O (NLIO) is designed to provide both C
and C++ language bindings to macros and functions for:

1. Determining, at both compile- and run-time, the byte ordering
   of the target system.
2. Performing in place byte-swapping of compile-time constants
   via the C preprocessor as well as functions for performing
   byte-swapping by value and in place by pointer for 16-, 32-, and
   64-bit types.
3. Safely performing simple, efficient memory-mapped accesses,
   potentially to unaligned memory locations, with or without byte
   reordering, to 8-, 16-, 32-, and 64-bit quantities. Functions
   both with and without pointer management are also available.

[nlio-travis]: https://travis-ci.com/nestlabs/nlio
[nlio-travis-svg]: https://travis-ci.com/nestlabs/nlio.svg?branch=master
[nlio-codecov]: https://codecov.io/gh/nestlabs/nlio
[nlio-codecov-svg]: https://codecov.io/gh/nestlabs/nlio/branch/master/graph/badge.svg

# Interact

There are numerous avenues for nlio support:

  * Bugs and feature requests — [submit to the Issue Tracker](https://github.com/nestlabs/nlio/issues)
  * Google Groups — discussion and announcements
    * [nlio-announce](https://groups.google.com/forum/#!forum/nlio-announce) — release notes and new updates on nlio
    * [nlio-users](https://groups.google.com/forum/#!forum/nlio-users) — discuss use of and enhancements to nlio

# Versioning

nlio follows the [Semantic Versioning guidelines](http://semver.org/) 
for release cycle transparency and to maintain backwards compatibility.

# License

nlio is released under the [Apache License, Version 2.0 license](https://opensource.org/licenses/Apache-2.0). 
See the `LICENSE` file for more information.
