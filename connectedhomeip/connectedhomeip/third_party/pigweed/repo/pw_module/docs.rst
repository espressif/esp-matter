.. _module-pw_module:

---------
pw_module
---------
The ``pw_module`` module contains tools for managing Pigweed modules.
For information on the structure of a Pigweed module, refer to
:ref:`docs-module-guides`.

Commands
--------

.. _module-pw_module-module-check:

``pw module check``
^^^^^^^^^^^^^^^^^^^
The ``pw module check`` command exists to ensure that your module conforms to
the Pigweed module norms.

For example, at time of writing ``pw module check pw_module`` is not passing
its own lint:

.. code-block:: none

  $ pw module check pw_module

   ▒█████▄   █▓  ▄███▒  ▒█    ▒█ ░▓████▒ ░▓████▒ ▒▓████▄
    ▒█░  █░ ░█▒ ██▒ ▀█▒ ▒█░ █ ▒█  ▒█   ▀  ▒█   ▀  ▒█  ▀█▌
    ▒█▄▄▄█░ ░█▒ █▓░ ▄▄░ ▒█░ █ ▒█  ▒███    ▒███    ░█   █▌
    ▒█▀     ░█░ ▓█   █▓ ░█░ █ ▒█  ▒█   ▄  ▒█   ▄  ░█  ▄█▌
    ▒█      ░█░ ░▓███▀   ▒█▓▀▓█░ ░▓████▒ ░▓████▒ ▒▓████▀

  20191205 17:05:19 INF Checking module: pw_module
  20191205 17:05:19 ERR PWCK005: Missing ReST documentation; need at least e.g. "docs.rst"
  20191205 17:05:19 ERR FAIL: Found errors when checking module pw_module


.. _module-pw_module-module-create:

``pw module create``
^^^^^^^^^^^^^^^^^^^^
The ``pw module create`` command is used to generate all of the required
boilerplate for a new Pigweed module.

.. note::

   ``pw module create`` is still under construction and mostly experimental.
   It is only usable in upstream Pigweed, and has limited feature support, with
   a command-line API subject to change.

   Once the command is more stable, it will be properly documented. For now,
   running ``pw module create --help`` will display the current set of options.
