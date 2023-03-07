

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_DISPOSITION_H
#define AMQP_DEFINITIONS_DISPOSITION_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct DISPOSITION_INSTANCE_TAG* DISPOSITION_HANDLE;

    MOCKABLE_FUNCTION(, DISPOSITION_HANDLE, disposition_create , role, role_value, delivery_number, first_value);
    MOCKABLE_FUNCTION(, DISPOSITION_HANDLE, disposition_clone, DISPOSITION_HANDLE, value);
    MOCKABLE_FUNCTION(, void, disposition_destroy, DISPOSITION_HANDLE, disposition);
    MOCKABLE_FUNCTION(, bool, is_disposition_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_disposition, AMQP_VALUE, value, DISPOSITION_HANDLE*, DISPOSITION_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_disposition, DISPOSITION_HANDLE, disposition);

    MOCKABLE_FUNCTION(, int, disposition_get_role, DISPOSITION_HANDLE, disposition, role*, role_value);
    MOCKABLE_FUNCTION(, int, disposition_set_role, DISPOSITION_HANDLE, disposition, role, role_value);
    MOCKABLE_FUNCTION(, int, disposition_get_first, DISPOSITION_HANDLE, disposition, delivery_number*, first_value);
    MOCKABLE_FUNCTION(, int, disposition_set_first, DISPOSITION_HANDLE, disposition, delivery_number, first_value);
    MOCKABLE_FUNCTION(, int, disposition_get_last, DISPOSITION_HANDLE, disposition, delivery_number*, last_value);
    MOCKABLE_FUNCTION(, int, disposition_set_last, DISPOSITION_HANDLE, disposition, delivery_number, last_value);
    MOCKABLE_FUNCTION(, int, disposition_get_settled, DISPOSITION_HANDLE, disposition, bool*, settled_value);
    MOCKABLE_FUNCTION(, int, disposition_set_settled, DISPOSITION_HANDLE, disposition, bool, settled_value);
    MOCKABLE_FUNCTION(, int, disposition_get_state, DISPOSITION_HANDLE, disposition, AMQP_VALUE*, state_value);
    MOCKABLE_FUNCTION(, int, disposition_set_state, DISPOSITION_HANDLE, disposition, AMQP_VALUE, state_value);
    MOCKABLE_FUNCTION(, int, disposition_get_batchable, DISPOSITION_HANDLE, disposition, bool*, batchable_value);
    MOCKABLE_FUNCTION(, int, disposition_set_batchable, DISPOSITION_HANDLE, disposition, bool, batchable_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_DISPOSITION_H */
