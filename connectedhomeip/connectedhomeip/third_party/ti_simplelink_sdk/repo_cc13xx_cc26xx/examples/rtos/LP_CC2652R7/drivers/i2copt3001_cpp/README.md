## Example Summary

The C++ OPT3001 example application reads values from an OPT3001 ambient light
sensor. It provides a basic example of utilizing TI-Drivers in a C++
application.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_GPIO_LED_0` - Indicator LED
* `CONFIG_GPIO_OPT3001_POWER` - OPT3001 power pin
* `CONFIG_GPIO_OPT3001_INTERRUPT` - OPT3001 limit interrupt
* `CONFIG_I2C_OPT3001` - I2C bus used to communicate with the OPT3001 sensor.

## BoosterPacks, Board Resources & Jumper Settings

This example requires a
[__BP-BASSENSORSMKII BoosterPack__][bp-bassensorsmkii].

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

The connection will have the following settings:

```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```

* Run the example. `CONFIG_GPIO_LED_0` turns ON to indicate driver
initialization is complete.

* The examples first attempts to find every slave address on the bus
and print out the address to the console.

* The example will request lux samples from the OPT3001 and display them
via the UART. A total of 60 lux samples are read and printed before
the example exits. Terminal output should resemble:

```
    I2C Initialized!

    I2C device found at address 0x40!
    I2C device found at address 0x44!
    Finished looking for I2C devices.

    Information about the OPT3001 device..
    I2C Slave Address       0x44
    Device ID:              0x3001
    Manufacturer ID:        0x5449
    Configuration Register: 0x8c90
    Low Limit:              0 lux
    High Limit:             83865 lux

    Set low limit to 30 lux
    Set high limit to 4000 lux

    Reading samples from OPT3001:
    Sample #1)   553 lux
    Sample #2)   556 lux
           .
           .
           .
    Sample #59)   552 lux
    Sample #60)   549 lux

    I2C closed!
```

To test the interrupt feature, cover the OPT3001 to trigger the low limit
interrupt. Conversely, expose the sensor to additional light to trigger the
high limit interrupt. When the sensor detects a value outside of the limit
registers, you should see a message that resembles:

```
    ALARM: Low limit crossed!
    The sensor detected a value of 3 lux.
    The low limit is 20 lux
```

## Application Design Details

This application uses two POSIX threads:

`mainThread` - performs the following actions:

1. Opens and initializes an I2C, GPIO, Display and semaphore object.

2. Displays information stored on the OPT3001 data registers and sets the
   limits of the interrupt mechanism

3. Uses the I2C driver to get data from the OPT3001 sensor.

4. Extracts the light intensity value (in lux) and prints the value via the
   UART.

5. The task sleeps for 1 second.

6. After 60 light samples are recovered, the limit registers are reset and the
   device is set to shutdown mode. The I2C peripheral is closed and the
   example exits.

`alarmThread` - performs the following actions:

1. Waits on a semaphore to be posted by an interrupt triggered by the OPT3001
   interrupt pin

2. Displays an "ALARM!" message whenever the OPT3001 sensor reads a value
   outside the programmed limits

3. Uses the I2C driver to get data from the OPT3001 sensor.

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

[bp-bassensorsmkii]: http://www.ti.com/tool/bp-bassensorsmkii
