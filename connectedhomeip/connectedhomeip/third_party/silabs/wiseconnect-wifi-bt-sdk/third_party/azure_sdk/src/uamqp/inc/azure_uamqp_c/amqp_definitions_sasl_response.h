

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SASL_RESPONSE_H
#define AMQP_DEFINITIONS_SASL_RESPONSE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct SASL_RESPONSE_INSTANCE_TAG* SASL_RESPONSE_HANDLE;

    MOCKABLE_FUNCTION(, SASL_RESPONSE_HANDLE, sasl_response_create , amqp_binary, response_value);
    MOCKABLE_FUNCTION(, SASL_RESPONSE_HANDLE, sasl_response_clone, SASL_RESPONSE_HANDLE, value);
    MOCKABLE_FUNCTION(, void, sasl_response_destroy, SASL_RESPONSE_HANDLE, sasl_response);
    MOCKABLE_FUNCTION(, bool, is_sasl_response_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_sasl_response, AMQP_VALUE, value, SASL_RESPONSE_HANDLE*, SASL_RESPONSE_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sasl_response, SASL_RESPONSE_HANDLE, sasl_response);

    MOCKABLE_FUNCTION(, int, sasl_response_get_response, SASL_RESPONSE_HANDLE, sasl_response, amqp_binary*, response_value);
    MOCKABLE_FUNCTION(, int, sasl_response_set_response, SASL_RESPONSE_HANDLE, sasl_response, amqp_binary, response_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SASL_RESPONSE_H */
