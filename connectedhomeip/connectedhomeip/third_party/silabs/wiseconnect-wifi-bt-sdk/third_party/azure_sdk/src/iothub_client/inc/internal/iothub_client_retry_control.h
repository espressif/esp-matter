// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUB_CLIENT_RETRY_CONTROL
#define IOTHUB_CLIENT_RETRY_CONTROL

#include <stdlib.h>
#include <stdbool.h>
#include "azure_c_shared_utility/optionhandler.h"
#include "umock_c/umock_c_prod.h"
#include "iothub_client_core_ll.h"
#include "internal/iothubtransport.h"
#include "azure_c_shared_utility/const_defines.h"

#ifdef __cplusplus
extern "C"
{
#endif

static STATIC_VAR_UNUSED const char* RETRY_CONTROL_OPTION_INITIAL_WAIT_TIME_IN_SECS = "initial_wait_time_in_secs";
static STATIC_VAR_UNUSED const char* RETRY_CONTROL_OPTION_MAX_JITTER_PERCENT = "max_jitter_percent";
static STATIC_VAR_UNUSED const char* RETRY_CONTROL_OPTION_MAX_DELAY_IN_SECS = "max_delay_in_secs";
static STATIC_VAR_UNUSED const char* RETRY_CONTROL_OPTION_SAVED_OPTIONS = "retry_control_saved_options";

typedef enum RETRY_ACTION_TAG
{
    RETRY_ACTION_RETRY_NOW,
    RETRY_ACTION_RETRY_LATER,
    RETRY_ACTION_STOP_RETRYING
} RETRY_ACTION;

struct RETRY_CONTROL_INSTANCE_TAG;
typedef struct RETRY_CONTROL_INSTANCE_TAG* RETRY_CONTROL_HANDLE;

MOCKABLE_FUNCTION(, RETRY_CONTROL_HANDLE, retry_control_create, IOTHUB_CLIENT_RETRY_POLICY, policy, unsigned int, max_retry_time_in_secs);
MOCKABLE_FUNCTION(, int, retry_control_should_retry, RETRY_CONTROL_HANDLE, retry_control_handle, RETRY_ACTION*, retry_action);
MOCKABLE_FUNCTION(, void, retry_control_reset, RETRY_CONTROL_HANDLE, retry_control_handle);
MOCKABLE_FUNCTION(, int, retry_control_set_option, RETRY_CONTROL_HANDLE, retry_control_handle, const char*, name, const void*, value);
MOCKABLE_FUNCTION(, OPTIONHANDLER_HANDLE, retry_control_retrieve_options, RETRY_CONTROL_HANDLE, retry_control_handle);
MOCKABLE_FUNCTION(, void, retry_control_destroy, RETRY_CONTROL_HANDLE, retry_control_handle);

MOCKABLE_FUNCTION(, int, is_timeout_reached, time_t, start_time, unsigned int, timeout_in_secs, bool*, is_timed_out);

#ifdef __cplusplus
}
#endif

#endif // IOTHUB_CLIENT_RETRY_CONTROL
