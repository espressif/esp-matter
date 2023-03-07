

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_FLOW_H
#define AMQP_DEFINITIONS_FLOW_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct FLOW_INSTANCE_TAG* FLOW_HANDLE;

    MOCKABLE_FUNCTION(, FLOW_HANDLE, flow_create , uint32_t, incoming_window_value, transfer_number, next_outgoing_id_value, uint32_t, outgoing_window_value);
    MOCKABLE_FUNCTION(, FLOW_HANDLE, flow_clone, FLOW_HANDLE, value);
    MOCKABLE_FUNCTION(, void, flow_destroy, FLOW_HANDLE, flow);
    MOCKABLE_FUNCTION(, bool, is_flow_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_flow, AMQP_VALUE, value, FLOW_HANDLE*, FLOW_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_flow, FLOW_HANDLE, flow);

    MOCKABLE_FUNCTION(, int, flow_get_next_incoming_id, FLOW_HANDLE, flow, transfer_number*, next_incoming_id_value);
    MOCKABLE_FUNCTION(, int, flow_set_next_incoming_id, FLOW_HANDLE, flow, transfer_number, next_incoming_id_value);
    MOCKABLE_FUNCTION(, int, flow_get_incoming_window, FLOW_HANDLE, flow, uint32_t*, incoming_window_value);
    MOCKABLE_FUNCTION(, int, flow_set_incoming_window, FLOW_HANDLE, flow, uint32_t, incoming_window_value);
    MOCKABLE_FUNCTION(, int, flow_get_next_outgoing_id, FLOW_HANDLE, flow, transfer_number*, next_outgoing_id_value);
    MOCKABLE_FUNCTION(, int, flow_set_next_outgoing_id, FLOW_HANDLE, flow, transfer_number, next_outgoing_id_value);
    MOCKABLE_FUNCTION(, int, flow_get_outgoing_window, FLOW_HANDLE, flow, uint32_t*, outgoing_window_value);
    MOCKABLE_FUNCTION(, int, flow_set_outgoing_window, FLOW_HANDLE, flow, uint32_t, outgoing_window_value);
    MOCKABLE_FUNCTION(, int, flow_get_handle, FLOW_HANDLE, flow, handle*, handle_value);
    MOCKABLE_FUNCTION(, int, flow_set_handle, FLOW_HANDLE, flow, handle, handle_value);
    MOCKABLE_FUNCTION(, int, flow_get_delivery_count, FLOW_HANDLE, flow, sequence_no*, delivery_count_value);
    MOCKABLE_FUNCTION(, int, flow_set_delivery_count, FLOW_HANDLE, flow, sequence_no, delivery_count_value);
    MOCKABLE_FUNCTION(, int, flow_get_link_credit, FLOW_HANDLE, flow, uint32_t*, link_credit_value);
    MOCKABLE_FUNCTION(, int, flow_set_link_credit, FLOW_HANDLE, flow, uint32_t, link_credit_value);
    MOCKABLE_FUNCTION(, int, flow_get_available, FLOW_HANDLE, flow, uint32_t*, available_value);
    MOCKABLE_FUNCTION(, int, flow_set_available, FLOW_HANDLE, flow, uint32_t, available_value);
    MOCKABLE_FUNCTION(, int, flow_get_drain, FLOW_HANDLE, flow, bool*, drain_value);
    MOCKABLE_FUNCTION(, int, flow_set_drain, FLOW_HANDLE, flow, bool, drain_value);
    MOCKABLE_FUNCTION(, int, flow_get_echo, FLOW_HANDLE, flow, bool*, echo_value);
    MOCKABLE_FUNCTION(, int, flow_set_echo, FLOW_HANDLE, flow, bool, echo_value);
    MOCKABLE_FUNCTION(, int, flow_get_properties, FLOW_HANDLE, flow, fields*, properties_value);
    MOCKABLE_FUNCTION(, int, flow_set_properties, FLOW_HANDLE, flow, fields, properties_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_FLOW_H */
