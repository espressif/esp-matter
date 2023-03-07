# Interoperability Example

Interoperability (IOP) is one of the key value propositions of Bluetooth Low Energy and something that consumers have come to expect from Bluetooth-enabled end products.

This readme describes the Silicon Labs IOP test framework, composed of hardware kits, embedded software, and a mobile app. It also explains the requirements for building the IOP test setup, running the test, and collecting data for further analysis.

> Note: this example expects a specific Gecko Bootloader to be present on your device. For details see the Troubleshooting section.

**Because some optional steps need to be taken before the IOP test starts, read this document before running the IOP test.**

## Introduction

IOP is a cornerstone of Bluetooth and one of the key reasons why this wireless technology has become ubiquitous. It enables end users to mix and match devices between different vendors without fearing connectivity issues, for example, whether a heart rate monitor from company A will connect to a smart watch from company B.

It is therefore essential that Bluetooth solution suppliers can offer their customers a means to test IOP between the customer's Bluetooth solution and third-party devices

One of the most common use cases for Bluetooth-enabled devices is interaction with smartphones, where a mobile app is used for command and control of the Bluetooth device. This use case places IOP in the spotlight because of the large number of permutations between smartphone hardware (namely Bluetooth chipset), low-level firmware (typically the Bluetooth LE (BLE) link layer), mobile OS (typically the BLE host stack), and mobile OS version.

Silicon Labs provides a framework to test IOP between the EFR32 family of SoCs and a large number of smartphones currently on the market. This framework is used to run IOP testing periodically against a large list of devices. [AN1309](https://www.silabs.com/documents/public/application-notes/an1309-ble-interop-testing-report.pdf) contains both IOP test results and the IOP test plan.

Subsequent sections describe:

- Requirements for the IOP test framework
- Bringing up the test environment
- Running the IOP test
- Collecting data for further analysis

## Requirements for the IOP Test Framework

### Hardware Requirements

The IOP embedded software is available for virtually any Silicon Labs kit that supports Bluetooth technology.

### Software Requirements

The IOP example application is available beginning with Bluetooth SDK 3.3.0. Install Simplicity Studio 5 and the Bluetooth SDK that is part of the GSDK. For more information about installing Simplicity Studio 5, see the [Simplicity Studio 5 documentation](https://docs.silabs.com/simplicity-studio-5-users-guide/5.2.1/ss-5-users-guide-getting-started/install-ss-5-and-software).

### Mobile App Requirements

To enable IOP testing framework on a smartphone, install the EFR Connect mobile app, version 2.4 or newer. The app is available for both [Android](https://play.google.com/store/apps/details?id=com.siliconlabs.bledemo&hl=en&gl=US) and [iOS](https://apps.apple.com/us/app/efr-connect/id1030932759) and the source is available on [GitHub](https://github.com/SiliconLabs?q=efrconnect&type=&language=&sort=).

Ensure that there is **no existing bond** with the embedded device before initiating the IOP test sequence, which you can check from the phone Bluetooth settings. If the device is already bonded, the bond must be removed before proceeding with IOP testing.

### Minimum Mobile Operating System Versions

The minimum OS versions supported by the EFR Connect mobile app are Android™ 9 and iOS®12.

## Bringing up the Test Environment

The IOP test consists of a sequence of BLE operations executed between a mobile device and an EFR32 SoC running the interoperability test embedded software (the embedded device).

To flash the embedded software into one of the supported boards, create the example **Bluetooth - SoC Interoperability Test**, build it, and flash it to the target. 

Then run the script *iop_create_bl_files.sh* (for MacOS/Linux) or *iop_create_bl_files.ps1* (for Windows powershell). The script generates two files into the *output_gbl* folder that is inside the project folder: *ota-dfu_ack.gbl* and *ota-dfu_non_ack.gbl*.

These files must be provided to the IOP Test on EFR Connect mobile app when prompted to do so. Copy them to the mobile phone's local storage or a cloud drive that is accessible from the mobile phone. The file *ota-dfu_ack.gbl* is used for the first OTA test and *ota-dfu_non_ack.gbl* for the second OTA test.

Note that you must have a bootloader flashed to the board as well, otherwise the firmware will not run. See below under **Troubleshooting** for instructions on various ways to flash a bootloader. 

Once the example and bootloader are flashed to the target you should see the information on the mainboard display shown below. If you are using a mainboard without display (e.g., Explorer Kit) then you will see information being sent out through the UART, which can be captured by a terminal on the PC (more information [here](#collecting-additional-data-from-the-embedded-device)).

![](readme_img1.png)

On your smartphone, launch the EFR Connect mobile app, which automatically opens in Develop view. Tap the Interoperability Test tile to bring up a list of all the nearby boards running the IOP Test firmware. Tap the board that you want to test against. The app automatically goes to the IOP view, where you can tap “Run Tests” to get started.

![](readme_img2.png)

## Running the IOP Test

After the IOP test sequence starts running, the mobile app scrolls through the test cases and indicates Pass/Fail when the test is completed, as shown below.

![](readme_img3.png)

Most tests do not require user intervention, except for the OTA and security tests. 

During OTA tests you are prompted to upload the gbl file. The file can be retrieved from local or cloud storage, using OS standard methods. For the first OTA test the *ota-dfu_ack.gbl* must be used, and for the second OTA test *ota-dfu_non_ack.gbl*. 

![](readme_img11.png)

During the security tests, you are prompted several times to bond with the device on the mobile app side. Some of those prompts require simple confirmation (Just Works pairing) while other prompts require entering a PIN (authenticated pairing), which can be read from the mainboard display or from the UART logs, if you are using a mainboard without display.

![](readme_img4.png)

![](readme_img5.png)

## Logging and Sharing data

After the test is finalized on the mobile app, you can rerun the test or share the results.

![](readme_img6.png)

To rerun the tests, first reset the embedded device by pressing the reset button on the lower right side of the mainboard. Additionally, remove the bond from the phone’s Bluetooth settings.

The *Share* option allows sharing the test log through OS-standard mediums, such as cloud storage (e.g., Dropbox, Google Drive, iCloud, and so on) or email, or saving it locally. The log is in xml format and contains information about the phone model, OS version, Bluetooth connection parameters, and the result of each test. Below is an example of a test log from running IOP test on a Pixel 2 with Android 11.

![](readme_img7.png)

### Collecting Additional Data from the Embedded Device

The IOP embedded software also sends logging data over UART, which can be captured by a terminal emulator on the PC. Furthermore, the Packet Trace Interface (PTI) is enabled, which means that the radio traffic can be captured using the Network Analyzer.

For a more comprehensive data set around an individual IOP test sequence, capture both UART logs and radio traces using Simplicity Studio. Radio trace capture must be initiated before starting the IOP test.

To start the radio capture, right-click the debug adapter and select "Connect". Then, right-click the debug adapter once again and select "Start Capture".

![](readme_img8.png)

This automatically opens the Network Analyzer perspective, where the traffic is logged and every packet can be decoded for further analysis if required. At the end of the IOP test, the trace can be saved through File -> Save as, as shown below.

![](readme_img9.png)

While UART logs have multiple COMPort emulators such as tera term, you can also capture in Simplicity Studio’s terminal. Right-click the debug adapter and select "Launch Console". In the console view, click the "Serial 1" tab. Enter any character at the prompt at the bottom of the screen. The connection symbol should change, indicating that the connection is successful. Then logging should start, depending at which phase of the IOP test this is initialized. Otherwise, reset the board to ensure that the log is being received.

![](readme_img10.png)

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

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).