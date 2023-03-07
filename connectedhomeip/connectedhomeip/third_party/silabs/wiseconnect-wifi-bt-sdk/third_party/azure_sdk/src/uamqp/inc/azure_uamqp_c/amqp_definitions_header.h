

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_HEADER_H
#define AMQP_DEFINITIONS_HEADER_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct HEADER_INSTANCE_TAG* HEADER_HANDLE;

    MOCKABLE_FUNCTION(, HEADER_HANDLE, header_create );
    MOCKABLE_FUNCTION(, HEADER_HANDLE, header_clone, HEADER_HANDLE, value);
    MOCKABLE_FUNCTION(, void, header_destroy, HEADER_HANDLE, header);
    MOCKABLE_FUNCTION(, bool, is_header_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_header, AMQP_VALUE, value, HEADER_HANDLE*, HEADER_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_header, HEADER_HANDLE, header);

    MOCKABLE_FUNCTION(, int, header_get_durable, HEADER_HANDLE, header, bool*, durable_value);
    MOCKABLE_FUNCTION(, int, header_set_durable, HEADER_HANDLE, header, bool, durable_value);
    MOCKABLE_FUNCTION(, int, header_get_priority, HEADER_HANDLE, header, uint8_t*, priority_value);
    MOCKABLE_FUNCTION(, int, header_set_priority, HEADER_HANDLE, header, uint8_t, priority_value);
    MOCKABLE_FUNCTION(, int, header_get_ttl, HEADER_HANDLE, header, milliseconds*, ttl_value);
    MOCKABLE_FUNCTION(, int, header_set_ttl, HEADER_HANDLE, header, milliseconds, ttl_value);
    MOCKABLE_FUNCTION(, int, header_get_first_acquirer, HEADER_HANDLE, header, bool*, first_acquirer_value);
    MOCKABLE_FUNCTION(, int, header_set_first_acquirer, HEADER_HANDLE, header, bool, first_acquirer_value);
    MOCKABLE_FUNCTION(, int, header_get_delivery_count, HEADER_HANDLE, header, uint32_t*, delivery_count_value);
    MOCKABLE_FUNCTION(, int, header_set_delivery_count, HEADER_HANDLE, header, uint32_t, delivery_count_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_HEADER_H */
