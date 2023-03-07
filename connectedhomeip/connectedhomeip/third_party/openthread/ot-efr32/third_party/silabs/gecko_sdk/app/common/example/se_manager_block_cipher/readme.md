# SE Manager Block Cipher


This example uses the SE Manager API to perform the block ciphers on the supported Series 2 device.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


The example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `SE_MANAGER_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## Key Type


The following key types are supported in this example:


* Plaintext key in RAM
* Wrapped key in RAM (Secure Vault High only)
* Volatile key in SE (Secure Vault High only)


## Key Size


The following key sizes are supported in this example:


* 128-bit
* 192-bit
* 256-bit


## Payload Size


The following payload sizes are supported in this example:


* `PLAIN_MSG_SIZE`/16
* `PLAIN_MSG_SIZE`/4
* `PLAIN_MSG_SIZE`


The default `PLAIN_MSG_SIZE` is `4096`, it is defined in `app_se_manager_block_cipher.h`.


## Hash Algorithm (HMAC)


The following hash algorithms are supported in this example:


* `SHA-1`
* `SHA-224`
* `SHA-256`
* `SHA-384` (Secure Vault High only)
* `SHA-512` (Secure Vault High only)


## Cipher Algorithm


The following cipher algorithms are supported in this example:


* `AES ECB`
* `AES CTR`
* `AES CCM`
* `AES GCM`
* `AES CBC`
* `AES CFB8`
* `AES CFB128`
* `AES CMAC`
* `HMAC`
* `ChaCha20-Poly1305` (Secure Vault High only)


The `ChaCha20-Poly1305` can only use a 256-bit key.


## SE Manager API


The following SE Manager APIs are used in this example:


* `sl_se_init`
* `sl_se_deinit`
* `sl_se_init_command_context`
* `sl_se_deinit_command_context`
* `sl_se_get_random`
* `sl_se_validate_key`
* `sl_se_get_storage_size`
* `sl_se_generate_key`
* `sl_se_delete_key` (Secure Vault High only)
* `sl_se_aes_crypt_ecb`
* `sl_se_aes_crypt_ctr`
* `sl_se_ccm_encrypt_and_tag`
* `sl_se_ccm_auth_decrypt`
* `sl_se_gcm_crypt_and_tag`
* `sl_se_gcm_auth_decrypt`
* `sl_se_aes_crypt_cbc`
* `sl_se_aes_crypt_cfb8`
* `sl_se_aes_crypt_cfb128`
* `sl_se_cmac`
* `sl_se_hmac`
* `sl_se_poly1305_genkey_tag` (Secure Vault High only)
* `sl_se_chacha20_poly1305_encrypt_and_tag` (Secure Vault High only)
* `sl_se_chacha20_poly1305_auth_decrypt` (Secure Vault High only)


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


