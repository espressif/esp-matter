Overview
========
The acomp basic driver example demostrates the basic usage of the ACOMP module. This example compares the user input
analog signal with interanl reference voltage(VDDIO_3 * 0.5) and will toggle the LED when the result changes. The purpose
of this demo is to show how to use the ACOMP driver in SDK software. In this driver example the 'outPinMode' is set
as 'kACOMP_PinOutSynInverted', so the output signal from gpio is the inversion of the comparator actual output signal.

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
This example project does not call for any special hardware configurations.
Although not required, the recommendation is to leave the development board's jumper settings
and configurations in default state when running this example.

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
These instructions are displayed/shown on the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ACOMP Basic Example!

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Connect the analog signal to HD4(IO_42), if input analog signal's voltage greater than 1.65V then D8 will be turned on, else D8 will be turned off.
