# SE Manager Tamper

This example uses the SE Manager API to demonstrate the tamper feature on the supported Series 2 Secure Vault High device.

For demonstration purposes, a private command key is stored in the device's memory to sign the access certificate for tamper disable. The device's public command key in the SE OTP must match with the public key of this private command key to disable tamper responses.

The default private command key (`cmd-unsafe-privkey.pem`) in PEM format can be found in the Windows folder below.

_C:\SiliconLabs\SimplicityStudio\v5\developer\adapter_packs\secmgr\scripts\offline_

The public key of `cmd-unsafe-privkey.pem` in text format is:

`X - B1BC6F6FA56640ED522B2EE0F5B3CF7E5D48F60BE8148F0DC08440F0A4E1DCA4`

`Y - 7C04119ED6A1BE31B7707E5F9D001A659A051003E95E1B936F05C37EA793AD63`

If the device does not have public command key in the SE OTP, the program will prompt the user to program the public key above to the device.

The user can change the private command key (`private_command_key[]`) in `app_se_manager_tamper_disable.c` to match with the device's public command key in the SE OTP for tamper disable.

The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.

The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).

## Tamper Responses

| Level | Responses | Description |
| --- | --- | --- |
| 0 | Ignore | No action is taken |
| 1 | Interrupt | The SETAMPERHOST interrupt on the host is triggered |
| 2 | Filter | A counter in the tamper filter is increased |
| 4 | Reset | The device is reset |
| 7 | Erase OTP | Erases the OTP configuration of the device (make the device and all wrapped secrets unrecoverable) |

## Tamper Sources (EFR32xG21B Device)

| Number | Name | Default level | User level in this example |
| --- | --- | --- | --- |
| 0 | Reserved | — | — |
| 1 | Filter Counter | 0 | 1 |
| 2 | SE Watchdog | 4 | 4 |
| 3 | Reserved | — | — |
| 4 | SE RAM CRC | 4 | 4 |
| 5 | SE Hardfault | 4 | 4 |
| 6 | Reserved | — | — |
| 7 | Software Assertion | 4 | 4 |
| 8 | SE CodeAuth | 4 | 4 |
| 9 | UserCodeAuth | 0 | 0 |
| 10 | MailboxAuth | 0 | 1 |
| 11 | DCIAuth | 0 | 0 |
| 12 | OTP Read | 4 | 4 |
| 13 | Reserved | — | — |
| 14 | Self-test | 4 | 4 |
| 15 | TRNG Monitor | 0 | 1 |
| 16 | PRS0 | 0 | 1 (PRS source: Push button PB0) |
| 17 | PRS1 | 0 | 1 (PRS source: None) |
| 18 | PRS2 | 0 | 2 (PRS source: Push button PB0) |
| 19 | PRS3 | 0 | 2 (PRS source: None) |
| 20 | PRS4 | 0 | 4 (PRS source: Push button PB1) |
| 21 | PRS5 | 0 | 4 (PRS source: Software) |
| 22 | PRS6 | 0 | 7 (PRS source: None) |
| 23 | PRS7 | 0 | 7 (PRS source: None) |
| 24 | DECOUPLE BOD | 4 | 4 |
| 25 | TempSensor | 0 | 2 |
| 26 | VGlitch Falling | 0 | 2 |
| 27 | VGlitch Rising | 0 | 2 |
| 28 | SecureLock | 4 | 4 |
| 29 | SE Debug | 0 | 0 |
| 30 | Digital glitch | 0 | 2 |
| 31 | SE ICACHE | 4 | 4 |

## Tamper Sources (Other Series 2 Secure Vault High Devices)

| Number | Name | Default level | User level in this example |
| --- | --- | --- | --- |
| 0 | Reserved | — | — |
| 1 | Filter Counter | 0 | 1 |
| 2 | SE Watchdog | 4 | 4 |
| 3 | Reserved | — | — |
| 4 | SE RAM ECC 2 | 4 | 4 |
| 5 | SE Hardfault | 4 | 4 |
| 6 | Reserved | — | — |
| 7 | Software Assertion | 4 | 4 |
| 8 | SE CodeAuth | 4 | 4 |
| 9 | UserCodeAuth | 0 | 0 |
| 10 | MailboxAuth | 0 | 1 |
| 11 | DCIAuth | 0 | 0 |
| 12 | OTP Read | 4 | 4 |
| 13 | Reserved | — | — |
| 14 | Self-test | 4 | 4 |
| 15 | TRNG Monitor | 0 | 1 |
| 16 | SecureLock | 4 | 4 |
| 17 | DGlitch | 0 | 2 |
| 18 | VGlitch | 0 | 2 |
| 19 | SE ICACHE | 4 | 4 |
| 20 | SE RAM ECC 1 | 0 | 1 |
| 21 | BOD | 4 | 4 |
| 22 | TempSensor | 0 | 2 |
| 23 | DPLL Fall | 0 | 2 |
| 24 | DPLL Rise | 0 | 2 |
| 25 | PRS0 | 0 | 1 (PRS source: None) |
| 26 | PRS1 | 0 | 1 (PRS source: Push button PB0) |
| 27 | PRS2 | 0 | 2 (PRS source: Push button PB0) |
| 28 | PRS3 | 0 | 2 (PRS source: None) |
| 29 | PRS4 | 0 | 4 (PRS source: Push button PB1) |
| 30 | PRS5 | 0 | 4 (PRS source: Software) |
| 31 | PRS6 | 0 | 7 (PRS source: None) |

The disable tamper command reverts all masked tamper sources (`TAMPER_DISABLE_MASK` in `app_se_manager_tamper_disable.h`) to the hardcoded configuration (default levels in tables above).

For EFR32xG21B devices, the default value of `TAMPER_DISABLE_MASK` is `0x00fa0000`. It restores PRS7, PRS6, PRS5, PRS4, PRS3, and PRS1 to the default level 0 (Ignore) after running the disable tamper command.

For other Series 2 Secure Vault High devices, the default value of `TAMPER_DISABLE_MASK` is `0xf2000000`. It restores PRS6, PRS5, PRS4, PRS3, and PRS0 to the default level 0 (Ignore) after running the disable tamper command.

## Tamper Settings

| Setting | User value in this example |
| --- | --- |
| Filter - trigger threshold | 4 |
| Filter - reset period | ~33 seconds |
| Flag | Digital Glitch Detector Always On: Disabled |
| Flag (not available on EFR32xG21B devices) | Keep Tamper Alive During Sleep: Disabled |
| Reset threshold | 5 |

## SE Manager API

The following SE Manager APIs are used in this example:

* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_reset_cause`
* `sl_se_get_status`
* `sl_se_read_otp`
* `sl_se_init_otp`
* `sl_se_validate_key`
* `sl_se_get_storage_size`
* `sl_se_generate_key`
* `sl_se_export_public_key`
* `sl_se_read_pubkey`
* `sl_se_init_otp_key`
* `sl_se_get_serialnumber`
* `sl_se_get_challenge`
* `sl_se_ecc_sign`
* `sl_se_disable_tamper`
* `sl_se_roll_challenge`

## Getting Started

1. Upgrade the kit's firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in Simplicity Studio 5 Users Guide).
2. Upgrade the device's SE firmware to the latest version (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in Simplicity Studio 5 Users Guide).
3. Open any terminal program and connect to the kit's VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in Simplicity Studio 5 Users Guide).
6. Run the example and follow the instructions shown on the console.

## Additional Information

1. The hard-coded private command key is an insecure method so the user should find a way to import the signed access certificate for tamper disable.
2. This example does not enable the secure boot when provisioning the tamper configuration in `app_se_manager_tamper.c`.
3. The device should disconnect from the debugger when running this example.
4. **Warning:** Loading the tamper configuration and a public command key into the SE are a **ONE-TIME-ONLY** process. Both of these assignment operations are irrevocable and persist for the life of the device.
5. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.

## Resources

[SE Manager API](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)<br>

[AN1247: Anti-Tamper Protection Configuration and Use](https://www.silabs.com/documents/public/application-notes/an1247-efr32-secure-vault-tamper.pdf)
