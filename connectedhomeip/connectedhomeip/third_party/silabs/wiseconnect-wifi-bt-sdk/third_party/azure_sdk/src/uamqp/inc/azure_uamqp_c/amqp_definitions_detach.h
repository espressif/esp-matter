

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_DETACH_H
#define AMQP_DEFINITIONS_DETACH_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct DETACH_INSTANCE_TAG* DETACH_HANDLE;

    MOCKABLE_FUNCTION(, DETACH_HANDLE, detach_create , handle, handle_value);
    MOCKABLE_FUNCTION(, DETACH_HANDLE, detach_clone, DETACH_HANDLE, value);
    MOCKABLE_FUNCTION(, void, detach_destroy, DETACH_HANDLE, detach);
    MOCKABLE_FUNCTION(, bool, is_detach_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_detach, AMQP_VALUE, value, DETACH_HANDLE*, DETACH_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_detach, DETACH_HANDLE, detach);

    MOCKABLE_FUNCTION(, int, detach_get_handle, DETACH_HANDLE, detach, handle*, handle_value);
    MOCKABLE_FUNCTION(, int, detach_set_handle, DETACH_HANDLE, detach, handle, handle_value);
    MOCKABLE_FUNCTION(, int, detach_get_closed, DETACH_HANDLE, detach, bool*, closed_value);
    MOCKABLE_FUNCTION(, int, detach_set_closed, DETACH_HANDLE, detach, bool, closed_value);
    MOCKABLE_FUNCTION(, int, detach_get_error, DETACH_HANDLE, detach, ERROR_HANDLE*, error_value);
    MOCKABLE_FUNCTION(, int, detach_set_error, DETACH_HANDLE, detach, ERROR_HANDLE, error_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_DETACH_H */
