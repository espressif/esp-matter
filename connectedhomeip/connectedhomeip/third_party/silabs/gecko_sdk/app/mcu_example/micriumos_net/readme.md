# Micrium OS Network Example

  This example shows how to use the Micrium OS network stack with the ETH 
  peripheral on the EFM32GG11B starter kit. This example will initialize 
  the RMII interface to the external and setup a 100 Mbit connection.

  The example will output messages on the VCOM port and it will show status 
  on the memory LCD display on the kit. The display will show the current 
  link status along with the current IPv4 address and the currently 
  configured IPv6 addresses.

  Micrium OS Support SEGGER SystemView to view the runtime behavior or a system.
  SystemView Trace is enabled by default when the segger_systemview component
  is included in the project. SystemView can be used to inspect the runtime
  behaviour of this example. It will give an overview
  of the tasks and interrupts in the application. SystemView can be downloaded 
  from https://www.segger.com/systemview.html.