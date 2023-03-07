Overview
========
The adc_software_trigger example shows how to software trigger ADC conversion.

In this example, ADC resolution is set as 16bit, the reference voltage is selected as the internal 1.8V bandgap, and input
gain is set as 1. So the input voltage range is from 0 to 1.8V. Users can trigger ADC conversion by pressing any key in the
terminal. ADC interrupt will be asserted once the conversion is completed.

The conversion result can be calculated via the following formula:
    Conversion Result = (Vin / Vref) * 32767

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
- ADC CH0 input signal HD4(IO_42).

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
ADC Software Trigger Example!
Resolution: 16 bit.
Input Mode: Single Ended.
Input Range: 0V to 1.8V.

Calibration Success!
Please press any key to trigger conversion.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

