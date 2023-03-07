.. _module-pw_watch:

========
pw_watch
========
``pw_watch`` is similar to file system watchers found in the web development
space. These watchers trigger a web server reload on source change, increasing
iteration. In the embedded space, file system watchers are less prevalent but no
less useful! The Pigweed watcher module makes it easy to instantly compile,
flash, and run tests upon save.

.. image:: doc_resources/pw_watch_on_device_demo.gif

.. note::

  ``pw_watch`` currently only works with Pigweed's GN and CMake builds.

------------
Module Usage
------------
The simplest way to get started with ``pw_watch`` is to launch it from a shell
using the Pigweed environment as ``pw watch``. By default, ``pw_watch`` watches
for repository changes and triggers the default Ninja build target at out/. To
override this behavior, provide the ``-C`` argument to ``pw watch``.

.. code:: sh

  # Use ./out/ as the build directory and build the default target
  pw watch

  # Use ./out/ as the build directory and build the stm32f429i target
  pw watch python.lint stm32f429i

  # Build pw_run_tests.modules in the out/cmake directory
  pw watch -C out/cmake pw_run_tests.modules

  # Build the default target in out/ and pw_apps in out/cmake
  pw watch -C out -C out/cmake pw_apps

  # Build python.tests in out/ and build pw_apps in out/cmake
  pw watch python.tests -C out/cmake pw_apps

  # Build the default target, but only run up to 8 jobs in parallel.
  pw watch -j8

  # Build the default target and keep building past the first error as far as
  # possible.
  pw watch --keep-going

  # Build the default target and start a docs server on http://127.0.0.1:8000
  pw watch --serve-docs

  # Build the default target and start a docs server on http://127.0.0.1:5555
  pw watch --serve-docs --serve-docs-port=5555

  # Build with a full screen terminal user interface similar to pw_console.
  pw watch --fullscreen

``pw watch`` only rebuilds when a file that is not ignored by Git changes.
Adding exclusions to a ``.gitignore`` causes watch to ignore them, even if the
files were forcibly added to a repo. By default, only files matching certain
extensions are applied, even if they're tracked by Git. The ``--patterns`` and
``--ignore_patterns`` arguments can be used to include or exclude specific
patterns. These patterns do not override Git's ignoring logic.

The ``--exclude_list`` argument can be used to exclude directories from being
watched. This decreases the number of files monitored with inotify in Linux.

By default, ``pw watch`` automatically restarts an ongoing build when files
change. This can be disabled with the ``--no-restart`` option. While running
``pw watch``, you may also press enter to immediately restart a build.

When using ``--serve-docs``, by default the docs will be rebuilt when changed,
just like code files. However, you will need to manually reload the page in
your browser to see changes. If you install the ``httpwatcher`` Python package
into your Pigweed environment (``pip install httpwatcher``), docs pages will
automatically reload when changed.

See ``pw watch -h`` for the full list of command line arguments.

---------------------
Unit Test Integration
---------------------
Thanks to GN's understanding of the full dependency tree, only the tests
affected by a file change are run when ``pw_watch`` triggers a build. By
default, host builds using ``pw_watch`` will run unit tests. To run unit tests
on a device as part of ``pw_watch``, refer to your device's
:ref:`target documentation<docs-targets>`.

