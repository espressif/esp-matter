## Example Summary

Application that uses the PDM driver and the UART driver.

## Peripherals & Pin Assignments

* `Board_PIN_LED1` - Indicates that the board was initialized within main(),
and that sampling is not in progress.

* `Board_PIN_LED2` - Toggles every 16 sample blocks

* `Board_PIN_BUTTON0` - Button to start stream

* `Board_PIN_BUTTON1` - Button to stop stream

* `Display` - Used to write information about the stream on the UART.
  That includes volume indication.

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

This example requires an external PDM microphone to function. The example is
configured to use the [Adafruit PDM MEMS Microphone Breakout](https://www.adafruit.com/product/3492)
board by default.

The Adafruit PDM breakout board should be connected to the LaunchPad as follows:

| Breakout Board | LaunchPad DIO |
|----------------|---------------|
| DAT            | IOID_20       |
| CLK            | IOID_19       |
| SEL            | IOID_18       |
| 3V             | 3V3           |
| GND            | GND           |

## Example Usage

* Example output is generated through use of Display driver APIs. Refer to the
Display driver documentation found in the SimpleLink MCU SDK User's Guide.

Run the example. `Board_PIN_LED1` turns `ON` to indicate that driver
initialization is complete. It is turned off as the PDM driver is started.

A stream is started when the user presses `Board_PIN_BUTTON0`. `Board_PIN_LED1`
remains off until the stream ends. The stream ends when user presses
`Board_PIN_BUTTON1`, or if there is an error in the stream.

While the stream is running `Board_PIN_LED1` toggles on and off with a period
based on the sample rate. The period is 32 frames, duty cycle 50%. One frame is
192 samples, @16kHz, which gives a period of 364ms.

When the application is running, open a serial session (e.g. HyperTerminal,
puTTY, etc.) to the appropriate COM port.

> The COM port can be determined via the Device Manager in Windows or via
`ls /dev/tty*` in Linux.

The connection should have the following settings

```
    Baud-rate:  115200
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None
```

If the serial session is started before the target completes initialization,
the following is displayed:
    `Starting PDM stream:`

## Application Design Details

* This example shows how to use the PDM driver to start and stop a stream from a
digital microphone.

* It uses the UART driver to write information about the stream
to a host application, and uses the most basic button driver to let user control
start/stop of stream.

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
