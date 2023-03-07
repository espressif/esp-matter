## Example Summary

Sample application to handle buttons and LEDs via the Button and LED drivers.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_BUTTON_0` - Button instance used to register button presses
* `CONFIG_BUTTON_1` - Button instance used to register button presses
* `CONFIG_LED_0` - LED instance used to control the onboard LED using GPIO.
* `CONFIG_LED_1` - LED instance used to control the onboard LED using GPIO.
* `CONFIG_LED_2` - LED instance used to control the onboard LED using PWM.

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

* Run the example.

* Boards with a third LED that is dimmable will show a 'pulse' effect.

* Both `CONFIG_BUTTON_0` and `CONFIG_BUTTON_1` will register button presses.
  Each button controls one of the GPIO controlled LEDs. Different kinds of
  presses will result in different LED behavior. The example will output to
  the serial session when buttons are pressed.

| Action                                  | LED Behavior            |
|:---------------------------------------:|:-----------------------:|
| Button pressed and released             | Toggle LED              |
| Button double pressed                   | Fast blink three times  |
| Button long press                       | Start slow LED blink    |
| Button long press release               | Stop slow LED blink     |

## Application Design Details

This application uses one thread, `mainThread` , which performs the following
actions:

1. Opens and initializes LED, Button, and Display instances.

2. If board supports a dimmable LED, a PWM LED starts a heartbeat effect in the
   mainThread.

3. Button presses result in an application callback which filters for certain
   button events and calls LED APIs appropriately. Subscribed events are also
   stored in a circular buffer.

4. mainThread maintains the heartbeat by setting the PWM duty via LED_setOn().

5. mainThread consumes Button_Events from the circular buffer and prints
   messages to the UART.

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
