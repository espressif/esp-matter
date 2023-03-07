

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_AMQP_ERROR_H
#define AMQP_DEFINITIONS_AMQP_ERROR_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"


    typedef const char* amqp_error;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_amqp_error, amqp_error, value);


    #define amqpvalue_get_amqp_error amqpvalue_get_symbol

    #define amqp_error_internal_error "amqp:internal-error"
    #define amqp_error_not_found "amqp:not-found"
    #define amqp_error_unauthorized_access "amqp:unauthorized-access"
    #define amqp_error_decode_error "amqp:decode-error"
    #define amqp_error_resource_limit_exceeded "amqp:resource-limit-exceeded"
    #define amqp_error_not_allowed "amqp:not-allowed"
    #define amqp_error_invalid_field "amqp:invalid-field"
    #define amqp_error_not_implemented "amqp:not-implemented"
    #define amqp_error_resource_locked "amqp:resource-locked"
    #define amqp_error_precondition_failed "amqp:precondition-failed"
    #define amqp_error_resource_deleted "amqp:resource-deleted"
    #define amqp_error_illegal_state "amqp:illegal-state"
    #define amqp_error_frame_size_too_small "amqp:frame-size-too-small"


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_AMQP_ERROR_H */
