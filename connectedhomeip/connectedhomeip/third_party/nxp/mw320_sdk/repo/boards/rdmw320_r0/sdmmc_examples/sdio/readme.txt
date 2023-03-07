Overview
========
The SDIO project is a demonstration program that uses the SDK software. It reads/writes the SDIO card reigister. The purpose of this example is to show how to use SDio driver and this is a very simple example.
Note: If the sdio card need WL_REG_ON, please connect WL_REG_ON to the sdio card VDD pin for this example.

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
The log below shows the output of the hello world demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SDIO card simple example.

Please insert a card into board.

Card inserted.

Read function CIS, in direct way

Read function CIS, in extended way, non-block mode, non-word aligned size

Read function CIS, in extended way, block mode, non-word aligned size
The read content is consistent.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
