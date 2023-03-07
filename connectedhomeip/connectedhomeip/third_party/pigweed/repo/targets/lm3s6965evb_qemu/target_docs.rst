.. _target-lm3s6965evb-qemu:

----------------
lm3s6965evb-qemu
----------------
This target is specifically for emulation of the Texas Instruments Stellaris
LM3S lm3s6965evb using QEMU. This may be useful for testing ARMv7-M code without
physical hardware.

This target configuration has **not** been tested on the physical Stellaris
development board.

Building
========
To build for this Pigweed target, simply build the top-level "qemu_gcc" Ninja
target.

.. code:: sh

  $ ninja -C out qemu_gcc

Testing
=======
This target does not yet support automatic test running (though it would be
relatively easy to do so). To run a QEMU binary, see the instructions below.

Executing Binaries
==================
When running a QEMU binary, you may chose to run it interactively with GDB, or
allow the binary to run in a hands-off manner.

Running Without GDB
-------------------
When running without GDB, the firmware will execute normally without requiring
further interaction.

.. code:: sh

  $ qemu-system-arm -cpu cortex-m3 -machine lm3s6965evb \
    -nographic -no-reboot \
    -kernel path/to/firmware.elf

Run With GDB
------------------
When running with GDB, execution of the binary will begin in a halted state. To
begin running the code, you must connect using GDB, set any breakpoints you
wish, and then continue execution.

.. code:: sh

  # Start the VM and GDB server.
  $ qemu-system-arm -cpu cortex-m3 -machine lm3s6965evb \
    -gdb tcp::3333 -S
    -nographic -no-reboot \
    -kernel path/to/firmware.elf

In another window

.. code:: sh

  $ arm-none-eabi-gdb path/to/firmare.elf
  (gdb) target remote :3333
  (gdb) break SomeFunction()
  (gdb) continue
