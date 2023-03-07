## Example Summary

Example that uses the DAC driver to output a data buffer that represents
a sine wave and sets an output reference voltage of 1.5 [V].

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_DAC_0`
* `CONFIG_DAC_1`
* `CONFIG_ADC_0`
* `CONFIG_GPTIMER_0`

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>


## Example Usage

* Example output requires the use of an oscilloscope connected to the output
pin and a jumper wire to connect the ADC's input pin to the DAC's output pin.
The Display driver is also used to output information.

* Connect `CONFIG_DAC_0` to the oscilloscope probe and `CONFIG_DAC_1` to
`CONFIG_ADC_0`.

>__Important:__ Caution should be exercised when connecting the output pin to an
external load. Refer to the Reference DAC documentation found in the Technical
Reference Manual to get more information about load support.

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

Example snippet of output:
```
        The measured voltage with the ADC is: 1528896 [uV].
```

* The actual conversion result values may vary depending on the reference
voltage settings. Please refer to the board specific datasheet for more details.

## Application Design Details

This application uses one thread:

`threadFxn0` - performs the following actions:

1. Opens two DAC driver objects.

2. Opens an ADC driver object and a GPtimer driver object.

3. Enables the DAC for the first object.

4. Uses the GPtimer to produce a time base to trigger the output of each sample of the data buffer.

5. Uses the first DAC object to output the data buffer a defined number of times.

6. Stops the timer.

7. Disables the DAC for the first object.

8. Enables the DAC for the second object and sets an output voltage of 1500000 [uV].

9. Measures the DAC's output voltage with the ADC object.

10. Shows the voltage measured by the ADC using the Display driver.

11. Disables the DAC for the second object and closes the DAC driver object, the GPtimer object,
    and the ADC driver object.

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