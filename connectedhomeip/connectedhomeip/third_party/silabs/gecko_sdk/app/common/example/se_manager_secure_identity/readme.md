# SE Manager Secure Identity


This example uses the SE Manager API and Mbed TLS to perform the secure identity operation on the supported Series 2 Secure Vault High device.


This example uses the CRYPTO engine in the SE to accelerate the X.509 API functions of Mbed TLS. The Private Device Key in the Secure Key Storage on the chip is used to sign the challenge from the remote device.


The CRYPTO hardware acceleration on Mbed TLS can be switched off by defining the `NO_CRYPTO_ACCELERATION` symbol in the IDE setting (`Preprocessor->Defined symbols`). Increase the heap size to 10240 (`sl_memory_config.h`) to use this option in IAR Embedded Workbench.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


The certificates are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT_CERT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## SE Manager API


The following SE Manager APIs are used in this example:


* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_random`
* `sl_se_read_cert_size`
* `sl_se_read_cert`
* `sl_se_ecc_sign`
* `sl_se_read_pubkey`
* `sl_se_ecc_verify`


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Upgrade the device’s SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
3. Open any terminal program and connect to the kit’s VCOM port.
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
6. Run the example and the console should display the process steps of this example.


## Additional Information


1. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)


[AN1268: Authenticating Silicon Labs Devices Using Device Certificates](https://www.silabs.com/documents/public/application-notes/an1268-efr32-secure-identity.pdf)


