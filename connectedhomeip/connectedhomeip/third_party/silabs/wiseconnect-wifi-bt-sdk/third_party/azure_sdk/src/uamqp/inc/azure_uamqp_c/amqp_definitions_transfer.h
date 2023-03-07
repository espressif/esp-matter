

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_TRANSFER_H
#define AMQP_DEFINITIONS_TRANSFER_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "umock_c/umock_c_prod.h"

    typedef struct TRANSFER_INSTANCE_TAG* TRANSFER_HANDLE;

    MOCKABLE_FUNCTION(, TRANSFER_HANDLE, transfer_create , handle, handle_value);
    MOCKABLE_FUNCTION(, TRANSFER_HANDLE, transfer_clone, TRANSFER_HANDLE, value);
    MOCKABLE_FUNCTION(, void, transfer_destroy, TRANSFER_HANDLE, transfer);
    MOCKABLE_FUNCTION(, bool, is_transfer_type_by_descriptor, AMQP_VALUE, descriptor);
    MOCKABLE_FUNCTION(, int, amqpvalue_get_transfer, AMQP_VALUE, value, TRANSFER_HANDLE*, TRANSFER_handle);
    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_transfer, TRANSFER_HANDLE, transfer);

    MOCKABLE_FUNCTION(, int, transfer_get_handle, TRANSFER_HANDLE, transfer, handle*, handle_value);
    MOCKABLE_FUNCTION(, int, transfer_set_handle, TRANSFER_HANDLE, transfer, handle, handle_value);
    MOCKABLE_FUNCTION(, int, transfer_get_delivery_id, TRANSFER_HANDLE, transfer, delivery_number*, delivery_id_value);
    MOCKABLE_FUNCTION(, int, transfer_set_delivery_id, TRANSFER_HANDLE, transfer, delivery_number, delivery_id_value);
    MOCKABLE_FUNCTION(, int, transfer_get_delivery_tag, TRANSFER_HANDLE, transfer, delivery_tag*, delivery_tag_value);
    MOCKABLE_FUNCTION(, int, transfer_set_delivery_tag, TRANSFER_HANDLE, transfer, delivery_tag, delivery_tag_value);
    MOCKABLE_FUNCTION(, int, transfer_get_message_format, TRANSFER_HANDLE, transfer, message_format*, message_format_value);
    MOCKABLE_FUNCTION(, int, transfer_set_message_format, TRANSFER_HANDLE, transfer, message_format, message_format_value);
    MOCKABLE_FUNCTION(, int, transfer_get_settled, TRANSFER_HANDLE, transfer, bool*, settled_value);
    MOCKABLE_FUNCTION(, int, transfer_set_settled, TRANSFER_HANDLE, transfer, bool, settled_value);
    MOCKABLE_FUNCTION(, int, transfer_get_more, TRANSFER_HANDLE, transfer, bool*, more_value);
    MOCKABLE_FUNCTION(, int, transfer_set_more, TRANSFER_HANDLE, transfer, bool, more_value);
    MOCKABLE_FUNCTION(, int, transfer_get_rcv_settle_mode, TRANSFER_HANDLE, transfer, receiver_settle_mode*, rcv_settle_mode_value);
    MOCKABLE_FUNCTION(, int, transfer_set_rcv_settle_mode, TRANSFER_HANDLE, transfer, receiver_settle_mode, rcv_settle_mode_value);
    MOCKABLE_FUNCTION(, int, transfer_get_state, TRANSFER_HANDLE, transfer, AMQP_VALUE*, state_value);
    MOCKABLE_FUNCTION(, int, transfer_set_state, TRANSFER_HANDLE, transfer, AMQP_VALUE, state_value);
    MOCKABLE_FUNCTION(, int, transfer_get_resume, TRANSFER_HANDLE, transfer, bool*, resume_value);
    MOCKABLE_FUNCTION(, int, transfer_set_resume, TRANSFER_HANDLE, transfer, bool, resume_value);
    MOCKABLE_FUNCTION(, int, transfer_get_aborted, TRANSFER_HANDLE, transfer, bool*, aborted_value);
    MOCKABLE_FUNCTION(, int, transfer_set_aborted, TRANSFER_HANDLE, transfer, bool, aborted_value);
    MOCKABLE_FUNCTION(, int, transfer_get_batchable, TRANSFER_HANDLE, transfer, bool*, batchable_value);
    MOCKABLE_FUNCTION(, int, transfer_set_batchable, TRANSFER_HANDLE, transfer, bool, batchable_value);


#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_TRANSFER_H */
