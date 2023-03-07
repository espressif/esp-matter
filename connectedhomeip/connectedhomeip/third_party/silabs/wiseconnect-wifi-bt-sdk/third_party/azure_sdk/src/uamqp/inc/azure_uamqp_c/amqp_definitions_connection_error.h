

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_CONNECTION_ERROR_H
#define AMQP_DEFINITIONS_CONNECTION_ERROR_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef const char* connection_error;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_connection_error, connection_error, value);


    #define amqpvalue_get_connection_error amqpvalue_get_symbol

    #define connection_error_connection_forced "amqp:connection:forced"
    #define connection_error_framing_error "amqp:connection:framing-error"
    #define connection_error_redirect "amqp:connection:redirect"


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_CONNECTION_ERROR_H */
