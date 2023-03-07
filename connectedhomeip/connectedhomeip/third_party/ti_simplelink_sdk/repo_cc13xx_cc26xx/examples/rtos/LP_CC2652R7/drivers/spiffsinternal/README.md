---
# spiffsinternal

---

## Example Summary

This example shows how to use the SPI Flash File System (SPIFFS) with the
Non-Volatile Storage (NVS) driver as the interface. This example uses on-chip
internal flash memory.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_NVSINTERNAL` - NVS instance which will be used by the file system.

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

## Example Usage

* Example output is generated through use of Display driver APIs. Refer to the
Display driver documentation found in the  SimpleLink MCU SDK User's Guide.

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
 Homepage"), etc.) to the appropriate COM port.
* The COM port can be determined via Device Manager in Windows or via
 `ls /dev/tty*` in Linux.

The connection will have the following settings:
```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```

* Run the example.
    * The example mounts a SPIFFS file system within a NVS region.  If a file
system is not found the example will format the region & mount it.

    * The file system checks if a file named "spiffsFile" exists.  If the file
exists the contents are output to the UART & the file is removed.  Otherwise,
the application will create "spiffsFile" and write "Hello from SPIFFS!!!" to it.

    * Disconnect the device from the debug session.

    * When prompted, reset the device. This will cause the application to start
over.


    __NOTE__: This application will erase flash memory if no file system is found.


The following is an example output.
```
    Mounting file system...
    File system not found; creating new SPIFFS fs...
    Creating spiffsFile...
    Writing to spiffsFile...
    Reset the device.
    ==================================================
```
After device reset:
```
    Mounting file system...
    Reading spiffsFile...

    spiffsFile: Hello from SPIFFS!!!

    Erasing spiffsFile...
    Reset the device.
    ==================================================
```


## Application Design Details

* SPIFFS source is included and pre-built with support TI-RTOS or FreeRTOS
synchronization.  A SPIFFSNVS driver is included in the library which serves as
the interface between the SPIFFS file system & the NVS driver.  The SPIFFS file
system will operate within the memory region allocated for a NVS driver
instance.  NVS driver specifications are found in the application's board file.
Refer to NVS driver documentation for configuration details.

* To use a SPIFFS file system some configuration parameters & RAM must be
provided at runtime (all sizes in bytes):
    * Amount of memory allocated for SPIFFS - This is the amount of memory that
has been allocated for NVS driver instance which SPIFFS will work within.  This
is configurable in the application's board file.
    * Physical block size (also known as sector size) - Amount of flash cleared
on a single erase operation.  This varies according to the type of memory & is
defined in the NVS configuration in the application's board file.
    * Logical block size - The file system divides the entire memory region into
logical blocks.  This value must be an integer multiple of the physical block
size:
> logical_block_size = n * physical_block_size
    * Logical page size - The file system divides logical blocs into logical
pages.  Files are divided into pages when stored in memory.  The logical block
size must also be an integer multiple of the logical page size:
> logical_block_size = i * logical_page_size
    * A RAM 'work' buffer - This is memory used internally by SPIFFS.  This must
be twice the logical page size (2 * logical_page_size) in length.
    * A RAM file descriptor cache - Temporary storage for the file descriptors
of frequently used files.

* The application utilizes a single thread (`mainThread`) to demonstrate how to
use SPIFFS APIs to read and write files.

* SPIFFSNVS_config() is called to:
    * Open NVS driver instance in which SPIFFS file system will exits.
    * Validate configuration parameters.
    * Initialize SPIFFS configuration & file system structures.

* The application then attempts to mount the SPIFFS file system.  If none is
found, the memory is formatted & the file system is mounted.

* The file system is checked for a file named "spiffsFile" exists.  If the file
exists, its contents are output to the UART.  The file is then removed from the
file system.

* If "spiffsFile" is not found it will be created & "Hello from SPIFFS!!!" is
written to it.

* At the end of execution, the application prompts the user to reset the
device. Upon a reset, the application will start over.


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

## References
SPIFFS design & usage documentation can be found here:
    https://github.com/pellepl/spiffs
