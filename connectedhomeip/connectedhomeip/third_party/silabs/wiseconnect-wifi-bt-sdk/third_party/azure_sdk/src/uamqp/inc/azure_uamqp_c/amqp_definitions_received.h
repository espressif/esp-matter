

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_RECEIVED_H
#define AMQP_DEFINITIONS_RECEIVED_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct RECEIVED_INSTANCE_TAG* RECEIVED_HANDLE;

    MOCKABLE_FUNCTION(, RECEIVED_HANDLE, received_create , uint32_t, section_number_value, uint64_t, section_offset_value);
    MOCKABLE_FUNCTION(, RECEIVED_HANDLE, received_clone, RECEIVED_HANDLE, value);
    MOCKABLE_FUNCTION(, void, received_destroy, RECEIVED_HANDLE, received);
    MOCKABLE_FUNCTION(, bool, is_received_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_received, AMQP_VALUE, value, RECEIVED_HANDLE*, RECEIVED_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_received, RECEIVED_HANDLE, received);

    MOCKABLE_FUNCTION(, int, received_get_section_number, RECEIVED_HANDLE, received, uint32_t*, section_number_value);
    MOCKABLE_FUNCTION(, int, received_set_section_number, RECEIVED_HANDLE, received, uint32_t, section_number_value);
    MOCKABLE_FUNCTION(, int, received_get_section_offset, RECEIVED_HANDLE, received, uint64_t*, section_offset_value);
    MOCKABLE_FUNCTION(, int, received_set_section_offset, RECEIVED_HANDLE, received, uint64_t, section_offset_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_RECEIVED_H */
