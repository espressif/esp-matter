# Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

project(tfm-ns-interface)

include(TF-M-fetch-and-build.cmake)

# TF-M NS interface for the non-secure side

add_library(tfm-ns-interface ${tfm_ns_interface_generated})
add_dependencies(tfm-ns-interface tf-m-build)

target_include_directories(tfm-ns-interface
    PUBLIC
        ${BINARY_DIR}/generated/interface/include
        ${BINARY_DIR}/install/interface/include
)

target_link_libraries(tfm-ns-interface
    PRIVATE
        ${s_veneers_generated}
)

target_compile_definitions(tfm-ns-interface
    PUBLIC
        BL2
)

if(TFM_LIB_MODEL)
    target_include_directories(tfm-ns-interface
        PRIVATE
            ${BINARY_DIR}/install/interface/include/tfm/veneers
    )

    target_compile_definitions(tfm-ns-interface
        PRIVATE
            TFM_PARTITION_PROTECTED_STORAGE
            TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
            TFM_PARTITION_AUDIT_LOG
            TFM_PARTITION_CRYPTO
            TFM_PARTITION_INITIAL_ATTESTATION
            TFM_PARTITION_PLATFORM
    )
    if(TFM_PARTITION_FIRMWARE_UPDATE)
        target_compile_definitions(tfm-ns-interface
            PRIVATE
                TFM_PARTITION_FIRMWARE_UPDATE
        )
    endif()
endif()

if(TARGET cmsis-rtos-api)
    # OS layer for the TF-M NS interface, based on CMSIS-RTOS
    add_library(tfm-ns-interface-cmsis-rtos ${tfm_ns_interface_cmsis_rtos_api_generated})
    add_dependencies(tfm-ns-interface-cmsis-rtos tf-m-build)

    target_include_directories(tfm-ns-interface-cmsis-rtos
        PUBLIC
            ${BINARY_DIR}/lib/ext/tfm_test_repo-src/ns_interface/
            ${BINARY_DIR}/lib/ext/tfm_test_repo-src/ns_interface/ns_client_ext
    )

    target_link_libraries(tfm-ns-interface-cmsis-rtos
        PUBLIC
            tfm-ns-interface
            cmsis-rtos-api
    )
endif()

if(TARGET cmsis-rtx)
    # TrustZone support of RTX requires symbols from s_veneers.o
    target_link_libraries(cmsis-rtx
        PRIVATE
            tfm-ns-interface
            tfm-ns-interface-cmsis-rtos
    )
endif()

# Reference NS startup files from TF-M
# They are optional - an application can provide its own.

add_library(tfm-ns-startup-an552 OBJECT
    ${tf-m_SOURCE_DIR}/platform/ext/target/arm/mps3/an552/device/source/startup_an552_ns.c
)
target_include_directories(tfm-ns-startup-an552
    PRIVATE
        ${tf-m_SOURCE_DIR}/platform/include
        ${tf-m_SOURCE_DIR}/platform/ext/cmsis
        ${tf-m_SOURCE_DIR}/platform/ext/target/arm/mps3/an552/device/include
        ${tf-m_SOURCE_DIR}/platform/ext/target/arm/mps3/an552/partition
)

add_library(tfm-ns-startup-an555 OBJECT
    ${tf-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310_fvp/device/source/startup_corstone310_ns.c
)
target_include_directories(tfm-ns-startup-an555
    PRIVATE
        ${tf-m_SOURCE_DIR}/platform/include
        ${tf-m_SOURCE_DIR}/platform/ext/cmsis
        ${tf-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310_fvp/device/include
        ${tf-m_SOURCE_DIR}/platform/ext/target/arm/mps3/corstone310_fvp/partition
)
