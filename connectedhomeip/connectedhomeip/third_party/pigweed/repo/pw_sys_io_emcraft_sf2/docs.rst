.. _module-pw_sys_io_emcraft_sf2:

---------------------
pw_sys_io_emcraft_sf2
---------------------

``pw_sys_io_emcraft_sf2`` implements the ``pw_sys_io`` facade over
UART.

The Emcraft SF2 sys IO backend provides a UART driver layer that allows
applications built against the ``pw_sys_io`` interface to run on a
SmartFusion/2 chip and do simple input/output via UART. However, this should
work with all Smartfusion/2 variations.

This backend allows you to configure which UART to use. The point of it is to
provide bare-minimum platform code needed to do UART reads/writes.

Setup
=====
This module requires relatively minimal setup:

  1. Write code against the ``pw_sys_io`` facade.
  2. Specify the ``dir_pw_sys_io_backend`` GN global variable to point to this
     backend.
  3. pw_sys_io_Init() provided by this module needs to be called in early boot
     to get pw_sys_io into a working state.
  4. Build an executable with a main() function using a toolchain that
     supports Cortex-M3.

.. note::
  This module provides early firmware init, so it will conflict with other
  modules that do any early device init.

Module usage
============
After building an executable that utilizes this backend, flash the
produced .elf binary to the development board. Then, using a serial
communication terminal like minicom/screen (Linux/Mac) or TeraTerm (Windows),
connect to the device at a baud rate of 57600 (8N1).

Dependencies
============
  * ``pw_sys_io`` facade
  * ``pw_preprocessor`` module
