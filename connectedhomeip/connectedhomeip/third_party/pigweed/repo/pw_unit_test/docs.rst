.. _module-pw_unit_test:

============
pw_unit_test
============
``pw_unit_test`` provides a `GoogleTest`_-compatible unit testing API for
Pigweed. The default implementation is the embedded-friendly
``pw_unit_test:light`` backend. Upstream GoogleTest may be used as well (see
`Using upstream GoogleTest`_).

.. _GoogleTest: https://github.com/google/googletest

.. note::

  This documentation is currently incomplete.

-------------------------------------------
pw_unit_test:light: GoogleTest for Embedded
-------------------------------------------
``pw_unit_test:light`` implements a subset of `GoogleTest`_ with lightweight,
embedded-friendly primitives. It is also highly portable and can run on almost
any system from bare metal to a full-fledged desktop OS. It does this by
offloading the responsibility of test reporting and output to the underlying
system, communicating its results through a common interface. Unit tests can be
written once and run under many different environments, empowering developers to
write robust, high quality code.

``pw_unit_test:light`` usage is the same as GoogleTest;
refer to the `GoogleTest documentation <https://google.github.io/googletest/>`_
for examples of how to define unit test cases.

``pw_unit_test:light`` is still under development and lacks many features
expected in a complete testing framework; nevertheless, it is already used
heavily within Pigweed.

.. note::

  Many of GoogleTest's more advanced features are not yet implemented. Missing
  features include:

  * Any GoogleMock features (e.g. :c:macro:`EXPECT_THAT`)
  * Floating point comparison macros (e.g. :c:macro:`EXPECT_FLOAT_EQ`)
  * Death tests (e.g. :c:macro:`EXPECT_DEATH`); ``EXPECT_DEATH_IF_SUPPORTED``
    does nothing but silently passes
  * Value-parameterized tests

  To request a feature addition, please
  `let us know <mailto:pigweed@googlegroups.com>`_.

  See `Using upstream GoogleTest`_ below for information
  about using upstream GoogleTest instead.

The EventHandler interface
==========================
The ``EventHandler`` class in ``public/pw_unit_test/event_handler.h`` defines
the interface through which ``pw_unit_test:light`` communicates the results of
its test runs. A platform using the ``pw_unit_test:light`` backend must register
an event handler with the unit testing framework to receive test output.

As the framework runs tests, it calls the event handler's callback functions to
notify the system of various test events. The system can then choose to perform
any necessary handling or aggregation of these events, and report them back to
the developer.

Predefined event handlers
-------------------------
Pigweed provides some standard event handlers upstream to simplify the process
of getting started using ``pw_unit_test:light``. All event handlers provide for
GoogleTest-style output using the shared
:cpp:class:`pw::unit_test::GoogleTestStyleEventHandler` base.

  .. code-block::

    [==========] Running all tests.
    [ RUN      ] Status.Default
    [       OK ] Status.Default
    [ RUN      ] Status.ConstructWithStatusCode
    [       OK ] Status.ConstructWithStatusCode
    [ RUN      ] Status.AssignFromStatusCode
    [       OK ] Status.AssignFromStatusCode
    [ RUN      ] Status.CompareToStatusCode
    [       OK ] Status.CompareToStatusCode
    [ RUN      ] Status.Ok_OkIsTrue
    [       OK ] Status.Ok_OkIsTrue
    [ RUN      ] Status.NotOk_OkIsFalse
    [       OK ] Status.NotOk_OkIsFalse
    [ RUN      ] Status.KnownString
    [       OK ] Status.KnownString
    [ RUN      ] Status.UnknownString
    [       OK ] Status.UnknownString
    [==========] Done running all tests.
    [  PASSED  ] 8 test(s).

.. cpp:namespace-push:: pw::unit_test

.. cpp:class:: GoogleTestStyleEventHandler

   Provides GoogleTest-style output for ``pw_unit_test:light`` events. Must be
   extended to define how to output the results.

.. cpp:class:: SimplePrintingEventHandler : public GoogleTestStyleEventHandler

  An event handler that writes GoogleTest-style output to a specified sink.

.. cpp:class:: LoggingEventHandler : public GoogleTestStyleEventHandler

  An event handler which uses the ``pw_log`` module to output test results, to
  integrate with the system's existing logging setup.

.. cpp:class:: PrintfEventHandler : public GoogleTestStyleEventHandler

  A C++14-compatible event handler that uses ``std::printf`` to output test
  results.

.. cpp:namespace-pop::

Test filtering
==============
If using C++17, filters can be set on the test framework to run only a subset of
the registered unit tests. This is useful when many tests are bundled into a
single application image.

Currently, only a test suite filter is supported. This is set by calling
``pw::unit_test::SetTestSuitesToRun`` with a list of suite names.

.. note::
  Test filtering is only supported in C++17.

Tests in static libraries
=========================
The linker usually ignores tests linked through a static library (``.a`` file).
This is because test registration relies on the test instance's static
constructor adding itself to a global list of tests. When linking against a
static library, static constructors in an object file will be ignored unless at
least one entity in that object file is linked.

Pigweed's ``pw_unit_test`` implementation provides the
:c:macro:`PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST` macro to support running tests
in a static library.

.. c:macro:: PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST(test_suite_name, test_name)

  Ensures tests in a static library are linked and executed. Provide the test
  suite name and test name for one test in the file linked into a static
  library. Any test in the file may be used, but it is recommended to use the
  first for consistency. The test must be in a static library that is a
  dependency of this target. Referring to a test that does not exist causes a
  linker error.

.. _running-tests:

-------------
Running tests
-------------
To run unit tests, link the tests into a single binary with the unit testing
framework, configure the backend if needed, and call the ``RUN_ALL_TESTS``
macro.

The following example shows how to write a main function that runs
``pw_unit_test`` with the ``light`` backend.

.. code-block:: cpp

  #include "gtest/gtest.h"

  // pw_unit_test:light requires an event handler to be configured.
  #include "pw_unit_test/simple_printing_event_handler.h"

  void WriteString(const std::string_view& string, bool newline) {
    printf("%s", string.data());
    if (newline) {
      printf("\n");
    }
  }

  int main() {
    // Since we are using pw_unit_test:light, set up an event handler.
    pw::unit_test::SimplePrintingEventHandler handler(WriteString);
    pw::unit_test::RegisterEventHandler(&handler);
    return RUN_ALL_TESTS();
  }

Build system integration
========================
``pw_unit_test`` integrates directly into Pigweed's GN and CMake build systems.

The ``pw_unit_test`` module provides a few optional libraries to simplify setup:

 - ``simple_printing_event_handler``: When running tests, output test results
   as plain text over ``pw_sys_io``.
 - ``simple_printing_main``: Implements a ``main()`` function that simply runs
   tests using the ``simple_printing_event_handler``.
 - ``logging_event_handler``: When running tests, log test results as
   plain text using pw_log (ensure your target has set a ``pw_log`` backend).
 - ``logging_main``: Implements a ``main()`` function that simply runs tests
   using the ``logging_event_handler``.


GN
--
To define simple unit tests, set the ``pw_unit_test_MAIN`` build variable to a
target which configures the test framework as described in the
:ref:`running-tests` section, and use the ``pw_test`` template to register your
test code.

.. code-block::

  import("$dir_pw_unit_test/test.gni")

  pw_test("foo_test") {
    sources = [ "foo_test.cc" ]
  }

pw_test template
````````````````
``pw_test`` defines a single unit test suite. It creates several sub-targets.

* ``<target_name>``: The test suite within a single binary. The test code is
  linked against the target set in the build arg ``pw_unit_test_MAIN``.
* ``<target_name>.run``: If ``pw_unit_test_AUTOMATIC_RUNNER`` is set, this
  target runs the test as part of the build.
* ``<target_name>.lib``: The test sources without ``pw_unit_test_MAIN``.

**Arguments**

* All GN executable arguments are accepted and forwarded to the underlying
  ``pw_executable``.
* ``enable_if``: Boolean indicating whether the test should be built. If false,
  replaces the test with an empty target. Default true.
* ``test_main``: Target label to add to the tests's dependencies to provide the
  ``main()`` function. Defaults to ``pw_unit_test_MAIN``. Set to ``""`` if
  ``main()`` is implemented in the test's ``sources``.

**Example**

.. code::

  import("$dir_pw_unit_test/test.gni")

  pw_test("large_test") {
    sources = [ "large_test.cc" ]
    enable_if = device_has_1m_flash
  }

pw_test_group template
``````````````````````
``pw_test_group`` defines a collection of tests or other test groups. It creates
several sub-targets:

* ``<target_name>``: The test group itself.
* ``<target_name>.run``: If ``pw_unit_test_AUTOMATIC_RUNNER`` is set, this
  target runs all of the tests in the group and all of its group dependencies
  individually.
* ``<target_name>.lib``: The sources of all of the tests in this group and its
  dependencies.
* ``<target_name>.bundle``: All of the tests in the group and its dependencies
  bundled into a single binary.
* ``<target_name>.bundle.run``: Automatic runner for the test bundle.

**Arguments**

* ``tests``: List of the ``pw_test`` targets in the group.
* ``group_deps``: List of other ``pw_test_group`` targets on which this one
  depends.
* ``enable_if``: Boolean indicating whether the group target should be created.
  If false, an empty GN group is created instead. Default true.

**Example**

.. code::

  import("$dir_pw_unit_test/test.gni")

  pw_test_group("tests") {
    tests = [
      ":bar_test",
      ":foo_test",
    ]
  }

  pw_test("foo_test") {
    # ...
  }

  pw_test("bar_test") {
    # ...
  }

pw_facade_test template
```````````````````````
Pigweed facade test templates allow individual unit tests to build under the
current device target configuration while overriding specific build arguments.
This allows these tests to replace a facade's backend for the purpose of testing
the facade layer.

Facade tests are disabled by default. To build and run facade tests, set the GN
arg :option:`pw_unit_test_FACADE_TESTS_ENABLED` to ``true``.

.. warning::
   Facade tests are costly because each facade test will trigger a re-build of
   every dependency of the test. While this sounds excessive, it's the only
   technically correct way to handle this type of test.

.. warning::
   Some facade test configurations may not be compatible with your target. Be
   careful when running a facade test on a system that heavily depends on the
   facade being tested.

Build arguments
```````````````
.. option:: pw_unit_test_GOOGLETEST_BACKEND <source_set>

  The GoogleTest implementation to use for Pigweed unit tests. This library
  provides "gtest/gtest.h" and related headers. Defaults to pw_unit_test:light,
  which implements a subset of GoogleTest.

  Type: string (GN path to a source set)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_MAIN <source_set>

  Implementation of a main function for ``pw_test`` unit test binaries.

  Type: string (GN path to a source set)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_AUTOMATIC_RUNNER <executable>

  Path to a test runner to automatically run unit tests after they are built.

  If set, a ``pw_test`` target's ``<target_name>.run`` action will invoke the
  test runner specified by this argument, passing the path to the unit test to
  run. If this is unset, the ``pw_test`` target's ``<target_name>.run`` step
  will do nothing.

  Targets that don't support parallelized execution of tests (e.g. a on-device
  test runner that must flash a device and run the test in serial) should
  set pw_unit_test_POOL_DEPTH to 1.

  Type: string (name of an executable on the PATH, or path to an executable)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_AUTOMATIC_RUNNER_ARGS <args>

  An optional list of strings to pass as args to the test runner specified
  by pw_unit_test_AUTOMATIC_RUNNER.

  Type: list of strings (args to pass to pw_unit_test_AUTOMATIC_RUNNER)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_AUTOMATIC_RUNNER_TIMEOUT <timeout_seconds>

  An optional timeout to apply when running the automatic runner. Timeout is
  in seconds. Defaults to empty which means no timeout.

  Type: string (number of seconds to wait before killing test runner)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_POOL_DEPTH <pool_depth>

  The maximum number of unit tests that may be run concurrently for the
  current toolchain. Setting this to 0 disables usage of a pool, allowing
  unlimited parallelization.

  Note: A single target with two toolchain configurations (e.g. release/debug)
        will use two separate test runner pools by default. Set
        pw_unit_test_POOL_TOOLCHAIN to the same toolchain for both targets to
        merge the pools and force serialization.

  Type: integer
  Usage: toolchain-controlled only

.. option:: pw_unit_test_POOL_TOOLCHAIN <toolchain>

  The toolchain to use when referring to the pw_unit_test runner pool. When
  this is disabled, the current toolchain is used. This means that every
  toolchain will use its own pool definition. If two toolchains should share
  the same pool, this argument should be by one of the toolchains to the GN
  path of the other toolchain.

  Type: string (GN path to a toolchain)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_EXECUTABLE_TARGET_TYPE <template name>

  The name of the GN target type used to build pw_unit_test executables.

  Type: string (name of a GN template)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_EXECUTABLE_TARGET_TYPE_FILE <gni file path>

  The path to the .gni file that defines pw_unit_test_EXECUTABLE_TARGET_TYPE.

  If pw_unit_test_EXECUTABLE_TARGET_TYPE is not the default of
  `pw_executable`, this .gni file is imported to provide the template
  definition.

  Type: string (path to a .gni file)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_FACADE_TESTS_ENABLED <boolean>

  Controls whether to build and run facade tests. Facade tests add considerably
  to build time, so they are disabled by default.

CMake
-----
pw_add_test function
````````````````````
``pw_add_test`` declares a single unit test suite. It creates several
sub-targets.

* ``{NAME}`` depends on ``${NAME}.run`` if ``pw_unit_test_AUTOMATIC_RUNNER`` is
  set, else it depends on ``${NAME}.bin``
* ``{NAME}.lib`` contains the provided test sources as a library target, which
  can then be linked into a test executable.
* ``{NAME}.bin`` is a standalone executable which contains only the test sources
  specified in the pw_unit_test_template.
* ``{NAME}.run`` which runs the unit test executable after building it if
  ``pw_unit_test_AUTOMATIC_RUNNER`` is set, else it fails to build.

**Required Arguments**

* ``NAME``: name to use for the produced test targets specified above

**Optional Arguments**

* ``SOURCES`` - source files for this library
* ``HEADERS``- header files for this library
* ``PRIVATE_DEPS``- private pw_target_link_targets arguments
* ``PRIVATE_INCLUDES``- public target_include_directories argument
* ``PRIVATE_DEFINES``- private target_compile_definitions arguments
* ``PRIVATE_COMPILE_OPTIONS``- private target_compile_options arguments
* ``PRIVATE_LINK_OPTIONS``- private target_link_options arguments

**Example**

.. code::

  include($ENV{PW_ROOT}/pw_unit_test/test.cmake)

  pw_add_test(my_module.foo_test
    SOURCES
      foo_test.cc
    PRIVATE_DEPS
      my_module.foo
  )

pw_add_test_group function
``````````````````````````
``pw_add_test_group`` defines a collection of tests or other test groups. It
creates several sub-targets:

* ``{NAME}`` depends on ``${NAME}.run`` if ``pw_unit_test_AUTOMATIC_RUNNER`` is
  set, else  it depends on ``${NAME}.bin``.
* ``{NAME}.bundle`` depends on ``${NAME}.bundle.run`` if
  ``pw_unit_test_AUTOMATIC_RUNNER`` is set, else it depends on
  ``${NAME}.bundle.bin``.
* ``{NAME}.lib`` depends on ``${NAME}.bundle.lib``.
* ``{NAME}.bin`` depends on the provided ``TESTS``'s ``<test_dep>.bin`` targets.
* ``{NAME}.run`` depends on the provided ``TESTS``'s ``<test_dep>.run`` targets
  if ``pw_unit_test_AUTOMATIC_RUNNER`` is set, else it fails to build.
* ``{NAME}.bundle.lib`` contains the provided tests bundled as a library target,
  which can then be linked into a test executable.
* ``{NAME}.bundle.bin`` standalone executable which contains the bundled tests.
* ``{NAME}.bundle.run`` runs the ``{NAME}.bundle.bin`` test bundle executable
  after building it if ``pw_unit_test_AUTOMATIC_RUNNER`` is set, else it fails
  to build.

**Required Arguments**

* ``NAME`` - The name of the executable target to be created.
* ``TESTS`` - ``pw_add_test`` targets and ``pw_add_test_group`` bundles to be
  included in this test bundle

**Example**

.. code::

  include($ENV{PW_ROOT}/pw_unit_test/test.cmake)

  pw_add_test_group(tests
    TESTS
      bar_test
      foo_test
  )

  pw_add_test(foo_test
    # ...
  )

  pw_add_test(bar_test
    # ...
  )

Build arguments
```````````````
.. option:: pw_unit_test_GOOGLETEST_BACKEND <target>

  The GoogleTest implementation to use for Pigweed unit tests. This library
  provides "gtest/gtest.h" and related headers. Defaults to pw_unit_test.light,
  which implements a subset of GoogleTest.

  Type: string (CMake target name)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_AUTOMATIC_RUNNER <executable>

  Path to a test runner to automatically run unit tests after they are built.

  If set, a ``pw_test`` target's ``${NAME}`` and ``${NAME}.run`` targets will
  invoke the test runner specified by this argument, passing the path to the
  unit test to run. If this is unset, the ``pw_test`` target's ``${NAME}`` will
  only build the unit test(s) and ``${NAME}.run`` will fail to build.

  Type: string (name of an executable on the PATH, or path to an executable)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_AUTOMATIC_RUNNER_ARGS <args>

  An optional list of strings to pass as args to the test runner specified
  by pw_unit_test_AUTOMATIC_RUNNER.

  Type: list of strings (args to pass to pw_unit_test_AUTOMATIC_RUNNER)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_AUTOMATIC_RUNNER_TIMEOUT_SECONDS <timeout_seconds>

  An optional timeout to apply when running the automatic runner. Timeout is
  in seconds. Defaults to empty which means no timeout.

  Type: string (number of seconds to wait before killing test runner)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_ADD_EXECUTABLE_FUNCTION <function name>

  The name of the CMake function used to build pw_unit_test executables. The
  provided function must take a ``NAME`` and a ``TEST_LIB`` argument which are
  the expected name of the executable target and the target which provides the
  unit test(s).

  Type: string (name of a CMake function)
  Usage: toolchain-controlled only

.. option:: pw_unit_test_ADD_EXECUTABLE_FUNCTION_FILE <cmake file path>

  The path to the .cmake file that defines pw_unit_test_ADD_EXECUTABLE_FUNCTION.

  Type: string (path to a .cmake file)
  Usage: toolchain-controlled only


RPC service
===========
``pw_unit_test`` provides an RPC service which runs unit tests on demand and
streams the results back to the client. The service is defined in
``pw_unit_test_proto/unit_test.proto``, and implemented by the GN target
``$dir_pw_unit_test:rpc_service``.

The RPC service is primarily intended for use with the default
``pw_unit_test:light`` backend. It has some support for the GoogleTest backend,
however some features (such as test suite filtering) are missing.

To set up RPC-based unit tests in your application, instantiate a
``pw::unit_test::UnitTestService`` and register it with your RPC server.

.. code:: c++

  #include "pw_rpc/server.h"
  #include "pw_unit_test/unit_test_service.h"

  // Server setup; refer to pw_rpc docs for more information.
  pw::rpc::Channel channels[] = {
   pw::rpc::Channel::Create<1>(&my_output),
  };
  pw::rpc::Server server(channels);

  pw::unit_test::UnitTestService unit_test_service;

  void RegisterServices() {
    server.RegisterService(unit_test_services);
  }

All tests flashed to an attached device can be run via python by calling
``pw_unit_test.rpc.run_tests()`` with a RPC client services object that has
the unit testing RPC service enabled. By default, the results will output via
logging.

.. code:: python

  from pw_hdlc.rpc import HdlcRpcClient
  from pw_unit_test.rpc import run_tests

  PROTO = Path(os.environ['PW_ROOT'],
               'pw_unit_test/pw_unit_test_proto/unit_test.proto')

  client = HdlcRpcClient(serial.Serial(device, baud), PROTO)
  run_tests(client.rpcs())

pw_unit_test.rpc
----------------
.. automodule:: pw_unit_test.rpc
  :members: EventHandler, run_tests

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module.

.. c:macro:: PW_UNIT_TEST_CONFIG_EVENT_BUFFER_SIZE

  The size of the event buffer that the UnitTestService contains.
  This buffer is used to encode events.  By default this is set to
  128 bytes.

.. c:macro:: PW_UNIT_TEST_CONFIG_MEMORY_POOL_SIZE

  The size of the memory pool to use for test fixture instances. By default this
  is set to 16K.

Using upstream GoogleTest
=========================
Upstream `GoogleTest`_ may be used as the backend for ``pw_unit_test``. A clone
of the GoogleTest repository is required. See the
:ref:`third_party/googletest documentation <module-pw_third_party_googletest>`
for details.
