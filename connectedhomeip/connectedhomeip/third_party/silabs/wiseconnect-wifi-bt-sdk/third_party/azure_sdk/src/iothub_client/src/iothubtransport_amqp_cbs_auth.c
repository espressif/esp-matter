// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "internal/iothubtransport_amqp_cbs_auth.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/sastoken.h"

#define RESULT_OK                                 0
#define INDEFINITE_TIME                           ((time_t)(-1))
#define SAS_TOKEN_TYPE                            "servicebus.windows.net:sastoken"
#define IOTHUB_DEVICES_PATH_FMT                   "%s/devices/%s"
#define IOTHUB_DEVICES_MODULE_PATH_FMT            "%s/devices/%s/modules/%s"
#define DEFAULT_CBS_REQUEST_TIMEOUT_SECS          UINT32_MAX
#define SAS_REFRESH_MULTIPLIER                    .8

typedef struct AUTHENTICATION_INSTANCE_TAG
{
    const char* device_id;
    const char* module_id;
    STRING_HANDLE iothub_host_fqdn;

    ON_AUTHENTICATION_STATE_CHANGED_CALLBACK on_state_changed_callback;
    void* on_state_changed_callback_context;

    ON_AUTHENTICATION_ERROR_CALLBACK on_error_callback;
    void* on_error_callback_context;

    uint64_t cbs_request_timeout_secs;

    AUTHENTICATION_STATE state;
    CBS_HANDLE cbs_handle;

    bool is_cbs_put_token_in_progress;
    bool is_sas_token_refresh_in_progress;

    time_t current_sas_token_put_time;

    // Auth module used to generating handle authorization
    // with either SAS Token, x509 Certs, and Device SAS Token
    IOTHUB_AUTHORIZATION_HANDLE authorization_module;
} AUTHENTICATION_INSTANCE;


// Helper functions:

static void update_state(AUTHENTICATION_INSTANCE* instance, AUTHENTICATION_STATE new_state)
{
    if (new_state != instance->state)
    {
        AUTHENTICATION_STATE previous_state = instance->state;
        instance->state = new_state;

        if (instance->on_state_changed_callback != NULL)
        {
            instance->on_state_changed_callback(instance->on_state_changed_callback_context, previous_state, new_state);
        }
    }
}

static void notify_error(AUTHENTICATION_INSTANCE* instance, AUTHENTICATION_ERROR_CODE error_code)
{
    if (instance->on_error_callback != NULL)
    {
        instance->on_error_callback(instance->on_error_callback_context, error_code);
    }
}

static int verify_cbs_put_token_timeout(AUTHENTICATION_INSTANCE* instance, bool* is_timed_out)
{
    int result;

    if (instance->current_sas_token_put_time == INDEFINITE_TIME)
    {
        result = MU_FAILURE;
        LogError("Failed verifying if cbs_put_token has timed out (current_sas_token_put_time is not set)");
    }
    else
    {
        time_t current_time;

        if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
        {
            result = MU_FAILURE;
            LogError("Failed verifying if cbs_put_token has timed out (get_time failed)");
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_083: [authentication_do_work() shall check for authentication timeout comparing the current time since `instance->current_sas_token_put_time` to `instance->cbs_request_timeout_secs`]
        else if ((uint64_t)get_difftime(current_time, instance->current_sas_token_put_time) >= instance->cbs_request_timeout_secs)
        {
            *is_timed_out = true;
            result = RESULT_OK;
        }
        else
        {
            *is_timed_out = false;
            result = RESULT_OK;
        }
    }

    return result;
}

static int verify_sas_token_refresh_timeout(AUTHENTICATION_INSTANCE* instance, bool* is_timed_out)
{
    int result;
    uint64_t sas_token_expiry;

    if (instance->current_sas_token_put_time == INDEFINITE_TIME)
    {
        result = MU_FAILURE;
        LogError("Failed verifying if SAS token refresh timed out (current_sas_token_put_time is not set)");
    }
    else if ((sas_token_expiry = IoTHubClient_Auth_Get_SasToken_Expiry(instance->authorization_module)) == 0)
    {
        result = MU_FAILURE;
        LogError("Failed Getting SasToken Expiry");
    }
    else
    {
        time_t current_time;
        if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
        {
            result = MU_FAILURE;
            LogError("Failed verifying if SAS token refresh timed out (get_time failed)");
        }
        else if ((uint64_t)get_difftime(current_time, instance->current_sas_token_put_time) >= (sas_token_expiry*SAS_REFRESH_MULTIPLIER))
        {
            *is_timed_out = true;
            result = RESULT_OK;
        }
        else
        {
            *is_timed_out = false;
            result = RESULT_OK;
        }
    }
    return result;
}

static STRING_HANDLE create_device_and_module_path(STRING_HANDLE iothub_host_fqdn, const char* device_id, const char* module_id)
{
    STRING_HANDLE devices_and_modules_path;

    if (module_id == NULL)
    {
        if ((devices_and_modules_path = STRING_construct_sprintf(IOTHUB_DEVICES_PATH_FMT, STRING_c_str(iothub_host_fqdn), device_id)) == NULL)
        {
            LogError("Failed creating devices_and_modules_path (STRING_new failed)");
        }
    }
    else
    {
        if ((devices_and_modules_path = STRING_construct_sprintf(IOTHUB_DEVICES_MODULE_PATH_FMT, STRING_c_str(iothub_host_fqdn), device_id, module_id)) == NULL)
        {
            LogError("Failed creating devices_and_modules_path (STRING_new failed)");
        }
    }
    return devices_and_modules_path;
}

static void on_cbs_put_token_complete_callback(void* context, CBS_OPERATION_RESULT operation_result, unsigned int status_code, const char* status_description)
{
#ifdef NO_LOGGING
    UNUSED(status_code);
    UNUSED(status_description);
#endif
    AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)context;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_095: [`instance->is_sas_token_refresh_in_progress` and `instance->is_cbs_put_token_in_progress` shall be set to FALSE]
    instance->is_cbs_put_token_in_progress = false;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_091: [If `result` is CBS_OPERATION_RESULT_OK `instance->state` shall be set to AUTHENTICATION_STATE_STARTED and `instance->on_state_changed_callback` invoked]
    if (operation_result == CBS_OPERATION_RESULT_OK)
    {
        update_state(instance, AUTHENTICATION_STATE_STARTED);
    }
    else
    {
        LogError("CBS reported status code %u, error: '%s' for put-token operation for device '%s'", status_code, status_description, instance->device_id);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_092: [If `result` is not CBS_OPERATION_RESULT_OK `instance->state` shall be set to AUTHENTICATION_STATE_ERROR and `instance->on_state_changed_callback` invoked]
        update_state(instance, AUTHENTICATION_STATE_ERROR);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_094: [If `result` is not CBS_OPERATION_RESULT_OK and `instance->is_sas_token_refresh_in_progress` is TRUE, `instance->on_error_callback`shall be invoked with AUTHENTICATION_ERROR_SAS_REFRESH_FAILED]
        if (instance->is_sas_token_refresh_in_progress)
        {
            notify_error(instance, AUTHENTICATION_ERROR_SAS_REFRESH_FAILED);
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_093: [If `result` is not CBS_OPERATION_RESULT_OK and `instance->is_sas_token_refresh_in_progress` is FALSE, `instance->on_error_callback`shall be invoked with AUTHENTICATION_ERROR_AUTH_FAILED]
        else
        {
            notify_error(instance, AUTHENTICATION_ERROR_AUTH_FAILED);
        }
    }

    instance->is_sas_token_refresh_in_progress = false;
}

static int put_SAS_token_to_cbs(AUTHENTICATION_INSTANCE* instance, STRING_HANDLE cbs_audience, const char* sas_token)
{
    int result;

    if (instance == NULL)
    {
        result = MU_FAILURE;
        LogError("Invalid AUTHENTICATION_INSTANCE");
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_043: [authentication_do_work() shall set `instance->is_cbs_put_token_in_progress` to TRUE]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_057: [authentication_do_work() shall set `instance->is_cbs_put_token_in_progress` to TRUE]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_075: [authentication_do_work() shall set `instance->is_cbs_put_token_in_progress` to TRUE]
        instance->is_cbs_put_token_in_progress = true;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_046: [The SAS token provided shall be sent to CBS using cbs_put_token(), using `servicebus.windows.net:sastoken` as token type, `devices_and_modules_path` as audience and passing on_cbs_put_token_complete_callback]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_058: [The SAS token shall be sent to CBS using cbs_put_token(), using `servicebus.windows.net:sastoken` as token type, `devices_and_modules_path` as audience and passing on_cbs_put_token_complete_callback]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_076: [The SAS token shall be sent to CBS using cbs_put_token(), using `servicebus.windows.net:sastoken` as token type, `devices_and_modules_path` as audience and passing on_cbs_put_token_complete_callback]
        const char* cbs_audience_c_str = STRING_c_str(cbs_audience);
        if (cbs_put_token_async(instance->cbs_handle, SAS_TOKEN_TYPE, cbs_audience_c_str, sas_token, on_cbs_put_token_complete_callback, instance) != RESULT_OK)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_048: [If cbs_put_token() failed, authentication_do_work() shall set `instance->is_cbs_put_token_in_progress` to FALSE, destroy `devices_and_modules_path` and return]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_060: [If cbs_put_token() fails, `instance->is_cbs_put_token_in_progress` shall be set to FALSE]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_078: [If cbs_put_token() fails, `instance->is_cbs_put_token_in_progress` shall be set to FALSE]
            instance->is_cbs_put_token_in_progress = false;
            result = MU_FAILURE;
            LogError("Failed putting SAS token to CBS for device '%s' (cbs_put_token failed)", instance->device_id);
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_047: [If cbs_put_token() succeeds, authentication_do_work() shall set `instance->current_sas_token_put_time` with current time]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_059: [If cbs_put_token() succeeds, authentication_do_work() shall set `instance->current_sas_token_put_time` with current time]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_077: [If cbs_put_token() succeeds, authentication_do_work() shall set `instance->current_sas_token_put_time` with the current time]
            time_t current_time;

            if ((current_time = get_time(NULL)) == INDEFINITE_TIME)
            {
                LogError("Failed setting current_sas_token_put_time for device '%s' (get_time() failed)", instance->device_id);
            }

            instance->current_sas_token_put_time = current_time; // If it failed, fear not. `current_sas_token_put_time` shall be checked for INDEFINITE_TIME wherever it is used.

            result = RESULT_OK;
        }
    }

    return result;
}

static int create_and_put_SAS_token_to_cbs(AUTHENTICATION_INSTANCE* instance)
{
    int result;
    char* sas_token;
    STRING_HANDLE device_and_module_path;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_053: [A STRING_HANDLE, referred to as `devices_and_modules_path`, shall be created from: iothub_host_fqdn + "/devices/" + device_id (+ "/modules/" + module_id if a module)]
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_071: [A STRING_HANDLE, referred to as `devices_and_modules_path`, shall be created from: iothub_host_fqdn + "/devices/" + device_id (+ "/modules/" + module_id if a module)]
    if ((device_and_module_path = create_device_and_module_path(instance->iothub_host_fqdn, instance->device_id, instance->module_id)) == NULL)
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_054: [If `devices_and_modules_path` failed to be created, authentication_do_work() shall fail and return]
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_072: [If `devices_and_modules_path` failed to be created, authentication_do_work() shall fail and return]
        result = MU_FAILURE;
        sas_token = NULL;
        LogError("Failed creating a SAS token (create_device_and_module_path() failed)");
    }
    else
    {
        /* Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_07_001: [ authentication_do_work() shall determine what credential type is used SAS_TOKEN or DEVICE_KEY by calling IoTHubClient_Auth_Get_Credential_Type ] */
        IOTHUB_CREDENTIAL_TYPE cred_type = IoTHubClient_Auth_Get_Credential_Type(instance->authorization_module);
        if (cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY || cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_049: [authentication_do_work() shall create a SAS token using IoTHubClient_Auth_Get_SasToken, unless it has failed previously] */
            sas_token = IoTHubClient_Auth_Get_SasToken(instance->authorization_module, STRING_c_str(device_and_module_path), 0, NULL);
            if (sas_token == NULL)
            {
                LogError("failure getting sas token.");
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
        else if (cred_type == IOTHUB_CREDENTIAL_TYPE_SAS_TOKEN)
        {
            /* Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_07_002: [ If credential Type is SAS_TOKEN authentication_do_work() shall validate the sas_token, and fail if it's not valid. ] */
            SAS_TOKEN_STATUS token_status = IoTHubClient_Auth_Is_SasToken_Valid(instance->authorization_module);
            if (token_status == SAS_TOKEN_STATUS_INVALID)
            {
                LogError("sas token is invalid.");
                sas_token = NULL;
                result = MU_FAILURE;
            }
            else if (token_status == SAS_TOKEN_STATUS_FAILED)
            {
                LogError("testing Sas Token failed.");
                sas_token = NULL;
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_049: [authentication_do_work() shall create a SAS token using IoTHubClient_Auth_Get_SasToken, unless it has failed previously] */
                sas_token = IoTHubClient_Auth_Get_SasToken(instance->authorization_module, NULL, 0, NULL);
                if (sas_token == NULL)
                {
                    LogError("failure getting sas Token.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = RESULT_OK;
                }
            }
        }
        else if (cred_type == IOTHUB_CREDENTIAL_TYPE_X509 || cred_type == IOTHUB_CREDENTIAL_TYPE_X509_ECC)
        {
            sas_token = NULL;
            result = RESULT_OK;
        }
        else
        {
            LogError("failure unknown credential type found.");
            sas_token = NULL;
            result = MU_FAILURE;
        }


        if (sas_token != NULL)
        {
            if (put_SAS_token_to_cbs(instance, device_and_module_path, sas_token) != RESULT_OK)
            {
                result = MU_FAILURE;
                LogError("Failed putting SAS token to CBS");
            }
            else
            {
                result = RESULT_OK;
            }
            free(sas_token);
        }

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_081: [authentication_do_work() shall free the memory it allocated for `devices_and_modules_path`, `sasTokenKeyName` and SAS token]
        STRING_delete(device_and_module_path);
    }
    return result;
}

// ---------- Set/Retrieve Options Helpers ----------//
static void* authentication_clone_option(const char* name, const void* value)
{
    void* result;

    if (name == NULL)
    {
        LogError("Failed to clone authentication option (name is NULL)");
        result = NULL;
    }
    else if (value == NULL)
    {
        LogError("Failed to clone authentication option (value is NULL)");
        result = NULL;
    }
    else
    {
        if (strcmp(AUTHENTICATION_OPTION_CBS_REQUEST_TIMEOUT_SECS, name) == 0 ||
            strcmp(AUTHENTICATION_OPTION_SAVED_OPTIONS, name) == 0)
        {
            result = (void*)value;
        }
        else
        {
            LogError("Failed to clone authentication option (option with name '%s' is not suppported)", name);
            result = NULL;
        }
    }

    return result;
}

static void authentication_destroy_option(const char* name, const void* value)
{
    if (name == NULL)
    {
        LogError("Failed to destroy authentication option (name is NULL)");
    }
    else if (value == NULL)
    {
        LogError("Failed to destroy authentication option (value is NULL)");
    }
    else
    {
        if (strcmp(name, AUTHENTICATION_OPTION_SAVED_OPTIONS) == 0)
        {
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
    }
}

// Public APIs:
int authentication_start(AUTHENTICATION_HANDLE authentication_handle, const CBS_HANDLE cbs_handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_025: [If authentication_handle is NULL, authentication_start() shall fail and return MU_FAILURE as error code]
    if (authentication_handle == NULL)
    {
        result = MU_FAILURE;
        LogError("authentication_start failed (authentication_handle is NULL)");
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_026: [If `cbs_handle` is NULL, authentication_start() shall fail and return MU_FAILURE as error code]
    else if (cbs_handle == NULL)
    {
        result = MU_FAILURE;
        LogError("authentication_start failed (cbs_handle is NULL)");
    }
    else
    {
        AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)authentication_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_027: [If authenticate state has been started already, authentication_start() shall fail and return MU_FAILURE as error code]
        if (instance->state != AUTHENTICATION_STATE_STOPPED)
        {
            result = MU_FAILURE;
            LogError("authentication_start failed (messenger has already been started; current state: %d)", instance->state);
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_028: [authentication_start() shall save `cbs_handle` on `instance->cbs_handle`]
            instance->cbs_handle = cbs_handle;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_029: [If no failures occur, `instance->state` shall be set to AUTHENTICATION_STATE_STARTING and `instance->on_state_changed_callback` invoked]
            update_state(instance, AUTHENTICATION_STATE_STARTING);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_030: [If no failures occur, authentication_start() shall return 0]
            result = RESULT_OK;
        }
    }

    return result;
}

int authentication_stop(AUTHENTICATION_HANDLE authentication_handle)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_031: [If `authentication_handle` is NULL, authentication_stop() shall fail and return MU_FAILURE]
    if (authentication_handle == NULL)
    {
        result = MU_FAILURE;
        LogError("authentication_stop failed (authentication_handle is NULL)");
    }
    else
    {
        AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)authentication_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_032: [If `instance->state` is AUTHENTICATION_STATE_STOPPED, authentication_stop() shall fail and return MU_FAILURE]
        if (instance->state == AUTHENTICATION_STATE_STOPPED)
        {
            result = MU_FAILURE;
            LogError("authentication_stop failed (messenger is already stopped)");
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_033: [`instance->cbs_handle` shall be set to NULL]
            instance->cbs_handle = NULL;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_034: [`instance->state` shall be set to AUTHENTICATION_STATE_STOPPED and `instance->on_state_changed_callback` invoked]
            update_state(instance, AUTHENTICATION_STATE_STOPPED);

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_035: [authentication_stop() shall return success code 0]
            result = RESULT_OK;
        }
    }

    return result;
}

void authentication_destroy(AUTHENTICATION_HANDLE authentication_handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_106: [If authentication_handle is NULL, authentication_destroy() shall return]
    if (authentication_handle == NULL)
    {
        LogError("authentication_destroy failed (authentication_handle is NULL)");
    }
    else
    {
        AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)authentication_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_107: [If `instance->state` is AUTHENTICATION_STATE_STARTING or AUTHENTICATION_STATE_STARTED, authentication_stop() shall be invoked and its result ignored]
        if (instance->state != AUTHENTICATION_STATE_STOPPED)
        {
            (void)authentication_stop(authentication_handle);
        }
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_108: [authentication_destroy() shall destroy all resouces used by this module]
        if (instance->iothub_host_fqdn != NULL)
            STRING_delete(instance->iothub_host_fqdn);

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_108: [authentication_destroy() shall destroy all resouces used by this module]
        free(instance);
    }
}

AUTHENTICATION_HANDLE authentication_create(const AUTHENTICATION_CONFIG* config)
{
    AUTHENTICATION_HANDLE result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_001: [If parameter `config` is NULL, authentication_create() shall fail and return NULL.]
    if (config == NULL)
    {
        result = NULL;
        LogError("authentication_create failed (config is NULL)");
    }
    else if (config->authorization_module == NULL)
    {
        result = NULL;
        LogError("authentication_create failed (config->authorization_module is NULL)");
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_004: [If `config->iothub_host_fqdn` is NULL, authentication_create() shall fail and return NULL.]
    else if (config->iothub_host_fqdn == NULL)
    {
        result = NULL;
        LogError("authentication_create failed (config->iothub_host_fqdn is NULL)");
    }
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_005: [If `config->on_state_changed_callback` is NULL, authentication_create() shall fail and return NULL]
    else if (config->on_state_changed_callback == NULL)
    {
        result = NULL;
        LogError("authentication_create failed (config->on_state_changed_callback is NULL)");
    }
    else
    {
        AUTHENTICATION_INSTANCE* instance;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_006: [authentication_create() shall allocate memory for a new authenticate state structure AUTHENTICATION_INSTANCE.]
        if ((instance = (AUTHENTICATION_INSTANCE*)malloc(sizeof(AUTHENTICATION_INSTANCE))) == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_007: [If malloc() fails, authentication_create() shall fail and return NULL.]
            result = NULL;
            LogError("authentication_create failed (malloc failed)");
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_123: [authentication_create() shall initialize all fields of `instance` with 0 using memset().]
            memset(instance, 0, sizeof(AUTHENTICATION_INSTANCE));

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_008: [authentication_create() shall save the device_id into the `instance->device_id`]
            if ((instance->device_id = IoTHubClient_Auth_Get_DeviceId(config->authorization_module) ) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_009: [If STRING_construct() fails, authentication_create() shall fail and return NULL]
                result = NULL;
                LogError("authentication_create failed (config->device_id could not be copied; STRING_construct failed)");
            }
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_016: [If provided, authentication_create() shall save a copy of `config->iothub_host_fqdn` into `instance->iothub_host_fqdn`]
            else if ((instance->iothub_host_fqdn = STRING_construct(config->iothub_host_fqdn)) == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_017: [If STRING_clone() fails to copy `config->iothub_host_fqdn`, authentication_create() shall fail and return NULL]
                result = NULL;
                LogError("authentication_create failed (config->iothub_host_fqdn could not be copied; STRING_construct failed)");
            }
            else
            {
                instance->state = AUTHENTICATION_STATE_STOPPED;

                instance->module_id = IoTHubClient_Auth_Get_ModuleId(config->authorization_module);

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_018: [authentication_create() shall save `config->on_state_changed_callback` and `config->on_state_changed_callback_context` into `instance->on_state_changed_callback` and `instance->on_state_changed_callback_context`.]
                instance->on_state_changed_callback = config->on_state_changed_callback;
                instance->on_state_changed_callback_context = config->on_state_changed_callback_context;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_019: [authentication_create() shall save `config->on_error_callback` and `config->on_error_callback_context` into `instance->on_error_callback` and `instance->on_error_callback_context`.]
                instance->on_error_callback = config->on_error_callback;
                instance->on_error_callback_context = config->on_error_callback_context;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_021: [authentication_create() shall set `instance->cbs_request_timeout_secs` with the default value of UINT32_MAX]
                instance->cbs_request_timeout_secs = DEFAULT_CBS_REQUEST_TIMEOUT_SECS;

                instance->authorization_module = config->authorization_module;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_024: [If no failure occurs, authentication_create() shall return a reference to the AUTHENTICATION_INSTANCE handle]
                result = (AUTHENTICATION_HANDLE)instance;
            }

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_020: [If any failure occurs, authentication_create() shall free any memory it allocated previously]
            if (result == NULL)
            {
                authentication_destroy((AUTHENTICATION_HANDLE)instance);
            }
        }
    }

    return result;
}

void authentication_do_work(AUTHENTICATION_HANDLE authentication_handle)
{
    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_036: [If authentication_handle is NULL, authentication_do_work() shall fail and return]
    if (authentication_handle == NULL)
    {
        LogError("authentication_do_work failed (authentication_handle is NULL)");
    }
    else
    {
        AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)authentication_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_038: [If `instance->is_cbs_put_token_in_progress` is TRUE, authentication_do_work() shall only verify the authentication timeout]
        if (instance->is_cbs_put_token_in_progress)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_084: [If no timeout has occurred, authentication_do_work() shall return]

            bool is_timed_out;
            if (verify_cbs_put_token_timeout(instance, &is_timed_out) == RESULT_OK && is_timed_out)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_085: [`instance->is_cbs_put_token_in_progress` shall be set to FALSE]
                instance->is_cbs_put_token_in_progress = false;

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_086: [`instance->state` shall be updated to AUTHENTICATION_STATE_ERROR and `instance->on_state_changed_callback` invoked]
                update_state(instance, AUTHENTICATION_STATE_ERROR);

                if (instance->is_sas_token_refresh_in_progress)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_087: [If `instance->is_sas_token_refresh_in_progress` is TRUE, `instance->on_error_callback` shall be invoked with AUTHENTICATION_ERROR_SAS_REFRESH_TIMEOUT]
                    notify_error(instance, AUTHENTICATION_ERROR_SAS_REFRESH_TIMEOUT);
                }
                else
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_088: [If `instance->is_sas_token_refresh_in_progress` is FALSE, `instance->on_error_callback` shall be invoked with AUTHENTICATION_ERROR_AUTH_TIMEOUT]
                    notify_error(instance, AUTHENTICATION_ERROR_AUTH_TIMEOUT);
                }

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_089: [`instance->is_sas_token_refresh_in_progress` shall be set to FALSE]
                instance->is_sas_token_refresh_in_progress = false;
            }
        }
        else if (instance->state == AUTHENTICATION_STATE_STARTED)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_040: [If `instance->state` is AUTHENTICATION_STATE_STARTED and user-provided SAS token was used, authentication_do_work() shall return]
            IOTHUB_CREDENTIAL_TYPE cred_type = IoTHubClient_Auth_Get_Credential_Type(instance->authorization_module);
            if (cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_KEY || cred_type == IOTHUB_CREDENTIAL_TYPE_DEVICE_AUTH)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_039: [If `instance->state` is AUTHENTICATION_STATE_STARTED and device keys were used, authentication_do_work() shall only verify the SAS token refresh time]
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_066: [If SAS token does not need to be refreshed, authentication_do_work() shall return]
                bool is_timed_out;
                if (verify_sas_token_refresh_timeout(instance, &is_timed_out) == RESULT_OK && is_timed_out)
                {
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_119: [authentication_do_work() shall set `instance->is_sas_token_refresh_in_progress` to TRUE]
                    instance->is_sas_token_refresh_in_progress = true;

                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_067: [authentication_do_work() shall create a SAS token using `instance->device_primary_key`, unless it has failed previously]
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_069: [If using `instance->device_primary_key` has failed previously, a SAS token shall be created using `instance->device_secondary_key`]
                    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_068: [If using `instance->device_primary_key` has failed previously and `instance->device_secondary_key` is not provided,  authentication_do_work() shall fail and return]
                    if (create_and_put_SAS_token_to_cbs(instance) != RESULT_OK)
                    {
                        LogError("Failed refreshing SAS token '%s'", instance->device_id);
                    }

                    if (!instance->is_cbs_put_token_in_progress)
                    {
                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_120: [If cbs_put_token() fails, `instance->is_sas_token_refresh_in_progress` shall be set to FALSE]
                        instance->is_sas_token_refresh_in_progress = false;

                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_079: [If cbs_put_token() fails, `instance->state` shall be updated to AUTHENTICATION_STATE_ERROR and `instance->on_state_changed_callback` invoked]
                        update_state(instance, AUTHENTICATION_STATE_ERROR);

                        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_080: [If cbs_put_token() fails, `instance->on_error_callback` shall be invoked with AUTHENTICATION_ERROR_SAS_REFRESH_FAILED]
                        notify_error(instance, AUTHENTICATION_ERROR_SAS_REFRESH_FAILED);
                    }
                }
            }
        }
        else if (instance->state == AUTHENTICATION_STATE_STARTING)
        {
            if (create_and_put_SAS_token_to_cbs(instance) != RESULT_OK)
            {
                LogError("Failed authenticating device '%s' using device keys", instance->device_id);
            }

            if (!instance->is_cbs_put_token_in_progress)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_061: [If cbs_put_token() fails, `instance->state` shall be updated to AUTHENTICATION_STATE_ERROR and `instance->on_state_changed_callback` invoked]
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_121: [If cbs_put_token() fails, `instance->state` shall be updated to AUTHENTICATION_STATE_ERROR and `instance->on_state_changed_callback` invoked]
                update_state(instance, AUTHENTICATION_STATE_ERROR);

                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_062: [If cbs_put_token() fails, `instance->on_error_callback` shall be invoked with AUTHENTICATION_ERROR_AUTH_FAILED]
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_122: [If cbs_put_token() fails, `instance->on_error_callback` shall be invoked with AUTHENTICATION_ERROR_AUTH_FAILED]
                notify_error(instance, AUTHENTICATION_ERROR_AUTH_FAILED);
            }
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_037: [If `instance->state` is not AUTHENTICATION_STATE_STARTING or AUTHENTICATION_STATE_STARTED, authentication_do_work() shall fail and return]
            // Nothing to be done.
        }
    }
}

int authentication_set_option(AUTHENTICATION_HANDLE authentication_handle, const char* name, void* value)
{
    int result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_097: [If `authentication_handle` or `name` or `value` is NULL, authentication_set_option shall fail and return a non-zero value]
    if (authentication_handle == NULL || name == NULL || value == NULL)
    {
        LogError("authentication_set_option failed (one of the followin are NULL: authentication_handle=%p, name=%p, value=%p)",
            authentication_handle, name, value);
        result = MU_FAILURE;
    }
    else
    {
        AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)authentication_handle;

        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_098: [If name matches AUTHENTICATION_OPTION_CBS_REQUEST_TIMEOUT_SECS, `value` shall be saved on `instance->cbs_request_timeout_secs`]
        if (strcmp(AUTHENTICATION_OPTION_CBS_REQUEST_TIMEOUT_SECS, name) == 0)
        {
            instance->cbs_request_timeout_secs = *((size_t*)value);
            result = RESULT_OK;
        }
        else if (strcmp(AUTHENTICATION_OPTION_SAVED_OPTIONS, name) == 0)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_098: [If name matches AUTHENTICATION_OPTION_SAVED_OPTIONS, `value` shall be applied using OptionHandler_FeedOptions]
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, authentication_handle) != OPTIONHANDLER_OK)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_126: [If OptionHandler_FeedOptions fails, authentication_set_option shall fail and return a non-zero value]
                LogError("authentication_set_option failed (OptionHandler_FeedOptions failed)");
                result = MU_FAILURE;
            }
            else
            {
                result = RESULT_OK;
            }
        }
        else
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_128: [If name does not match any supported option, authentication_set_option shall fail and return a non-zero value]
            LogError("authentication_set_option failed (option with name '%s' is not suppported)", name);
            result = MU_FAILURE;
        }
    }

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_099: [If no errors occur, authentication_set_option shall return 0]
    return result;
}

OPTIONHANDLER_HANDLE authentication_retrieve_options(AUTHENTICATION_HANDLE authentication_handle)
{
    OPTIONHANDLER_HANDLE result;

    // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_100: [If `authentication_handle` is NULL, authentication_retrieve_options shall fail and return NULL]
    if (authentication_handle == NULL)
    {
        LogError("Failed to retrieve options from authentication instance (authentication_handle is NULL)");
        result = NULL;
    }
    else
    {
        // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_101: [An OPTIONHANDLER_HANDLE instance shall be created using OptionHandler_Create]
        OPTIONHANDLER_HANDLE options = OptionHandler_Create(authentication_clone_option, authentication_destroy_option, (pfSetOption)authentication_set_option);

        if (options == NULL)
        {
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_102: [If an OPTIONHANDLER_HANDLE instance fails to be created, authentication_retrieve_options shall fail and return NULL]
            LogError("Failed to retrieve options from authentication instance (OptionHandler_Create failed)");
            result = NULL;
        }
        else
        {
            AUTHENTICATION_INSTANCE* instance = (AUTHENTICATION_INSTANCE*)authentication_handle;

            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_103: [Each option of `instance` shall be added to the OPTIONHANDLER_HANDLE instance using OptionHandler_AddOption]
            // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_104: [If OptionHandler_AddOption fails, authentication_retrieve_options shall fail and return NULL]
            if (OptionHandler_AddOption(options, AUTHENTICATION_OPTION_CBS_REQUEST_TIMEOUT_SECS, (void*)&instance->cbs_request_timeout_secs) != OPTIONHANDLER_OK)
            {
                LogError("Failed to retrieve options from authentication instance (OptionHandler_Create failed for option '%s')", AUTHENTICATION_OPTION_CBS_REQUEST_TIMEOUT_SECS);
                result = NULL;
            }
            else
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_127: [If no failures occur, authentication_retrieve_options shall return the OPTIONHANDLER_HANDLE instance]
                result = options;
            }

            if (result == NULL)
            {
                // Codes_SRS_IOTHUBTRANSPORT_AMQP_AUTH_09_105: [If authentication_retrieve_options fails, any allocated memory shall be freed]
                OptionHandler_Destroy(options);
            }
        }
    }
    return result;
}
