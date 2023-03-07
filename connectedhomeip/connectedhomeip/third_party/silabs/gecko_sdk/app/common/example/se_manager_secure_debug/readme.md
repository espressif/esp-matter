# SE Manager Secure Debug


This example uses the SE Manager API to perform secure debug on the supported Series 2 device.


For demonstration purposes, a private command key is stored in the device’s memory to sign the access certificate for secure debug unlock. The device’s public command key in the SE OTP must match with the public key of this private command key to perform secure debug unlock.


The default private command key (`cmd-unsafe-privkey.pem`) in PEM format can be found in the Windows folder below.


*C:\SiliconLabs\SimplicityStudio\v5\developer\adapter\_packs\secmgr\scripts\offline*


The public key of `cmd-unsafe-privkey.pem` in text format is:


`X - B1BC6F6FA56640ED522B2EE0F5B3CF7E5D48F60BE8148F0DC08440F0A4E1DCA4`


`Y - 7C04119ED6A1BE31B7707E5F9D001A659A051003E95E1B936F05C37EA793AD63`


If the device does not have a public command key in the SE OTP, the program will prompt the user to program the public key above to the device.


The user can change the private command key (`private_command_key[]`) in `app_se_manager_secure_debug.c` to match with the device’s public command key in the SE OTP for the secure debug unlock test.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## SE Manager API


The following SE Manager APIs are used in this example:


* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_status`
* `sl_se_get_debug_lock_status`
* `sl_se_validate_key`
* `sl_se_get_storage_size`
* `sl_se_generate_key`
* `sl_se_export_public_key`
* `sl_se_read_pubkey`
* `sl_se_init_otp_key`
* `sl_se_apply_debug_lock`
* `sl_se_erase_device`
* `sl_se_enable_secure_debug`
* `sl_se_disable_secure_debug`
* `sl_se_disable_device_erase`
* `sl_se_set_debug_options`
* `sl_se_get_serialnumber`
* `sl_se_get_challenge`
* `sl_se_ecc_sign`
* `sl_se_open_debug`
* `sl_se_roll_challenge`


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Upgrade the device’s SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
3. Open any terminal program and connect to the kit’s VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
6. Run the example and follow the instructions shown on the console.


## Additional Information


1. The hard-coded private command key is an insecure method so the user should find a way to import the signed access certificate for secure debug unlock.
2. The device should disconnect from the debugger when locking or unlocking the debug interface.
3. **Warning:** Loading a public command key into the SE and disable the device erase are a **ONE-TIME-ONLY** process. Both of these assignment operations are irrevocable and persist for the life of the device.
4. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)


[AN1190: Series 2 Secure Debug](https://www.silabs.com/documents/public/application-notes/an1190-efr32-secure-debug.pdf)


