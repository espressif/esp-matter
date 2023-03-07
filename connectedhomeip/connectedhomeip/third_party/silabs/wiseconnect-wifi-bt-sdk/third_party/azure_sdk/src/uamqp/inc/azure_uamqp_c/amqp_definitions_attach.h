

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_ATTACH_H
#define AMQP_DEFINITIONS_ATTACH_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct ATTACH_INSTANCE_TAG* ATTACH_HANDLE;

    MOCKABLE_FUNCTION(, ATTACH_HANDLE, attach_create , const char*, name_value, handle, handle_value, role, role_value);
    MOCKABLE_FUNCTION(, ATTACH_HANDLE, attach_clone, ATTACH_HANDLE, value);
    MOCKABLE_FUNCTION(, void, attach_destroy, ATTACH_HANDLE, attach);
    MOCKABLE_FUNCTION(, bool, is_attach_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_attach, AMQP_VALUE, value, ATTACH_HANDLE*, ATTACH_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_attach, ATTACH_HANDLE, attach);

    MOCKABLE_FUNCTION(, int, attach_get_name, ATTACH_HANDLE, attach, const char**, name_value);
    MOCKABLE_FUNCTION(, int, attach_set_name, ATTACH_HANDLE, attach, const char*, name_value);
    MOCKABLE_FUNCTION(, int, attach_get_handle, ATTACH_HANDLE, attach, handle*, handle_value);
    MOCKABLE_FUNCTION(, int, attach_set_handle, ATTACH_HANDLE, attach, handle, handle_value);
    MOCKABLE_FUNCTION(, int, attach_get_role, ATTACH_HANDLE, attach, role*, role_value);
    MOCKABLE_FUNCTION(, int, attach_set_role, ATTACH_HANDLE, attach, role, role_value);
    MOCKABLE_FUNCTION(, int, attach_get_snd_settle_mode, ATTACH_HANDLE, attach, sender_settle_mode*, snd_settle_mode_value);
    MOCKABLE_FUNCTION(, int, attach_set_snd_settle_mode, ATTACH_HANDLE, attach, sender_settle_mode, snd_settle_mode_value);
    MOCKABLE_FUNCTION(, int, attach_get_rcv_settle_mode, ATTACH_HANDLE, attach, receiver_settle_mode*, rcv_settle_mode_value);
    MOCKABLE_FUNCTION(, int, attach_set_rcv_settle_mode, ATTACH_HANDLE, attach, receiver_settle_mode, rcv_settle_mode_value);
    MOCKABLE_FUNCTION(, int, attach_get_source, ATTACH_HANDLE, attach, AMQP_VALUE*, source_value);
    MOCKABLE_FUNCTION(, int, attach_set_source, ATTACH_HANDLE, attach, AMQP_VALUE, source_value);
    MOCKABLE_FUNCTION(, int, attach_get_target, ATTACH_HANDLE, attach, AMQP_VALUE*, target_value);
    MOCKABLE_FUNCTION(, int, attach_set_target, ATTACH_HANDLE, attach, AMQP_VALUE, target_value);
    MOCKABLE_FUNCTION(, int, attach_get_unsettled, ATTACH_HANDLE, attach, AMQP_VALUE*, unsettled_value);
    MOCKABLE_FUNCTION(, int, attach_set_unsettled, ATTACH_HANDLE, attach, AMQP_VALUE, unsettled_value);
    MOCKABLE_FUNCTION(, int, attach_get_incomplete_unsettled, ATTACH_HANDLE, attach, bool*, incomplete_unsettled_value);
    MOCKABLE_FUNCTION(, int, attach_set_incomplete_unsettled, ATTACH_HANDLE, attach, bool, incomplete_unsettled_value);
    MOCKABLE_FUNCTION(, int, attach_get_initial_delivery_count, ATTACH_HANDLE, attach, sequence_no*, initial_delivery_count_value);
    MOCKABLE_FUNCTION(, int, attach_set_initial_delivery_count, ATTACH_HANDLE, attach, sequence_no, initial_delivery_count_value);
    MOCKABLE_FUNCTION(, int, attach_get_max_message_size, ATTACH_HANDLE, attach, uint64_t*, max_message_size_value);
    MOCKABLE_FUNCTION(, int, attach_set_max_message_size, ATTACH_HANDLE, attach, uint64_t, max_message_size_value);
    MOCKABLE_FUNCTION(, int, attach_get_offered_capabilities, ATTACH_HANDLE, attach, AMQP_VALUE*, offered_capabilities_value);
    MOCKABLE_FUNCTION(, int, attach_set_offered_capabilities, ATTACH_HANDLE, attach, AMQP_VALUE, offered_capabilities_value);
    MOCKABLE_FUNCTION(, int, attach_get_desired_capabilities, ATTACH_HANDLE, attach, AMQP_VALUE*, desired_capabilities_value);
    MOCKABLE_FUNCTION(, int, attach_set_desired_capabilities, ATTACH_HANDLE, attach, AMQP_VALUE, desired_capabilities_value);
    MOCKABLE_FUNCTION(, int, attach_get_properties, ATTACH_HANDLE, attach, fields*, properties_value);
    MOCKABLE_FUNCTION(, int, attach_set_properties, ATTACH_HANDLE, attach, fields, properties_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_ATTACH_H */
