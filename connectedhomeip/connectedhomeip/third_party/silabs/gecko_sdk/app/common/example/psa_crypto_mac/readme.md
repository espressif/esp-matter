# PSA Crypto MAC


This example uses the PSA Crypto API to perform Message Authentication Code (MAC) operations on the supported device.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


Except for the Series 1 Cortex-M0+ device, the example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `PSA_CRYPTO_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## Key Storage


The following key storages are supported in this example:


* Volatile plain key in RAM
* Persistent plain key in [NVM3](https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3)
* Volatile wrapped key in RAM (Secure Vault High only)
* Persistent wrapped key in NVM3 (Secure Vault High only)


## Key Size


The following key sizes are supported in this example:


* 128-bit
* 192-bit
* 256-bit


## Payload Size


The following payload sizes are supported in this example:


* `MSG_SIZE`/16
* `MSG_SIZE`/4
* `MSG_SIZE`


The default `MSG_SIZE` is `4096`, it is defined in `app_process.h`.


## MAC Algorithm


The following MAC algorithms are supported in this example:


* `PSA_ALG_HMAC` (single-part and multi-part)
* `PSA_ALG_CMAC` (single-part and multi-part)


## Hash Algorithm (HMAC)


The following hash algorithms are supported in this example:


* `PSA_ALG_SHA_1`
* `PSA_ALG_SHA_224`
* `PSA_ALG_SHA_256`
* `PSA_ALG_SHA_384`
* `PSA_ALG_SHA_512`


## PSA Crypto API


The following PSA Crypto APIs are used in this example:


* `psa_crypto_init`
* `psa_key_attributes_init`
* `psa_set_key_type`
* `psa_set_key_bits`
* `psa_set_key_usage_flags`
* `psa_set_key_algorithm`
* `psa_set_key_id`
* `psa_set_key_lifetime`
* `psa_generate_key`
* `psa_import_key`
* `psa_get_key_attributes`
* `psa_get_key_algorithm`
* `psa_reset_key_attributes`
* `psa_destroy_key`
* `psa_generate_random`
* `psa_mac_compute`
* `psa_mac_verify`
* `psa_mac_operation_init`
* `psa_mac_sign_setup`
* `psa_mac_verify_setup`
* `psa_mac_update`
* `psa_mac_sign_finish`
* `psa_mac_verify_finish`


## Getting Started


1. Upgrade the kit’s firmware to the latest version (see `Adapter Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
2. Upgrade the device’s SE firmware to the latest version when Series 2 device is used (see `Secure Firmware` under [General Device Information](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-about-the-launcher/welcome-and-device-tabs#general-device-information) in the Simplicity Studio 5 User's Guide).
3. Open any terminal program and connect to the kit’s VCOM port (if using `Device Console` in Simplicity Studio 5, `Line terminator:` must be set to `None`).
4. Create this platform example project in the Simplicity IDE (see [Examples](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-getting-started/start-a-project#examples) in the Simplicity Studio 5 User's Guide).
5. Build the example and download it to the kit (see [Simple Build](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/building#simple-build) and [Flash Programmer](https://docs.silabs.com/simplicity-studio-5-users-guide/latest/ss-5-users-guide-building-and-flashing/flashing#flash-programmer) in the Simplicity Studio 5 User's Guide).
6. Run the example and follow the instructions shown on the console.


## Additional Information


1. The example uses the CTR-DRBG, a pseudo-random number generator (PRNG) included in [Mbed TLS](https://docs.silabs.com/mbed-tls/latest/) to generate the random number. If the example is running on a device that includes a TRNG (True Random Number Generator) hardware module, the TRNG will be used as an entropy source to seed the CTR-DRBG. If the device does not incorporate a TRNG, the example will use [RAIL](https://docs.silabs.com/rail/latest/) or NV (non-volatile) seed (requires NVM3) as the entropy source.
2. If an algorithm is not supported in the hardware accelerator of the selected device, the PSA Crypto will use the software fallback feature in Mbed TLS.
3. Change the `PERSISTENT_KEY_ID` value in `app_process.h` if this key ID had already existed in NVM3.
4. The Series 1 devices do not support 192-bit key on CMAC.
5. HMAC (`PSA_ALG_HMAC`) streaming with wrapped key is not supported yet.
6. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.

## Resources


[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)


