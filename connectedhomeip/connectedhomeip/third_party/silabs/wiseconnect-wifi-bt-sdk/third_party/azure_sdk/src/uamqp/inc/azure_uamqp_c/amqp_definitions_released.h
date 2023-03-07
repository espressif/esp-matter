

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_RELEASED_H
#define AMQP_DEFINITIONS_RELEASED_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct RELEASED_INSTANCE_TAG* RELEASED_HANDLE;

    MOCKABLE_FUNCTION(, RELEASED_HANDLE, released_create );
    MOCKABLE_FUNCTION(, RELEASED_HANDLE, released_clone, RELEASED_HANDLE, value);
    MOCKABLE_FUNCTION(, void, released_destroy, RELEASED_HANDLE, released);
    MOCKABLE_FUNCTION(, bool, is_released_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_released, AMQP_VALUE, value, RELEASED_HANDLE*, RELEASED_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_released, RELEASED_HANDLE, released);



#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_RELEASED_H */
