

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SASL_INIT_H
#define AMQP_DEFINITIONS_SASL_INIT_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct SASL_INIT_INSTANCE_TAG* SASL_INIT_HANDLE;

    MOCKABLE_FUNCTION(, SASL_INIT_HANDLE, sasl_init_create , const char*, mechanism_value);
    MOCKABLE_FUNCTION(, SASL_INIT_HANDLE, sasl_init_clone, SASL_INIT_HANDLE, value);
    MOCKABLE_FUNCTION(, void, sasl_init_destroy, SASL_INIT_HANDLE, sasl_init);
    MOCKABLE_FUNCTION(, bool, is_sasl_init_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_sasl_init, AMQP_VALUE, value, SASL_INIT_HANDLE*, SASL_INIT_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sasl_init, SASL_INIT_HANDLE, sasl_init);

    MOCKABLE_FUNCTION(, int, sasl_init_get_mechanism, SASL_INIT_HANDLE, sasl_init, const char**, mechanism_value);
    MOCKABLE_FUNCTION(, int, sasl_init_set_mechanism, SASL_INIT_HANDLE, sasl_init, const char*, mechanism_value);
    MOCKABLE_FUNCTION(, int, sasl_init_get_initial_response, SASL_INIT_HANDLE, sasl_init, amqp_binary*, initial_response_value);
    MOCKABLE_FUNCTION(, int, sasl_init_set_initial_response, SASL_INIT_HANDLE, sasl_init, amqp_binary, initial_response_value);
    MOCKABLE_FUNCTION(, int, sasl_init_get_hostname, SASL_INIT_HANDLE, sasl_init, const char**, hostname_value);
    MOCKABLE_FUNCTION(, int, sasl_init_set_hostname, SASL_INIT_HANDLE, sasl_init, const char*, hostname_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SASL_INIT_H */
