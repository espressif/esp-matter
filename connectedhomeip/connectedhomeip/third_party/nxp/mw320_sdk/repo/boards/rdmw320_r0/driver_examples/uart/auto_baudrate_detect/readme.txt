Overview
========
The uart_auto_baudrate_detect example shows how to use uart auto baud rate detect feature:

In this example, one uart instance is connected to PC through uart. First, we should send character U(0x55, or any byte whose last bit is 1) to board.
The board will set baud rate automatically. After baud rate is set, the board will send back all characters that PC
sends to the board.

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
    - Any baud rate, suggest 115200
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3. Download the program to the target board.
4. Launch the debugger in your IDE to begin running the example.

Running the demo
================
Set any baud rate in your terminal, and send character U(0x55, or any byte whose last bit is 1) to board,
then if the demo runs successfully, the log would be seen on the debug terminal like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UART has detected one character U
Baud rate has been set automatically!
Board will send back received characters
Please input no more than FIFO depth of data at a time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Note that the auto baud rate detect may fail when the remote transmitter's actual baud rate is lower than
28.8 Kbps and differs by more than 1 percent of its setting. 
