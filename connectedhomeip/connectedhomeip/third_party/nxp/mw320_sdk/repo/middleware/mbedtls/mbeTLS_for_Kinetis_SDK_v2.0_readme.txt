mbedTLS_for_Kinetis_SDK_v2.0

Content
-------
1. Installation.
2. mbedTLS Add-on for Kinetis SDK 2.0 directory structure.
3. Try examples with mbedTLS.
4. How to add mbedTLS support to user applications.
5. Release Notes


1. Installation.
----------------
1.1. Install Kinetis SDK 2.0 package
1.2. Copy content of the mbedTLS add-on to the root directory of the Kinetis SDK 2.0 installation
     The package does not overwrite or modify any Kinetis SDK 2.0 file, just adds new ones.

2. mbedTLS_for_Kinetis_SDK_v2.0 directory structure.
----------------------------------------------------------
  mbedTLS_for_Kinetis_SDK_v2.0
  |-- boards
  |   -- <board>                           - see 3. Try examples with mbedTLS.
  |      -- demo_apps
  |         -- mbedtls
  |            -- mbedtls_benchmark        - Measure throughput of various crypto algorithms.
  |            -- mbedtls_selftest         - Tests right output from various crypto algorithms.
  |-- middleware
  |   -- mbedtls_<version>                     - mbedTLS library source code.
  |      -- include
  |         -- mbedtls
  |            -- *.h
  |      -- library
  |         -- *.c
  |      -- port
  |         -- ksdk                        - KSDK mbedTLS porting files.
  |            -- *.c, *h 
    
3. Try examples with mbedTLS.
-----------------------------
The package contains two demo applications: mbedtls_benchmark and mbedtls_selftest.
The build projects can be found in boards/<board>/demo_apps/mbedtls/mbedtls_selftest and 
boards/<board>/demo_apps/mbedtls/mbedtls_benchmark.
The following boards are enabled out of the box: frdmk64f, frdmk66f, frdmk82f, twrk21f120m, 
twrk64f120m, twrk65f180m, twrk80f150m, twrk81f150m.
As mbedTLS contains software implementation of all algorithms, it could be used on any devices
with enough FLASH and RAM size (see section 4 of this readme). 

4. How to add mbedTLS support to user applications.
---------------------------------------------------
4.1. Add "middleware/mbedtls_<version>/mbedtls" to compiler include path settings.
4.2. Add source files to the application build project.
4.3. Review and possibly configure mbedTLS configuration file "middleware/mbetls_<version>/port/ksdk_mbedtls_config.h"
     according to the application needs. Hardware crypto acceleration can be enabled or disabled
     at compile time in this file. Be sure that MBEDTLS_CONFIG_FILE definition is set to "ksdk_mbedtls_config.h".
4.4. If you're using hardware acceleration for crypto algorithms (such as Kinetis MMCAU or LTC modules)
     make sure their driver is also included in the application build project.
     For MMCAU acceleration, add "middleware/mmcau_<version>" to compiler include path settings
     and add "middleware/mmcau_<version>/asm-<core>/lib_mmcau.a" to the linker settings.
     For LTC acceleration, add "devices/<your_device>/drivers/fsl_ltc.h"
     and "devices/<your_device>/drivers/fsl_ltc.c" to your application build project.

5. Release notes.
-----------------
     The middleware/mbedtls_<version> library is based on the mbedTLS package from https://tls.mbed.org/.
     The original mbedTLS change log can be found in the middleware/mbedtls_<version>/ChangeLog file.
     The KSDK mbedTLS change log can be found in the middleware/mbedtls_<version>/ChangeLogKSDK file.
     Main modifications introduced to the original library source code:
       * Added support for Kinetis LTC and MMCAU hardware acceleration modules.
       * Added support for Kinetis TRNG and RNGA random number generator modules.   

