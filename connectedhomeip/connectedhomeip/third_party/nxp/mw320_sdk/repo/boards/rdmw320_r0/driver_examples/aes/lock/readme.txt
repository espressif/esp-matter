Overview
========
This project demostrates how to use AES driver for CBC, ECB, CTR, CCM, MMO
calculation in multi-task case. In this example, there are two tasks running
at the same time to do AES calculation. To make sure the current calculation
is not interrupted by the other task, the AES functions with lock are used.
The rtos mutex protects the AES calculation.

When the example runs, the log of two tasks are output in the debug console.
If error happens, the example stuck and throws error information. If no error
happens, the example runs forever.

The output log is like this:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AES lock example started























...

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
No special settings are required.

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
The log described in overview is shown in terminal.
