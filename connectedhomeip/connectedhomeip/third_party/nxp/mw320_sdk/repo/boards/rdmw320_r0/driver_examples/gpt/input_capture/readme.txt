Overview
========
The GPT project is a demonstration program of the SDK GPT driver's input capture feature.
The example sets up a GPT channel for counter capture. Once the input signal is received,
this example will print the captured value.

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
Connect IO_24 to IO_01 to allow SW1 to trigger the pulse for capture.

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
Once pushing the SW1 button, the counter will be captured.

The results are displayed/shown on the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~
GPT input capture example

Once the input signal is received the input capture value is printed

Capture counter value is 0x2345(*)
~~~~~~~~~~~~~~~~~~~~~~~
(*) This value maybe difference, depend on input signal.
