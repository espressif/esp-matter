.. _target-stm32f429i-disc1:

----------------
stm32f429i-disc1
----------------
The STMicroelectronics STM32F429I-DISC1 development board is currently Pigweed's
primary target for on-device testing and development.

Building
========
To build for this Pigweed target, simply build the top-level "stm32f429i" Ninja
target.

.. code:: sh

  $ ninja -C out stm32f429i

Testing
=======
When working in upstream Pigweed, building this target will build all Pigweed modules' unit tests.
These tests can be run on-device in a few different ways.

Run a unit test
---------------
If using ``out`` as a build directory, tests will be located in
``out/stm32f429i_disc1_debug/obj/[module name]/[test_name].elf``. To run these
on device, the stm32f429i-disc1 target provides a helper script that flashes the
test to a device and then runs it.

.. code:: sh

  # Setup pigweed environment.
  $ source activate.sh
  # Run test.
  $ stm32f429i_disc1_unit_test_runner /path/to/binary

Run multiple tests
------------------
Running all tests one-by-one is rather tedious. To make running multiple
tests easier, use Pigweed's ``pw test`` command and pass it a path to the build
directory and the name of the test runner. By default, ``pw test`` will run all
tests, but it can be restricted it to specific ``pw_test_group`` targets using
the ``--group`` argument. Alternatively, individual test binaries can be
specified with the ``--test`` option.

.. code:: sh

  # Setup Pigweed environment.
  $ source activate.sh
  # Run test.
  $ pw test --root out/stm32f429i_disc_debug/  \
        --runner stm32f429i_disc1_unit_test_runner

Run tests affected by code changes
----------------------------------
When writing code that will impact multiple modules, it's helpful to only run
all tests that are affected by a given code change. Thanks to the GN/Ninja
build, this is possible! This is done by using a ``pw_target_runner_server``
that Ninja can send the tests to as it rebuilds affected targets.

Additionally, this method enables distributed testing. If multiple devices are
connected, the tests will be run across all attached devices to further speed up
testing.

Step 1: Start test server
^^^^^^^^^^^^^^^^^^^^^^^^^
To allow Ninja to properly serialize tests to run on an arbitrary number of
devices, Ninja will send test requests to a server running in the background.
The first step is to launch this server. By default, the script will attempt
to automatically detect all attached STM32f429I-DISC1 boards and use them for
testing. To override this behavior, provide a custom server configuration file
with ``--server-config``.

.. tip::

  If you unplug or plug in any boards, you'll need to restart the test server
  for hardware changes to properly be detected.

.. code:: sh

  $ stm32f429i_disc1_test_server

Step 2: Configure GN
^^^^^^^^^^^^^^^^^^^^
By default, this hardware target has incremental testing via
``pw_target_runner`` disabled. Enabling the ``pw_use_test_server`` build arg
tells GN to send requests to a running ``stm32f429i_disc1_test_server``.

.. code:: sh

  $ gn args out
  # Modify and save the args file to use pw_target_runner.
  pw_use_test_server = true

Step 3: Build changes
^^^^^^^^^^^^^^^^^^^^^
Whenever you run ``ninja -C out stm32f429i``, affected tests will be built and
run on the attached device(s). Alternatively, you may use ``pw watch`` to set up
Pigweed to build/test whenever it sees changes to source files.

RPC server
==========
The stm32f429i target implements a system RPC server that over a simple UART
driver. To communicate with a device running the RPC server, run
``pw rpc -d <device> -b 115200 <protos>``.

Debugging
=========
There are multiple ways to debug the device, including using commercial tools
like SEGGER's J-Link. However, the Discovery board has an on-board STLink
debugger, which is supported by the open source OpenOCD debugger. To debug with
OpenOCD requires a few steps. Summary version of the steps:

#. Connect OpenOCD to the device in terminal A. Leave this running

   .. code:: sh

     $ openocd -f targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg

#. Connect GDB to the running OpenOCD instance in terminal B

   .. code:: sh

     $ arm-none-eabi-gdb -ex "target remote :3333" \
       out/stm32f429i_disc1_debug/obj/pw_assert/test/assert_facade_test.elf

#. Flash (``load``), run (``mon reset run; continue``), and debug

   .. code:: none

     (gdb) set print pretty on
     (gdb) load
     (gdb) mon reset run
     (gdb) continue

#. You can re-flash the device after compiling by running ``load``.


Step 1: Start an OpenOCD server and connect to the device
---------------------------------------------------------
OpenOCD is a persistent server that you run and leave running to bridge between
GDB and the device. To run it for the Discovery board:

.. code:: sh

  $ openocd -f targets/stm32f429i_disc1/py/stm32f429i_disc1_utils/openocd_stm32f4xx.cfg

Typical output:

.. code:: none

  Open On-Chip Debugger 0.10.0+dev-01243-ge41c0f49-dirty (2020-05-21-10:27)
  Licensed under GNU GPL v2
  For bug reports, read
          http://openocd.org/doc/doxygen/bugs.html
  DEPRECATED! use 'adapter driver' not 'interface'
  Info : The selected transport took over low-level target control. The results might differ compared to plain JTAG/SWD
  srst_only separate srst_nogate srst_open_drain connect_deassert_srst

  Info : Listening on port 6666 for tcl connections
  Info : Listening on port 4444 for telnet connections
  Info : clock speed 2000 kHz
  Info : STLINK V2J25M14 (API v2) VID:PID 0483:374B
  Info : Target voltage: 2.871879
  Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
  Info : starting gdb server for stm32f4x.cpu on 3333
  Info : Listening on port 3333 for gdb connections

Step 2: Start GDB and connect to the OpenOCD server
---------------------------------------------------
Start GDB pointing to the correct .elf file, and tell it to connect to the
OpenOCD server (running on port 333 by default).

.. code:: sh

  $ arm-none-eabi-gdb -ex "target remote :3333" \
    out/stm32f429i_disc1_debug/obj/pw_assert/test/assert_facade_test.elf

In this case the assert facade test is debugged, but substitute your own ELF
file. This should produce output similar to the following:

.. code:: none

  GNU gdb (GNU Arm Embedded Toolchain 9-2020-q2-update) 8.3.1.20191211-git
  Copyright (C) 2019 Free Software Foundation, Inc.
  License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
  This is free software: you are free to change and redistribute it.
  There is NO WARRANTY, to the extent permitted by law.
  Type "show copying" and "show warranty" for details.
  This GDB was configured as "--host=x86_64-apple-darwin10 --target=arm-none-eabi".
  Type "show configuration" for configuration details.
  For bug reporting instructions, please see:
  <http://www.gnu.org/software/gdb/bugs/>.
  Find the GDB manual and other documentation resources online at:
      <http://www.gnu.org/software/gdb/documentation/>.

  For help, type "help".
  Type "apropos word" to search for commands related to "word"...
  Reading symbols from out/stm32f429i_disc1_debug/obj/pw_assert//test/assert_facade_test.elf...
  Remote debugging using :3333
  pw_BootEntry () at ../pw_boot_cortex_m/core_init.c:117
  117	  }

Step 3: Flash, run, and debug
-----------------------------
Now that the GDB instance is connected to the device, you can flash, run, and debug.

To flash

.. code:: none

  (gdb) load

This will produce output similar to:

.. code:: none

  (gdb) load
  Loading section .vector_table, size 0x10 lma 0x8000000
  Loading section .code, size 0xdb8c lma 0x8000200
  Loading section .ARM, size 0x8 lma 0x800dd90
  Loading section .static_init_ram, size 0x1d0 lma 0x800dd98
  Start address 0x8007c48, load size 56692
  Transfer rate: 25 KB/sec, 8098 bytes/write.

To reset the device and halt on the first instruction (before main):

.. code:: none

  (gdb) mon reset run


This will produce output similar to:

.. code:: none

  (gdb) mon reset run
  Unable to match requested speed 2000 kHz, using 1800 kHz
  Unable to match requested speed 2000 kHz, using 1800 kHz
  target halted due to debug-request, current mode: Thread
  xPSR: 0x01000000 pc: 0x08007930 msp: 0x20030000
  Unable to match requested speed 8000 kHz, using 4000 kHz
  Unable to match requested speed 8000 kHz, using 4000 kHz

The device is now ready for debugging. You can place breakpoints and start the
device with ``continue``.
