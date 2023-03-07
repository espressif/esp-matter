

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_BEGIN_H
#define AMQP_DEFINITIONS_BEGIN_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct BEGIN_INSTANCE_TAG* BEGIN_HANDLE;

    MOCKABLE_FUNCTION(, BEGIN_HANDLE, begin_create , transfer_number, next_outgoing_id_value, uint32_t, incoming_window_value, uint32_t, outgoing_window_value);
    MOCKABLE_FUNCTION(, BEGIN_HANDLE, begin_clone, BEGIN_HANDLE, value);
    MOCKABLE_FUNCTION(, void, begin_destroy, BEGIN_HANDLE, begin);
    MOCKABLE_FUNCTION(, bool, is_begin_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_begin, AMQP_VALUE, value, BEGIN_HANDLE*, BEGIN_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_begin, BEGIN_HANDLE, begin);

    MOCKABLE_FUNCTION(, int, begin_get_remote_channel, BEGIN_HANDLE, begin, uint16_t*, remote_channel_value);
    MOCKABLE_FUNCTION(, int, begin_set_remote_channel, BEGIN_HANDLE, begin, uint16_t, remote_channel_value);
    MOCKABLE_FUNCTION(, int, begin_get_next_outgoing_id, BEGIN_HANDLE, begin, transfer_number*, next_outgoing_id_value);
    MOCKABLE_FUNCTION(, int, begin_set_next_outgoing_id, BEGIN_HANDLE, begin, transfer_number, next_outgoing_id_value);
    MOCKABLE_FUNCTION(, int, begin_get_incoming_window, BEGIN_HANDLE, begin, uint32_t*, incoming_window_value);
    MOCKABLE_FUNCTION(, int, begin_set_incoming_window, BEGIN_HANDLE, begin, uint32_t, incoming_window_value);
    MOCKABLE_FUNCTION(, int, begin_get_outgoing_window, BEGIN_HANDLE, begin, uint32_t*, outgoing_window_value);
    MOCKABLE_FUNCTION(, int, begin_set_outgoing_window, BEGIN_HANDLE, begin, uint32_t, outgoing_window_value);
    MOCKABLE_FUNCTION(, int, begin_get_handle_max, BEGIN_HANDLE, begin, handle*, handle_max_value);
    MOCKABLE_FUNCTION(, int, begin_set_handle_max, BEGIN_HANDLE, begin, handle, handle_max_value);
    MOCKABLE_FUNCTION(, int, begin_get_offered_capabilities, BEGIN_HANDLE, begin, AMQP_VALUE*, offered_capabilities_value);
    MOCKABLE_FUNCTION(, int, begin_set_offered_capabilities, BEGIN_HANDLE, begin, AMQP_VALUE, offered_capabilities_value);
    MOCKABLE_FUNCTION(, int, begin_get_desired_capabilities, BEGIN_HANDLE, begin, AMQP_VALUE*, desired_capabilities_value);
    MOCKABLE_FUNCTION(, int, begin_set_desired_capabilities, BEGIN_HANDLE, begin, AMQP_VALUE, desired_capabilities_value);
    MOCKABLE_FUNCTION(, int, begin_get_properties, BEGIN_HANDLE, begin, fields*, properties_value);
    MOCKABLE_FUNCTION(, int, begin_set_properties, BEGIN_HANDLE, begin, fields, properties_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_BEGIN_H */
