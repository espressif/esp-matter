# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    add_library(azure-umqtt-client EXCLUDE_FROM_ALL
        ${azure-sdk_SOURCE_DIR}/umqtt/src/mqtt_client.c
        ${azure-sdk_SOURCE_DIR}/umqtt/src/mqtt_codec.c
        ${azure-sdk_SOURCE_DIR}/umqtt/src/mqtt_message.c
    )

    target_include_directories(azure-umqtt-client
        PUBLIC
            ${azure-sdk_SOURCE_DIR}/umqtt/inc
    )

    target_link_libraries(azure-umqtt-client
        PUBLIC
            azure-uamqp-client
    )
endif()
