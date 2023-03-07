# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# Mbed TLS library configuration
# Programs and testing builds are not required
set(ENABLE_PROGRAMS OFF)
set(ENABLE_TESTING OFF)

include(./mbedtls-fetch.cmake)

# mbedtls-config library is used to supply configuration for MbedTLS
# IMPORTANT: user applications must provide a "mbedtls_config.h" file which can be an empty
# file or contain MbedTLS options the user wants to override. This can be done by adding
# an include directory to mbed-config that contains the file.
add_library(mbedtls-config INTERFACE)

if(TARGET cmsis-rtos-api)
    add_library(mbedtls-threading-cmsis-rtos
        src/mbedtls_threading_cmsis_rtos.c
    )

    target_include_directories(mbedtls-threading-cmsis-rtos
        PUBLIC
            include
    )

    target_link_libraries(mbedtls-threading-cmsis-rtos
        PUBLIC
            mbedtls
            cmsis-rtos-api
    )
endif()

target_link_libraries(mbedtls
    PUBLIC
        mbedtls-config
)

target_link_libraries(mbedx509
    PUBLIC
        mbedtls-config
)

target_link_libraries(mbedcrypto
    PUBLIC
        mbedtls-config
)
