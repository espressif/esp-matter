# PSA Crypto Asymmetric Key


This example uses the PSA Crypto API to perform asymmetric key operations on the supported device.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


Except for the Series 1 Cortex-M0+ device, the example is written in such a way as to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `PSA_CRYPTO_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## Key Storage


The following key storages are supported in this example:


* Volatile plain key in RAM
* Persistent plain key in [NVM3](https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3)
* Volatile wrapped key in RAM (Secure Vault High only)
* Persistent wrapped key in NVM3 (Secure Vault High only)


## Elliptic Curve Key


The following elliptic curve keys are supported in this example:


##### `PSA_ECC_FAMILY_SECP_R1` :


* SECP192R1 - 192-bit
* SECP256R1 - 256-bit
* SECP384R1 - 384-bit
* SECP521R1 - 521-bit


##### `PSA_ECC_FAMILY_MONTGOMERY` :

* CURVE25519 (X25519) - 255-bit
* CURVE448 (X448) - 448-bit (Secure Vault High only)

##### `PSA_ECC_FAMILY_TWISTED_EDWARDS` :

* Ed25519 - 255-bit (HSE only)


## Key Usage


The following key usages can be selected in this example:


* `PSA_KEY_USAGE_EXPORT`
* `PSA_KEY_USAGE_COPY`


## Asymmetric Key Algorithm


The following asymmetric key algorithms are supported in this example:


* `PSA_ALG_ECDSA(hash_alg)`
* `PSA_ALG_ECDSA_ANY`
* `PSA_ALG_ECDH`
* `PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(hash_alg))`
* `PSA_ALG_PURE_EDDSA` (HSE only)

The `PSA_ECC_FAMILY_SECP_R1` key can be used for ECDSA and key agreement algorithms (use `PSA_ALG_ECDSA_ANY` in this example).

The `PSA_ECC_FAMILY_MONTGOMERY` key can only be used for key agreement algorithms (use `PSA_ALG_ECDH` in this example).

The `PSA_ECC_FAMILY_TWISTED_EDWARDS` key can only be used for the EdDSA algorithm (`PSA_ALG_PURE_EDDSA`).

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
* `psa_copy_key`
* `psa_reset_key_attributes`
* `psa_export_key`
* `psa_export_public_key`
* `psa_destroy_key`
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
3. The PSA Crypto does not yet support software fallback on the `CURVE448` and `Ed25519` curves.
4. The HSE Secure Vault Mid devices require SE firmware v1.2.11 or higher (EFR32xG21) and v2.1.7 or higher (other HSE devices) to support hardware acceleration on `CURVE25519` and `Ed25519`.
5. Change the `PERSISTENT_KEY_ID` and `PERSISTENT_COPY_KEY_ID` values in `app_process.h` if these key IDs had already existed in NVM3.
6. The `PSA_KEY_USAGE_COPY` policy does not apply to the wrapped key.
7. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)


