

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_ACCEPTED_H
#define AMQP_DEFINITIONS_ACCEPTED_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct ACCEPTED_INSTANCE_TAG* ACCEPTED_HANDLE;

    MOCKABLE_FUNCTION(, ACCEPTED_HANDLE, accepted_create );
    MOCKABLE_FUNCTION(, ACCEPTED_HANDLE, accepted_clone, ACCEPTED_HANDLE, value);
    MOCKABLE_FUNCTION(, void, accepted_destroy, ACCEPTED_HANDLE, accepted);
    MOCKABLE_FUNCTION(, bool, is_accepted_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_accepted, AMQP_VALUE, value, ACCEPTED_HANDLE*, ACCEPTED_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_accepted, ACCEPTED_HANDLE, accepted);



#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_ACCEPTED_H */
