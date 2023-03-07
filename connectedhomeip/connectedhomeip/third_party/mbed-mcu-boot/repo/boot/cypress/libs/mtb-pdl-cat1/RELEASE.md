# MTB CAT1A Peripheral Driver Library v2.1.0

Please refer to the [README.md](./README.md) and the
[PDL API Reference Manual](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/index.html)
for a complete description of the Peripheral Driver Library.

Some restrictions apply to the PSoC 64 devices configuration. Please refer to [PRA (Protected Register Access)](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__pra.html) driver documentation for the details.

## New Features

* External clock support for secure core.
* Provided interface for validating and configuring SRAM power modes.

## Updated Personalities

* BT - Added Support for BT personality disabled case
* SAR - Minor bug fix.
* SCB - Allow 50000 kbps baud rate if MISO is not used.
* TCPWM - Added Swap Underflow/Overflow Set/Clear functionality.
* USBFS - Improved filtering for allowed DMA connections.

## Added Drivers

## Updated Drivers

* [DMA 2.40](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__dma.html)
* [EFUSE 2.0](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__dma.html)
* [GPIO 1.40](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__gpio.html)
* [IPC 1.60](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__ipc.html)
* [LVD 1.40](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__lvd.html)
* [MCWDT 1.50](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__mcwdt.html)
* [PRA 2.20](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__pra.html)
* [PROT 1.60](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__prot.html)
* [RTC 2.50](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__rtc.html)
* [SCB 2.70](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__scb.html)
* [SDHOST 1.70](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__sd__host.html)
* [SMIF 2.10](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__smif.html)
* [SYSCLK 3.10](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__sysclk.html)
* [SYSINT 2.0](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__sysint.html)
* [SYSLIB 2.80](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__syslib.html)
* [SYSPM 5.50](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__syspm.html)
* [SYSTICK 1.40](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__arm__system__timer.html)
* [TCPWM 1.30](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__tcpwm.html)
* [TRIGMUX 1.30](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__trigmux.html)

### Drivers with patch version updates

Minor documentation changes:
* [FLASH 3.50.1](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/group__group__flash.html)

### Obsoleted part numbers

The ModusToolbox Device Configurator can not create the designs targeting the obsolete PSoC 6 part numbers.

Below is a list of PSoC 6 part numbers obsoleted in psoc6pdl release-v1.6.0, with the suggested next best alternative:

| Obsoleted part number | Next best alternative |
| :---                  | :----                 |
| CY8C624AAZI-D44       | CY8C624AAZI-S2D44     |
| CY8C624AFNI-D43       | CY8C624AFNI-S2D43     |
| CY8C624ABZI-D04       | CY8C624ABZI-S2D04     |
| CY8C624ABZI-D14       | CY8C624ABZI-S2D14     |
| CY8C624AAZI-D14       | CY8C624AAZI-S2D14     |
| CY8C6248AZI-D14       | CY8C6248AZI-S2D14     |
| CY8C6248BZI-D44       | CY8C6248BZI-S2D44     |
| CY8C6248AZI-D44       | CY8C6248AZI-S2D44     |
| CY8C6248FNI-D43       | CY8C6248FNI-S2D43     |
| CY8C624ALQI-D42       | N/A                   |

## Known Issues

## Defect Fixes

See the Changelog section of each Driver in the [PDL API Reference](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/modules.html) for all fixes and updates.

## Supported Software and Tools

This version of PDL was validated for compatibility with the following Software and Tools:

| Software and Tools                                                            | Version      |
| :---                                                                          | :----        |
| [Cypress Core Library](https://github.com/cypresssemiconductorco/core-lib)    | 1.1.4        |
| [Cypress HAL](https://github.com/cypresssemiconductorco/psoc6hal)             | 1.4.0        |
| CMSIS-Core(M)                                                                 | 5.4.0        |
| GCC Compiler                                                                  | 9.3.1        |
| IAR Compiler                                                                  | 8.42.2       |
| ARM Compiler 6                                                                | 6.13         |
| FreeRTOS                                                                      | 10.0.1       |

## More information

* [Peripheral Driver Library README.md](./README.md)
* [Peripheral Driver Library API Reference Manual](https://cypresssemiconductorco.github.io/mtb-pdl-cat1/pdl_api_reference_manual/html/index.html)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.cypress.com/products/modustoolbox-software-environment)
* [ModusToolbox Device Configurator Tool Guide](https://www.cypress.com/ModusToolboxDeviceConfig)
* [AN210781 Getting Started with PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity](http://www.cypress.com/an210781)
* [PSoC 6 Technical Reference Manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f%5b0%5d=meta_type%3Atechnical_documents&f%5b1%5d=resource_meta_type%3A583)
* [PSoC 6 MCU Datasheets](https://www.cypress.com/search/all?f%5b0%5d=meta_type%3Atechnical_documents&f%5b1%5d=resource_meta_type%3A575&f%5b2%5d=field_related_products%3A114026)
* [Cypress Semiconductor](http://www.cypress.com)

---
© Cypress Semiconductor Corporation, 2020.
