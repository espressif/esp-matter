# CLI Micrium OS Application

This example project demonstrates use of the command line interface component using a Micrium OS Kernel Task.

You can use the command line interface over the VCOM serial port.
The example provides three commands:
* echo-str: Echoes the input arguments as strings.
* echo-int: Parses the input arguments as integers before echoing them.
* LED: Allows the user to turn on, turn off, or toggle the LEDs on the board.

## Requirements

Silicon Labs board with USART and on-board LED.