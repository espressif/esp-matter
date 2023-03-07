# Cypress PSoC6 MCUs acceleration for mbedTLS library

### What's Included?
Please refer to the [README.md](./README.md) for a complete description of the PSoC6 MCUs acceleration for mbedTLS library.
New in this release:

* reorganized SHA implementation

### Limitations
Currently Cypress PSoC6 MCUs acceleration doesn't support:

- RSA
- ECP NIST-B curves
- ECP NIST-K curves
- ECP 25519 curve
- CHACHA20
- SHA3
- AES GCM
- POLY1305

### Supported Software and Tools
This version of the PSoC6 MCUs acceleration for mbedTLS library was validated for compatibility with the following Software and Tools:

| Software and Tools                                      | Version |
| :---                                                    | :----:  |
| ModusToolbox Software Environment                       | 2.0     |
| PSoC6 Peripheral Driver Library (PDL)                   | 1.3.1   |
| PSoC6 Hardware Abstraction Layer(HAL)                   | 1.0.0   |
| PSoC6 Core Library                                      | 1.0.0   |
| mbedTLS                                                 | 2.19.1  |
| GCC Compiler                                            | 7.2.1   |
| IAR Compiler                                            | 8.20    |
| ARM Compiler 6                                          | 6.12    |
| MBED OS                                                 | 5.13.1  |

### Change log

| Version |	Changes                                       | Reason for Change |
| :----:  |	:---                                          | :----             |
| 1.2     |	New internal resource management instead of using psoc6hal | Simple projects support without psoc6hal |
| 1.1     |	Reorganized SHA implementation                | New Cypress MCUs support |
| 1.0     |	The initial version                           |                   |

### More information
* [Peripheral Driver Library API Reference Manual](https://cypresssemiconductorco.github.io/psoc6pdl/pdl_api_reference_manual/html/index.html)
* [PSoC 6 Technical Reference Manuals](https://www.cypress.com/search/all/PSoC%206%20Technical%20Reference%20Manual?f%5b0%5d=meta_type%3Atechnical_documents&f%5b1%5d=resource_meta_type%3A583)
* [PSoC 6 MCU Datasheets](https://www.cypress.com/search/all?f%5b0%5d=meta_type%3Atechnical_documents&f%5b1%5d=resource_meta_type%3A575&f%5b2%5d=field_related_products%3A114026)
* [mbed-os repository](https://github.com/ARMmbed/mbed-os)
* [mbedtls repository](https://github.com/ARMmbed/mbedtls)
* [Alternative cryptography engines implementation](https://tls.mbed.org/kb/development/hw_acc_guidelines)
* [mbedTLS supported features](https://tls.mbed.org/core-features)
* [Cypress Semiconductor](http://www.cypress.com)

---
© Cypress Semiconductor Corporation, 2019.
