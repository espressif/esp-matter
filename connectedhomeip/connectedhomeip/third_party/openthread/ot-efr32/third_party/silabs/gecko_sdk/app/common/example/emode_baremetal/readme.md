# Emode Baremetal Application

This example project demonstrates the power consumption using Energy Profiler for different combinations of emodes, clocks running, with/without DCDC, operation performed. This project uses EMU for entering EM1-4 modes and CMU for handling clocks. 

The required configuration (Emode, Oscillator, and so on) is selected using the LCD display.
* Press Button 1 to cycle through the energy mode options available. 
* Press Button 0 to select the option and start selected test.

After the test, reset the board to select and test for different configurations.

#### Note: 
> This project is a **demo only** to understand the power consumption for various configurations and **should not be used as a starting point for implementation**.
>
>The EMU power configuration register can only be written once after a power-on reset and may be locked to a different configuration. For the emode demo to function properly, a power-on reset may be required to configure the DCDC properly.

## Requirements
Silicon Labs board with 2 buttons and an LCD screen.

