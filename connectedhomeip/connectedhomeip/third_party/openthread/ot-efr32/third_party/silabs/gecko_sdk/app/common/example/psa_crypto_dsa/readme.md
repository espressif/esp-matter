# PSA Crypto DSA


This example uses the PSA Crypto API to perform ECDSA and EdDSA digital signature on the supported device.


In this example, the PSA Crypto API is used to generate a private-public key pair using a given Elliptic Curve Cryptography (ECC) curve. The private key is first used to sign the hash of a message buffer. The signature is then verified using the public key.


The built-in asymmetric keys can be selected for ECDSA operations if these keys are available on the supported device. The private sign key and private command keys are stored in device memory to sign the hash. The device’s public sign key and public command key in the SE OTP must match with the public keys of the private sign key and private command key to verify the signature.


The default private sign key (`rootsign-unsafe-privkey.pem`) and private command key (`cmd-unsafe-privkey.pem`) in PEM format can be found in the Windows folder below.


*C:\SiliconLabs\SimplicityStudio\v5\developer\adapter\_packs\secmgr\scripts\offline*


The public key of `rootsign-unsafe-privkey.pem` in text format is:


`X - C4AF4AC69AAB9512DB50F7A26AE5B4801183D85417E729A56DA974F4E08A562C`


`Y - DE6019DEA9411332DC1A743372D170B436238A34597C410EA177024DE20FC819`


The public key of `cmd-unsafe-privkey.pem` in text format is:


`X - B1BC6F6FA56640ED522B2EE0F5B3CF7E5D48F60BE8148F0DC08440F0A4E1DCA4`


`Y - 7C04119ED6A1BE31B7707E5F9D001A659A051003E95E1B936F05C37EA793AD63`


The public sign key or public command key can be provisioned by the **SE Manager Key Provisioning** platform example or Simplicity Commander. If the public sign key or public command key has not been provisioned on the device, the program will skip the signature verification of this public key.


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


Except for the Series 1 Cortex-M0+ device, the example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `PSA_CRYPTO_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


## Key Storage


The following key storages are supported in this example:


* Volatile plain key in RAM
* Persistent plain key in [NVM3](https://docs.silabs.com/gecko-platform/3.1/driver/api/group-nvm3)
* Volatile wrapped key in RAM (Secure Vault High only)
* Persistent wrapped key in NVM3 (Secure Vault High only)


## Elliptic Curve Key


The following elliptic curve keys are supported in this example:


##### `PSA_ECC_FAMILY_SECP_R1` (ECDSA) :


* SECP192R1 - 192-bit
* SECP256R1 - 256-bit
* SECP384R1 - 384-bit
* SECP521R1 - 521-bit


##### `PSA_ECC_FAMILY_TWISTED_EDWARDS` (EdDSA) :

* Ed25519 - 255-bit (HSE only)


## Asymmetric Signature Algorithm


The following asymmetric signature algorithms are supported in this example:


* `PSA_ALG_ECDSA(hash_alg)`
* `PSA_ALG_ECDSA_ANY`
* `PSA_ALG_PURE_EDDSA` (HSE only)

## Hash Algorithm


The following hash algorithms are supported in this example:


* `PSA_ALG_SHA_1`
* `PSA_ALG_SHA_224`
* `PSA_ALG_SHA_256`
* `PSA_ALG_SHA_384`
* `PSA_ALG_SHA_512`


## Payload Size


The following payload sizes are supported in this example:


* `MSG_SIZE`/16
* `MSG_SIZE`/4
* `MSG_SIZE`


The default `MSG_SIZE` is `4096`, it is defined in `app_process.h`.


## Built-in Key


The following built-in keys are supported in this example:


* Public sign key in the SE OTP (HSE only)
* Public command key in the SE OTP (HSE only)
* Private device key in the secure key storage (Secure Vault High only)


The built-in keys can only use for ECDSA precomputed hash operations.


## PSA Crypto API


The following PSA Crypto APIs are used in this example:


* `psa_crypto_init`
* `psa_generate_random`
* `psa_key_attributes_init`
* `psa_set_key_type`
* `psa_set_key_bits`
* `psa_set_key_usage_flags`
* `psa_set_key_algorithm`
* `psa_set_key_id`
* `psa_set_key_lifetime`
* `psa_generate_key`
* `psa_import_key`
* `psa_reset_key_attributes`
* `psa_export_public_key`
* `psa_sign_hash`
* `psa_verify_hash`
* `psa_sign_message`
* `psa_verify_message`
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
3. The PSA Crypto does not yet support software fallback on the `Ed25519` curve.
4. The HSE Secure Vault Mid devices require SE firmware v1.2.11 or higher (EFR32xG21) and v2.1.7 or higher (other HSE devices) to support hardware acceleration on `Ed25519`.
5. Change the `PERSISTENT_KEY_ID` value in `app_process.h` if this key ID had already existed in NVM3.
6. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.

## Resources


[AN1222: Production Programming of Series 2 Devices](https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf)


[AN1268: Authenticating Silicon Labs Devices Using Device Certificates](https://www.silabs.com/documents/public/application-notes/an1268-efr32-secure-identity.pdf)


[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)


