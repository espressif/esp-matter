

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_REJECTED_H
#define AMQP_DEFINITIONS_REJECTED_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct REJECTED_INSTANCE_TAG* REJECTED_HANDLE;

    MOCKABLE_FUNCTION(, REJECTED_HANDLE, rejected_create );
    MOCKABLE_FUNCTION(, REJECTED_HANDLE, rejected_clone, REJECTED_HANDLE, value);
    MOCKABLE_FUNCTION(, void, rejected_destroy, REJECTED_HANDLE, rejected);
    MOCKABLE_FUNCTION(, bool, is_rejected_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_rejected, AMQP_VALUE, value, REJECTED_HANDLE*, REJECTED_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_rejected, REJECTED_HANDLE, rejected);

    MOCKABLE_FUNCTION(, int, rejected_get_error, REJECTED_HANDLE, rejected, ERROR_HANDLE*, error_value);
    MOCKABLE_FUNCTION(, int, rejected_set_error, REJECTED_HANDLE, rejected, ERROR_HANDLE, error_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_REJECTED_H */
