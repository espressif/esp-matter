USBXpress Firmware Library

Overview
----------------------------
USBXpress is an open-source firmware library that sits on top of the EFM32 USB Library.
It simplifies USB firmware development by providing a simple, bi-directional bulk
data pipe as well as a callback function with various USB events (device opened, 
device closed, tx complete, rx complete, etc.). It communicates with the host via 
the WinUSB driver and a DLL provided by Silicon Labs called SiUSBXp.dll (see 
https://www.silabs.com/products/mcu/Pages/USBXpress.aspx for the DLL download).
The firmware library supports Microsoft OS Descriptors, which allows the host
Windows machine to associate the device to the WinUSB driver without any .inf files.

This library is a port of the 8-bit USBXpress library. For this reason, it may 
deviate slightly from best practices for 32-bit ARM firmware development. It does
adhere to the Silicon Labs firwmare coding guidelines as closely as possible.

Target Devices
----------------------------
  - Giant Gecko
  - Happy Gecko
  - Leopard Gecko
  - Wonder Gecko

Known Issues and Limitations
----------------------------

Version Information:
----------------------------

Version 5.0:
  - Initial release for EFM32 (see revision history for 8-bit USBXpress 
                               for more revision information).
