// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LINK_H
#define LINK_H

#include <stddef.h>
#include "azure_c_shared_utility/tickcounter.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_uamqp_c/session.h"
#include "azure_uamqp_c/amqpvalue.h"
#include "azure_uamqp_c/async_operation.h"
#include "azure_uamqp_c/amqp_definitions_sender_settle_mode.h"
#include "azure_uamqp_c/amqp_definitions_receiver_settle_mode.h"
#include "azure_uamqp_c/amqp_definitions_fields.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

typedef struct LINK_INSTANCE_TAG* LINK_HANDLE;

#define LINK_STATE_VALUES \
    LINK_STATE_DETACHED, \
    LINK_STATE_HALF_ATTACHED_ATTACH_SENT, \
    LINK_STATE_HALF_ATTACHED_ATTACH_RECEIVED, \
    LINK_STATE_ATTACHED, \
    LINK_STATE_ERROR

MU_DEFINE_ENUM(LINK_STATE, LINK_STATE_VALUES)

#define LINK_TRANSFER_RESULT_VALUES \
    LINK_TRANSFER_ERROR, \
    LINK_TRANSFER_BUSY

MU_DEFINE_ENUM(LINK_TRANSFER_RESULT, LINK_TRANSFER_RESULT_VALUES)

#define LINK_DELIVERY_SETTLE_REASON_VALUES \
    LINK_DELIVERY_SETTLE_REASON_DISPOSITION_RECEIVED, \
    LINK_DELIVERY_SETTLE_REASON_SETTLED, \
    LINK_DELIVERY_SETTLE_REASON_NOT_DELIVERED, \
    LINK_DELIVERY_SETTLE_REASON_TIMEOUT, \
    LINK_DELIVERY_SETTLE_REASON_CANCELLED

MU_DEFINE_ENUM(LINK_DELIVERY_SETTLE_REASON, LINK_DELIVERY_SETTLE_REASON_VALUES)

typedef struct ON_LINK_DETACH_EVENT_SUBSCRIPTION_TAG* ON_LINK_DETACH_EVENT_SUBSCRIPTION_HANDLE;

typedef void(*ON_DELIVERY_SETTLED)(void* context, delivery_number delivery_no, LINK_DELIVERY_SETTLE_REASON reason, AMQP_VALUE delivery_state);
typedef AMQP_VALUE(*ON_TRANSFER_RECEIVED)(void* context, TRANSFER_HANDLE transfer, uint32_t payload_size, const unsigned char* payload_bytes);
typedef void(*ON_LINK_STATE_CHANGED)(void* context, LINK_STATE new_link_state, LINK_STATE previous_link_state);
typedef void(*ON_LINK_FLOW_ON)(void* context);
typedef void(*ON_LINK_DETACH_RECEIVED)(void* context, ERROR_HANDLE error);

MOCKABLE_FUNCTION(, LINK_HANDLE, link_create, SESSION_HANDLE, session, const char*, name, role, role, AMQP_VALUE, source, AMQP_VALUE, target);
MOCKABLE_FUNCTION(, LINK_HANDLE, link_create_from_endpoint, SESSION_HANDLE, session, LINK_ENDPOINT_HANDLE, link_endpoint, const char*, name, role, role, AMQP_VALUE, source, AMQP_VALUE, target);
MOCKABLE_FUNCTION(, void, link_destroy, LINK_HANDLE, handle);
MOCKABLE_FUNCTION(, int, link_set_snd_settle_mode, LINK_HANDLE, link, sender_settle_mode, snd_settle_mode);
MOCKABLE_FUNCTION(, int, link_get_snd_settle_mode, LINK_HANDLE, link, sender_settle_mode*, snd_settle_mode);
MOCKABLE_FUNCTION(, int, link_set_rcv_settle_mode, LINK_HANDLE, link, receiver_settle_mode, rcv_settle_mode);
MOCKABLE_FUNCTION(, int, link_get_rcv_settle_mode, LINK_HANDLE, link, receiver_settle_mode*, rcv_settle_mode);
MOCKABLE_FUNCTION(, int, link_set_initial_delivery_count, LINK_HANDLE, link, sequence_no, initial_delivery_count);
MOCKABLE_FUNCTION(, int, link_get_initial_delivery_count, LINK_HANDLE, link, sequence_no*, initial_delivery_count);
MOCKABLE_FUNCTION(, int, link_set_max_message_size, LINK_HANDLE, link, uint64_t, max_message_size);
MOCKABLE_FUNCTION(, int, link_get_max_message_size, LINK_HANDLE, link, uint64_t*, max_message_size);
MOCKABLE_FUNCTION(, int, link_get_peer_max_message_size, LINK_HANDLE, link, uint64_t*, peer_max_message_size);
MOCKABLE_FUNCTION(, int, link_set_attach_properties, LINK_HANDLE, link, fields, attach_properties);
MOCKABLE_FUNCTION(, int, link_set_max_link_credit, LINK_HANDLE, link, uint32_t, max_link_credit);
MOCKABLE_FUNCTION(, int, link_get_name, LINK_HANDLE, link, const char**, link_name);
MOCKABLE_FUNCTION(, int, link_get_received_message_id, LINK_HANDLE, link, delivery_number*, message_id);
MOCKABLE_FUNCTION(, int, link_send_disposition, LINK_HANDLE, link, delivery_number, message_number, AMQP_VALUE, delivery_state);
MOCKABLE_FUNCTION(, int, link_attach, LINK_HANDLE, link, ON_TRANSFER_RECEIVED, on_transfer_received, ON_LINK_STATE_CHANGED, on_link_state_changed, ON_LINK_FLOW_ON, on_link_flow_on, void*, callback_context);
MOCKABLE_FUNCTION(, int, link_detach, LINK_HANDLE, link, bool, close, const char*, error_condition, const char*, error_description, AMQP_VALUE, info);
MOCKABLE_FUNCTION(, ASYNC_OPERATION_HANDLE, link_transfer_async, LINK_HANDLE, handle, message_format, message_format, PAYLOAD*, payloads, size_t, payload_count, ON_DELIVERY_SETTLED, on_delivery_settled, void*, callback_context, LINK_TRANSFER_RESULT*, link_transfer_result,tickcounter_ms_t, timeout);
MOCKABLE_FUNCTION(, void, link_dowork, LINK_HANDLE, link);

MOCKABLE_FUNCTION(, ON_LINK_DETACH_EVENT_SUBSCRIPTION_HANDLE, link_subscribe_on_link_detach_received, LINK_HANDLE, link, ON_LINK_DETACH_RECEIVED, on_link_detach_received, void*, context);
MOCKABLE_FUNCTION(, void, link_unsubscribe_on_link_detach_received, ON_LINK_DETACH_EVENT_SUBSCRIPTION_HANDLE, event_subscription);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LINK_H */
