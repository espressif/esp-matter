.. _module-pw_third_party_freertos:

========
FreeRTOS
========

The ``$dir_pw_third_party/freertos/`` module contains various helpers to use
FreeRTOS, including Pigweed backend modules which depend on FreeRTOS.

-------------
Build Support
-------------
This module provides support to compile FreeRTOS with GN and CMake. This is
required when compiling backends modules for FreeRTOS.

GN
==
In order to use this you are expected to configure the following variables from
``$dir_pw_third_party/freertos:freertos.gni``:

#. Set the GN ``dir_pw_third_party_freertos`` to the path of the FreeRTOS
   installation.
#. Set ``pw_third_party_freertos_CONFIG`` to a ``pw_source_set`` which provides
   the FreeRTOS config header.
#. Set ``pw_third_party_freertos_PORT`` to a ``pw_source_set`` which provides
   the FreeRTOS port specific includes and sources.

After this is done a ``pw_source_set`` for the FreeRTOS library is created at
``$dir_pw_third_party/freertos``.

CMake
=====
In order to use this you are expected to set the following variables from
``third_party/freertos/CMakeLists.txt``:

#. Set ``dir_pw_third_party_freertos`` to the path of the FreeRTOS installation.
#. Set ``pw_third_party_freertos_CONFIG`` to a library target which provides
   the FreeRTOS config header.
#. Set ``pw_third_party_freertos_PORT`` to a library target which provides
   the FreeRTOS port specific includes and sources.


.. _third_party-freertos_disable_task_statics:

Linking against FreeRTOS kernel's static internals
==================================================
In order to link against internal kernel data structures through the use of
extern "C", statics can be optionally disabled for the tasks.c source file
to enable use of things like pw_thread_freertos/util.h's ``ForEachThread``.

To facilitate this, Pigweed offers an opt-in option which can be enabled by
configuring GN through
``pw_third_party_freertos_DISABLE_TASKS_STATICS = true`` or CMake through
``set(pw_third_party_freertos_DISABLE_TASKS_STATICS ON CACHE BOOL "" FORCE)``.
This redefines ``static`` to nothing for the ``Source/tasks.c`` FreeRTOS source
file when building through ``$dir_pw_third_party/freertos`` in GN and through
``pw_third_party.freertos`` in CMake.

.. attention:: If you use this, make sure that your FreeRTOSConfig.h and port
  does not rely on any statics inside of tasks.c. For example, you cannot use
  ``PW_CHECK`` for ``configASSERT`` when this is enabled.

As a helper ``PW_THIRD_PARTY_FREERTOS_NO_STATICS=1`` is defined when statics are
disabled to help manage conditional configuration.

We highly recommend
:ref:`our configASSERT wrapper <third_party-freertos_config_assert>` when  using
this configuration, which correctly sets ``configASSERT`` to use ``PW_CHECK` and
``PW_ASSERT`` for you.

-----------------------------
OS Abstraction Layers Support
-----------------------------
Support for Pigweed's :ref:`docs-os_abstraction_layers` are provided for
FreeRTOS via the following modules:

* :ref:`module-pw_chrono_freertos`
* :ref:`module-pw_sync_freertos`
* :ref:`module-pw_thread_freertos`

.. _third_party-freertos_config_assert:

--------------------------
configASSERT and pw_assert
--------------------------
To make it easier to use :ref:`module-pw_assert` with FreeRTOS a helper header
is provided under ``pw_third_party/freertos/config_assert.h`` which defines
``configASSERT`` for you using Pigweed's assert system for your
``FreeRTOSConfig.h`` if you chose to use it.

.. code-block:: cpp

  // Instead of defining configASSERT, simply include this header in its place.
  #include "pw_third_party/freertos/config_assert.h"
