# SoC - Appication OTA DFU

 This example project demonstrates the Application Over-the-Air Device Firmware Upgrade (OTA DFU) service, which unlocks firmware update during application runtime without resetting the device into 'OTA DFU mode' and without installing any application loader utility to the device. The downloaded firmware is stored in dedicated flash storage (slot 0). Once the download has finished, the bootloader is configured to update the firmware on the device. During the reboot session the new firmware is copied to the application space in the flash and the new application is loaded.

 > Note: this example expects a specific Gecko Bootloader to be present on your device. For details see the Troubleshooting section.

## Getting Started

To learn the Bluetooth technology basics, see [UG103.14: Bluetooth® LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf).

To get started with Bluetooth and Simplicity Studio, see [QSG169: Bluetooth® Quick-Start Guide for SDK v3.x and Higher](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

OTA DFU means that the firmware upgrade image is sent to the device via a Bluetooth connection. The new firmware can be uploaded either by Apploader (a standalone Bluetooth application that runs in the bootloader or on its own) or by the user application. Both solutions have their advantages and disadvantages.
* Apploader runs independently of the user application and it can overwrite the old application right away without an internal step. This is useful if there is no extra storage space. It is also a safe solution, because even if there is a serious bug in the user application, the device can still be started in DFU mode, and a new firmware can be installed without issues.
* Application OTA DFU means that the image is uploaded during the user application's runtime. This needs extra storage space (since the application cannot overwrite itself). On the other hand, it allows for checking the firmware upgrade image validity before the image is installed and it offers more configurability and interactivity during the DFU sequence.

![OTA DFU with Apploader and Application OTA DFU](readme_img1.png)

This example project demonstrates Application OTA DFU, i.e. how to upload and install a new firmware image from the user application using the proper software components.

To learn more about Device Firmware Upgrade, see [AN1086: Using the Gecko Bootloader with the Silicon Labs Bluetooth® Applications](https://www.silabs.com/documents/public/application-notes/an1086-gecko-bootloader-bluetooth.pdf).


## Features Implemented in the Example

This example demonstrates how to use the Application OTA DFU software component.

* A simple application is implemented in the event handler function that starts advertising on boot (and on connection_closed event). This makes it possible for remote devices to find the device and connect to it.

* A simple serial port-based logger component is added to the project that comes handy for debugging and testing the application OTA features.

* A simple GATT database is defined by adding Generic Access and Device Information services. This makes it possible for remote devices to read out some basic information e.g., the device name.

* The application OTA software component is added, which extends both the event handlers (see sl_app_ota.c) and the GATT database (OTA DFU service). This enables OTA application updates during runtime without any additional application code.

By deafult, the Application OTA DFU software component takes care of the full OTA DFU process. It manages the upload process and it interfaces with the bootloader. The main application (app.c) only extends this component with some nice-to-have features - such as progress reporting and error handling - and takes care of the device reboot. The developer is free to modify this code.


## Testing the Example

This is a minimal example with the application OTA service that allows it to do a firmware update during runtime. There is no need to change to 'OTA mode' and no Apploader utility is required. Once the new firmware is downloaded to the device, a restart will happen and the new firmware will overwrite the original application content. The application OTA example has to be uploaded to a device that already has an **internal storage bootloader** with at least **one configured flash storage** (slot 0) with enough capacity to store the new firmware. When started, the application first checks this flash storage and if necessary it will erase its content. When the OTA update finished and reboot is done, this storage will be cleared again if necessary. To test this update feature do the following:

1. Build and flash an internal storage bootloader to your device. (See Troubleshooting section.)
2. Build and flash the SoC-Application-OTA-DFU sample app to your device.
3. Create the full.gbl firmware upgrade file with the create_bl_files.bat/.sh script, which is part of your project. If it does not work, see the instructions in [AN1086: Using the Gecko Bootloader with the Silicon Labs Bluetooth® Applications](https://www.silabs.com/documents/public/application-notes/an1086-gecko-bootloader-bluetooth.pdf).
4. Save full.gbl outside of your project for later use.
5. Modify the code so that you can differentiate the old and the new firmware (e.g., change the first app_log message in app_init() function or change the device name in the GATT database). 
6. Build the project again, but do not flash.
7. Create a new full.gbl file with create_bl_files.bat/.sh.
8. Open a terminal program and connect to your radio board via the JLink adapter to see the debug messages.
9. Download the **EFR Connect** smartphone application available on [iOS](https://apps.apple.com/us/app/efr-connect/id1030932759) and [Android](https://play.google.com/store/apps/details?id=com.siliconlabs.bledemo).
10. Open the app and choose the Bluetooth Browser.
   ![EFR Connect start screen](readme_img2.png)
11. Now you should find your device advertising as "Application OTA". Tap **Connect**.
   ![Bluetooth Browser](readme_img3.png)
12. The connection is opened, and the GATT database is automatically discovered. Find the device name characteristic under Generic Access service and try to read out the device name.
   ![GATT database of the device](readme_img4.png)
13. Select the 3 dots menu and choose OTA DFU.
   ![OTA DFU menu](readme_img5.png)
14. Use the Partial OTA tab (default)
   ![Select gbl file](readme_img6.png)
15. Select the full.gbl file and tap **OTA** to start the OTA transfer.
16. Once its done tap **END** to finalize the process.
17. Press push button 0 on your kit to restart your device. The bootloader will automatically install the new firmware image.
18. Check if the new firmware is started either by checking the (modified) device name in EFR Connect or by checking the (modified) logs in the terminal.


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

[UG103.14: Bluetooth® LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth® Quick-Start Guide for SDK v3.x and Higher](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[AN1086: Using the Gecko Bootloader with the Silicon Labs Bluetooth® Applications](https://www.silabs.com/documents/public/application-notes/an1086-gecko-bootloader-bluetooth.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

[Uploading Firmware Images Using OTA DFU](https://docs.silabs.com/bluetooth/latest/general/firmware-upgrade/uploading-firmware-images-using-ota-dfu)



## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).