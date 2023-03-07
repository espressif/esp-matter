Overview
========
The CRC project is a simple demonstration program of the SDK CRC driver. In this
project the CRC-16 CCITT, CRC-16/IBM, CRC-16 T10_DIF, CRC-32 IEEE, and CRC-16/DNP
are used.

Toolchain supported
===================
- IAR embedded Workbench  8.50.9
- GCC ARM Embedded  10.2.1

Hardware requirements
=====================
- Micro USB cable
- RDMW320-R0 board
- J-Link Debug Probe
- Personal Computer

Board settings
==============
No special settings are required.

Prepare the Demo
================
1.  Connect a micro USB cable between the PC host and the CMSIS DAP USB port (J40) on the board
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Launch the debugger in your IDE to begin running the demo.

Running the demo
================
The log below shows example output of the CRC driver demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CRC Peripheral Driver Example

Test string: 123456789
CRC-16 CCITT: 0x2189
CRC-16 IBM: 0xbb3d
CRC-16 T10_DIF: 0x39ee
CRC-32 IEEE: 0x2dfd2d88
CRC-16 DNP: 0x157d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
