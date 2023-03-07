

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_CLOSE_H
#define AMQP_DEFINITIONS_CLOSE_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct CLOSE_INSTANCE_TAG* CLOSE_HANDLE;

    MOCKABLE_FUNCTION(, CLOSE_HANDLE, close_create );
    MOCKABLE_FUNCTION(, CLOSE_HANDLE, close_clone, CLOSE_HANDLE, value);
    MOCKABLE_FUNCTION(, void, close_destroy, CLOSE_HANDLE, close);
    MOCKABLE_FUNCTION(, bool, is_close_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_close, AMQP_VALUE, value, CLOSE_HANDLE*, CLOSE_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_close, CLOSE_HANDLE, close);

    MOCKABLE_FUNCTION(, int, close_get_error, CLOSE_HANDLE, close, ERROR_HANDLE*, error_value);
    MOCKABLE_FUNCTION(, int, close_set_error, CLOSE_HANDLE, close, ERROR_HANDLE, error_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_CLOSE_H */
