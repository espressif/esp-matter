# What is this project

The Amazon FreeRTOS PKCS#11 to PSA shim layer provides a reference implementation of PKCS#11 API based on PSA API.

PSA is Platform Security Architecture which is initiated by Arm. Please get the details from this [link](https://www.arm.com/why-arm/architecture/platform-security-architecture).

Amazon FreeRTOS uses TLS (Transport Layer Security) to secure the connection between the devices and AWS IoT Cloud.
PKCS#11 (Public Key Cryptography Standards) provides some security functions to TLS in Amazon FreeRTOS.
For example, objective storage and signing.

The original PKCS#11 API implementation in Amazon FreeRTOS is based on [mbedTLS](https://github.com/ARMmbed/mbedtls).
This project is the PSA based PKCS#11 API implementation. This is done by adding the shim layer between these two API sets.

In general, this shim layer maps the PKCS#11 APIs to PSA Cryptography and Storage APIs. It follows the PSA Cryptography API V1.0 beta3 and PSA Storage API V1.0. Certificate objects and key objects are protected by PSA secure service. By default, the device private/public keys are persistent while the code verify key is volatile.

# License

Please check the **License.md** file in this project root folder for the detailed license information.

Unless stated otherwise, the software is provided under the [MIT License](https://spdx.org/licenses/MIT.html).

This project contains code derived from [mbedTLS](https://github.com/ARMmbed/mbedtls) local copy in [Amazon FreeRTOS project](https://github.com/aws/amazon-freertos). This code, including modifications, is provided under the [Apache-2.0](https://spdx.org/licenses/Apache-2.0.html) license.

For details:
- `iot_pkcs11_psa_input_format.h` and `iot_pkcs11_psa_input_format.c` are derivatives of
amazon-freertos\libraries\3rdparty\mbedtls\library\pkparse.c (amazon-freertos commit 74875b1d2)

# Integration guide

## Integrate PSA shim layer with the FreeRTOS project

In Amazon FreeRTOS, this shim layer is cloned into `libraries/abstractions/pkcs11/psa` folder by Git Submodule. To replace the default mbedTLS shim layer with PSA shim layer:

- Add all source files under `libraries/abstractions/pkcs11/psa` to the project (CMakeLists or IDE). Remove the files of other shim layers (for exmaple, the files of mbedTLS shim layer are under `libraries/abstractions/pkcs11/mbedtls`) from the project.
- Build the PSA implementation as the secure side image (check the Trusted Firmware-M example in the following section).
- Integrate the FreeRTOS project with the interface files of the PSA implementation (check the TF-M example below).
- Build the FreeRTOS project.
- Follow the platform specific instructions to sign/combine the FreeRTOS image and secure side image.

## Integrate FreeRTOS project with Trusted Firmware-M (TF-M)

[TF-M](https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/) is a PSA implementation. It implements the PSA Firmware Framework API and developer API such as Secure Storage, Cryptography, Initial Attestation, etc. Refer to [PSA website](https://developer.arm.com/architectures/security-architectures/platform-security-architecture) for more details.

Please follow the [Build instructions](https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/docs/getting_started/tfm_build_instruction.rst) of TF-M to build the secure side image for your platform.

Please check [Integration guide](https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/docs/getting_started/tfm_integration_guide.rst) of TF-M for integrating FreeRTOS project with TF-M.
