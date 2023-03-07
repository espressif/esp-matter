.. _docs-build-system:

============
Build system
============
Building software for embedded devices is a complex process. Projects often have
custom toolchains, target different hardware platforms, and require additional
configuration and post-processing of artifacts.

As a modern embedded framework, Pigweed's goal is to collect these embedded use
cases into a powerful and flexible build system, then extend it with support for
modern software development practices.

See :ref:`docs-python-build` for information about Python build automation with
Pigweed.

.. toctree::
  :hidden:

  python_build

What's in a build system?
=========================
A quality build system provides a variety of features beyond compiling code.
Throughout our experience with embedded development, we've found several build
features to be especially useful, and designed Pigweed's build system with them
in mind.

Simple toolchain configuration
------------------------------
Embedded projects often use custom build toolchains for their specific hardware.
Configuring these should be a simple process, both in their initial setup and
later adjustments.

Multi-target builds
-------------------
Virtually every consumer product has firmware that targets different boards or
MCUs during development. While building for a single board is simple enough, the
complexity of supporting different targets ranges from changing compiler flags
to swapping out entire libraries of firmware and drivers. This is often done by
running multiple builds, configuring each one accordingly. In Pigweed, we've
designed our build system with first-class multi-target support in mind,
allowing any number of target configurations to be built simultaneously.

Multi-language support
----------------------
Embedded projects are typically written in C, C++, and assembly. However, it is
possible to have firmware written in other languages, such as Rust.
Additionally, projects may have host-side tooling written in a wide variety of
languages. Having all of these build together proves to be a large time saver.

Custom scripting
----------------
Embedded projects often require post-processing of build artifacts; these may
include:

* Extracting ELF sections into a different container
* Injecting metadata into firmware images
* Image signing
* Creating databases of symbols for debugging
* Extracting string tokens into a database (for example, with
  :ref:`module-pw_tokenizer`)

These are run as steps during a build, facilitated by the build system.

See also
^^^^^^^^

* :ref:`module-pw_build-python-action`

Python
------
Python is a favorite scripting language of many development teams, and here at
Pigweed, we're no exception. Much of Pigweed's host-side tooling is written in
Python. While Python works great for local development, problems can arise when
scripts need to be packaged and distributed for vendors or factory teams. Having
proper support for packaging Python within a build system allows teams to focus
on writing code instead of worrying about distribution.

Size reporting
--------------
On embedded devices, memory is everything. Most projects have some sort of
custom tooling to determine how much flash and RAM space their firmware uses.
Being able to run size reports as part of a build ensures that they are always
up-to-date and allows space usage to be tracked over time.

See also
^^^^^^^^

* :ref:`module-pw_bloat`

Documentation
-------------
An oft-neglected part of software development, documentation is invaluable for
future maintainers of a project. As such, Pigweed has integrated documentation
which builds alongside its code and combines with other build features, such as
size reports, to provide high quality, up-to-date references for developers.

See also
^^^^^^^^

* :ref:`module-pw_docgen`

Unit testing
------------
Unit tests are essential to ensure that the functionality of code remains
consistent as changes are made to avoid accidental regressions. Running unit
tests as part of a build keeps developers constantly aware of the impact of
their changes.

Host-side unit tests
^^^^^^^^^^^^^^^^^^^^
Though Pigweed targets embedded devices, a lot of its code can be run and tested
on a host desktop by swapping out backends to host platform libraries. This is
highly beneficial during development, as it allows tests to consistently run
without having to go through the process of flashing a device.

Device-side unit tests
^^^^^^^^^^^^^^^^^^^^^^
As useful as host-side tests are, they are not sufficient for developing actual
firmware, and it is critical to run tests on the actual hardware. Pigweed has
invested into creating a test framework and build integration for running tests
across physical devices as part of a build.

See also
^^^^^^^^

* :ref:`module-pw_unit_test`
* :ref:`module-pw_target_runner`

Bonus: pw watch
---------------
In web development, it is common to have a file system watcher listening for
source file changes and triggering a build for quick iteration. When combined
with a fast incremental build system, this becomes a powerful feature, allowing
things such as unit tests and size reports to re-run whenever any dependent
code is modified.

While initially seen as somewhat of a gimmick, Pigweed's watcher has become a
staple of Pigweed development, with most Pigweed users having it permanently
running in a terminal window.

See also
^^^^^^^^

* :ref:`module-pw_watch`

Pigweed's build systems
=======================
Pigweed can be used either as a monolith or à la carte, slotting into an
existing project. To this end, Pigweed supports multiple build systems, allowing
Pigweed-based projects to choose the most suitable one for them.

Of the supported build systems, GN is the most full-featured, followed by CMake,
and finally Bazel.

.. note::
  A quick note on terminology: the word "target" is overloaded within GN/Bazel (and
  Pigweed)---it can refer to either a GN/Bazel build target, such as a ``source_set``
  or ``executable``, or to an output platform (e.g. a specific board, device, or
  system).

  To avoid confusing the two, we refer to the former as "GN/Bazel targets" and the
  latter as "Pigweed targets".

GN
--
A perhaps unfamiliar name, `GN (Generate Ninja)`_ is a meta-build system that
outputs `Ninja`_ build files, originally designed for use in Chromium. Pigweed
first experimented with GN after hearing about it from another team, and we
quickly came to appreciate its speed and simplicity. GN has become Pigweed's
primary build system; it is used for all upstream development and strongly
recommended for Pigweed-based projects where possible.

.. _CMake: https://cmake.org/
.. _Bazel: https://bazel.build/
.. _GN (Generate Ninja): https://gn.googlesource.com/gn
.. _Ninja: https://ninja-build.org/

The GN build
============
This section describes Pigweed's GN build structure, how it is used upstream,
build conventions, and recommendations for Pigweed-based projects. While
containing some details about how GN works in general, this section is not
intended to be a guide on how to use GN. To learn more about the tool itself,
refer to the official `GN reference`_.

.. _GN reference: https://gn.googlesource.com/gn/+/HEAD/docs/reference.md

Entrypoint: .gn
---------------
The entrypoint to a GN build is the ``.gn`` file, which defines a project's root
directory (henceforth ``//``).

``.gn`` must point to the location of a ``BUILDCONFIG.gn`` file for the project.
In Pigweed upstream, this is its only purpose.

Downstream projects may additionally use ``.gn`` to set global overrides for
Pigweed's build arguments, which apply across all Pigweed targets. For example,
a project could configure the protobuf libraries that it uses. This is done by
defining a ``default_args`` scope containing the overrides.

.. code::

  # The location of the BUILDCONFIG file.
  buildconfig = "//BUILDCONFIG.gn"

  # Build arguments set across all Pigweed targets.
  default_args = {
    dir_pw_third_party_nanopb = "//third_party/nanopb-0.4.2"
  }

Configuration: BUILDCONFIG.gn
-----------------------------
The file ``BUILDCONFIG.gn`` configures the GN build by defining any desired
global variables/options. It can be located anywhere in the build tree, but is
conventionally placed at the root. ``.gn`` points GN to this file.

``BUILDCONFIG.gn`` is evaluated before any other GN files, and variables defined
within it are placed into GN's global scope, becoming available in every file
without requiring imports.

The options configured in this file differ from those in ``.gn`` in two ways:

1. ``BUILDCONFIG.gn`` is evaluated for every GN toolchain (and Pigweed target),
   whereas ``.gn`` is only evaluated once. This allows ``BUILDCONFIG.gn`` to set
   different options for each Pigweed target.
2. In ``.gn``, only GN build arguments can be overridden. ``BUILDCONFIG.gn``
   allows defining arbitrary variables.

Generally, it is preferable to expose configuration options through build args
instead of globals in ``BUILDCONFIG.gn`` (something Pigweed's build previously
did), as they are more flexible, greppable, and easier to manage. However, it
may make sense to define project-specific constants in ``BUILDCONFIG.gn``.

Pigweed's upstream ``BUILDCONFIG.gn`` does not define any variables; it just
sets Pigweed's default toolchain, which GN requires.

.. _top-level-build:

Top-level GN targets: //BUILD.gn
--------------------------------
The root ``BUILD.gn`` file defines all of the libraries, images, tests, and
binaries built by a Pigweed project. This file is evaluated immediately after
``BUILDCONFIG.gn``, with the active toolchain (which is the default toolchain
at the start of a build).

``//BUILD.gn`` is responsible for enumerating each of the Pigweed targets built
by a project. This is done by instantiating a version of each of the project's
GN target groups with each Pigweed target's toolchain. For example, in upstream,
all of Pigweed's GN targets are contained within the ``pigweed_default`` group.
This group is instantiated multiple times, with different Pigweed target
toolchains.

These groups include the following:

* ``host`` -- builds ``pigweed_default`` with Clang or GCC, depending on the
  platform
* ``host_clang`` -- builds ``pigweed_default`` for the host with Clang
* ``host_gcc`` -- builds ``pigweed_default`` for the host with GCC
* ``stm32f429i`` -- builds ``pigweed_default`` for STM32F429i Discovery board
* ``docs`` -- builds the Pigweed documentation and size reports

Pigweed projects are recommended to follow this pattern, creating a top-level
group for each of their Pigweed targets that builds a common GN target with the
appropriate toolchain.

It is important that no dependencies are listed under the default toolchain
within ``//BUILD.gn``, as it does not configure any build parameters, and
therefore should not evaluate any other GN files. The pattern that Pigweed uses
to achieve this is to wrap all dependencies within a condition checking the
toolchain.

.. code::

  group("my_application_images") {
    deps = []  # Empty in the default toolchain.

    if (current_toolchain != default_toolchain) {
      # This is only evaluated by Pigweed target toolchains, which configure
      # all of the required options to build Pigweed code.
      deps += [ "//images:evt" ]
    }
  }

  # The images group is instantiated for each of the project's Pigweed targets.
  group("my_pigweed_target") {
    deps = [ ":my_application_images(//toolchains:my_pigweed_target)" ]
  }

.. warning::
  Pigweed's default toolchain is never used, so it is set to an empty toolchain
  which doesn't define any tools. ``//BUILD.gn`` contains conditions which check
  that the current toolchain is not the default before declaring any GN target
  dependencies to prevent the default toolchain from evaluating any other BUILD
  files. All GN targets added to the build must be placed under one of these
  conditional scopes.

"default" group
^^^^^^^^^^^^^^^
The root ``BUILD.gn`` file can define a special group named ``default``. If
present, Ninja will build this group when invoked without arguments.

.. tip::
  Defining a ``default`` group makes using ``pw watch`` simple!

Optimization levels
^^^^^^^^^^^^^^^^^^^
Pigweed's ``//BUILD.gn`` defines the ``pw_DEFAULT_C_OPTIMIZATION_LEVEL`` build
arg, which specifies the optimization level to use for the default groups
(``host``, ``stm32f429i``, etc.). The supported values for
``pw_DEFAULT_C_OPTIMIZATION_LEVEL`` are:

* ``debug`` -- create debugging-friendly binaries (``-Og``)
* ``size_optimized`` -- optimize for size (``-Os``)
* ``speed_optimized`` -- optimized for speed, without increasing code size
  (``-O2``)

Pigweed defines versions of its groups in ``//BUILD.gn`` for each optimization
level specified in the ``pw_C_OPTIMIZATION_LEVELS`` list. Rather than relying
on ``pw_DEFAULT_C_OPTIMIZATION_LEVEL``, you may directly build a group at the
desired optimization level: ``<group>_<optimization>``. Examples include
``host_clang_debug``, ``host_gcc_size_optimized``, and
``stm32f429i_speed_optimized``.

Upstream GN target groups
^^^^^^^^^^^^^^^^^^^^^^^^^
In upstream, Pigweed splits its top-level GN targets into a few logical groups,
which are described below. In order to build a GN target, it *must* be listed in
one of the groups in this file.

.. important::

  Pigweed's top-level ``BUILD.gn`` file should not be used by downstream
  projects. Projects that wish to pull all of Pigweed's code into their build
  may use the ``pw_modules`` and ``pw_module_tests`` variables in
  ``modules.gni``.

apps
~~~~
This group defines the application images built in Pigweed. It lists all of the
common images built across all Pigweed targets, such as modules' example
executables. Each Pigweed target can additionally provide its own specific
images through the ``pw_TARGET_APPLICATIONS`` build arg, which is included by
this group.

host_tools
~~~~~~~~~~
This group defines host-side tooling binaries built for Pigweed.

runtime_sanitizers
~~~~~~~~~~~~~~~~~~
This group defines host-side build targets for Clang runtime sanitizers.
Next runtime sanitizers supported:

* ``asan`` -- `AddressSanitizer`_ is a fast memory error detector.
* ``msan`` -- `MemorySanitizer`_ is a detector of uninitialized reads.
* ``ubsan`` -- `UndefinedBehaviorSanitizer`_ is a fast undefined behavior detector.
* ``ubsan_heuristic`` -- `UndefinedBehaviorSanitizer`_ with the following
  additional checks enabled:

   * ``integer``: Checks for undefined or suspicious integer behavior.
   * ``float-divide-by-zero``: Checks for floating point division by zero.
   * ``implicit-conversion``: Checks for suspicious behavior of implicit conversions.
   * ``nullability``: Checks for null as function arg, lvalue and return type.

  These additional checks are heuristic and may not correspond to undefined
  behavior.
* ``tsan`` -- `ThreadSanitizer`_ is a tool that detects data races.

Results of building this group are ``host_clang_<sanitizer>`` build directories
with ``pw_module_tests`` per supported sanitizer.

.. _AddressSanitizer: https://clang.llvm.org/docs/AddressSanitizer.html
.. _MemorySanitizer: https://clang.llvm.org/docs/MemorySanitizer.html
.. _UndefinedBehaviorSanitizer: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
.. _ThreadSanitizer: https://clang.llvm.org/docs/ThreadSanitizer.html

pw_modules
~~~~~~~~~~
This group lists the main libraries for all of Pigweed's modules.

The modules in the ``pw_modules`` group are listed in the ``pw_modules``
variable, which is provided by ``modules.gni``.

pw_module_tests
~~~~~~~~~~~~~~~
All modules' unit tests are collected here, so that they can all be run at once.

The test groups in ``pw_module_tests`` group are listed in the
``pw_module_tests`` variable, which is provided by ``modules.gni``.

pigweed_default
~~~~~~~~~~~~~~~
This group defines everything built in a Pigweed build invocation by collecting
the above groups and conditionally depending on them based on the active Pigweed
target's configuration. Generally, new dependencies should not be added here;
instead, use one of the groups listed above.

The ``pigweed_default`` group is instantiated for each upstream Pigweed target's
toolchain.

Pigweed target instantiations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
These groups wrap ``pigweed_default`` with a specific target toolchain. They are
named after the Pigweed target, e.g. ``host_clang``, ``stm32f429i``, etc.

Other BUILD files: //\*\*/BUILD.gn
----------------------------------
The rest of the ``BUILD.gn`` files in the tree define libraries, configs, and
build args for each of the modules in a Pigweed project.

Project configuration: //build_overrides/pigweed.gni
----------------------------------------------------
Each Pigweed project must contain a Pigweed configuration file at a known
location in the GN build tree. Currently, this file only contains a single build
argument, which must be set to the GN build path to the root of the Pigweed
repository within the project.

Module variables
----------------
As Pigweed is intended to be a subcomponent of a larger project, it cannot assume
where it or its modules is located. Therefore, Pigweed's upstream BUILD.gn files
do not use absolute paths; instead, variables are defined pointing to each of
Pigweed's modules, set relative to a project-specific ``dir_pigweed``.

To depend on Pigweed modules from GN code, import Pigweed's overrides file and
reference these module variables.

.. code::

  # This must be imported before .gni files from any other Pigweed modules. To
  # prevent gn format from reordering this import, it must be separated by a
  # blank line from other imports.

  import("//build_overrides/pigweed.gni")

GN target type wrappers
-----------------------
To facilitate injecting global configuration options, Pigweed defines wrappers
around builtin GN target types such as ``source_set`` and ``executable``. These
are defined within ``$dir_pw_build/target_types.gni``.

.. note::
  To take advantage of Pigweed's flexible target configuration system, use
  ``pw_*`` target types (e.g. ``pw_source_set``) in your BUILD.gn files instead
  of GN builtins.

Pigweed targets
---------------
To build for a specific hardware platform, builds define Pigweed targets. These
are essentially GN toolchains which set special arguments telling Pigweed how to
build. For information on Pigweed's target system, refer to
:ref:`docs-targets`.

The empty toolchain
-------------------
Pigweed's ``BUILDCONFIG.gn`` sets the project's default toolchain to an "empty"
toolchain which does not specify any compilers or override any build arguments.
Downstream projects are recommended to do the same, following the steps
described in :ref:`top-level-build` to configure builds for each of their
Pigweed targets.

.. admonition:: Why use an empty toolchain?

  To support some of its advanced (and useful!) build features, Pigweed requires
  the ability to generate new toolchains on the fly. This requires having
  knowledge of the full configuration of the current toolchain (which is easy if
  it's all defined within a scope), something which is impractical to achieve
  using the default toolchain.

  Additionally, there are some cases where GN treats default and non-default
  toolchains differently. By not using the default toolchain, we avoid having
  to deal with these inconsistencies.

  It is possible to build Pigweed using only the default toolchain, but it
  requires a more complicated setup to get everything working and should be
  avoided unless necessary (for example, when integrating with a large existing
  GN-based project).

Upstream development examples
-----------------------------
If developing for upstream Pigweed, some common build use cases are described
below.

Building a custom executable/app image
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. Define your executable GN target using the ``pw_executable`` template.

   .. code::

     # //foo/BUILD.gn
     pw_executable("foo") {
       sources = [ "main.cc" ]
       deps = [ ":libfoo" ]
     }

2. In the root ``BUILD.gn`` file, add the executable's GN target to the ``apps``
   group.

   .. code::

     # //BUILD.gn
     group("apps") {
       deps = [
         # ...
         "//foo",  # Shorthand for //foo:foo
       ]
     }

3. Run the ninja build to compile your executable. The apps group is built by
   default, so there's no need to provide a target. The executable will be
   compiled for every supported Pigweed target.

   .. code::

     ninja -C out

   Alternatively, build your executable by itself by specifying its path to
   Ninja. When building a GN target manually, the Pigweed target for which it
   is built must be specified on the Ninja command line.

   For example, to build for the Pigweed target ``host_gcc_debug``:

   .. code::

     ninja -C out host_gcc_debug/obj/foo/bin/foo

   .. note::

     The path passed to Ninja is a filesystem path within the ``out`` directory,
     rather than a GN path. This path can be found by running ``gn outputs``.

4. Retrieve your compiled binary from the out directory. It is located at the
   path

   .. code::

     out/<pw_target>/obj/<gn_path>/{bin,test}/<executable>

   where ``pw_target`` is the Pigweed target for which the binary was built,
   ``gn_path`` is the GN path to the BUILD.gn file defining the executable,
   and ``executable`` is the executable's GN target name (potentially with an
   extension). Note that the executable is located within a ``bin`` subdirectory
   in the module (or ``test`` for unit tests defined with ``pw_test``).

   For example, the ``foo`` executable defined above and compiled for the
   Pigweed target stm32f429i_disc1_debug is found at:

   .. code::

     out/stm32f429i_disc1_debug/obj/foo/bin/foo

CMake
-----
A well-known name in C/C++ development, `CMake`_ is widely used by all kinds of
projects, including embedded devices. Pigweed's CMake support is provided
primarily for projects that have an existing CMake build and wish to integrate
Pigweed modules.

Bazel
-----
The open source version of Google's internal build system. `Bazel`_ has been
growing in popularity within the open source world, as well as being adopted by
various proprietary projects. Its modular structure makes it a great fit for
à la carte usage.

.. note::
  Bazel support is experimental and only for the brave for now. If you are
  looking for stable set of build API's please use GN.

The Bazel build
===============
This section describes Pigweed's Bazel build structure, how it is used upstream,
build conventions, and recommendations for Pigweed-based projects. While
containing some details about how Bazel works in general, this section is not
intended to be a guide on how to use Bazel. To learn more about the tool itself,
refer to the official `Bazel reference`_.

.. _Bazel reference: https://www.bazel.build/

General usage
-------------
While described in more detail in the Bazel docs there a few Bazel features that
are of particular importance when targeting embedded platforms. The most
commonly used commands used in bazel are;

.. code:: sh

  bazel build //your:target
  bazel test //your:target
  bazel coverage //your:target

.. note:: Code coverage support is only available on the host for now.

Building
^^^^^^^^
When it comes to building/testing your Bazel target for a specific Pigweed
target (e.g. stm32f429i-discovery) a slight variation is required.

.. code:: sh

  bazel build //your:target \
    --platforms=@pigweed//pw_build/platforms:stm32f429i-disc1

For more information on how to create your own platforms refer to the official
`Bazel platforms reference`_. You may also find helpful examples of constraints
and platforms in the '//pw_build/platforms' and '//pw_build/constraints'
directories.

.. _Bazel platforms reference: https://docs.bazel.build/versions/main/platforms.html

Testing
^^^^^^^
Running tests on an embedded target with Bazel is possible although support for
this is experimental. The easiest way of achieving this at the moment is to use
Bazel's '--run_under' flag. To make this work create a Bazel target
('//your_handler') that;

1. Takes a single argument (the path to the elf) and uploads the elf to your
   Pigweed target.
2. Connects with your target using serial or other communication method.
3. Listens to the communication transport for the keywords ("PASSED", "FAIL")
   and returns (0, 1) respectively if one of the keywords is intercepted. (This
   step assumes you are using the pw_unit_test package and it is configured for
   your target).
4. Run;

   .. code:: sh

    bazel test //your:test --platforms=//your/platform --run_under=//your_handler

Code Coverage
^^^^^^^^^^^^^
Making use of the code coverage functionality in Bazel is straightforward.

1. Add the following lines to your '.bazelrc'.

  .. code:: sh

    coverage --experimental_generate_llvm_lcov
    coverage --combined_report=lcov

2. Generate a combined lcov coverage report. This will produce a combined lcov
   coverage report at the path 'bazel-out/_coverage/_coverage_report.dat'. e.g.

  .. code:: sh

    bazel coverage //pw_log/...

3. View the results using the command line utility 'lcov'.

  .. code:: sh

    lcov --list bazel-out/_coverage/_coverage_report.dat

Configuration
-------------
Generally speaking there are three primary concepts that make up Bazel's
configuration API.

1. Selects
2. Compatibility lists
3. Flags/Build settings

Selects
^^^^^^^
Selects are useful for specifying different dependencies/source depending on the
platform that is currently being targeted. For more information on this please
see the `Bazel selects reference`_. e.g.

.. code:: py

  pw_cc_library(
    name = "some_platform_dependant_library",
    deps = select({
      "@platforms//cpu:armv7e-m": [":arm_libs"],
      "//conditions:default": [":host_libs"],
    }),
  )

Compatibility lists
^^^^^^^^^^^^^^^^^^^
Compatibility lists allow you to specify which platforms your targets are
compatible with. Consider an example where you want to specify that a target is
compatible with only a host os;

.. code:: py

  pw_cc_library(
    name = "some_host_only_lib",
    srcs = ["host.cc"],
    target_compatible_with = select({
      "@platforms//os:windows": [],
      "@platforms//os:linux": [],
      "@platforms//os:ios": [],
      "@platforms//os:macos": [],
      "//conditions:default": ["@platforms//:incompatible"],
    }),
  )

In this case building from or for either Windows/Linux/Mac will be supported, but
other OS's will fail if this target is explicitly depended on. However if
building with a wild card for a non-host platform this target will be skipped
and the build will continue. e.g.

.. code:: sh

  bazel build //... --platforms=@pigweed//pw_build/platforms:cortex_m0

This allows for you to easily create compatibility matricies without adversely
affecting your ability build your entire repo for a given Pigweed target.
For more detailed information on how to use the target_compatible_with attribute
please see `Bazel target_compatible_with reference`_.

Flags/build settings
^^^^^^^^^^^^^^^^^^^^
Flags/build settings are particularly useful in scenarios where you may want to
be able to quickly inject a dependency from the command line but don't
necessarily want to create an entirely new set of constraints to use with a
select statement.

.. note::
  The scope for what is possible with build flags/settings goes well beyond
  what will be described here. For more detailed information on flags/settings
  please see `Bazel config reference`_.

A simple example of when it is useful to use a label_flag is when you want to
swap out a single dependency from the command line. e.g.

.. code:: py

  pw_cc_library(
    name = "some_default_io",
    srcs = ["default_io.cc"],
  )

  pw_cc_library(
    name = "some_other_io",
    srcs = ["other_io.cc"],
  )

  label_flag(
    name = "io",
    default_build_setting = ":some_default_io",
  )

  pw_cc_library(
    name = "some_target_that_needs_io",
    deps = [":io"],
  )

From here the label_flag by default redirects to the target ":some_default_io",
however it is possible to override this from the command line. e.g.

.. code:: sh

  bazel build //:some_target_that_needs_io --//:io=//:some_other_io



.. _Bazel selects reference: https://docs.bazel.build/versions/main/configurable-attributes.html#select-and-dependencies

.. _Bazel target_compatible_with reference: https://docs.bazel.build/versions/main/platforms.html#skipping-incompatible-targets

.. _Bazel config reference: https://docs.bazel.build/versions/main/skylark/config.html


Pigweeds configuration
^^^^^^^^^^^^^^^^^^^^^^
Pigweeds Bazel configuration API is designed to be distributed across the
Pigweed repository and/or your downstream repository. If you are coming from
GN's centralized configuration API it might be useful to think about
Pigweed+Bazel's configuration as the transpose of GN's configuration. The
configuration interface that is supported by Pigweed is designed to start simple
and then grow with your project.

.. note::
  There are plans to extend the configuration API for Bazel. However,
  currently the only configurations that are available under the Bazel+Pigweed
  configuration API is the ability to switch facade backends. For more
  information on what this is please see the
  :ref:`docs-module-structure-facades` section of :ref:`docs-module-structure`.

Consider a scenario that you are building a flight controller for a
spacecraft. But have very little experience with Pigweed and you have just
landed here. First things first you would;

1. Set up your WORKSPACE to fetch the Pigweeds repository. Then add the
   dependencies that you need from Pigweeds WORKSPACE.

2. Add a pigweed_config rule to your WORKSPACE, using Pigweed's default
   configuration.

  .. code:: py

    # WORKSPACE ...
    load("//pw_build:target_config.bzl", "pigweed_config")

    # Configure Pigweeds backend.
    pigweed_config(
        name = "pigweed_config",
        build_file = "@pigweed//targets:default_config.BUILD",
    )

.. note::
  We are aware, that the experience of setting up your WORKSPACE file to work
  with pigweed is less than ideal. This API is under construction, but we are
  working on this!

..
  TODO: Add in a better way to pull down WORKSPACE dependencies in Bazel then
  add docs in here.

Now to explain what is going on here. Housed under the "pigweed_config" remote
repository is a set of configuration flags. These can be used to inject
dependencies into facades to override the default backend.

Continuing on with our scenario, consider that you maybe want to try using the
'//pw_chrono' module. So you create a target in your repository like so;

.. code::

  # BUILD
  pw_cc_library(
    name = "time_is_relative",
    srcs = ["relative_time_on_earth.cc"],
    deps = ["@pigweed//pw_chrono"],
  )

Now this should work out of the box for any host operating system. e.g. Running;

.. code::

  bazel build //:time_is_relative

will produce a working library. But as your probably here because Pigweed offers
a set of embedded libraries you might be interested in running your code on some
random micro-controller/FPGA combined with an RTOS. For now let's assume that by
some coincidence you are using FreeRTOS and are happy to make use
of our default '//pw_chrono' backend for FreeRTOS. You could build the following
with;

.. code:: sh

  bazel build //:time_is_relative \
  --platforms=@pigweed//pw_build/platforms:freertos

There is a fair bit to unpack here in terms of how our configuration system
is determining which dependencies to choose for your build. The dependency
tree (that is important for configuration) in a project such as this would
look like.

.. code::

  @pigweed//pw_chrono:pw_chrono_facade <-----------.
   ^                                               |
   |                            @pigweed//pw_chrono_freertos:system_clock
   |                            (Actual backend)
   |                                               ^
   |                                               |
   |                            @pigweed//pw_chrono:system_clock_backend_multiplexer
   |                            Select backend based on OS:
   |                            [FreeRTOS (X), Embos ( ), STL ( ), Threadx ( )]
   |                                               ^
   |                                               |
  @pigweed//pw_chrono  -------> @pigweed_config//:pw_chrono_system_clock_backend
   ^                            (Injectable)
   |
  //:time_is_relative

So when evaluating this setup Bazel checks the dependencies for '//pw_chrono'
and finds that it depends on "@pigweed_config//:pw_chrono_system_clock_backend" which looks
like this;

.. code:: py

  # pw_chrono config.
  label_flag(
      name = "pw_chrono_system_clock_backend",
      build_setting_default = "@pigweed//pw_chrono:system_clock_backend_multiplexer",
  )

Looking at the 'build_setting_default' we can see that by default it depends
back on the target "@pigweed//pw_chrono:system_clock_backend_multiplexer". If
you only had one backend you could actually just change the
'build_setting_default' to point directly to your backend. However because we
have four different backends we have to use the select semantics to choose the
right one. In this case it looks like;

.. code:: py

  pw_cc_library(
    name = "system_clock_backend_multiplexer",
    visibility = ["@pigweed_config//:__pkg__"],
    deps = select({
        "@pigweed//pw_build/constraints/rtos:freertos":
            ["//pw_chrono_freertos:system_clock"],
        "@pigweed//pw_build/constraints/rtos:embos":
            ["//pw_chrono_embos:system_clock"],
        "@pigweed//pw_build/constraints/rtos:threadx":
            ["//pw_chrono_threadx:system_clock"],
        "//conditions:default": ["//pw_chrono_stl:system_clock"],
    }),
  )

Intuitively you can see that the first option was selected, which terminates
the configuration chain.

Continuing on with our scenario let's say that you have read
:ref:`docs-module-structure` and now want to implement your own backend for
'//pw_chrono' using a hardware RTC. In this case you would create a new
directory 'pw_chrono_my_hardware_rtc'. To ensure that your new backend compiles
with the facade an easy and temporary way to override the dependency tree is
to override the label flag in '@pigweed_config'. For example;

.. code:: sh

  bazel build //:time_is_relative \
    --@pigweed_config//pw_chrono_system_clock_backend=//pw_chrono_my_hardware_rtc:system_clock

This temporarily modifies the build graph to look something like this;

.. code::

  @pigweed//pw_chrono:pw_chrono_facade <-----.
   ^                                         |
   |                      @your_workspace//pw_chrono_my_hardware_rtc:system_clock
   |                      (Actual backend)
   |                                         ^
   |                                         |
  @pigweed//pw_chrono  -> @pigweed_config//:pw_chrono_system_clock_backend
   ^                      (Injectable)
   |
  //:time_is_relative

Now while this is a nice temporary change, but you might find yourself in need
of a more permanent configuration. Particularly if you want to override multiple
different backends. In other words if you had several backends to override, that
would translate to several different command line flags (one for each override).
This problem further compounds as you have multiple Pigweed targets all
requiring different combinations of different backends as you can't even reuse
your command line entries. Instead you would have to memorize the correct
combination of backends for each of your targets.

So continuing on with our scenario, let's say we add a backup micro-controller,
to our spacecraft. But this backup computer doesn't have a hardware RTC. We
still want to share the bulk of the code between the two computers but now we
need two separate implementations for our pw_chrono facade. Let's say we choose
to keep the primary flight computer using the hardware RTC and switch the backup
computer over to use Pigweeds default FreeRTOS backend. In this case we might,
want to do something similar to
'@pigweed//pw_chrono:system_clock_backend_multiplexer' and create selectable
dependencies for the two different computers. Now because there are no default
constraint_setting's that meet our requirements we are going to have to;

1. Create a constraint_setting and a set of constraint_value's for the flight
   computer. For example;

  .. code:: py

    # //platforms/flight_computer/BUILD
    constraint_setting(
      name = "flight_computer",
    )

    constraint_value(
      name = "primary",
      constraint_setting = ":flight_computer",
    )

    constraint_value(
      name = "backup",
      constraint_setting = ":flight_computer",
    )

2. Create a set of platforms that can be used to switch constraint_value's.
   For example;

  .. code:: py

    # //platforms/BUILD
    platform(
      name = "primary_computer",
      constraint_values = ["//platforms/flight_computer:primary"],
    )

    platform(
      name = "backup_computer",
      constraint_values = ["//platforms/flight_computer:backup"],
    )

3. Create a target multiplexer that will select the right backend depending on
   which computer you are using. For example;

  .. code:: py

    # //pw_chrono/BUILD
    load("//pw_build:pigweed.bzl", "pw_cc_library")

    pw_cc_library(
      name = "system_clock_backend_multiplexer",
      deps = select({
        "//platforms/flight_computer:primary": [
          "//pw_chrono_my_hardware_rtc:system_clock",
        ],
        "//platforms/flight_computer:backup": [
          "@pigweed//pw_chrono_freertos:system_clock",
        ],
        "//conditions:default": [
          "@pigweed//pw_chrono_stl:system_clock",
        ],
      }),
    )

4. Copy and paste across the target/default_config.BUILD across from the
   Pigweed repository and modifying the build_setting_default for the target
   'pw_chrono_system_clock_backend' to point to your new system_clock_backend_multiplexer
   target. For example;

   This;

  .. code:: py

    # @pigweed//target:default_config.BUILD
    label_flag(
        name = "pw_chrono_system_clock_backend",
        build_setting_default = "@pigweed//pw_chrono:system_clock_backend_multiplexer",
    )

  Becomes this;

  .. code:: py

    # @your_workspace//target:your_config.BUILD
    label_flag(
      name = "pw_chrono_system_clock_backend",
      build_setting_default =
        "@your_workspace//pw_chrono:system_clock_backend_multiplexer",
    )

5. Switch your workspace 'pigweed_config' rule over to use your custom config.

  .. code:: py

    # WORKSPACE
    pigweed_config(
      name = "pigweed_config",
      build_file = "//target/your_config.BUILD",
    )

Building your target now will result in slightly different build graph. For
example, running;

.. code:: sh

  bazel build //:time_is_relative --platforms=//platforms:primary_computer

Will result in a build graph that looks like;

.. code::

  @pigweed//pw_chrono:pw_chrono_facade <---.
   ^                                        |
   |                     @your_workspace//pw_chrono_my_hardware_rtc:system_clock
   |                     (Actual backend)
   |                                        ^
   |                                        |
   |                     @your_workspace//pw_chrono:system_clock_backend_multiplexer
   |                     Select backend based on OS:
   |                     [Primary (X), Backup ( ), Host only default ( )]
   |                                        ^
   |                                        |
  @pigweed//pw_chrono -> @pigweed_config//:pw_chrono_system_clock_backend
   ^                     (Injectable)
   |
  //:time_is_relative
