

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_ERROR_H
#define AMQP_DEFINITIONS_ERROR_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct ERROR_INSTANCE_TAG* ERROR_HANDLE;

    MOCKABLE_FUNCTION(, ERROR_HANDLE, error_create , const char*, condition_value);
    MOCKABLE_FUNCTION(, ERROR_HANDLE, error_clone, ERROR_HANDLE, value);
    MOCKABLE_FUNCTION(, void, error_destroy, ERROR_HANDLE, error);
    MOCKABLE_FUNCTION(, bool, is_error_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_error, AMQP_VALUE, value, ERROR_HANDLE*, ERROR_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_error, ERROR_HANDLE, error);

    MOCKABLE_FUNCTION(, int, error_get_condition, ERROR_HANDLE, error, const char**, condition_value);
    MOCKABLE_FUNCTION(, int, error_set_condition, ERROR_HANDLE, error, const char*, condition_value);
    MOCKABLE_FUNCTION(, int, error_get_description, ERROR_HANDLE, error, const char**, description_value);
    MOCKABLE_FUNCTION(, int, error_set_description, ERROR_HANDLE, error, const char*, description_value);
    MOCKABLE_FUNCTION(, int, error_get_info, ERROR_HANDLE, error, fields*, info_value);
    MOCKABLE_FUNCTION(, int, error_set_info, ERROR_HANDLE, error, fields, info_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_ERROR_H */
