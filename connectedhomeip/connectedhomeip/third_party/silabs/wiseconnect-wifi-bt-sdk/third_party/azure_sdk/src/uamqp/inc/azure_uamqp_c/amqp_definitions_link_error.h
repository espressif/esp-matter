

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_LINK_ERROR_H
#define AMQP_DEFINITIONS_LINK_ERROR_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef const char* link_error;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_link_error, link_error, value);


    #define amqpvalue_get_link_error amqpvalue_get_symbol

    #define link_error_detach_forced "amqp:link:detach-forced"
    #define link_error_transfer_limit_exceeded "amqp:link:transfer-limit-exceeded"
    #define link_error_message_size_exceeded "amqp:link:message-size-exceeded"
    #define link_error_redirect "amqp:link:redirect"
    #define link_error_stolen "amqp:link:stolen"


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_LINK_ERROR_H */
