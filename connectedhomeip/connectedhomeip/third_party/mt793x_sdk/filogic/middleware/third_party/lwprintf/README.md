# Lightweight printf stdio manager

<h3>Read first: <a href="http://docs.majerle.eu/projects/lwprintf/">Documentation</a></h3>

## Features

* Written in ANSI C99, compatible with ``size_t`` and ``uintmax_t`` types for some specifiers
* Implements output functions compatible with ``printf``, ``vprintf``, ``snprintf``, ``sprintf`` and ``vsnprintf``
* Low-memory footprint, suitable for embedded systems
* Reentrant access to all API functions
* Operating-system ready
* Requires single output function to be implemented by user for ``printf``-like API calls
* With optional functions for operating systems to protect multiple threads printing to the same output stream
* Allows multiple output stream functions (unlike standard ``printf`` which supports only one) to separate parts of application
* Added additional specifiers vs original features
* User friendly MIT license

## Contribute

Fresh contributions are always welcome. Simple instructions to proceed::

1. Fork Github repository
2. Respect [C style & coding rules](https://github.com/MaJerle/c-code-style) used by the library
3. Create a pull request to develop branch with new features or bug fixes

Alternatively you may:

1. Report a bug
2. Ask for a feature request
