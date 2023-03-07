.. _docs-automated-analysis:

==================
Automated analysis
==================

The correctness and style of Pigweed's source code is continuously verified
using a suite of automated tools. We also make it easy to use the same tools
to verify the code of projects using Pigweed.

-------
Summary
-------
On presubmit or in CI we verify Pigweed using:

* pylint
* mypy
* clang-tidy
* AddressSanitizer (asan)
* ThreadSanitizer (tsan)
* UndefinedBehaviorSanitizer (ubsan)
* OSS-Fuzz

The rest of this document discusses these tools and their configuration in
greater detail, and how to use them in your own project.

--------------
Analysis tools
--------------

Static analysis
===============

PyLint
------
`PyLint`_ is a customizable Python linter. Pigweed complies with almost all
the default checks; see `.pylintrc`_ for details. PyLint detects problems such
as overly broad catch statements, unused arguments/variables, and mutable
default parameter values.

For upstream Pigweed, PyLint can be run with ``ninja python.lint.pylint`` or
``ninja python.lint``.  It's also included in a variety of presubmit steps,
like ``static_analysis`` and ``python_checks.gn_python_check``.  See the
`Enabling analysis for your project`_ section to learn how to run PyLint on
your Pigweed-based project.

.. _PyLint: https://pylint.org/
.. _.pylintrc: https://cs.pigweed.dev/pigweed/+/main:.pylintrc

Mypy
----
Python 3 allows for `type annotations`_ for variables, function arguments, and
return values. Most, but not all, of Pigweed's Python code has type
annotations, and these annotations have caught real bugs in code that didn't
yet have unit tests. `Mypy`_ is an analysis tool that enforces these
annotations.

Mypy helps find bugs like when a string is passed into a function that expects
a list of strings---since both are iterables this bug might otherwise be hard
to track down.

Mypy can be run with ``ninja python.lint.mypy`` or ``ninja python.lint``. It's
also included in a variety of presubmit steps, like ``static_analysis`` and
``python_checks.gn_python_check``.

.. _type annotations: https://docs.python.org/3/library/typing.html
.. _Mypy: http://mypy-lang.org/

clang-tidy
----------
`clang-tidy`_ is a C++ "linter" and static analysis tool. It identifies
bug-prone patterns (e.g., use after move), non-idiomatic usage (e.g., creating
``std::unique_ptr`` with ``new`` rather than ``std::make_unique``), and
performance issues (e.g., unnecessary copies of loop variables).

While powerful, clang-tidy defines a very large number of checks, many of which
are special-purpose (e.g., only applicable to FPGA HLS code, or code using the
`Abseil`_ library) or have high false positive rates. Pigweed enables over 50
checks which are relevant to an embedded C/C++ library and have good
signal-to-noise ratios. The full list of Pigweed's checks is in `.clang-tidy`_.

We do not currently enable the `Clang Static Analyzers`_ because they suffer
from false positives, and their findings are time-consuming to manually verify.

clang-tidy can be run with ``ninja static_analysis`` or ``pw presubmit --step
static_analysis``. Note that as a static analysis tool, clang-tidy will not
produce any runnable binaries: it simply analyzes the source files.

.. _clang-tidy: https://clang.llvm.org/extra/clang-tidy/
.. _Abseil: https://abseil.io/
.. _.clang-tidy: https://cs.pigweed.dev/pigweed/+/main:.clang-tidy
.. _Clang Static Analyzers: https://clang-analyzer.llvm.org/available_checks.html


Clang sanitizers
================
We run all of Pigweed's unit tests with the additional instrumentation
described in this section. For more detail about these sanitizers, see the
`Github documentation`_.

* asan: `AddressSanitizer`_ detects memory errors such as out-of-bounds access
  and use-after-free.
* msan: `MemorySanitizer`_ detects reads of uninitialized memory.
* tsan: `ThreadSanitizer`_ detects data races.
* ubsan: `UndefinedBehaviorSanitizer`_ is a fast undefined behavior detector.
  We use the default ``-fsanitize=undefined`` option.

.. note::
   Pigweed does not currently support msan. See
   https://issuetracker.google.com/234876100 for details.

The exact configurations we use for these sanitizers are in
`pw_toolchain/host_clang/BUILD.gn <https://cs.pigweed.dev/pigweed/+/main:pw_toolchain/host_clang/BUILD.gn>`_.
You can see the current status of the sanitizer builds in the `Pigweed CI
console`_, as ``pigweed-linux-san-*``.

Unlike clang-tidy, the clang sanitizers are runtime instrumentation: the
instrumented binary needs to be run for issues to be detected.

.. _Github documentation: https://github.com/google/sanitizers
.. _AddressSanitizer: https://clang.llvm.org/docs/AddressSanitizer.html
.. _MemorySanitizer: https://clang.llvm.org/docs/MemorySanitizer.html
.. _Pigweed CI console: https://ci.chromium.org/p/pigweed/g/pigweed/console
.. _ThreadSanitizer: https://clang.llvm.org/docs/ThreadSanitizer.html
.. _UndefinedBehaviorSanitizer: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html


Fuzzers
=======
`Fuzz testing`_ detects errors in software by providing it with randomly
generated inputs.  We use `OSS-fuzz`_ to continuously uncover potential
vulnerabilities in Pigweed.  `Dashboard with Pigweed's latest results`_. See
the :ref:`module-pw_fuzzer` module documentation for more details.

.. _Dashboard with Pigweed's latest results: https://oss-fuzz-build-logs.storage.googleapis.com/index.html#pigweed
.. _Fuzz testing: https://en.wikipedia.org/wiki/Fuzzing
.. _OSS-fuzz: https://github.com/google/oss-fuzz

.. _Enabling analysis for your project:

----------------------------------
Enabling analysis for your project
----------------------------------

PyLint and Mypy
===============
PyLint and Mypy can be configured to run every time your project is built by
adding ``python.lint`` to your default build group. (You can also add one or both
individually using ``python.lint.mypy`` and ``python.lint.pylint``.) Likewise,
these can be added to individual presubmit steps (`examples`_). You can also
directly include the `python_checks.gn_python_lint`_ presubmit step.

.. _examples: https://cs.opensource.google/search?q=file:pigweed_presubmit.py%20%22python.lint%22&sq=&ss=pigweed%2Fpigweed
.. _python_checks.gn_python_lint: https://cs.pigweed.dev/pigweed/+/main:pw_presubmit/py/pw_presubmit/python_checks.py?q=file:python_checks.py%20gn_python_lint&ss=pigweed%2Fpigweed

clang-tidy
==========
`pw_toolchain/static_analysis_toolchain.gni`_ provides the
``pw_static_analysis_toolchain`` template that can be used to create a build
group performing static analysis. See :ref:`module-pw_toolchain` documentation
for more details. This group can then be added as a presubmit step using
pw_presubmit.

You can place a ``.clang-tidy`` file at the root of your repository to control
which checks are executed. See the `clang documentation`_ for a discussion of how
the tool chooses which ``.clang-tidy`` files to apply when run on a particular
source file.

.. _pw_toolchain/static_analysis_toolchain.gni: https://cs.pigweed.dev/pigweed/+/main:pw_toolchain/static_analysis_toolchain.gni
.. _clang documentation: https://clang.llvm.org/extra/clang-tidy/

Clang sanitizers
================
There are two ways to enable sanitizers for your build.

GN args on debug toolchains
---------------------------
If you are already building your tests with one of the following toolchains (or
a toolchain derived from one of them):

* ``pw_toolchain_host_clang.debug``
* ``pw_toolchain_host_clang.speed_optimized``
* ``pw_toolchain_host_clang.size_optimized``

you can enable the clang sanitizers simply by setting the gn arg
``pw_toolchain_SANITIZERS`` to the desired subset of
``["address", "thread", "undefined"]``.

Example
^^^^^^^
If your project defines a toolchain ``host_clang_debug`` that is derived from
one of the above toolchains, and you'd like to run the ``pw_executable`` target
``sample_binary`` defined in the ``BUILD.gn`` file in ``examples/sample`` with
asan, you would run,

.. code-block:: bash

    gn gen out --args='pw_toolchain_SANITIZERS=["address"]'
    ninja -C out host_clang_debug/obj/example/sample/bin/sample_binary
    out/host_clang_debug/obj/example/sample/bin/sample_binary

Sanitizer toolchains
--------------------
Otherwise, instead of using ``gn args`` you can build your tests with the
appropriate toolchain from the following list (or a toolchain derived from one
of them):

* ``pw_toolchain_host_clang.asan``
* ``pw_toolchain_host_clang.ubsan``
* ``pw_toolchain_host_clang.tsan``

See the :ref:`module-pw_toolchain` module documentation for more
about Pigweed toolchains.

Fuzzers
=======
See the :ref:`module-pw_fuzzer` module documentation.

