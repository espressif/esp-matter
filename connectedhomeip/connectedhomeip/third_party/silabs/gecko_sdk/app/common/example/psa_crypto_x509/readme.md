# PSA Crypto X.509


This example uses the PSA Crypto API and Mbed TLS to implement the X.509 standard for certificates on the supported device.


This example uses the CRYPTO engine in the device to accelerate the X.509 API functions of Mbed TLS.


In this example, the PSA Crypto API is used to generate a private-public key pair using a given Elliptic Curve Cryptography (ECC) curve. The private key of the root certificate is first used to sign the root and device certificates. The signature in the device certificate is then verified using the public key of the root certificate.


The built-in private device key (aka attestation key) can be selected to sign the device certificate if this key is available on the supported device.


The program flow of this example (two-level certificate chain) is as follows:


* Initialize a root Certificate Signing Request (CSR) and create an ECC key pair for root CSR
* Create a root CSR in Privacy Enhanced Mail (PEM) format
* Load the root CSR and initialize a root certificate
* Set the parameters in the root certificate
* Create a root certificate (self-signed by private key in the root certificate) in PEM format
* Load the root certificate and store its Distinguished Name (DN) for the device certificate
* Initialize a device CSR and create a ECC key pair for device CSR
* Create a device CSR in PEM format
* Load the device CSR and initialize a device certificate
* Set the parameters in the device certificate
* Create a device certificate (signed by private key in the root certificate) in PEM format
* Load the device certificate to verify the root and device certificate chain


The example redirects standard I/O to the virtual serial port (VCOM) of the kit. By default, the serial port setting is 115200 bps and 8-N-1 configuration.


Except for the Series 1 Cortex-M0+ device, the example has been instrumented with code to count the number of clock cycles spent in different operations. The results are printed on the VCOM serial port console. This feature can be disabled by defining `PSA_CRYPTO_PRINT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


The certificates are printed on the VCOM serial port console. This feature can be disabled by defining `PSA_CRYPTO_PRINT_CERT=0` (default is 1) in the IDE setting (`Preprocessor->Defined symbols`).


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


## Hash Algorithm


The following hash algorithms are supported in this example:


* `MBEDTLS_MD_SHA224`
* `MBEDTLS_MD_SHA256`
* `MBEDTLS_MD_SHA384`
* `MBEDTLS_MD_SHA512`


## Built-in Key


The following built-in key is supported in this example:


* Private device key in the secure key storage (Secure Vault High only)


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
* `psa_reset_key_attributes`
* `psa_destroy_key`
* `mbedtls_psa_crypto_free`


## Mbed TLS API


The following Mbed TLS APIs are used in this example:


* `mbedtls_x509write_csr_init`
* `mbedtls_x509write_csr_set_md_alg`
* `mbedtls_x509write_csr_set_subject_name`
* `mbedtls_pk_init`
* `mbedtls_pk_setup_opaque`
* `mbedtls_x509write_csr_set_key`
* `mbedtls_x509write_csr_pem`
* `mbedtls_x509_csr_init`
* `mbedtls_x509_csr_parse`
* `mbedtls_x509_dn_gets`
* `mbedtls_x509write_crt_init`
* `mbedtls_x509write_crt_set_issuer_key`
* `mbedtls_x509write_crt_set_subject_key`
* `mbedtls_x509write_crt_set_issuer_name`
* `mbedtls_x509write_crt_set_subject_name`
* `mbedtls_x509write_crt_set_version`
* `mbedtls_x509write_crt_set_md_alg`
* `mbedtls_mpi_init`
* `mbedtls_mpi_read_string`
* `mbedtls_x509write_crt_set_serial`
* `mbedtls_x509write_crt_set_validity`
* `mbedtls_x509write_crt_set_basic_constraints` (MBEDTLS\_X509\_CRT\_VERSION\_3 only)
* `mbedtls_x509write_crt_set_key_usage` (MBEDTLS\_X509\_CRT\_VERSION\_3 only)
* `mbedtls_x509write_crt_set_ns_cert_type` (MBEDTLS\_X509\_CRT\_VERSION\_3 only)
* `mbedtls_x509write_crt_set_subject_key_identifier` (MBEDTLS\_X509\_CRT\_VERSION\_3 only)
* `mbedtls_x509write_crt_set_authority_key_identifier` (MBEDTLS\_X509\_CRT\_VERSION\_3 only)
* `mbedtls_x509write_crt_pem`
* `mbedtls_x509_crt_init`
* `mbedtls_x509_crt_parse`
* `mbedtls_x509_crt_verify`
* `mbedtls_x509write_csr_free`
* `mbedtls_x509_csr_free`
* `mbedtls_x509write_crt_free`
* `mbedtls_mpi_free`
* `mbedtls_pk_free`
* `mbedtls_x509_crt_free`


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
3. The default certificate version is `MBEDTLS_X509_CRT_VERSION_3`, it is defined in `app_mbedtls_x509.h`.
4. The error code of the Mbed TLS X.509 API is different from the PSA Crypto.
5. Change the `ROOT_KEY_ID` and `DEVICE_KEY_ID` values in `app_process.h` if these key IDs had already existed in NVM3.
6. The default optimization level is `Optimize for debugging (-Og)` on Simplicity IDE and `None` on IAR Embedded Workbench.


## Resources


[AN1268: Authenticating Silicon Labs Devices Using Device Certificates](https://www.silabs.com/documents/public/application-notes/an1268-efr32-secure-identity.pdf)


[AN1311: Integrating Crypto Functionality Using PSA Crypto Compared to Mbed TLS](https://www.silabs.com/documents/public/application-notes/an1311-mbedtls-psa-crypto-porting-guide.pdf)


