

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SASL_MECHANISMS_H
#define AMQP_DEFINITIONS_SASL_MECHANISMS_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct SASL_MECHANISMS_INSTANCE_TAG* SASL_MECHANISMS_HANDLE;

    MOCKABLE_FUNCTION(, SASL_MECHANISMS_HANDLE, sasl_mechanisms_create , AMQP_VALUE, sasl_server_mechanisms_value);
    MOCKABLE_FUNCTION(, SASL_MECHANISMS_HANDLE, sasl_mechanisms_clone, SASL_MECHANISMS_HANDLE, value);
    MOCKABLE_FUNCTION(, void, sasl_mechanisms_destroy, SASL_MECHANISMS_HANDLE, sasl_mechanisms);
    MOCKABLE_FUNCTION(, bool, is_sasl_mechanisms_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_sasl_mechanisms, AMQP_VALUE, value, SASL_MECHANISMS_HANDLE*, SASL_MECHANISMS_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sasl_mechanisms, SASL_MECHANISMS_HANDLE, sasl_mechanisms);

    MOCKABLE_FUNCTION(, int, sasl_mechanisms_get_sasl_server_mechanisms, SASL_MECHANISMS_HANDLE, sasl_mechanisms, AMQP_VALUE*, sasl_server_mechanisms_value);
    MOCKABLE_FUNCTION(, int, sasl_mechanisms_set_sasl_server_mechanisms, SASL_MECHANISMS_HANDLE, sasl_mechanisms, AMQP_VALUE, sasl_server_mechanisms_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SASL_MECHANISMS_H */
