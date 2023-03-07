### Cypress PSoC 6 Hardware Abstraction Layer (HAL)
Cypress PSoC 6 Hardware Abstraction Layer (HAL) provides an implementation of the Cypress Hardware Abstraction Layer for the PSoC 6 family of chips. This API provides convenience methods for initializing and manipulating different hardware peripherals. Depending on the specific chip being used, not all features may be supported.

### What's Included?
This release of the PSoC 6 HAL includes support for the following drivers:
* ADC
* CRC
* DAC
* DMA
* EZ-I2C
* Flash
* GPIO
* Hardware Manager
* I2C
* LP Timer
* PWM
* QSPI
* RTC
* SDHC
* SDIO
* SPI
* System
* Timer
* TRNG
* UART
* USB Device
* WDT

### What Changed?
#### v1.1.1
* Improved documentation for a number of drivers
* Fixed incompatibility with PDL 1.4.1 release
#### v1.1.0
* Added new DMA driver
* Added new EZ-I2C driver
* Extended System driver to allow getting information about reset
* Extended System driver to provide delay functions
* Updated PWM driver to provide additional configuration options
* Updated Timer driver to allow reading the current count
* Updated RTC driver to support Day Light Savings time
* Updated LP Timer driver to improve performance
* Minor updates up avoid potential warnings on some toolchains
* Multiple bug fixes across drivers
#### v1.0.0
* Initial release (ADC, CRC, DAC, Flash, GPIO, Hardware Manager, I2C, LP Timer, PWM, QSPI, RTC, SDHC, SDIO, SPI, System, Timer, TRNG, UART, USB Device, WDT)

### Supported Software and Tools
This version of the Cypress PSoC 6 Hardware Abstraction Layer was validated for compatibility with the following Software and Tools:

| Software and Tools                        | Version |
| :---                                      | :----:  |
| ModusToolbox Software Environment         | 2.0     |
| GCC Compiler                              | 7.4     |
| IAR Compiler                              | 8.32    |
| ARM Compiler                              | 6.11    |

### More information
Use the following links for more information, as needed:
* [API Reference Guide](https://cypresssemiconductorco.github.io/psoc6hal/html/modules.html)
* [Cypress Semiconductor](http://www.cypress.com)
* [Cypress Semiconductor GitHub](https://github.com/cypresssemiconductorco)
* [ModusToolbox](https://www.cypress.com/products/modustoolbox-software-environment)

---
© Cypress Semiconductor Corporation, 2019-2020.