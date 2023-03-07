# Micrium OS CANopen Example

This example shows how to use the Micrium OS CANopen stack. It uses the
EFM32GG11B starter kit's two CAN peripherals in external loopback mode.  
It requires CAN expansion board ISO-CAN-EXP REV 1.0 or REV 2.0. 
When the user presses either push buttons (BTN0 or BTN1), this example will update one entry 
in the CANopen object dictionary of node 1 on the 'can0' bus with a predefined 
value for each button. Upon changing the value, a PDO message will be
triggered, which will be caught by node 2 on the 'can1' bus. Node 2 will
in turn update its object dictionary with the received value. The value of
the object of both nodes is continuously displayed on the LCD.

Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
SystemView Trace is enabled by default and can be disabled by changing the 
OS_CFG_TRACE_EN configuration inside the os_cfg.h file. SystemView can be used
to inspect the runtime behaviour of this example, it will give a nice overview
of the tasks and interrupts in the application. SystemView can be downloaded
from https://www.segger.com/systemview.html.

*Board*:  Silicon Labs SLSTK3701A_EFM32GG11 Starter Kit  
*Device*: EFM32GG11B820F2048GL192