.. _module-pw_perf_test:

============
pw_perf_test
============
Pigweed's perf test module provides an easy way to measure performance on
any test setup. By using an API similar to GoogleTest, this module aims to bring
a comprehensive and intuitive testing framework to our users, much like
:ref:`module-pw_unit_test`.

.. warning::
  The PW_PERF_TEST macro is still under construction and should not be relied
  upon yet

-------------------
Perf Test Interface
-------------------
The user experience of writing a performance test is intended to be as
friction-less as possible. With the goal of being used for micro-benchmarking
code, writing a performance test is as easy as:

.. code-block:: cpp

  void TestFunction(::pw::perf_test::State& state) {
    // space to create any needed variables.
    while (state.KeepRunning()){
      // code to measure here
    }
  }
  PW_PERF_TEST(PerformanceTestName, TestFunction);

However, it is recommended to read this guide to understand and write tests that
are suited towards your platform and the type of code you are trying to
benchmark.

State
=====
Within the testing framework, the state object is responsible for calling the
timing interface and keeping track of testing iterations. It contains only one
publicly accessible function, since the object is intended for internal use
only. The ``KeepRunning()`` function collects timestamps to measure the code
and ensures that only a certain number of iterations are run. To use the state
object properly, pass it as an argument of the test function and pass in the
``KeepRunning()`` function as the condition in a ``while()`` loop. The
``KeepRunning()`` function collects timestamps to measure the code and ensures
that only a certain number of iterations are run. Therefore the code to be
measured should be in the body of the ``while()`` loop like so:

.. code-block:: cpp

  // The State object is injected into a performance test by including it as an
  // argument to the function.
  void TestFunction(::pw::perf_test::State& state_obj) {
    while (state_obj.KeepRunning()) {
      /*
        Code to be measured here
      */
    }
  }

Macro Interface
===============
The test collection and registration process is done by a macro, much like
:ref:`module-pw_unit_test`.

.. c:macro:: PW_PERF_TEST(test_name, test_function, ...)

  Registers a performance test. Any additional arguments are passed to the test
  function.

.. c:macro:: PW_PERF_TEST_SIMPLE(test_name, test_function, ...)

  Like the original PW_PERF_TEST macro it registers a performance test. However
  the test function does not need to have a state object. Internally this macro
  runs all of the input function inside of its own state loop. Any additional
  arguments are passed into the function to be tested.

.. code-block:: cpp

  // Declare performance test functions.
  // The first argument is the state, which is passed in by the test framework.
  void TestFunction(pw::perf_test::State& state) {
    // Test set up code
    Items a[] = {1, 2, 3};

    // Tests a KeepRunning() function, similar to Fuchsia's Perftest.
    while (state.KeepRunning()) {
      // Code under test, ran for multiple iterations.
      DoStuffToItems(a);
    }
  }

  void TestFunctionWithArgs(pw::perf_test::State& state, int arg1, bool arg2) {
    // Test set up code
    Thing object_created_outside(arg1);

    while (state.KeepRunning()) {
      // Code under test, ran for multiple iterations.
      object_created_outside.Do(arg2);
    }
  }

  // Tests are declared with any callable object. This is similar to Benchmark's
  // BENCMARK_CAPTURE() macro.
  PW_PERF_TEST(Name1, [](pw::perf_test::State& state) {
        TestFunctionWithArgs(1, false);
      })

  PW_PERF_TEST(Name2, TestFunctionWithArgs, 1, true);
  PW_PERF_TEST(Name3, TestFunctionWithArgs, 2, false);

  void Sum(int a, int b) {
    return a + b;
  }

  PW_PERF_TEST_SIMPLE(SimpleExample, Sum, 4, 2);
  PW_PERF_TEST_SIMPLE(Name4, MyExistingFunction, "input");

.. warning::
  Internally, the testing framework stores the testing function as a function
  pointer. Therefore the test function argument must be converible to a function
  pointer.

Event Handler
=============
The performance testing framework relies heavily on the member functions of
EventHandler to report iterations, the beginning of tests and other useful
information. The ``EventHandler`` class is a virtual interface meant to be
overridden, in order to provide flexibility on how data gets transferred.

.. cpp:class:: pw::perf_test::EventHandler

  Handles events from a performance test.

  .. cpp:function:: virtual void RunAllTestsStart(const TestRunInfo& summary)

    Called before all tests are run

  .. cpp:function:: virtual void RunAllTestsEnd()

    Called after all tests are run

  .. cpp:function:: virtual void TestCaseStart(const TestCase& info)

    Called when a new performance test is started

  .. cpp:function:: virtual void TestCaseIteration(const IterationResult& result)

    Called to output the results of an iteration

  .. cpp:function:: virtual void TestCaseEnd(const TestCase& info, const Results& end_result)

    Called after a performance test ends

Logging Event Handler
---------------------
The default method of running performance tests is using the
``LoggingEventHandler``. This event handler only logs the test results to the
console and nothing more. It was chosen as the default method due to its
portability and to cut down on the time it would take to implement other
printing log handlers. Make sure to set a ``pw_log`` backend.

Timing API
==========
In order to provide meaningful performance timings for given functions, events,
etc a timing interface must be implemented from scratch to be able to provide
for the testing needs. The timing API meets these needs by implementing either
clock cycle record keeping or second based recordings.

Time-Based Measurement
----------------------
For most host applications, pw_perf_test depends on :ref:`module-pw_chrono` for
its timing needs. At the moment, the interface will only measure performance in
terms of nanoseconds. To see more information about how pw_chrono works, see the
module documentation.

Cycle Count Measurement
------------------------------------
In the case of running tests on an embedded system, clock cycles may give more
insight into the actual performance of the system. The timing API gives you this
option by providing time measurements through a facade. In this case, by setting
the ccynt timer as the backend, perf tests can be measured in clock cycles for
ARM Cortex devices.

This implementation directly accesses the registers of the Cortex, and therefore
needs no operating system to function. This is achieved by enabling the
`DWT register <https://developer.arm.com/documentation/ddi0337/e/System-Debug/DWT?lang=en>`_
through the `DEMCR register <https://developer.arm.com/documentation/ddi0337/e/CEGHJDCF>`_.
While this provides cycle counts directly from the CPU, notably it is vulnerable
to rollover upon a duration of a test exceeding 2^32 clock cycles. This works
out to a 43 second duration limit per iteration at 100 mhz.

.. warning::
  The interface only measures raw clock cycles and does not take into account
  other possible sources of pollution such as LSUs, Sleeps and other registers.
  `Read more on the DWT methods of counting instructions. <https://developer.arm.com/documentation/ka001499/1-0/>`_

------------------------
Build System Integration
------------------------
As of this moment, pw_perf_test provides build integration with Bazel and GN.
Performance tests can be built in CMake, but must be built as regular
executables.

While each build system has their own names for their variables, each test must
configure an ``EventHandler`` by choosing an associated ``main()`` function, and
they must configure a ``timing interface``. At the moment, only a
:ref:`module-pw_log` based event handler exists, timing is only supported
where :ref:`module-pw_chrono` is supported, and cycle counts are only supported
on ARM Cortex M series microcontrollers with a Data Watchpoint and Trace (DWT)
unit.

GN
===
To get tests building in GN, set the ``pw_perf_test_TIMER_INTERFACE_BACKEND``
variable to whichever implementation is necessary for timings. Next, set the
``pw_perf_test_MAIN_FUNCTION`` variable to the preferred event handler. Finally
use the ``pw_perf_test`` template to register your code.

.. code-block::

 import("$dir_pw_perf_test/perf_test.gni")

 pw_perf_test("foo_perf_test") {
   sources = [ "foo_perf_test.cc" ]
 }

.. note::
   If you use ``pw_watch``, the template is configured to build automatically
   with ``pw_watch``. However you will still need to add your test group to the
   pw_perf_tests group in the top level BUILD.gn.

pw_perf_test template
---------------------
``pw_perf_test`` defines a single perf test suite. It creates two sub-targets.

* ``<target_name>``: The test suite within a single binary. The test code is
  linked against the target set in the build arg ``pw_unit_test_MAIN``.
* ``<target_name>.lib``: The test sources without ``pw_unit_test_MAIN``.

**Arguments**

* All GN executable arguments are accepted and forwarded to the underlying
  ``pw_executable``.
* ``enable_if``: Boolean indicating whether the test should be built. If false,
  replaces the test with an empty target. Default true.

**Example**

.. code::

 import("$dir_pw_perf_test/perf_test.gni")

 pw_perf_test("large_test") {
   sources = [ "large_test.cc" ]
   enable_if = device_has_1m_flash
 }

Grouping
--------
For grouping tests, no special template is required. Simply create a basic GN
``group()`` and add each perf test as a dependency.

**Example**

.. code::

  import("$dir_pw_perf_test/perf_test.gni")

  pw_perf_test("foo_test") {
    sources = [ "foo.cc" ]
  }

  pw_perf_test("bar_test") {
    sources = [ "bar.cc" ]
  }

  group("my_perf_tests_collection") {
    deps = [
      ":foo_test",
      ":bar_test",
    ]
  }

Running
-------
To run perf tests from gn, locate the associated binaries from the ``out``
directory and run/flash them manually.

Bazel
=====
Bazel is a very efficient build system for running tests on host, needing very
minimal setup to get tests running. To configure the timing interface, set the
``pw_perf_test_timer_backend`` variable to use the preferred method of
timekeeping. Right now, only the logging event handler is supported for Bazel.

Template
--------
To use the ``pw_ccp_perf_test()`` template, load the ``pw_cc_perf_test``
template from ``//pw_build:pigweed.bzl``.

**Arguments**

* All bazel executable arguments are accepted and forwarded to the underlying
  ``native.cc_binary``.

**Example**

.. code::

  load(
    "//pw_build:pigweed.bzl",
    "pw_cc_test",
  )

  pw_cc_perf_test(
    name = "foo_test",
    srcs = ["foo_perf_test.cc"],
  )

Running
-------
Running tests in Bazel is like running any other program. Use the default bazel
run command: ``bazel run //path/to:target``.

