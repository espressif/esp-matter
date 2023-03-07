.. _target-host:

====
host
====
The Pigweed host target is used for unit testing and some host side tooling.

----------
Toolchains
----------
Pigweed several toolchains preconfigured for compiling for the host.

.. list-table::

  * - Toolchain name
    - GN path
    - Compiler
    - Optimization
  * - ``host_clang_debug``
    - ``//targets/host:host_clang_debug``
    - Clang
    - ``-Og``
  * - ``host_clang_size_optimized``
    - ``//targets/host:host_clang_size_optimized``
    - Clang
    - ``-Os``
  * - ``host_clang_speed_optimized``
    - ``//targets/host:host_clang_speed_optimized``
    - Clang
    - ``-O2``
  * - ``host_gcc_debug``
    - ``//targets/host:host_gcc_debug``
    - GCC
    - ``-Og``
  * - ``host_gcc_size_optimized``
    - ``//targets/host:host_gcc_size_optimized``
    - GCC
    - ``-Os``
  * - ``host_gcc_speed_optimized``
    - ``//targets/host:host_gcc_speed_optimized``
    - GCC
    - ``-O2``

These toolchains may be used directly by downstream projects if desired. For
upstream builds, Pigweed uses internal-only variants of these toolchains. The
upstream toolchains are defined in ``//targets/host/pigweed_internal`` and are
prefixed with ``pw_strict_``. The upstream toolchains may not be used by
downstream projects.

Toolchains for other C++ standards
==================================
Most Pigweed code requires C++17, but a few modules, such as ``pw_tokenizer``,
work with C++14. All Pigweed code is compatible with C++20. Pigweed defines
toolchains for testing with C++14 and C++20.

* ``pw_strict_host_clang_debug_cpp14`` -- Builds with ``-std=c++14``.
* ``pw_strict_host_clang_size_optimized_cpp20`` -- Builds with ``-std=c++20``.

These toolchains are only permitted for use in upstream pigweed, but downstream
users may create similar toolchains as needed.

--------
Building
--------
To build for the host with a default configuration, invoke Ninja with the
top-level ``host`` group as the target to build.

.. code-block:: sh

  $ ninja -C out host


``host`` may be replaced with ``host_clang``, ``host_gcc``,
``host_clang_debug``, etc. to build with a more specific host toolchain. Not all
toolchains are supported on all platforms. Unless working specifically on one
toolchain, it is recommended to use the default.

-------------
Running Tests
-------------
Tests are automatically run as part of the host build, but if you desire to
manually run tests, you may invoke them from a shell directly.

Example:

.. code-block:: sh

  $ ./out/host_[compiler]_debug/obj/pw_status/status_test

----------
RPC server
----------
The host target implements a system RPC server that runs over a local socket,
defaulting to port 33000. To communicate with a process running the host RPC
server, use ``pw rpc -s localhost:33000 <protos>``.
