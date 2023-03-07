// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UAMQP_MESSAGING_H
#define UAMQP_MESSAGING_H

#include "iothub_message.h"
#include "azure_uamqp_c/message.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C"
{
#endif

    MOCKABLE_FUNCTION(, int, message_create_IoTHubMessage_from_uamqp_message, MESSAGE_HANDLE, uamqp_message, IOTHUB_MESSAGE_HANDLE*, iothubclient_message);
    MOCKABLE_FUNCTION(, int, message_create_uamqp_encoding_from_iothub_message, MESSAGE_HANDLE, message_batch_container, IOTHUB_MESSAGE_HANDLE, message_handle, BINARY_DATA*, body_binary_data);

#ifdef __cplusplus
}
#endif

#endif /*UAMQP_MESSAGING_H*/
