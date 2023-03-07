Overview
========
The DAC normal driver example demostrates the basic useage of DAC module. In this example, users input value from the
terminal, and then the related voltage will be output through DAC output pin.

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
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the example.

Running the demo
================
The log below shows the output of the hello world demo in the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DAC Normal Driver Example!

Output = 0.18V + (1.42V * input_code / 1023)

Please input the value(Ranges from 0 to  1023) to be converted.
1000
Please input the value(Ranges from 0 to  1023) to be converted.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Use the oscilloscope or universal meter to probe IO_43, then the output voltage can be measured.
