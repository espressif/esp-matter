Overview
========
The uart_hardware_flow_control example project is to demonstrate usage of the hardware flow control function.
This example will send data to itself, and hardware flow control is enabled in the example.
The CTS(clear-to-send) pin is for transmiter to check if receiver is ready, if the CTS pin is asserted, transmiter start
to send data. The RTS(request-to-send) is a pin for receiver to inform the transmiter if receiver is ready to receive
data. So, please connect RTS to CTS pin directly.

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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Pin Name    Board Location    connect to     Pin Name    Board Location
UART2_TX    HD4   IO_48       ----------     UART2_RX    HD4   IO_49
UART2_CTS   HD4   IO_46       ----------     UART2_RTS   HD4   IO_47
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
This is UART hardware flow control example on one board.
This example will send data to itself and will use hardware flow control to avoid the overflow.
Please make sure you make the correct line connection. Basically, the connection is: 
      UART_TX    --     UART_RX    
      UART_RTS   --     UART_CTS   
Data matched! Transfer successfully.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
