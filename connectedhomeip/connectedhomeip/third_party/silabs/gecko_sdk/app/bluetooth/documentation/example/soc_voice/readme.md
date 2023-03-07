# SoC - Voice

This is a Voice over Bluetooth Low Energy example. It is supported by a Thunderboard Sense 2 board and demonstrates how to send voice data over GATT, which is acquired from the on-board microphone.

> Note: this example expects a specific Gecko Bootloader to be present on your device. For details see the Troubleshooting section.

## Getting started

To get started with Silicon Labs Bluetooth and Simplicity Studio, see [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

To rune the example you need a Thunderboard Sense 2 and another board, capable of running an NCP application. [AN1259: Using the v3.x Silicon Labs Bluetooth Stack in Network CoProcessor Mode](https://www.silabs.com/documents/public/application-notes/an1259-bt-ncp-mode-sdk-v3x.pdf) provides a detailed description of how NCP works and how to configure it.

One part of the example, the SoC Voice, runs on the Thunderboard Sense. It provides the Voice-Over-Bluetooth Low Energy GATT service. It uses the microphone of the board to record and transmit voice.

The other part of the project runs on a PC. It connects to a board which is running the **NCP empty** example project. The program on the PC uses the NCP target to scan for the Thunderboard, connect to it, and save the received audio data to the file system of the PC.

## Project Setup

The whole project setup:
![](readme_img1.png)

### Thunderboard part
Build and flash the provided code example. The device advertises itself after startup with the name "VoBLE Ex". If you have multiple devices, you need to determine the Bluetooth address of the device. This can be done either with the EFR Connect app, or through Simplicity Commander.

The EFR32 on the TB Sense samples the analog microphone using the ADC with the sampling rate and resolution configured by the GATT client. The sampled data is then run through a digital filter (if the filter usage is enabled) and coded using ADPCM codec before being sent via the Bluetooth link to the GATT client using notifications.

![](readme_img2.png)


### NCP Host part
The PC part of the example can be found under *<SDK-installation-location>\app\bluetooth\example_host\voice*. 
To build the project into an executable, you also need a make-tool, which is part of GNU developer tools. On Windows MinGW is recommended. More details can be found in [AN1259: Using the v3.x Silicon Labs Bluetooth Stack in Network CoProcessor Mode](https://www.silabs.com/documents/public/application-notes/an1259-bt-ncp-mode-sdk-v3x.pdf).

To compile the NCP application:

1. Open your terminal and navigate to the above mentioned folder.
2. Run mingw32-make
3. The build output is created in a new "exe" folder

Running the exe without any parameters will give you this help response:
![](readme_img3.png)
The program can be configured using the flags shown in the above image. When giving the parameters to the program, all units must be omitted.

Some notes about the parameters:
1. **COM Port**: This is the serial port to be used. It should point to the port used by the NCP target. You can check the correct port number with BGTool or Device Manager, the mainboard lists as a “JLink CDC UART Port”. (If you are having problems identifying the port, you should unplug all WSTKs except the one you wish to use as the NCP.)
2. **Baud Rate**: The baud rate used for communication, default: 115200
3. **Output file name**: Filename for the audio data output (without the file extension).
4. **Bluetooth address**: Bluetooth address of the TB Sense board that you want to connect to. If left out, the application tries to search devices that match the default UUID of the TB Sense application.
5. **Enable/Disable filtering**: Toggles whether filtering is used. See filter types in the previous section for options. The filter is disabled by default.
6. **Enable/Disable encoding**: Toggles whether the audio data should be encoded, enabled by default. If encoding is disabled, the output filetype will be either “.s8” or “.s16” depending on the sample rate. Encoded filetype is always “.ima” (Dialogic ADPCM -format).
7. **Verbose**: If this switch is added, the application prints out status messages.

#### Saving the audio to a file
To record audio into an audio file using this ncp-host-application, you must provide at least the following parameters (examples in parenthesis):

* Serial port for the ncp-target ( -p COM6 )
* output filename ( -o my_audio_file )

The following example uses the verbose mode with the default settings for both sampling rate and resolution (16kHz and 12 bits, respectively) and saves the audio data to a file called *audio_file*.

![](readme_img4.png)

If you have activated the verbose mode, as in the above image, you will get status messages from the GATT client application. The application will stop printing status messages after is has written all the configurations, which in the above image happens after transfer status has successfully been enabled. When the initialization is done, the GATT client (ncp-host) is ready to receive data from the TB Sense.

To start recording and streaming audio data over the BLE link, press and hold TB Sense BTN0 (left button). Once the button is pressed, you should see activity on the terminal window that summarizes the transmission progress. All the audio data will be saved to a file defined by the output filename -parameter ( audio_file.ima in this example ). If a file with a same name already exists, the audio data is appended to the end of that file.

To end the transmission in progress, send a interrupt signal to the application. On a Windows keyboard, the interrupt signal can be sent with CTRL+C key combination.

## SoC project structure
The example project contains the GATT database with the necessary Voice-over-Bluetooth Low Energy Service. GATT definitions can be extended using the GATT Configurator, which can be found under Advanced Configurators in the Software Components tab of the Project Configurator. To open the Project Configurator open the .slcp file of the project.

![](readme_img5.png)

To learn how to use the GATT Configurator, see [UG438: GATT Configurator User’s Guide for Bluetooth SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug438-gatt-configurator-users-guide-sdk-v3x.pdf).

The Bluetooth event handling is implemented in the *app.c* file, in the function sl_bt_on_event. This handles the advertising, accepts the configuration parameters, and sends out the data.
The button handling logic is also implemented in this file.
The handling of the microphone, the encoding, buffering and filtering can be found in their respective files:
* adpcm.c
* filter.c
* voice.c
* circular_buffer.c

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