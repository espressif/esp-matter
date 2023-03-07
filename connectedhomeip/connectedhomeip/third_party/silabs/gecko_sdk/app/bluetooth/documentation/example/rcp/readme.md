# RCP

The RCP (Radio Co-Processor)) example application runs the Bluetooth Controller (radio + Link Layer) and implements the controller part of the HCI, as defined in the *Bluetooth Core Specification, Vol 4: Host Controller Interface*. The HCI is a standardized way for Bluetooth host and controller to communicate with each other. Because the interface is standard, the host and controller can be from different vendors. Currently, Silicon Labs Bluetooth Controller supports UART (Universal Asynchronous Receiver-Transmitter) as the HCI transport layer.

> Note: this example expects a specific Gecko Bootloader to be present on your device. For details see the Troubleshooting section.

## Getting Started

To get started with Silicon Labs Bluetooth and Simplicity Studio, see [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

## HCI Protocol

The HCI layer provides set of commands and events and ACL data packets. The Host sends commands to the controller. The commands are used to start advertising or scanning, establish a connection to another Bluetooth device, read status information from the controller, and so on.

Events are sent from the Controller side to the Host. Events are used as a response to commands or to indicate various events in the controller such as scanning reports, establishing or closing a connection, and various failures.

Silicon Labs Bluetooth LE Controller runs on EFR32 Radio Co-Processors. External Bluetooth Host stacks communicates with the controller over the HCI, also called RCP mode, as shown in the following figure.

![](readme_img1.png)

For more details, see [AN1328: Enabling a Radio Co-Processor using the Bluetooth HCI Function](https://www.silabs.com/documents/public/application-notes/an1328-enabling-rcp-using-bt-hci.pdf)

## Supported Modes

ACL (Asynchronous Connection Less) data packets deliver user application data between the host and the controller in both directions.

The Silicon Labs Bluetooth LE Controller does not support SCO (Synchronous Connection Oriented) and ISOC (Isochronous Channels) modes. Also, the related HCI messages are not supported.

The Bluetooth specification defines Low Energy (LE), BR/EDR (classic), and AMP (alternate MAC and PHY) controllers. Note that Silicon Labs only supports the LE controller.

## Communication Interface

The application uses UART as the HCI Transport layer. By default, UART is configured as follows:
  - Hardware flow control: disabled
  - Speed: 115200 kbps
  - Data bits: 8
  - Parity bit: N
  - Stop bits: 1

The default configuration can be changed with the **vcom** component. For more details about the customization of the project, see [AN1328: Enabling a Radio Co-Processor using the Bluetooth HCI Function](https://www.silabs.com/documents/public/application-notes/an1328-enabling-rcp-using-bt-hci.pdf).

## Usage

Because HCI is a standardized interface, it can be used with any Host that implements the HCI via UART. For example, it can be used with the BlueZ stack included in Linux systems. Connect your device flashed with the RCP firmware to your system. Then, attach it as a HCI device with the command *btattach*:

![](readme_img2.png)

Now, it can be controlled with any tool that uses HCI commands, e.g., *bluetoothctl* :

![](readme_img3.png)

![](readme_img4.png)

##  Troubleshooting

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

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)



## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found via [Silicon Labs Community](https://www.silabs.com/community).