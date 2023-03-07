Overview
========
The DMAC memory to memory example is a simple demonstration program that uses the SDK software.
It executes one shot transfer from source buffer to destination buffer using the SDK DMAC drivers 
functional interface.
The purpose of this example is to show how to use the DMAC and to provide a simple example for
debugging and further development.
                 

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
When the example runs successfully, you can see the similar information from the terminal as below.

~~~~~~~~~~~~~~~~~~~~~
DMAC memory to memory example begin.



Destination Buffer:

0	0	0	0	



DMAC memory to memory example finish.



Destination Buffer:

1	2	3	4	
~~~~~~~~~~~~~~~~~~~~~






