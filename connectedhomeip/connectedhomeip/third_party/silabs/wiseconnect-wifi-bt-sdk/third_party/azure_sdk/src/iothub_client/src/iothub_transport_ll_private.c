// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

#include "azure_c_shared_utility/xlogging.h"
#include "internal/iothub_transport_ll_private.h"

int IoTHub_Transport_ValidateCallbacks(const TRANSPORT_CALLBACKS_INFO* transport_cb)
{
    int result;
    if (
        transport_cb == NULL ||
        transport_cb->connection_status_cb == NULL ||
        transport_cb->msg_input_cb == NULL ||
        transport_cb->msg_cb == NULL ||
        transport_cb->send_complete_cb == NULL ||
        transport_cb->prod_info_cb == NULL ||
        transport_cb->twin_rpt_state_complete_cb == NULL ||
        transport_cb->twin_retrieve_prop_complete_cb == NULL ||
        transport_cb->method_complete_cb == NULL
        )
    {
        LogError("Failure callback function NULL");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}
