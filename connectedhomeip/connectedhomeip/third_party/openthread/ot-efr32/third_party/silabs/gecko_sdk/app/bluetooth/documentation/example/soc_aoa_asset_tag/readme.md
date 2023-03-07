# SoC - AoA Asset Tag

This example application demonstrates a CTE (Constant Tone Extension) transmitter that can be used as an asset tag in a direction finding setup estimating Angle of Arrival (AoA). Test this example with **NCP - AoA Locator**, which (when used together with the **aoa_locator host** applications) can estimate the direction of the asset tag. 

> Note: this example expects a specific Gecko Bootloader to be present on your device. For details see the Troubleshooting section.

## Getting Started

To learn the basics of Bluetooth direction finding technology , see [UG103.18: Bluetooth Direction Finding Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-18-bluetooth-direction-finding-fundamentals.pdf).

To get started with Silicon Labs Bluetooth and Simplicity Studio, see [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

To get started with Silicon Labs' Direction Finding Solution, see [QSG175: Silicon Labs Direction Finding Solution Quick-Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg175-direction-finding-solution-quick-start-guide.pdf)

In an AoA direction finding use case, the tag acts as a transmitter and the locator acts as a receiver. The locator determines the direction of the tag by sampling the signal of the tag on different antennas of an antenna array and measuring phase differences.

![Locators determine the direction of the tag by sampling different antennas](readme_img1.png) 

To estimate the direction of the incoming signal, the AoA - Locator needs to receive a special Bluetooth packet, which has a Constant Tone Extension (CTE). CTEs can be transmitted:

* Via a Bluetooth connections (connection oriented mode).
* In periodic advertisements (connectionless mode).
* Attached to extended advertisements. This is not part of the Bluetooth standard, but is a Silicon Labs proprietary solution (Silabs mode)

This example:

* Transmits connectable advertisements, and makes it possible to enable CTE on the established connections
* Transmits Silicon Labs proprietary extended advertisements with CTE to let multiple locators receive the same signal

This makes it possible for locators to locate the asset tag.

## Configuring the CTE Transmit Method

By default the asset tag is connectable, and CTEs can be enabled and requested via a connection. Additionally, Silicon Labs proprietary non-connectable extended advertisements with CTE are also sent out periodically, so that (multiple) locators can locate the tag without connecting to it. These features are enabled by the installed **Constant Tone Extension GATT Service (Connection)** and **Constant Tone Extension GATT Service (Silabs proprietary)** software components

![CTE related software components](readme_img2.png)

To transmit standard periodic advertisements instead of Silicon Labs proprietary extended advertisements, in the Project Configurator uninstall the **Constant Tone Extension GATT Service (Silabs proprietary)** component and install the **Constant Tone Extension GATT Service (Connectionless)** software component.

## Testing the SoC - AoA Asset Tag Application

AoA Asset Tag can be tested together with an AoA Locator. After programming the board with the **SoC - AoA Asset Tag** example, attach an antenna array board, create an **NCP - AoA Locator example**, and then follow the instructions in [AN1296: Application Development with Silicon Labs’ RTL Library](https://www.silabs.com/documents/public/application-notes/an1296-application-development-with-rtl-library.pdf).

## Troubleshooting

### Bootloader Issues

Note that Example Projects do not include a bootloader. However, Bluetooth-based Example Projects expect a bootloader to be present on the device in order to support device firmware upgrade (DFU). To get your application to work, you should either 
- flash the proper bootloader or
- remove the DFU functionality from the project.

**If you do not wish to add a bootloader**, then remove the DFU functionality by uninstalling the *Bootloader Application Interface* software component -- and all of its dependants. This will automatically put your application code to the start address of the flash, which means that a bootloader is no longer needed, but also that you will not be able to upgrade your firmware.

**If you want to add a bootloader**, then either 
- Create a bootloader project, build it and flash it to your device. Note that different projects expect different bootloaders:
  - for NCP and RCP projects create a *BGAPI UART DFU* type bootloader
  - for SoC projects on Series 1 devices create a *Bluetooth in-place OTA DFU* type bootloader or any *Internal Storage* type bootloader
  - for SoC projects on Series 2 devices create a *Bluetooth Apploader OTA DFU* type bootloader

- or run a precompiled Demo on your device from the Launcher view before flashing your application. Precompiled demos flash both bootloader and application images to the device. Flashing your own application image after the demo will overwrite the demo application but leave the bootloader in place. 
  - For NCP and RCP projects, flash the *Bluetooth - NCP* demo.
  - For SoC projects, flash the *Bluetooth - SoC Thermometer* demo.

**Important Notes:** 
- when you flash your application image to the device, use the *.hex* or *.s37* output file. Flashing *.bin* files may overwrite (erase) the bootloader.

- On Series 1 devices (EFR32xG1x), both first stage and second stage bootloaders have to be flashed. This can be done at once by flashing the *-combined.s37* file found in the bootloader project after building the project.

- On Series 2 devices SoC example projects require a *Bluetooth Apploader OTA DFU* type bootloader by default. This bootloader needs a lot of flash space and does not fit into the regular bootloader area, hence the application start address must be shifted. This shift is automatically done by the *Apploader Support for Applications* software component, which is installed by default. If you want to use any other bootloader type, you should remove this software component in order to shift the application start address back to the end of the regular bootloader area. Note, that in this case you cannot do OTA DFU with Apploader, but you can still implement application-level OTA DFU by installing the *Application OTA DFU* software component instead of *In-place OTA DFU*.

For more information on bootloaders, see [UG103.6: Bootloader Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-06-fundamentals-bootloading.pdf) and [UG489: Silicon Labs Gecko Bootloader User's Guide for GSDK 4.0 and Higher](https://cn.silabs.com/documents/public/user-guides/ug489-gecko-bootloader-user-guide-gsdk-4.pdf).


### Programming the Radio Board

Before programming the radio board mounted on the mainboard, make sure the power supply switch is in the AEM position (right side) as shown below.

![Radio board power supply switch](readme_img0.png)


## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[UG103.18: Bluetooth Direction Finding Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-18-bluetooth-direction-finding-fundamentals.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[QSG175: Silicon Labs Direction Finding Solution Quick-Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg175-direction-finding-solution-quick-start-guide.pdf)

[AN1296: Application Development with Silicon Labs’ RTL Library](https://www.silabs.com/documents/public/application-notes/an1296-application-development-with-rtl-library.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).