# Retarget IO

### Overview

A utility library to retarget the standard input/output (STDIO) messages to a UART port. With this library, you can directly print messages on a UART terminal using `printf()`. You can specify the TX pin, RX pin, and the baud rate through the `cy_retarget_io_init()` function. The UART HAL object is externally accessible so that you can use it with other UART HAL functions. 

**NOTE:** If the application is built using newlib-nano, by default, floating point format strings (%f) are not supported. To enable this support, you must add `-u _printf_float` to the linker command line.

### Quick Start
1. Add `#include "cy_retarget_io.h"`
2. Call `cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);`

    `CYBSP_DEBUG_UART_TX` and `CYBSP_DEBUG_UART_RX` pins are defined in the BSP and `CY_RETARGET_IO_BAUDRATE` is set to 115200. You can use a different baud rate if you prefer.

3. Start printing using `printf()`

### Enabling Conversion of '\\n' into "\r\n"
If you want to use only '\\n' instead of "\r\n" for printing a new line using printf(), define the macro `CY_RETARGET_IO_CONVERT_LF_TO_CRLF` using the *DEFINES* variable in the application Makefile. The library will then append '\\r' before '\\n' character on the output direction (STDOUT). No conversion occurs if "\r\n" is already present. 

### More information

* [API Reference Guide](https://cypresssemiconductorco.github.io/retarget-io/html/modules.html)
* [Cypress Semiconductor](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)
* [PSoC 6 Code Examples using ModusToolbox IDE](https://github.com/cypresssemiconductorco/Code-Examples-for-ModusToolbox-Software)
* [PSoC 6 Middleware](https://github.com/cypresssemiconductorco/psoc6-middleware)
* [PSoC 6 Resources - KBA223067](https://community.cypress.com/docs/DOC-14644)

---
© Cypress Semiconductor Corporation, 2019.
