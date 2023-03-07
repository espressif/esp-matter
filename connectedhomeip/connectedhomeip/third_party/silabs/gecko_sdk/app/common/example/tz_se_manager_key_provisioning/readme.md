# SE Manager Key Provisioning (TrustZone)


This example uses the SE Manager API to perform the key provisioning and OTP initialization on the supported Series 2 device.


The following items can be programmed to the SE OTP:


* `Tamper (Secure Vault High only) and secure boot configuration`
* `AES-128 key (HSE only)`
* `Public sign key`
* `Public command key`


The AES-128 key (`encrypt-unsafe-key.prv`) in binary format, the corresponding private sign key (`rootsign-unsafe-privkey.pem`), and private command key (`cmd-unsafe-privkey.pem`) in PEM format can be found in the Windows folder below.


*C:\SiliconLabs\SimplicityStudio\v5\developer\adapter\_packs\secmgr\scripts\offline*


The AES-128 key (`encrypt-unsafe-key.prv`) in text format is:


`81a5e21fa15286f1df445c2cc120fa3f`


The public key of `rootsign-unsafe-privkey.pem` in text format is:


`X - C4AF4AC69AAB9512DB50F7A26AE5B4801183D85417E729A56DA974F4E08A562C`


`Y - DE6019DEA9411332DC1A743372D170B436238A34597C410EA177024DE20FC819`


The public key of `cmd-unsafe-privkey.pem` in text format is:


`X - B1BC6F6FA56640ED522B2EE0F5B3CF7E5D48F60BE8148F0DC08440F0A4E1DCA4`


`Y - 7C04119ED6A1BE31B7707E5F9D001A659A051003E95E1B936F05C37EA793AD63`


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## SE Manager API


The following SE Manager APIs are used in this example:


* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_se_version`
* `sl_se_get_otp_version`
* `sl_se_read_otp`
* `sl_se_read_pubkey`
* `sl_se_init_otp_key`
* `sl_se_init_otp`
* `sl_se_read_cert_size` (Secure Vault High only)
* `sl_se_read_cert` (Secure Vault High only)
* `sl_se_read_executed_command` (VSE only)
* `sl_se_ack_command` (VSE only)


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Upgrade the device’s SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
3. Open any terminal program and connect to the kit’s VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide, check `Platform()` checkbox to browse the platform examples).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
6. Run the example and follow the instructions shown on the console.


## Additional Information


1. The key cannot be read if it has not been provisioned.
2. The key provisioning and OTP initialization will fail if the key and OTP had already been provisioned and initialized.
3. The secure boot cannot be enabled if the public sign key has not been provisioned.
4. For a device with VSE, a reset will be issued when running specified SE Manager APIs.
5. **Warning:** Loading the tamper and secure boot configuration, AES-128 key, public sign key and public command key into the SE OTP are a **ONE-TIME-ONLY** process. These assignment operations are irrevocable and persist for the life of the device.
6. This example requires SE firmware version `v1.2.12` or above to run on Non-secure Processing Environment (NSPE).
7. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)


[AN1190: Series 2 Secure Debug](https://www.silabs.com/documents/public/application-notes/an1190-efr32-secure-debug.pdf)


[AN1218: Series 2 Secure Boot with RTSL](https://www.silabs.com/documents/public/application-notes/an1218-secure-boot-with-rtsl.pdf)


[AN1222: Production Programming of Series 2 Devices](https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf)


[AN1247: Anti-Tamper Protection Configuration and Use](https://www.silabs.com/documents/public/application-notes/an1247-efr32-secure-vault-tamper.pdf)


