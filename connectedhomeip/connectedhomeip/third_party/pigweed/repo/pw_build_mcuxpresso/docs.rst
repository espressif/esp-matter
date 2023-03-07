.. _module-pw_build_mcuxpresso:

-------------------
pw_build_mcuxpresso
-------------------

The ``pw_build_mcuxpresso`` module provides helper utilizies for building a
target based on an NXP MCUXpresso SDK.

The GN build files live in ``third_party/mcuxpresso`` but are documented here.
The rationale for keeping the build files in ``third_party`` is that code
depending on an MCUXpresso SDK can clearly see that their dependency is on
third party, not pigweed code.

Using an MCUXpresso SDK
=======================
An MCUXpresso SDK consists of a number of components, each of which has a set
of sources, headers, pre-processor defines, and dependencies on other
components. These are all described in an XML "manifest" file included in the
SDK package.

To use the SDK within a Pigweed project, the set of components you need must be
combined into a ``pw_source_set`` that you can depend on. This source set will
include all of the sources and headers, along with necessary pre-processor
defines, for those components and their dependencies.

The source set is defined using the ``pw_mcuxpresso_sdk`` template, providing
the path to the ``manifest`` XML, along with the names of the components you
wish to ``include``.

.. code-block:: text

   import("$dir_pw_third_party/mcuxpresso/mcuxpresso.gni")

   pw_mcuxpresso_sdk("sample_project_sdk") {
     manifest = "$dir_pw_third_party/mcuxpresso/evkmimxrt595/EVK-MIMXRT595_manifest_v3_8.xml"
     include = [
       "component.serial_manager_uart.MIMXRT595S",
       "project_template.evkmimxrt595.MIMXRT595S",
       "utility.debug_console.MIMXRT595S",
     ]
   }

   pw_executable("hello_world") {
     sources = [ "hello_world.cc" ]
     deps = [ ":sample_project_sdk" ]
   }

Where the components you include have optional dependencies, they must be
satisfied by the set of components you include otherwise the GN generation will
fail with an error.

Excluding components
--------------------
Components can be excluded from the generated source set, for example to
suppress errors about optional dependencies your project does not need, or to
prevent an unwanted component dependency from being introduced into your
project.

This is accomplished by providing the list of components to ``exclude`` as an
argument to the template.

For example to replace the FreeRTOS kernel bundled with the MCUXpresso SDK with
the Pigweed third-party target:

.. code-block:: text

   pw_mcuxpresso_sdk("freertos_project_sdk") {
     // manifest and includes ommitted for clarity
     exclude = [ "middleware.freertos-kernel.MIMXRT595S" ]
     public_deps = [ "$dir_pw_third_party/freertos" ]
   }

Introducing dependencies
------------------------
As seen above, the generated source set can have dependencies added by passing
the ``public_deps`` (or ``deps``) arguments to the template.

You can also pass the ``allow_circular_includes_from``, ``configs``, and
``public_configs`` arguments to augment the generated source set.

For example it is very common to replace the ``project_template`` component with
a source set of your own that provides modified copies of the files from the
SDK.

To resolve circular dependencies, in addition to the generated source set, two
configs named with the ``__defines`` and ``__includes`` suffixes on the template
name are generated, to provide the pre-processor defines and include paths that
the source set uses.

.. code-block:: text

   pw_mcuxpresso_sdk("my_project_sdk") {
     manifest = "$dir_pw_third_party/mcuxpresso/evkmimxrt595/EVK-MIMXRT595_manifest_v3_8.xml"
     include = [
       "component.serial_manager_uart.MIMXRT595S",
       "utility.debug_console.MIMXRT595S",
     ]
     public_deps = [ ":my_project_config" ]
     allow_circular_includes_from = [ ":my_project_config" ]
   }

   pw_source_set("my_project_config") {
     sources = [ "board.c", "clock_config.c", "pin_mux.c" ]
     public = [ "board.h", "clock_config.h", "pin_mux.h "]
     public_configs = [
       ":my_project_sdk__defines",
       ":my_project_sdk__includes"
     ]
   }

mcuxpresso_builder
==================
``mcuxpresso_builder`` is a utility that contains the backend scripts used by
the GN build scripts in ``third_party/mcuxpresso``. You should only need to
interact with ``mcuxpresso_builder`` directly if you are doing something custom.

project
-------
This command outputs a GN scope describing the result of expanding the set of
included and excluded components.

The ``--prefix`` option specifies the GN location of the SDK files.

.. code-block:: bash

  mcuxpresso_builder project /path/to/manifest.xml \
      --include project_template.evkmimxrt595.MIMXRT595S \
      --include utility.debug_console.MIMXRT595S \
      --include component.serial_manager_uart.MIMXRT595S \
      --exclude middleware.freertos-kernel.MIMXRT595S \
      --prefix //path/to/sdk
