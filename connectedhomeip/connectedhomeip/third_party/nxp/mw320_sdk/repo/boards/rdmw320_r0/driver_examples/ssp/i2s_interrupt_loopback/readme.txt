Overview
========
The SSP I2S DMA loopback Example project is a demonstration program that uses the KSDK software.
The example demostrate how to use ssp peripheral driver to simulate i2s protocol and transmit/receive data through ssp driver.

Toolchain supported
===================
- IAR embedded Workbench  8.50.9
- GCC ARM Embedded  10.2.1

Hardware requirements
=====================
- Mini USB cable
- RDMW320-R0 board
- J-Link Debug Probe
- Personal Computer

Board settings
==============
Connect IO-48 to IO-49.

Prepare the Demo
================
1.  Connect a USB cable between the host PC and Mini USB port on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Launch the debugger in your IDE to begin running the example.

Running the demo
================
These instructions are displayed/shown on the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SSP i2s interrupt loopback Driver example

SSP i2s interrupt loopback success

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

