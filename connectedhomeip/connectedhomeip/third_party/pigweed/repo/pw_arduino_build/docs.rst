.. _module-pw_arduino_build:

-----------------
pw_arduino_build
-----------------

The ``pw_arduino_build`` module contains both the `arduino_builder`_ command
line utility and an `Arduino Main Wrapper`_.

.. seealso::
   See the :ref:`target-arduino` target documentation for a list of supported
   hardware.

Arduino Main Wrapper
====================

``arduino_main_wrapper.cc`` implements the standard ``setup()`` and ``loop()``
functions [#f1]_ that are expected in Arduino sketches.

Pigweed executables rely on being able to define the ``main()`` function. This
is a problem for Arduino code as each core defines it's own ``main()``. To get
around this the Pigweed Arduino target renames ``main()`` to ``ArduinoMain()``
using a preprocessor macro: ``-Dmain(...)=ArduinoMain()``. This macro only
applies when compiling Arduino core source files. That frees up ``main()`` to be
used elsewhere.

Most Arduino cores will do some internal initialization before calling
``setup()`` followed by ``loop()``. To make sure Pigweed ``main()`` is started
after that early init we run it within ``setup()``:

.. code-block:: cpp

  void setup() {
    pw_arduino_Init();
    // Start Pigweed main()
    main();
  }

  void loop() {}

.. note::
   ``pw_arduino_Init()`` initializes the :ref:`module-pw_sys_io_arduino`
   module.

.. warning::
   You may notice ``loop()`` is empty in ``arduino_main_wrapper.cc`` and never
   called. This will cause any code appearing after ``loop()`` in an Arduino
   core to never be executed. For most cores this should be ok but may cause
   issues in some scenarios.

arduino_builder
===============

``arduino_builder`` is utility that can extract compile and tooling information
from an Arduino core. It's used within Pigweed to shovel compiler flags into
the `GN <https://gn.googlesource.com/gn/>`_ build system. It will also work
without Pigweed and can be used with other build systems.

Full documentation is pending. For now run ``arduino_builder --help`` for
details.

.. rubric::
   Footnotes

.. [#f1]
   See the Arduino Reference documentation on `setup()
   <https://www.arduino.cc/reference/en/language/structure/sketch/setup/>`_, and
   `loop()
   <https://www.arduino.cc/reference/en/language/structure/sketch/loop/>`_.

