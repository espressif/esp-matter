# Mbed TLS

Adds support for mbedtls v2.26.0.

**NB:** User must provide an mbedtls configuration file. It can be an empty file or override MbedTLS default [configurations](https://tls.mbed.org/api/config_8h.html).

The configuration file specified by the application is retrieved by adding the C macro `MBEDTLS_CONFIG_FILE=<filename>` to the `mbedtls-config` target and its include path.

Example:

    target_include_directories(mbedtls-config
        INTERFACE
            mbedtls-config
    )

    target_compile_definitions(mbedtls-config
        INTERFACE
            MBEDTLS_CONFIG_FILE="mbedtls_config.h"
    )

### CMSIS RTOS threading support
To enable the CMSIS RTOS threading protection `#define MBEDTLS_THREADING_ALT` should be present in the user provided mbedtls configuration file.

The library *mbedtls-threading-cmsis-rtos* should be linked to the `mbedtls-config` library. The application must call `mbedtls_threading_set_cmsis_rtos()` to enable the multi threading protection.
