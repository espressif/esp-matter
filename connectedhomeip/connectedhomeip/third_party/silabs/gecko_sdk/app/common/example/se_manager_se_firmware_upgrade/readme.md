# SE Manager SE Firmware Upgrade

This example uses the SE Manager API to upgrade the SE firmware on the supported Series 2 device.

The SE upgrade firmware image must be stored to the device's internal flash in `.seu` format. The latest SE firmware image (`.seu` and `.hex`) and release notes can be found in the Windows folder below.

For GSDK v3.2 and lower:<br>

_C:\SiliconLabs\SimplicityStudio\v5\developer\sdks\GSDK VERSION\util\se_release\public_

For GSDK v4.0 and higher:<br>

_C:\Users\PC USER NAME\SimplicityStudio\SDKs\gecko_sdk\util\se_release\public_

The SE firmware image (`.seu`) can be converted to a C source file by SEGGER free utility [Bin2C.exe](https://www.segger.com/free-utilities/bin2c/). Copy the SE firmware image data array (discard the last `NULL-0x00` character) in converted C file to `se_firmware_image[]` array in `app_se_firmware_image.c`.

The SE firmware image validation will fail if the image version is equal to or less than the current SE firmware version.

The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.

The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).

## SE Manager API

The following SE Manager APIs are used in this example:

* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_se_version`
* `sl_se_get_upgrade_status_se_image`
* `sl_se_check_se_image`
* `sl_se_apply_se_image`
* `sl_se_read_executed_command` (VSE only)
* `sl_se_ack_command` (VSE only)

## Getting Started

1. Upgrade the kit's firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in Simplicity Studio 5 Users Guide).
2. Upgrade the device's SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in Simplicity Studio 5 Users Guide).
3. Open any terminal program and connect to the kit's VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in Simplicity Studio 5 Users Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in Simplicity Studio 5 Users Guide).
6. Run the example and follow the instructions shown on the console.

## Additional Information

1. The current version for HSE or VSE firmware upgrade can be found in the `app_se_firmware_image.c`.
2. For a device with VSE, a reset will be issued when running specified SE Manager APIs.
3. EFR32xG23 and EFR32xG24 devices require SE firmware v2.0.1 or above to run this example.
4. The device should disconnect from the debugger when upgrading the HSE or VSE firmware.
5. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.

## Resources

[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)

[AN1222: Production Programming of Series 2 Devices](https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf)
