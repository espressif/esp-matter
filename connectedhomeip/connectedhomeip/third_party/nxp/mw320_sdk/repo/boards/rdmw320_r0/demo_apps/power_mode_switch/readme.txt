Overview
========
The Power mode switch demo application demonstrates the use of power modes in the KSDK. The demo prints the power mode menu
through the debug console, where the user can set the MCU to a specific power mode. The user can also set the wakeup
source by following the debug console prompts. The purpose of this demo is to show how to switch between different power
 modes, and how to configure a wakeup source and wakeup the MCU from low power modes.

 Tips:
 This demo is to show how the various power mode can switch to each other. However, in actual low power use case, to save energy and reduce the consumption even more, many things can be done including:
 - Disable the clock for unnecessary module during low power mode. That means, programmer can disable the clocks before entering the low power mode and re-enable them after exiting the low power mode when necessary.
 - Disable the function for unnecessary part of a module when other part would keep working in low power mode. At the most time, more powerful function means more power consumption. For example, disable the digital function for the unnecessary pin mux, and so on.
 - Set the proper pin state (direction and logic level) according to the actual application hardware. Otherwise, the pin cirrent would be activied unexpectedly waste some energy.
 - Other low power consideration based on the actual application hardware.
 - Debug pins(e.g SWD_DIO) would consume addtional power, had better to disable related pins or disconnect them. 


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
    Note: to verify PM4 mode, the application must be written to flash and be bootable.
    See "Getting Started with MCUXpresso SDK for RDMW320-R0.pdf" for details on how to make the application boot.

Running the demo
================

When running the demo, the debug console shows the menu to command the MCU to the target power mode (PM0 - Active, PM1 - Idle, PM2 - Standby, PM3 - Sleep, PM4 - Shutoff).
NOTE: Only input when the demo asks for input. Input entered at any other time might cause the debug console to overflow and receive the wrong input value.

Here's the initial menu when running successfully.
~~~~~~~~~~~~~~~~~~~~~
MCU wakeup source 0x0...

####################  Power Mode Switch ####################

    Build Time: Jan 18 2021--16:02:06
    Core Clock: 200000000Hz

Select the desired operation

Press 0 for enter: PM0
Press 1 for enter: PM1
Press 2 for enter: PM2
Press 3 for enter: PM3
Press 4 for enter: PM4

Waiting for power mode select..

1
Select the wake up source:
Press T for RTC.
Press U for UART wakeup.

Waiting for key press..

t
Select the wake up timeout in seconds.
The allowed range is 1s ~ 9s.
Eg. enter 5 to wake up in 5 seconds.

Waiting for input timeout value...

2
Will wakeup in 2 seconds.
Woken up by RTC
Exit from power mode 1

####################  Power Mode Switch ####################

    Build Time: Jan 18 2021--16:02:06
    Core Clock: 200000000Hz

Select the desired operation

Press 0 for enter: PM0
Press 1 for enter: PM1
Press 2 for enter: PM2
Press 3 for enter: PM3
Press 4 for enter: PM4

Waiting for power mode select..

2
Input the IO domain bitmap unaffected by PM2:
  - e.g. 5 means VDDIO0 and VDDIO2 will not be powered off during PM2
  -      A means VDDIO1 and VDDIO3 will not be powered off during PM2
0
Select the wake up source:
Press T for RTC.
Press 0 for Pin0 wakeup.
Press 1 for both Pin0/Pin1 wakeup.

Waiting for key press..

t
Select the wake up timeout in seconds.
The allowed range is 1s ~ 9s.
Eg. enter 5 to wake up in 5 seconds.

Waiting for input timeout value...

2
Will wakeup in 2 seconds.
Woken up by RTC
Exit from power mode 2

####################  Power Mode Switch ####################

    Build Time: Jan 18 2021--16:02:06
    Core Clock: 200000000Hz

Select the desired operation

Press 0 for enter: PM0
Press 1 for enter: PM1
Press 2 for enter: PM2
Press 3 for enter: PM3
Press 4 for enter: PM4

Waiting for power mode select..

3
Select the wake up source:
Press T for RTC.
Press 0 for Pin0 wakeup.
Press 1 for both Pin0/Pin1 wakeup.

Waiting for key press..

0
Press wakeup pin0 to wake up.
Woken up by PIN0
Exit from power mode 3

####################  Power Mode Switch ####################

    Build Time: Jan 18 2021--16:02:06
    Core Clock: 200000000Hz

Select the desired operation

Press 0 for enter: PM0
Press 1 for enter: PM1
Press 2 for enter: PM2
Press 3 for enter: PM3
Press 4 for enter: PM4

Waiting for power mode select..

4
Press wakeup pin0 or pin1 to wake up.

MCU wakeup source 0x0...

####################  Power Mode Switch ####################

    Build Time: Jan 18 2021--16:02:06
    Core Clock: 200000000Hz

Select the desired operation

Press 0 for enter: PM0
Press 1 for enter: PM1
Press 2 for enter: PM2
Press 3 for enter: PM3
Press 4 for enter: PM4

Waiting for power mode select..
~~~~~~~~~~~~~~~~~~~~~
