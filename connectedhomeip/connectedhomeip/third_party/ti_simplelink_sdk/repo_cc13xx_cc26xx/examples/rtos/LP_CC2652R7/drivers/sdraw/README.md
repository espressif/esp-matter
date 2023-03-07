## Example Summary

This example performs raw read and write operations with a Secure Digital (SD)
card. This example may corrupt any file system present on a SD card.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_GPIO_LED_0` - Indicates the SD card was successfully initialized.
* `CONFIG_SD_0`  - SD driver instance

## BoosterPacks, Board Resources & Jumper Settings

This example requires a
[__BOOSTXL-SHARP128 LCD & SD Card BoosterPack__][boostxl-sharp128] and a micro
SD card.

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

## Example Usage

* Example output is generated through use of Display driver APIs. Refer to the
Display driver documentation found in the SimpleLink MCU SDK User's Guide.

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
Homepage"), etc.) to the appropriate COM port.
    * The COM port can be determined via Device Manager in Windows or via
`ls /dev/tty*` in Linux.

The connection should have the following settings
```
    Baud-rate:  115200
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None
```

* Edit the sdraw.c file and set WRITEENABLE to 1, and build the example.
  This will allow data to be written to the SD card.  __WARNING:__ Any file
  system present on the SD card will be overwritten and corrupted by this
  action!

* Run the example. `CONFIG_GPIO_LED_0` turns on to indicate successful
driver initialization.

* The example reads and displays information about the SD card. An example is
shown below:
```
======== SD Card Information ========
Sectors:                15196160
Sector Size:            512 bytes
Card Capacity:          7168 MB
=====================================
```

* The example proceeds to write data to the SD card.

* The example then reads back any data written.

* The data read is compared with the data written. If they do not match, an
error message will be displayed.

* The data previously written is overwritten with 0.

* If WRITEENABLE is 0, the example will only perform read operations.

## Application Design Details

This application uses one thread:

The `mainThread` performs the following actions:

1. Create and initialize the SD driver.

2. Read the sector size and number of sectors from the SD card. This
information is used to calculate the total card capacity.

3. Write raw data to the SD card.

4. Read data from the SD card and compare with data written.

5. The data previously written is erased by overwriting the data with 0.

TI-RTOS:

* When building in Code Composer Studio, the kernel configuration project will
be imported along with the example. The kernel configuration project is
referenced by the example, so it will be built first. The "release" kernel
configuration is the default project used. It has many debug features disabled.
These feature include assert checking, logging and runtime stack checks. For a
detailed difference between the "release" and "debug" kernel configurations and
how to switch between them, please refer to the SimpleLink MCU SDK User's
Guide. The "release" and "debug" kernel configuration projects can be found
under &lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/(ccs|gcc).

FreeRTOS:
* Please view the `FreeRTOSConfig.h` header file for example configuration
information.

[boostxl-sharp128]: http://www.ti.com/tool/boostxl-sharp128
