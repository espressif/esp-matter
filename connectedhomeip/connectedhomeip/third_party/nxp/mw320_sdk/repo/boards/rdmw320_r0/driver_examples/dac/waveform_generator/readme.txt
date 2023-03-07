Overview
========
The DAC waveform generator driver example demonstrates how to use the DAC module to generate different types of waveforms
(including triangle waveform, sawtooth waveform, sine waveform, and noise). After starting this example, user can select
the waveform type based on the log, and then the selected waveform will be output from  DAC channel pin.

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
 DAC Waveform Generator Example!

Please select the waveform to be generated:
         A -- Full Triangle wave
         B -- Half Triangle wave
         C -- Sine wave
         D -- Noise wave
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Use the oscilloscope to probe IO_43, then the generated waveform can be observed.
