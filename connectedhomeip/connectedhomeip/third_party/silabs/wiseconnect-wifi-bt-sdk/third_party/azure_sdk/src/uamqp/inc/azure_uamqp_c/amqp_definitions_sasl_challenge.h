

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_SASL_CHALLENGE_H
#define AMQP_DEFINITIONS_SASL_CHALLENGE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct SASL_CHALLENGE_INSTANCE_TAG* SASL_CHALLENGE_HANDLE;

    MOCKABLE_FUNCTION(, SASL_CHALLENGE_HANDLE, sasl_challenge_create , amqp_binary, challenge_value);
    MOCKABLE_FUNCTION(, SASL_CHALLENGE_HANDLE, sasl_challenge_clone, SASL_CHALLENGE_HANDLE, value);
    MOCKABLE_FUNCTION(, void, sasl_challenge_destroy, SASL_CHALLENGE_HANDLE, sasl_challenge);
    MOCKABLE_FUNCTION(, bool, is_sasl_challenge_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_sasl_challenge, AMQP_VALUE, value, SASL_CHALLENGE_HANDLE*, SASL_CHALLENGE_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_sasl_challenge, SASL_CHALLENGE_HANDLE, sasl_challenge);

    MOCKABLE_FUNCTION(, int, sasl_challenge_get_challenge, SASL_CHALLENGE_HANDLE, sasl_challenge, amqp_binary*, challenge_value);
    MOCKABLE_FUNCTION(, int, sasl_challenge_set_challenge, SASL_CHALLENGE_HANDLE, sasl_challenge, amqp_binary, challenge_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_SASL_CHALLENGE_H */
