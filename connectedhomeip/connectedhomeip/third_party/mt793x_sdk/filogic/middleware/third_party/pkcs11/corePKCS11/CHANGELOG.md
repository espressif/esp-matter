# Change Log for corePKCS11 Library

## v3.0.1 (February 2021)
* Removed default `PKCS11_PAL_DestroyObject` implementation from `core_pkcs11_mbedtls.c`. [#74](https://github.com/FreeRTOS/corePKCS11/pull/74). This means that all PAL ports must implement `PKCS11_PAL_DestroyObject`.

## v3.0.0 (December 2020)
* Changed `xFindObjectWithLabelAndClass` to include a size parameter to allow the caller to specify the size of the passed in label.
* Added CBMC memory proofs for all functions
* Removed `threading_alt.h` from corePKCS11
* Restructured third party folder in order to align with other core repositories. Folders located in `corePKCS11/3rdparty` are now in `corePKCS11/source/dependency/3rdparty`.
* Updated logs and format specifiers to use standard C types.
* Added a POSIX PAL port.

## v2.0.1 (September 2020)
* Replaced *iot* prefix on files with *core* prefix.

## v2.0.0 (September 2020)
This is the first release of the corePKCS11 library in this repository.

This library is a software based implementation of the PKCS #11 specification.

* PKCS #11 library is now decoupled from the FreeRTOS-Kernel, and instead uses mutex and heap function abstractions provided by mbed TLS.
* The PKCS #11 library logging has been overhauled and is now decoupled from FreeRTOS.
* Added `PKCS11_PAL_Initialize` to `core_pkcs11_pal.h` to defer PAL layer initialization to PKCS #11 PAL.
