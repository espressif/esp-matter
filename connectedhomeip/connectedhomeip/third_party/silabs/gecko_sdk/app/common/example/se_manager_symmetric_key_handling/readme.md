# SE Manager Symmetric Key Handling


This example uses the SE Manager API to perform symmetric key handling on the supported Series 2 device.


In this example, SE Manager is used to generate a custom size symmetric key or a standard symmetric key for use with AES.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## Key Type


The following key types are supported in this example:


* Plaintext key in RAM
* Wrapped key in RAM (Secure Vault High only)
* Volatile key in SE (Secure Vault High only)


## Key Size


The following key sizes are supported in this example:


* AES-128 (16 bytes)
* AES-192 (24 bytes)
* AES-256 (32 bytes)
* Custom (28 bytes in this example)


## SE Manager API


The following SE Manager APIs are used in this example:


* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_validate_key`
* `sl_se_get_storage_size`
* `sl_se_generate_key`
* `sl_se_import_key` (Secure Vault High only)
* `sl_se_export_key` (Secure Vault High only)
* `sl_se_delete_key` (Secure Vault High only)
* `sl_se_transfer_key` (Secure Vault High only)


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Upgrade the device’s SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
3. Open any terminal program and connect to the kit’s VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
6. Run the example and follow the instructions shown on the console.


## Additional Information


1. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)


[AN1271: Secure Key Storage](https://www.silabs.com/documents/public/application-notes/an1271-efr32-secure-key-storage.pdf)


