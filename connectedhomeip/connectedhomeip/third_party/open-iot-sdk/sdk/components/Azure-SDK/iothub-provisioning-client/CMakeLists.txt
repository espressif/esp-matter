# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    add_library(azure-provisioning-client EXCLUDE_FROM_ALL
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/prov_auth_client.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/prov_security_factory.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/iothub_auth_client.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/iothub_security_factory.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/adapters/hsm_client_key.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/adapters/hsm_client_data.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/prov_transport_mqtt_client.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/prov_transport_mqtt_common.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/prov_device_client.c
        ${azure-sdk_SOURCE_DIR}/provisioning_client/src/prov_device_ll_client.c
    )

    target_include_directories(azure-provisioning-client
        PUBLIC
            ${azure-sdk_SOURCE_DIR}/provisioning_client/inc
            ${azure-sdk_SOURCE_DIR}/provisioning_client/adapters
    )

    target_compile_definitions(azure-provisioning-client
        PRIVATE
            HSM_TYPE_SYMM_KEY
    )

    target_link_libraries(azure-provisioning-client
        PUBLIC
            azure-umqtt-client
            parson
            azure-c-shared-utility
            $<$<STREQUAL:${CMAKE_C_COMPILER_ID},GNU>:m> # libm for GCC
    )
endif()
