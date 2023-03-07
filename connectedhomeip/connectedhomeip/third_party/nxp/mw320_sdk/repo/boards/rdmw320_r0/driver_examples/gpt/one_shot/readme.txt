Overview
========
The GPT project is a simple demonstration program of the SDK GPT driver. It sets up the GPT
hardware block to output a square waveform with 1s period. If having an LED connected to the
GPT pin, the user will see the LED blinking.

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
Connect IO_01 with IO_40 to allow square wave to drive the LED.

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
These instructions are displayed/shown on the terminal window:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GPT example to output square wave with 1s period.
You will see a blinking LED if an LED is connected to the GPT pin
If no LED is connected to the GPT pin, then probe the signal using an oscilloscope
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
- Amber led will be blinking.
