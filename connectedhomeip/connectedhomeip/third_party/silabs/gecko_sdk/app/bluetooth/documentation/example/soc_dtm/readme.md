# SoC - DTM

This example application provides the Direct Test Mode (DTM) through a 2-wire UART interface for the RF PHY testing of a Bluetooth Low Energy device.

> Note: this example expects a specific Gecko Bootloader to be present on your device. For details see the Troubleshooting section.

## Direct Test Mode (DTM) Overview

DTM is typically used with a separate Bluetooth Tester device.

DTM can be used either through a 2-wire UART interface, as described in this example, or over HCI, which is currently not supported. The 2-wire UART interface uses baud rates from 1200 up to 115200 bps (the default) always with 8 data bits, no parity, 1 stop bit and without flow control (no RTS nor CTS).

![Connecting the Bluetooth Tester to the WSTK](readme_img1.png)

DTM can be also used without a separate Bluetooth Tester device when testing more manually with a spectrum analyzer and signal generator setup by sending the command bytes with a special terminal. To use this setup more easily, consider using the [BGTool](https://www.silabs.com/documents/public/application-notes/an1267-bt-rf-phy-evaluation-using-dtm-sdk-v3x.pdf) desktop application instead, which uses the [NCP-mode](https://www.silabs.com/documents/public/application-notes/an1259-bt-ncp-mode-sdk-v3x.pdf) [BGAPI](https://docs.silabs.com/bluetooth/latest)-interface.

Although the DTM is defined for the Bluetooth Qualifications, it can be used to run most of the Regulatory Certification tests.

Typical use cases for the DTM:
- SoCs:
  - Regulatory Certifications testing
  - RF-PHY testing for the Bluetooth Qualification if the design does not match the reference design
  - Production testing
- Modules:
  - Regulatory Certifications testing (required for the modules in Europe)
  - Spot checks (strongly recommended by FCC)
  - RF-PHY testing for the Bluetooth Qualification if the design might affect the module operation
  - Production testing
- Radio Boards with WSTK:
  - Mostly for getting to know the DTM overall and testing the setup
  - Typically not much practical usage

For more information about using the DTM, see the following:

- [AN1267: Radio Frequency Physical Layer Evaluation in Bluetooth SDK v3.x](https://www.silabs.com/documents/public/application-notes/an1267-bt-rf-phy-evaluation-using-dtm-sdk-v3x.pdf)

The full DTM specification can be found here:

- [Bluetooth Core Specification Adopted Version 5.2, Vol 6: Low Energy Controller, Part F: Direct Test Mode](https://www.bluetooth.com/specifications/bluetooth-core-specification/)

## Getting Started

To get started with Silicon Labs Bluetooth and Simplicity Studio, see [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

The instructions to create and flash the DTM example application are in the [AN1267: Radio Frequency Physical Layer Evaluation in Bluetooth SDK v3.x](https://www.silabs.com/documents/public/application-notes/an1267-bt-rf-phy-evaluation-using-dtm-sdk-v3x.pdf). The same document contains information about customizing UART pins for your own hardware, and examples illustrating using the DTM.

Detailed specifications are in the Bluetooth Specifications.

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


**Note!** With the mainboard and the Radio Board, the DTM example uses EXP-header UART pins by default and not the USB UART as most of the other examples. Also, the default is no flow control.


## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

[AN1267: Radio Frequency Physical Layer Evaluation in Bluetooth SDK v3.x](https://www.silabs.com/documents/public/application-notes/an1267-bt-rf-phy-evaluation-using-dtm-sdk-v3x.pdf)

[Bluetooth Core Specification Adopted Version 5.2, Vol 6: Low Energy Controller, Part F: Direct Test Mode](https://www.bluetooth.com/specifications/bluetooth-core-specification/)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).