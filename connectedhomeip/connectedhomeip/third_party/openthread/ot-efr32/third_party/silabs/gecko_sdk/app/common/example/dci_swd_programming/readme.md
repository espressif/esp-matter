# Series 2 DCI and SWD Programming


This example uses a Wireless Starter Kit (WSTK) with the [BRD4182A Radio Board](https://www.silabs.com/documents/public/user-guides/ug430-brd4182a-user-guide.pdf) (EFR32MG22C224F512IM40) to program the Series 2 device through DCI and SWD interface.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in SWD programming operations. The results are printed on the VCOM serial port console.


## DCI Programming


The following DCI operations are supported in this example:


* GET SE STATUS
* READ SE OTP CONFIGURATION
* READ SERIAL NUMBER
* READ PUBLIC SIGN KEY
* READ PUBLIC COMMAND KEY
* ENABLE SECURE DEBUG
* DISABLE SECURE DEBUG
* LOCK DEVICE
* ERASE DEVICE (UNLOCK)
* RECOVER SECURE BOOT FAILURE DEVICE
* UPGRADE SE FIRMWARE THROUGH DCI
* INITIALIZE AES-128 KEY (EFR32xG21)
* INITIALIZE PUBLIC SIGN KEY
* INITIALIZE PUBLIC COMMAND KEY
* INITIALIZE HSE OTP (EFR32xG21A)
* INITIALIZE HSE OTP (EFR32xG21B)
* INITIALIZE VSE OTP (EFR32xG22)
* DISABLE DEVICE ERASE


## SWD Programming


The following SWD operations are supported in this example:


* ERASE MAIN FLASH
* PROGRAM MAIN FLASH
* ERASE USER DATA
* PROGRAM USER DATA
* UPGRADE SE FIRMWARE THROUGH APPLICATION FIRMWARE


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Open any terminal program and connect to the kit’s VCOM port.
3. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
4. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
5. Run the example and follow the instructions shown on the console.


## Additional Information


1. The default clock source is 80 MHz HFRCODPLL.
2. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[AN0062: Programming Internal Flash Over the Serial Wire Debug Interface](https://www.silabs.com/documents/public/application-notes/an0062.pdf)


[AN1190: Series 2 Secure Debug](https://www.silabs.com/documents/public/application-notes/an1190-efr32-secure-debug.pdf)


[AN1218: Series 2 Secure Boot with RTSL](https://www.silabs.com/documents/public/application-notes/an1218-secure-boot-with-rtsl.pdf)


[AN1222: Production Programming of Series 2 Devices](https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf)


[AN1247: Anti-Tamper Protection Configuration and Use](https://www.silabs.com/documents/public/application-notes/an1247-efr32-secure-vault-tamper.pdf)


[AN1303: Programming Series 2 Devices using the Debug Challenge Interface (DCI) and Serial Wire Debug (SWD)](https://www.silabs.com/documents/public/application-notes/an1303-efr32-dci-swd-programming.pdf)


