## Example Summary

This application demonstrates how to use the Watchdog driver.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_GPIO_LED_0`      - Indicator LED
* `CONFIG_GPIO_BUTTON_0`   - Used to control the application
* `CONFIG_WATCHDOG_0`      - Watchdog driver instance.

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

## Example Usage

1. After loading the example application to the board, disconnect the device
from the debug session. Then physically disconnect the device from power for 5
seconds. Reconnect the device and the application should run automatically.
Some devices need to skip this power cycle step as this may cause the example
application to fail. These devices should load the example and then disconnect
the device from the debug session. The example application will then run
automatically.

2. The application turns on `CONFIG_GPIO_LED_0` to indicate watchdog driver
initialization is complete.

3. `CONFIG_GPIO_LED_0` is toggled periodically. With each toggle, the watchdog
timer is cleared.

4. Pressing `CONFIG_GPIO_BUTTON_0` will enter an interrupt service routine which
turns `CONFIG_GPIO_LED_0` on and enters an infinite loop.

5. The watchdog expires and the device is reset. For some devices,
the application will restart.

## Application Design Details

* The application opens a Watchdog driver instance configured to reset
(`Watchdog_RESET_ON`) upon the watchdog timer expiring. After
`Watchdog_open()`, the watchdog timer is running with the reload value
specified in the board file.

* The application demonstrates how to configure the reload value at runtime.
Not all devices support `Watchdog_convertMsToTicks()` and
`Watchdog_setReload()`. `Watchdog_convertMsToTicks()` will return `0` if the
APIs are not support or if the specified reloadValue is not valid. This example
configures the reload value to be the same value used in the board file.

* The application's `mainThread` enters an infinite loop which periodically
clears the watchdog timer using `Watchdog_clear()`. Each time the watchdog is
cleared, `CONFIG_GPIO_LED_0` is toggled.

* `Power_disablePolicy()` and `Power_enablePolicy()` are used to disable and
enable the power driver respectively. Doing so affects how the device behaves
when idle. If the power policy is enabled, the device is allowed to enter a
low power state when the CPU is idle. Otherwise, the device will stay awake
when the CPU is idle. This demonstrates the watchdog driver's flexibility in an
application regardless of power driver usage.

* Pressing `CONFIG_GPIO_BUTTON_0` will cause the application to get stuck in the
GPIO callback interrupt service routine (ISR), `gpioButtonIsr()`. This is
intended to simulate the application getting stuck unintentionally.

* The `watchdogCallback()` is called by some watchdog drivers upon a watchdog
timeout event. The watchdog interrupt is a non-maskable interrupt (NMI) on
some watchdog drivers and therefore will preempt the `gpioButtonIsr()`. The
other watchdog drivers configure the watchdog interrupt as the highest priority
interrupt and will still preempt the `gpioButtonIsr()`. User's should refer
to the device specific watchdog driver documentation for their device's
behavior. This application's `watchdogCallback()` will loop infinitely until
the watchdog peripheral issues a device reset.

* When the watchdog peripheral issues a device reset, the behavior is
equivalent to the user manually pressing the LaunchPad reset button. Therefore,
behavior after a reset is device specific. In most instances, the example
application will restart.

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
