The purpose of the integration tests is to test and verify the user's system integration,
including the CryptoCell hardware and Software,
including the HAL and PAL layers required by the cc_lib.
The HAL and PAL layers are assumed to be implemented by the user for the user's system.

These integration tests use simple scenarios for verifying successful
integration of Arm's product on the customer's platform.

The user is advised to read the following:
- Chapter "Appendix C - Integration test" in the Software Integrators Manual
  document, in order to implement a subset of a function to serve as an
  abstraction layer between the integration test and the operating system of the user's choice.
- Chapter "2.2.1 Unpacking the shipment" in the Release Notes document - for
  the compilation process of the supplied code, which is as an example only.


The integration tests package was tested for the following environment:
.     MPS2+ board, deployed with Cortex M3.
.     Customized FreeRTOS
.     arm-compiler-5 5.06 compiler.

Assumptions :
1.    The user created a dedicated PAL and HAL layers, which match the user's platform.
2.    The user compiled mbedtls libraries (according to the instructions in the Release Notes).
4.    The code is extracted and compiled according to the description in the Release Notes document.
5.    The environment variables CROSS_COMPILE,KERNEL_DIR, ARCH and CORTEX are set prior to the compilation.

--------------

Copyright (c) 2001-2019, Arm Limited. All rights reserved.