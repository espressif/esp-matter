# SE Manager Host Firmware Upgrade and Debug Lock


This example uses the SE Manager API to upgrade the host firmware and enable debug lock on the supported Series 2 device.


The host firmware image (`.bin`) can be converted to a C source file using the SEGGER free utility [Bin2C.exe](https://www.segger.com/free-utilities/bin2c/). Copy the host firmware image data array (discard the last `NULL-0x00` character) in the converted C file to `host_firmware_image[]` array in `app_host_firmware_image.c`.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## SE Manager API


The following SE Manager APIs are used in this example:


* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_upgrade_status_host_image`
* `sl_se_check_host_image`
* `sl_se_apply_host_image`
* `sl_se_get_se_version`
* `sl_se_get_debug_lock_status`
* `sl_se_apply_debug_lock`
* `sl_se_read_executed_command` (VSE only)
* `sl_se_ack_command` (VSE only)


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Upgrade the device’s SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
3. Open any terminal program and connect to the kit’s VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
6. Run the example and follow the instructions shown on the console.


## Additional Information


1. The current host firmware image in `app_host_firmware_image.c` is a VCOM example that echoes typed characters to VCOM when a carriage return is hit.
2. For a device with VSE, a reset will be issued when running specified SE Manager APIs.
3. A device with VSE requires SE firmware version `v1.2.5` or above to enable debug lock through SE Manager API.
4. The device should disconnect from the debugger when upgrading the host firmware or applying the debug lock.
5. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)


