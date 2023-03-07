// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "azure_c_shared_utility/xio.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_umqtt_c/mqttconst.h"
#include "azure_umqtt_c/mqtt_message.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif // __cplusplus


typedef struct MQTT_CLIENT_TAG* MQTT_CLIENT_HANDLE;

#define MQTT_CLIENT_EVENT_VALUES     \
    MQTT_CLIENT_ON_CONNACK,          \
    MQTT_CLIENT_ON_PUBLISH_ACK,      \
    MQTT_CLIENT_ON_PUBLISH_RECV,     \
    MQTT_CLIENT_ON_PUBLISH_REL,      \
    MQTT_CLIENT_ON_PUBLISH_COMP,     \
    MQTT_CLIENT_ON_SUBSCRIBE_ACK,    \
    MQTT_CLIENT_ON_UNSUBSCRIBE_ACK,  \
    MQTT_CLIENT_ON_PING_RESPONSE,    \
    MQTT_CLIENT_ON_DISCONNECT

MU_DEFINE_ENUM(MQTT_CLIENT_EVENT_RESULT, MQTT_CLIENT_EVENT_VALUES);

#define MQTT_CLIENT_EVENT_ERROR_VALUES     \
    MQTT_CLIENT_CONNECTION_ERROR,          \
    MQTT_CLIENT_PARSE_ERROR,               \
    MQTT_CLIENT_MEMORY_ERROR,              \
    MQTT_CLIENT_COMMUNICATION_ERROR,       \
    MQTT_CLIENT_NO_PING_RESPONSE,          \
    MQTT_CLIENT_UNKNOWN_ERROR

MU_DEFINE_ENUM(MQTT_CLIENT_EVENT_ERROR, MQTT_CLIENT_EVENT_ERROR_VALUES);

typedef void(*ON_MQTT_OPERATION_CALLBACK)(MQTT_CLIENT_HANDLE handle, MQTT_CLIENT_EVENT_RESULT actionResult, const void* msgInfo, void* callbackCtx);
typedef void(*ON_MQTT_ERROR_CALLBACK)(MQTT_CLIENT_HANDLE handle, MQTT_CLIENT_EVENT_ERROR error, void* callbackCtx);
typedef void(*ON_MQTT_MESSAGE_RECV_CALLBACK)(MQTT_MESSAGE_HANDLE msgHandle, void* callbackCtx);
typedef void(*ON_MQTT_DISCONNECTED_CALLBACK)(void* callbackCtx);

MOCKABLE_FUNCTION(, void, mqtt_client_clear_xio, MQTT_CLIENT_HANDLE, handle);
MOCKABLE_FUNCTION(, MQTT_CLIENT_HANDLE, mqtt_client_init, ON_MQTT_MESSAGE_RECV_CALLBACK, msgRecv, ON_MQTT_OPERATION_CALLBACK, opCallback, void*, opCallbackCtx, ON_MQTT_ERROR_CALLBACK, onErrorCallBack, void*, errorCBCtx);
MOCKABLE_FUNCTION(, void, mqtt_client_deinit, MQTT_CLIENT_HANDLE, handle);

MOCKABLE_FUNCTION(, int, mqtt_client_connect, MQTT_CLIENT_HANDLE, handle, XIO_HANDLE, xioHandle, MQTT_CLIENT_OPTIONS*, mqttOptions);
MOCKABLE_FUNCTION(, int, mqtt_client_disconnect, MQTT_CLIENT_HANDLE, handle, ON_MQTT_DISCONNECTED_CALLBACK, callback, void*, ctx);

MOCKABLE_FUNCTION(, int, mqtt_client_subscribe, MQTT_CLIENT_HANDLE, handle, uint16_t, packetId, SUBSCRIBE_PAYLOAD*, subscribeList, size_t, count);
MOCKABLE_FUNCTION(, int, mqtt_client_unsubscribe, MQTT_CLIENT_HANDLE, handle, uint16_t, packetId, const char**, unsubscribeList, size_t, count);

MOCKABLE_FUNCTION(, int, mqtt_client_publish, MQTT_CLIENT_HANDLE, handle, MQTT_MESSAGE_HANDLE, msgHandle);

MOCKABLE_FUNCTION(, void, mqtt_client_dowork, MQTT_CLIENT_HANDLE, handle);

MOCKABLE_FUNCTION(, void, mqtt_client_set_trace, MQTT_CLIENT_HANDLE, handle, bool, traceOn, bool, rawBytesOn);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MQTTCLIENT_H
