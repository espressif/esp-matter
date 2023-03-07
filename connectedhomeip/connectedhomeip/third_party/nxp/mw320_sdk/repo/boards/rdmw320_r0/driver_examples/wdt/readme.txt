Overview
========
The WDT project is a simple demonstration program of the SDK WDT driver. It initializes the 
WDT module and enables interrupt to show the way of refreshing/clear using interrupt. Then 
disable interrupt and refresh the WDT module periodically, finally leave the WDT not refreshed 
to trigger time-out reset. 

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
 WDT Driver example

 WDT will enter timeout interrupt, clear status and wait for next interrupt.

 WDT interrupt again, this time refresh the counter.

 WDT refreshed, disable interrupt and refresh periodically to avoid reset.

 Stop refreshing, WDOG will reset.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
