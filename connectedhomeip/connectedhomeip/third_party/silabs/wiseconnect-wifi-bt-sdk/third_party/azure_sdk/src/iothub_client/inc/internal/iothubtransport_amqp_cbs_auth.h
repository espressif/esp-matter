// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef IOTHUBTRANSPORT_AMQP_CBS_AUTH_H
#define IOTHUBTRANSPORT_AMQP_CBS_AUTH_H

#include <stdint.h>
#include "internal/iothub_transport_ll_private.h"
#include "azure_uamqp_c/cbs.h"
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/optionhandler.h"

static const char* AUTHENTICATION_OPTION_SAVED_OPTIONS = "saved_authentication_options";
static const char* AUTHENTICATION_OPTION_CBS_REQUEST_TIMEOUT_SECS = "cbs_request_timeout_secs";

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum AUTHENTICATION_STATE_TAG
    {
        AUTHENTICATION_STATE_STOPPED,
        AUTHENTICATION_STATE_STARTING,
        AUTHENTICATION_STATE_STARTED,
        AUTHENTICATION_STATE_ERROR
    } AUTHENTICATION_STATE;

    typedef enum AUTHENTICATION_ERROR_TAG
    {
        AUTHENTICATION_ERROR_AUTH_TIMEOUT,
        AUTHENTICATION_ERROR_AUTH_FAILED,
        AUTHENTICATION_ERROR_SAS_REFRESH_TIMEOUT,
        AUTHENTICATION_ERROR_SAS_REFRESH_FAILED
    } AUTHENTICATION_ERROR_CODE;

    typedef void(*ON_AUTHENTICATION_STATE_CHANGED_CALLBACK)(void* context, AUTHENTICATION_STATE previous_state, AUTHENTICATION_STATE new_state);
    typedef void(*ON_AUTHENTICATION_ERROR_CALLBACK)(void* context, AUTHENTICATION_ERROR_CODE error_code);

    typedef struct AUTHENTICATION_CONFIG_TAG
    {
        const char* device_id;
        const char* module_id;
        char* iothub_host_fqdn;

        ON_AUTHENTICATION_STATE_CHANGED_CALLBACK on_state_changed_callback;
        void* on_state_changed_callback_context;

        ON_AUTHENTICATION_ERROR_CALLBACK on_error_callback;
        void* on_error_callback_context;

        IOTHUB_AUTHORIZATION_HANDLE authorization_module;                   // with either SAS Token, x509 Certs, and Device SAS Token

    } AUTHENTICATION_CONFIG;

    typedef struct AUTHENTICATION_INSTANCE* AUTHENTICATION_HANDLE;

    MOCKABLE_FUNCTION(, AUTHENTICATION_HANDLE, authentication_create, const AUTHENTICATION_CONFIG*, config);
    MOCKABLE_FUNCTION(, int, authentication_start, AUTHENTICATION_HANDLE, authentication_handle, const CBS_HANDLE, cbs_handle);
    MOCKABLE_FUNCTION(, int, authentication_stop, AUTHENTICATION_HANDLE, authentication_handle);
    MOCKABLE_FUNCTION(, void, authentication_do_work, AUTHENTICATION_HANDLE, authentication_handle);
    MOCKABLE_FUNCTION(, void, authentication_destroy, AUTHENTICATION_HANDLE, authentication_handle);
    MOCKABLE_FUNCTION(, int, authentication_set_option, AUTHENTICATION_HANDLE, authentication_handle, const char*, name, void*, value);
    MOCKABLE_FUNCTION(, OPTIONHANDLER_HANDLE, authentication_retrieve_options, AUTHENTICATION_HANDLE, authentication_handle);

#ifdef __cplusplus
}
#endif

#endif /*IOTHUBTRANSPORT_AMQP_CBS_AUTH_H*/
