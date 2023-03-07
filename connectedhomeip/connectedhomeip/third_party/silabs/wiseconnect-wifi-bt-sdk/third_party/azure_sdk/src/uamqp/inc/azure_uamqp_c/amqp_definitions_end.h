

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_END_H
#define AMQP_DEFINITIONS_END_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct END_INSTANCE_TAG* END_HANDLE;

    MOCKABLE_FUNCTION(, END_HANDLE, end_create );
    MOCKABLE_FUNCTION(, END_HANDLE, end_clone, END_HANDLE, value);
    MOCKABLE_FUNCTION(, void, end_destroy, END_HANDLE, end);
    MOCKABLE_FUNCTION(, bool, is_end_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_end, AMQP_VALUE, value, END_HANDLE*, END_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_end, END_HANDLE, end);

    MOCKABLE_FUNCTION(, int, end_get_error, END_HANDLE, end, ERROR_HANDLE*, error_value);
    MOCKABLE_FUNCTION(, int, end_set_error, END_HANDLE, end, ERROR_HANDLE, error_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_END_H */
