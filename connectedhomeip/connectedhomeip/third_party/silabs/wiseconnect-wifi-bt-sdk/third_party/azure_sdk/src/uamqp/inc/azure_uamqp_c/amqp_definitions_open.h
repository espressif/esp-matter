

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_OPEN_H
#define AMQP_DEFINITIONS_OPEN_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct OPEN_INSTANCE_TAG* OPEN_HANDLE;

    MOCKABLE_FUNCTION(, OPEN_HANDLE, open_create , const char*, container_id_value);
    MOCKABLE_FUNCTION(, OPEN_HANDLE, open_clone, OPEN_HANDLE, value);
    MOCKABLE_FUNCTION(, void, open_destroy, OPEN_HANDLE, open);
    MOCKABLE_FUNCTION(, bool, is_open_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_open, AMQP_VALUE, value, OPEN_HANDLE*, OPEN_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_open, OPEN_HANDLE, open);

    MOCKABLE_FUNCTION(, int, open_get_container_id, OPEN_HANDLE, open, const char**, container_id_value);
    MOCKABLE_FUNCTION(, int, open_set_container_id, OPEN_HANDLE, open, const char*, container_id_value);
    MOCKABLE_FUNCTION(, int, open_get_hostname, OPEN_HANDLE, open, const char**, hostname_value);
    MOCKABLE_FUNCTION(, int, open_set_hostname, OPEN_HANDLE, open, const char*, hostname_value);
    MOCKABLE_FUNCTION(, int, open_get_max_frame_size, OPEN_HANDLE, open, uint32_t*, max_frame_size_value);
    MOCKABLE_FUNCTION(, int, open_set_max_frame_size, OPEN_HANDLE, open, uint32_t, max_frame_size_value);
    MOCKABLE_FUNCTION(, int, open_get_channel_max, OPEN_HANDLE, open, uint16_t*, channel_max_value);
    MOCKABLE_FUNCTION(, int, open_set_channel_max, OPEN_HANDLE, open, uint16_t, channel_max_value);
    MOCKABLE_FUNCTION(, int, open_get_idle_time_out, OPEN_HANDLE, open, milliseconds*, idle_time_out_value);
    MOCKABLE_FUNCTION(, int, open_set_idle_time_out, OPEN_HANDLE, open, milliseconds, idle_time_out_value);
    MOCKABLE_FUNCTION(, int, open_get_outgoing_locales, OPEN_HANDLE, open, AMQP_VALUE*, outgoing_locales_value);
    MOCKABLE_FUNCTION(, int, open_set_outgoing_locales, OPEN_HANDLE, open, AMQP_VALUE, outgoing_locales_value);
    MOCKABLE_FUNCTION(, int, open_get_incoming_locales, OPEN_HANDLE, open, AMQP_VALUE*, incoming_locales_value);
    MOCKABLE_FUNCTION(, int, open_set_incoming_locales, OPEN_HANDLE, open, AMQP_VALUE, incoming_locales_value);
    MOCKABLE_FUNCTION(, int, open_get_offered_capabilities, OPEN_HANDLE, open, AMQP_VALUE*, offered_capabilities_value);
    MOCKABLE_FUNCTION(, int, open_set_offered_capabilities, OPEN_HANDLE, open, AMQP_VALUE, offered_capabilities_value);
    MOCKABLE_FUNCTION(, int, open_get_desired_capabilities, OPEN_HANDLE, open, AMQP_VALUE*, desired_capabilities_value);
    MOCKABLE_FUNCTION(, int, open_set_desired_capabilities, OPEN_HANDLE, open, AMQP_VALUE, desired_capabilities_value);
    MOCKABLE_FUNCTION(, int, open_get_properties, OPEN_HANDLE, open, fields*, properties_value);
    MOCKABLE_FUNCTION(, int, open_set_properties, OPEN_HANDLE, open, fields, properties_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_OPEN_H */
