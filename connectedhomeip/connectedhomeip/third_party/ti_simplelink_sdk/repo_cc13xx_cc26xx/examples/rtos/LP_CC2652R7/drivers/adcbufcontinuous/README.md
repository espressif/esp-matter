## Example Summary

Example that uses the ADCBuf driver to make a number of samples
and echo them via UART. This example shows how to initialize the
ADCBuf driver in continuous mode.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_ADCBUF_0` - ADCBuf instance
* `CONFIG_ADCBUF_0_CHANNEL_0` - ADC channel 0 of the `CONFIG_ADCBUF_0` instance

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

## Example Usage

* Connect the ADC channel to a sampling source.
    * For quick testing, connect `CONFIG_ADCBUF_0_CHANNEL_0` to `GND` or `3V3`.
    * __Note:__ Undefined values may be returned if the channel is not connected
      to a sampling source.

>__Important:__ Caution should be used when connecting the pins to analog
inputs greater than 3VDC.

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

* The target will send packages of 50 samples and a header to the serial
session. The buffer number and its average microvolt value is displayed
on respective new lines.

* Note that many ADCBuf driver implementations use the DMA peripheral in a
ping pong buffer configuration. Because of this setting breakpoints while
active ADC conversions and DMA transactions are happening will result in
unreliable behavior. It is recommended to not set breakpoints in this code
example and instead read results via the UART terminal.

## Application Design Details

* A single task sets up the UART connection to the serial console and starts
a continuous conversion with the ADCBuf driver. After this is completed, the
task sleeps forever.

* The ADCBuf driver performs 50 samples at 200 Hz each.
    * The driver supports higher sampling rates; however, 200 Hz was chosen for
easily interpretable output. The maximum sampling rates are device specific.

* The callback function `adcBufCallback` is called whenever an ADC buffer is
full. The contents of the buffer are first adjusted so that raw results are
comparable between devices of the same make. The adjusted values are then
converted into microvolts to be human-readable. Lastly, the buffer number and
its average microvolt value is displayed via UART.

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
