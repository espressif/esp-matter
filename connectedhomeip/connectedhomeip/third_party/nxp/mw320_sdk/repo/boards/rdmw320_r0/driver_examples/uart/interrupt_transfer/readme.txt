Overview
========
The uart_interrupt example shows how to use uart driver in interrupt way:

In this example, one uart instance is connected to PC through uart, the board will
send back all characters that PC sends to the board.

Note: The example echos every 8 characters, so input 8 characters every time.

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
3. Download the program to the target board.
4. Launch the debugger in your IDE to begin running the example.

Running the demo
================
When the demo runs successfully, the log would be seen on the Terminal like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Uart interrupt example
Board receives 8 characters then sends them out
Please input 8 characters at a time
Now please input:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
