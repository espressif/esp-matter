.. _module-pw_target_runner:

----------------
pw_target_runner
----------------
The target runner module implements a gRPC server designed to run executables
in parallel. These executables may be run directly on the host, or flashed to
one or more attached targets.

Overview
--------
The target runner server is responsible for processing requests to distribute
executables among a pool of workers that run in parallel. This allows things
like unit tests to be run across multiple devices simultaneously, greatly
reducing the overall time it takes to run a collection of tests.

Additionally, the server allows many executables to be queued up at once and
scheduled across available devices, making it possible to automatically run unit
tests from a Ninja build after code is updated. This integrates nicely with the
``pw watch`` command to re-run all affected unit tests after modifying code.

The target runner is implemented as a library in various programming languages.
This library provides the core gRPC server and a mechanism through which worker
routines can be registered. Code using the library instantiates a server with
some custom workers for the desired target to run passed executables.

The ``pw_target_runner`` module also provides a standalone
``pw_target_runner_server`` program which runs the server with configurable
workers that launch external processes to execute passed binaries. This
program should be sufficient to quickly get unit tests running in a simple
setup, such as multiple devices plugged into a development machine.

Standalone executable
---------------------
This section describes how to use the ``pw_target_runner_server`` program to
set up a simple unit test server with workers.

Configuration
^^^^^^^^^^^^^
The standalone server is configured from a file written in Protobuf text format
containing a ``pw.target_runner.ServerConfig`` message as defined in
``//pw_target_runner/pw_target_runner_server_protos/exec_server_config.proto``.

At least one ``worker`` message must be specified. Each of the workers refers to
a script or program which is invoked with the path to an executable file as a
positional argument. Other arguments provided to the program must be options/
switches.

For example, the config file below defines two workers, each connecting to an
STM32F429I Discovery board with a specified serial number.

**server_config.txt**

.. code:: text

  runner {
    command: "stm32f429i_disc1_unit_test_runner"
    args: "--openocd-config"
    args: "targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg"
    args: "--serial"
    args: "066DFF575051717867013127"
  }

  runner {
    command: "stm32f429i_disc1_unit_test_runner"
    args: "--openocd-config"
    args: "targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg"
    args: "--serial"
    args: "0667FF494849887767196023"
  }


Running the server
^^^^^^^^^^^^^^^^^^
To start the standalone server, run the ``pw_target_runner_server`` program and
point it to your config file.

.. code:: text

  $ pw_target_runner_server -config server_config.txt -port 8080


Sending requests
^^^^^^^^^^^^^^^^
To request the server to run an executable, run the ``pw_target_runner_client``,
specifying the path to the executable through a ``-binary`` option.

.. code:: text

  $ pw_target_runner_client -host localhost -port 8080 -binary /path/to/my/test.elf

This command blocks until the executable has finished running. Multiple
requests can be scheduled in parallel; the server will distribute them among its
available workers.

Library APIs
------------
To use the target runner library in your own code, refer to one of its
programming language APIs below.

.. toctree::
  :maxdepth: 1

  go/docs
