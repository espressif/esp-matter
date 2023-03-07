# PSA Crypto KDF


This example uses the PSA Crypto API to perform the Key Derivation Function (KDF) on the supported device.


In cryptography, a [key derivation function](https://en.wikipedia.org/wiki/Key_derivation_function) is a cryptographic hash function that derives one or more secret keys from a secret value such as a master key, a password, or a passphrase using a pseudo-random function. KDFs can be used to stretch keys into longer keys or to obtain keys of a required format, such as converting a group element that is the result of an ECDH key exchange into a symmetric key for use with AES.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


Except for the Series 1 Cortex-M0+ device, the example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `PSA_CRYPTO_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## Key Storage


The following key storages are supported in this example:


* Volatile plain key in RAM
* Persistent plain key in [NVM3](https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3)
* Volatile wrapped key in RAM (Secure Vault High only)
* Persistent wrapped key in NVM3 (Secure Vault High only)


## Key Derivation Algorithm


The following key derivation algorithms are supported in this example:


* `PSA_ALG_HKDF(hash_alg)`
* `PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(hash_alg))`


In this example, the ECDH key agreement is based on SECP256R1 if the `PSA_ALG_KEY_AGREEMENT` algorithm is used.


## Hash Algorithm (HKDF)


The following hash algorithms (HMAC) are supported in this example:


* `PSA_ALG_SHA_1`
* `PSA_ALG_SHA_224`
* `PSA_ALG_SHA_256`
* `PSA_ALG_SHA_384`
* `PSA_ALG_SHA_512`


## Derived Key Size


The following derived key sizes are supported in this example:


* 128-bit
* 192-bit
* 256-bit


## Derived Key Algorithm


The following derived key algorithms are supported in this example:


* `PSA_ALG_ECB_NO_PADDING`
* `PSA_ALG_CBC_NO_PADDING`
* `PSA_ALG_CFB`
* `PSA_ALG_CTR`
* `PSA_ALG_CCM`
* `PSA_ALG_GCM`
* `PSA_ALG_STREAM_CIPHER`
* `PSA_ALG_CHACHA20_POLY1305`
* `PSA_ALG_CMAC`
* `PSA_ALG_HMAC(hash_alg)`
* `PSA_ALG_HKDF(hash_alg)`


The default derived key algorithm is `PSA_ALG_CTR`, it is defined in `app_process.h`.


The `PSA_ALG_STREAM_CIPHER` for CHACHA20 and `PSA_ALG_CHACHA20_POLY1305` can only use a 256-bit key.


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
* `psa_import_key`
* `psa_key_derivation_output_key`
* `psa_export_key`
* `psa_get_key_attributes`
* `psa_get_key_algorithm`
* `psa_reset_key_attributes`
* `psa_destroy_key`
* `psa_key_derivation_operation_init`
* `psa_key_derivation_setup`
* `psa_key_derivation_set_capacity`
* `psa_key_derivation_input_bytes`
* `psa_key_derivation_input_key`
* `psa_key_derivation_key_agreement`
* `psa_key_derivation_abort`
* `sl_psa_key_derivation_single_shot` (Silicon Labs custom API)
* `mbedtls_psa_crypto_free`


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
3. Change the `BASE_KEY_ID` and `DERIVE_KEY_ID` values in `app_process.h` if these key IDs had already existed in NVM3.
4. The Silicon Labs custom API `sl_psa_key_derivation_single_shot()` is used to derive a symmetric wrapped key.
5. The `PSA_ALG_KEY_AGREEMENT` algorithm does not apply to the wrapped key.
6. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)


