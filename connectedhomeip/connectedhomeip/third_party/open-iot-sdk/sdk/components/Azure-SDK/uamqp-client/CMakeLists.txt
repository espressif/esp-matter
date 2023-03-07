# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

if(azure-sdk_POPULATED)
    add_library(azure-uamqp-client EXCLUDE_FROM_ALL
        ${azure-sdk_SOURCE_DIR}/uamqp/src/amqp_definitions.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/amqp_frame_codec.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/amqp_management.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/amqpvalue.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/amqpvalue_to_string.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/async_operation.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/cbs.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/connection.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/frame_codec.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/header_detect_io.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/link.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/message.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/message_receiver.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/message_sender.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/messaging.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/sasl_anonymous.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/sasl_frame_codec.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/sasl_mechanism.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/sasl_mssbcbs.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/sasl_plain.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/sasl_server_mechanism.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/saslclientio.c
        ${azure-sdk_SOURCE_DIR}/uamqp/src/session.c
    )

    target_include_directories(azure-uamqp-client
        PUBLIC
            ${azure-sdk_SOURCE_DIR}/uamqp/inc
    )

    target_link_libraries(azure-uamqp-client
        PUBLIC
            azure-c-shared-utility
            azure-macro-utils
    )
endif()
