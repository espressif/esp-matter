## Example Summary

This example demonstrates the APIs available in the ti/utils/json JSON library.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

## Example Usage

* This example uses the Display driver API to send diagnostic and status
strings to the UART, as the JSON APIs are demonstrated. For more information
on the Display driver refer to the Display driver documentation found in the
SimpleLink MCU SDK User's Guide.

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
Homepage"), etc.) to the appropriate COM port.
    * The COM port can be determined via Device Manager in Windows or via
`ls /dev/tty*` in Linux.

* The connection will have the following settings:
```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```

* Run the example.

* Once the example is run, a JSON string should be visible over UART:

```
    JSON template created

    JSON object created from template

    JSON buffer parsed

    firstName buffer size: 4

    JSON buffer parsed, firstName: John

    age value: 25

    new age value: 26

    serialized data:
    {"firstName" : "John" ,
    "lastName" : "Smith" ,
    "isAlive" : true ,
    "age" : 26 ,
    "address" : {"streetAddress" : "21 2nd Street" ,
    "city" : "New York" ,
    "state" : "NY" ,
    "postalCode" : "10021-3100"}
     ,
    "phoneNumbers" : [{"type" : "home" ,
    "number" : "212 555-1234"}
     ,
    {"type" : "office" ,
    "number" : "646 555-4567"}
     ,
    {"type" : "mobile" ,
    "number" : "123 456-7890"}
    ]
     ,
    "children" : []
     ,
    "spouse" : null}

    phoneNumbers array size: 3

    mobile phone number: 123 456-7890

    phoneNumbers[1] value: 646 555-4567

    Finished JSON example
```

## Application Design Details

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
