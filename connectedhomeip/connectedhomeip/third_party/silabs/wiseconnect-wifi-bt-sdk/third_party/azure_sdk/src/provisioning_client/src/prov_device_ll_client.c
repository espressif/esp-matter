// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "parson.h"

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/uniqueid.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/tickcounter.h"

#include "azure_prov_client/internal/prov_auth_client.h"
#include "azure_prov_client/internal/prov_transport_private.h"
#include "azure_prov_client/prov_device_ll_client.h"
#include "azure_prov_client/prov_client_const.h"

static const char* const OPTION_LOG_TRACE = "logtrace";

static const char* const JSON_NODE_STATUS = "status";
static const char* const JSON_NODE_REG_STATUS = "registrationState";
static const char* const JSON_NODE_AUTH_KEY = "authenticationKey";
static const char* const JSON_NODE_DEVICE_ID = "deviceId";
static const char* const JSON_NODE_KEY_NAME = "keyName";
static const char* const JSON_NODE_OPERATION_ID = "operationId";
static const char* const JSON_NODE_ASSIGNED_HUB = "assignedHub";
static const char* const JSON_NODE_TPM_NODE = "tpm";
static const char* const JSON_NODE_DATE_TIME = "lastUpdatedDateTimeUtc";
static const char* const JSON_NODE_ERROR_MSG = "errorMessage";
static const char* const JSON_NODE_ERROR_CODE = "errorCode";
static const char* const PROV_FAILED_STATUS = "failed";
static const char* const PROV_BLACKLISTED_STATUS = "blacklisted";
static const char* const JSON_CUSTOM_DATA_TAG = "payload";
static const char* const JSON_NODE_RETURNED_DATA = "payload";

static const char* const SAS_TOKEN_SCOPE_FMT = "%s/registrations/%s";

static const char* const REGISTRATION_ID = "registrationId";
static const char* const JSON_ENDORSMENT_KEY_NODE = "endorsementKey";
static const char* const JSON_STORAGE_ROOT_KEY_NODE = "storageRootKey";

#define DPS_HUB_ERROR_NO_HUB        400208
#define DPS_HUB_ERROR_UNAUTH        400209

#define SAS_TOKEN_DEFAULT_LIFETIME  2400
#define EPOCH_TIME_T_VALUE          (time_t)0
#define MAX_AUTH_ATTEMPTS           3
#define PROV_DEFAULT_TIMEOUT        60

typedef enum CLIENT_STATE_TAG
{
    CLIENT_STATE_READY,

    CLIENT_STATE_REGISTER_SEND,
    CLIENT_STATE_REGISTER_SENT,
    CLIENT_STATE_REGISTER_RECV,

    CLIENT_STATE_STATUS_SEND,
    CLIENT_STATE_STATUS_SENT,
    CLIENT_STATE_STATUS_RECV,

    CLIENT_STATE_ERROR
} CLIENT_STATE;

typedef struct IOTHUB_REQ_INFO_TAG
{
    char* iothub_url;
    char* iothub_key;
    char* device_id;
} IOTHUB_REQ_INFO;

typedef struct PROV_INSTANCE_INFO_TAG
{
    PROV_DEVICE_CLIENT_REGISTER_DEVICE_CALLBACK register_callback;
    void* user_context;
    PROV_DEVICE_CLIENT_REGISTER_STATUS_CALLBACK register_status_cb;
    void* status_user_ctx;
    PROV_DEVICE_RESULT error_reason;

    const PROV_DEVICE_TRANSPORT_PROVIDER* prov_transport_protocol;
    PROV_DEVICE_TRANSPORT_HANDLE transport_handle;
    bool transport_open;

    TICK_COUNTER_HANDLE tick_counter;

    tickcounter_ms_t last_send_time_ms;
    tickcounter_ms_t timeout_value;
    size_t retry_after_ms;

    uint8_t prov_timeout;

    char* registration_id;
    bool user_supplied_reg_id;

    PROV_AUTH_HANDLE prov_auth_handle;

    bool is_connected;

    PROV_AUTH_TYPE hsm_type;

    IOTHUB_REQ_INFO iothub_info;

    CLIENT_STATE prov_state;

    size_t auth_attempts_made;

    char* scope_id;

    char* custom_request_data;
    char* custom_response_data;
} PROV_INSTANCE_INFO;

static char* prov_transport_challenge_callback(const unsigned char* nonce, size_t nonce_len, const char* key_name, void* user_ctx)
{
    char* result;
    if (user_ctx == NULL)
    {
        LogError("Bad argument user_ctx is NULL");
        result = NULL;
    }
    else
    {
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)user_ctx;
        if ((prov_info->hsm_type == PROV_AUTH_TYPE_TPM) && nonce == NULL)
        {
            LogError("Bad argument nonce is NULL");
            result = NULL;
        }
        else
        {
            char* token_scope;
            size_t token_scope_len;

            size_t sec_since_epoch = (size_t)(difftime(get_time(NULL), EPOCH_TIME_T_VALUE) + 0);
            size_t expiry_time = sec_since_epoch + SAS_TOKEN_DEFAULT_LIFETIME;

            // Construct Token scope
            token_scope_len = strlen(SAS_TOKEN_SCOPE_FMT) + strlen(prov_info->scope_id) + strlen(prov_info->registration_id);

            token_scope = malloc(token_scope_len + 1);
            if (token_scope == NULL)
            {
                LogError("Failure to allocate token scope");
                result = NULL;
            }
            else if (sprintf(token_scope, SAS_TOKEN_SCOPE_FMT, prov_info->scope_id, prov_info->registration_id) <= 0)
            {
                LogError("Failure to constructing token_scope");
                free(token_scope);
                result = NULL;
            }
            else
            {
                STRING_HANDLE encoded_token = URL_EncodeString(token_scope);
                if (encoded_token == NULL)
                {
                    LogError("Failure to url encoding string");
                    result = NULL;
                }
                else
                {
                    if (prov_info->hsm_type == PROV_AUTH_TYPE_TPM && (prov_auth_import_key(prov_info->prov_auth_handle, nonce, nonce_len) != 0))
                    {
                        LogError("Failure to import the provisioning key");
                        result = NULL;
                    }
                    else
                    {
                        if ((result = prov_auth_construct_sas_token(prov_info->prov_auth_handle, STRING_c_str(encoded_token), key_name, expiry_time)) == NULL)
                        {
                            LogError("Failure to import the provisioning key");
                            result = NULL;
                        }
                    }
                    STRING_delete(encoded_token);
                }
                free(token_scope);
            }
        }
    }
    return result;
}

static void on_transport_error(PROV_DEVICE_TRANSPORT_ERROR transport_error, void* user_ctx)
{
    if (user_ctx != NULL)
    {
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)user_ctx;
        switch (transport_error)
        {
            case PROV_DEVICE_ERROR_KEY_FAIL:
                prov_info->error_reason = PROV_DEVICE_RESULT_KEY_ERROR;
                break;
            case PROV_DEVICE_ERROR_KEY_UNAUTHORIZED:
                prov_info->error_reason = PROV_DEVICE_RESULT_DEV_AUTH_ERROR;
                break;

            case PROV_DEVICE_ERROR_MEMORY:
                prov_info->error_reason = PROV_DEVICE_RESULT_MEMORY;
                break;
        }
    }
}

static PROV_DEVICE_TRANSPORT_STATUS retrieve_status_type(const char* prov_status)
{
    PROV_DEVICE_TRANSPORT_STATUS result;
    if (strcmp(prov_status, PROV_UNASSIGNED_STATUS) == 0)
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED;
    }
    else if (strcmp(prov_status, PROV_ASSIGNING_STATUS) == 0)
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING;
    }
    else if (strcmp(prov_status, PROV_ASSIGNED_STATUS) == 0)
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_ASSIGNED;
    }
    else if (strcmp(prov_status, PROV_FAILED_STATUS) == 0)
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_ERROR;
    }
    else if (strcmp(prov_status, PROV_BLACKLISTED_STATUS) == 0)
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_BLACKLISTED;
    }
    else if (strcmp(prov_status, PROV_DISABLE_STATUS) == 0)
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_DISABLED;
    }
    else
    {
        result = PROV_DEVICE_TRANSPORT_STATUS_ERROR;
    }
    return result;
}

static int retrieve_json_number(JSON_Object* json_object, const char* field_name)
{
    int result;
    JSON_Value* json_field;
    if ((json_field = json_object_get_value(json_object, field_name)) == NULL)
    {
        LogError("failure retrieving json operation id");
        result = 0;
    }
    else
    {
        result = (int)json_value_get_number(json_field);
    }
    return result;
}

static void retrieve_json_payload(JSON_Object* json_object, PROV_INSTANCE_INFO* prov_info)
{
    JSON_Value* json_field;

    // Returned Data is not available
    if ((json_field = json_object_get_value(json_object, JSON_NODE_RETURNED_DATA)) != NULL)
    {
        prov_info->custom_response_data = json_serialize_to_string(json_field);
    }
}

static char* retrieve_json_string(JSON_Object* json_object, const char* field_name, bool is_required)
{
    char* result;
    JSON_Value* json_field;
    if ((json_field = json_object_get_value(json_object, field_name)) == NULL)
    {
        if (is_required)
        {
            LogError("failure retrieving json object value %s", field_name);
        }
        result = NULL;
    }
    else
    {
        const char* json_item = json_value_get_string(json_field);
        if (json_item != NULL)
        {
            if (mallocAndStrcpy_s(&result, json_item) != 0)
            {
                LogError("failure retrieving operation id");
                result = NULL;
            }
        }
        else
        {
            result = NULL;
        }
    }
    return result;
}

static JSON_Value* construct_security_type_json(PROV_INSTANCE_INFO* prov_info, const char* ek_value, const char* srk_value)
{
    JSON_Value* result;

    JSON_Object* json_object;
    result = json_value_init_object();
    if (result == NULL)
    {
        LogError("Failure constructing json information");
    }
    else if ((json_object = json_value_get_object(result)) == NULL)
    {
        LogError("failure retrieving node root object");
        json_value_free(result);
        result = NULL;
    }
    else
    {
        if (json_object_set_string(json_object, REGISTRATION_ID, prov_info->registration_id) != JSONSuccess)
        {
            LogError("failure setting registration Id json node");
            json_value_free(result);
            result = NULL;
        }
        else
        {
            if (prov_info->hsm_type == PROV_AUTH_TYPE_TPM)
            {
                // tpm_node value only gets released on failure
                JSON_Object* tpm_object;
                JSON_Value* tpm_node = json_value_init_object();
                if (tpm_node == NULL)
                {
                    LogError("failure constructing json tpm object");
                    json_value_free(tpm_node);
                    json_value_free(result);
                    result = NULL;
                }
                else if ((tpm_object = json_value_get_object(tpm_node)) == NULL)
                {
                    LogError("failure retrieving node root object");
                    json_value_free(tpm_node);
                    json_value_free(result);
                    result = NULL;
                }
                else if (json_object_set_string(tpm_object, JSON_ENDORSMENT_KEY_NODE, ek_value) != JSONSuccess)
                {
                    LogError("failure setting endorsement key node");
                    json_value_free(tpm_node);
                    json_value_free(result);
                    result = NULL;
                }
                else if (json_object_set_string(tpm_object, JSON_STORAGE_ROOT_KEY_NODE, srk_value) != JSONSuccess)
                {
                    LogError("failure setting tpm storage root key node");
                    json_value_free(tpm_node);
                    json_value_free(result);
                    result = NULL;
                }
                else if (json_object_set_value(json_object, JSON_NODE_TPM_NODE, tpm_node) != JSONSuccess)
                {
                    LogError("failure constructing json tpm object");
                    json_value_free(tpm_node);
                    json_value_free(result);
                    result = NULL;
                }
            }
        }
    }
    return result;
}

static char* prov_transport_create_json_payload(const char* ek_value, const char* srk_value, void* user_ctx)
{
    char* result = NULL;
    if (user_ctx == NULL)
    {
        LogError("failure user_ctx is NULL");
    }
    else
    {
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)user_ctx;

        JSON_Value* json_root = construct_security_type_json(prov_info, ek_value, srk_value);
        if (json_root == NULL)
        {
            LogError("Failure constructing security json");
        }
        else
        {
            bool error_encountered = false;
            if (prov_info->custom_request_data != NULL)
            {
                JSON_Object* json_object;
                JSON_Value* json_custom_data = NULL;

                if ((json_object = json_value_get_object(json_root)) == NULL)
                {
                    LogError("failure retrieving node root object");
                    error_encountered = true;
                }
                else if ((json_custom_data = json_parse_string(prov_info->custom_request_data)) == NULL)
                {
                    LogError("failure parsing custom info.  This custom info MUST be valid json");
                    error_encountered = true;
                }
                // Success on json_object_set_value transfers ownership of json_custom_data to json_object, so do not
                // explicitly free json_custom_data after this point.
                else if (json_object_set_value(json_object, JSON_CUSTOM_DATA_TAG, json_custom_data) != JSONSuccess)
                {
                    LogError("failure setting %s value", JSON_CUSTOM_DATA_TAG);
                    json_value_free(json_custom_data);
                    error_encountered = true;
                }
            }

            if (!error_encountered)
            {
                char* json_string = json_serialize_to_string(json_root);
                if (json_string == NULL)
                {
                    LogError("failure serializing json to string");
                }
                else
                {
                    if (mallocAndStrcpy_s(&result, json_string) != 0)
                    {
                        LogError("failure constructing json result value");
                    }
                    json_free_serialized_string(json_string);
                }
            }
            json_value_free(json_root);
        }
    }

    return result;
}

static PROV_JSON_INFO* prov_transport_process_json_reply(const char* json_document, void* user_ctx)
{
    PROV_JSON_INFO* result;
    JSON_Value* root_value;
    JSON_Object* json_object;
    if (user_ctx == NULL)
    {
        LogError("failure user_ctx is NULL");
        result = NULL;
    }
    else if ((root_value = json_parse_string(json_document)) == NULL)
    {
        LogError("failure calling json_parse_string");
        result = NULL;
    }
    else if ((json_object = json_value_get_object(root_value)) == NULL)
    {
        LogError("failure retrieving node root object");
        json_value_free(root_value);
        result = NULL;
    }
    else if ((result = malloc(sizeof(PROV_JSON_INFO))) == NULL)
    {
        LogError("failure allocating PROV_JSON_INFO");
        json_value_free(root_value);
    }
    else
    {
        memset(result, 0, sizeof(PROV_JSON_INFO));
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)user_ctx;
        JSON_Value* json_status = json_object_get_value(json_object, JSON_NODE_STATUS);

        if (json_status == NULL)
        {
            // Under TPM the status will be error if this is an authorization request.  We need to make it unassigned.
            if (prov_info->hsm_type == PROV_AUTH_TYPE_TPM)
            {
                result->prov_status = PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED;
            }
            else
            {
                result->prov_status = PROV_DEVICE_TRANSPORT_STATUS_ERROR;
            }
        }
        else
        {
            // status can be NULL
            const char* json_string = json_value_get_string(json_status);
            if (json_string == NULL)
            {
                result->prov_status = PROV_DEVICE_TRANSPORT_STATUS_ERROR;
            }
            else
            {
                result->prov_status = retrieve_status_type(json_string);
            }
        }
        switch (result->prov_status)
        {
            case PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED:
            {
                JSON_Value* auth_key;
                if ((auth_key = json_object_get_value(json_object, JSON_NODE_AUTH_KEY)) == NULL)
                {
                    LogError("failure retrieving json auth key value");
                    prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                    free(result);
                    result = NULL;
                }
                else
                {
                    const char* nonce_field = json_value_get_string(auth_key);
                    if ((result->authorization_key = Azure_Base64_Decode(nonce_field)) == NULL)
                    {
                        LogError("failure creating buffer nonce field");
                        prov_info->error_reason = PROV_DEVICE_RESULT_MEMORY;
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->key_name = retrieve_json_string(json_object, JSON_NODE_KEY_NAME, true);
                        if (result->key_name == NULL)
                        {
                            LogError("failure retrieving keyname field");
                            prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                            BUFFER_delete(result->authorization_key);
                            free(result);
                            result = NULL;
                        }
                    }
                }
                break;
            }

            case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING:
                if ((result->operation_id = retrieve_json_string(json_object, JSON_NODE_OPERATION_ID, true)) == NULL)
                {
                    LogError("Failure: operation_id node is mising");
                    prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                    free(result);
                    result = NULL;
                }
                break;

            case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNED:
            {
                JSON_Object* json_reg_status_node;
                if ((json_reg_status_node = json_object_get_object(json_object, JSON_NODE_REG_STATUS)) == NULL)
                {
                    LogError("failure retrieving json registration status node");
                    prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                    free(result);
                    result = NULL;
                }
                else
                {
                    if (prov_info->hsm_type == PROV_AUTH_TYPE_TPM)
                    {
                        JSON_Object* json_tpm_node;
                        JSON_Value* auth_key;

                        if ((json_tpm_node = json_object_get_object(json_reg_status_node, JSON_NODE_TPM_NODE)) == NULL)
                        {
                            LogError("failure retrieving tpm node json_tpm_node: %p, auth key: %p", json_tpm_node, result->authorization_key);
                            free(result);
                            prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                            result = NULL;
                        }
                        else if ((auth_key = json_object_get_value(json_tpm_node, JSON_NODE_AUTH_KEY)) == NULL)
                        {
                            LogError("failure retrieving json auth key value");
                            free(result);
                            prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                            result = NULL;
                        }
                        else
                        {
                            const char* nonce_field = json_value_get_string(auth_key);
                            if (nonce_field == NULL)
                            {
                                LogError("failure getting nonce field from json");
                                prov_info->error_reason = PROV_DEVICE_RESULT_MEMORY;
                                free(result);
                                result = NULL;
                            }
                            else if ((result->authorization_key = Azure_Base64_Decode(nonce_field)) == NULL)
                            {
                                LogError("failure creating buffer nonce field");
                                prov_info->error_reason = PROV_DEVICE_RESULT_MEMORY;
                                free(result);
                                result = NULL;
                            }
                        }
                    }

                    if (result != NULL)
                    {
                        if (
                            ((result->iothub_uri = retrieve_json_string(json_reg_status_node, JSON_NODE_ASSIGNED_HUB, true)) == NULL) ||
                            ((result->device_id = retrieve_json_string(json_reg_status_node, JSON_NODE_DEVICE_ID, true)) == NULL)
                            )
                        {
                            LogError("failure retrieving json value assigned_hub: %p, device_id: %p", result->iothub_uri, result->device_id);
                            prov_info->error_reason = PROV_DEVICE_RESULT_PARSING;
                            free(result->iothub_uri);
                            free(result->authorization_key);
                            free(result);
                            result = NULL;
                        }
                        else
                        {
                            // Get the returned Data from the payload if it's there
                            retrieve_json_payload(json_reg_status_node, prov_info);
                        }
                    }
                }
                break;
            }

            case PROV_DEVICE_TRANSPORT_STATUS_BLACKLISTED:
                LogError("The device is unauthorized with service");
                prov_info->error_reason = PROV_DEVICE_RESULT_ERROR;
                free(result);
                result = NULL;
                break;

            case PROV_DEVICE_TRANSPORT_STATUS_ERROR:
            {
                char* json_operation_id = NULL;
                JSON_Object* json_reg_state = NULL;
                if ((json_reg_state = json_object_get_object(json_object, JSON_NODE_REG_STATUS)) != NULL &&
                    (json_operation_id = retrieve_json_string(json_object, JSON_NODE_OPERATION_ID, true)) != NULL)
                {
                    int error_code = retrieve_json_number(json_reg_state, JSON_NODE_ERROR_CODE);
                    switch (error_code)
                    {
                        case DPS_HUB_ERROR_NO_HUB:
                            prov_info->error_reason = PROV_DEVICE_RESULT_HUB_NOT_SPECIFIED;
                            break;
                        case DPS_HUB_ERROR_UNAUTH:
                            prov_info->error_reason = PROV_DEVICE_RESULT_UNAUTHORIZED;
                            break;
                        default:
                            prov_info->error_reason = PROV_DEVICE_RESULT_DEV_AUTH_ERROR;
                            break;
                    }
#ifndef NO_LOGGING
                    JSON_Value* json_error_date_time = NULL;
                    JSON_Value* json_error_msg = NULL;
                    if ((json_error_msg = json_object_get_value(json_reg_state, JSON_NODE_ERROR_MSG)) != NULL &&
                        (json_error_date_time = json_object_get_value(json_reg_state, JSON_NODE_DATE_TIME)) != NULL)
                    {
                        LogError("Provisioning Failure: OperationId: %s - Date: %s - Msg: %s", json_operation_id, json_value_get_string(json_error_date_time), json_value_get_string(json_error_msg) );
                    }
                    else
                    {
                        LogError("Unsuccessful json encountered: %s", json_document);
                    }
                    free(json_operation_id);
#endif
                }
                else
                {
                    prov_info->error_reason = PROV_DEVICE_RESULT_DEV_AUTH_ERROR;
                    LogError("Unsuccessful json encountered: %s", json_document);
                }
                free(result);
                result = NULL;
                break;
            }

            case PROV_DEVICE_TRANSPORT_STATUS_DISABLED:
                LogError("The device has been disabled by DPS service");
                prov_info->error_reason = PROV_DEVICE_RESULT_DISABLED;
                free(result);
                result = NULL;
                break;

            default:
                LogError("invalid json status specified %d", result->prov_status);
                free(result);
                result = NULL;
                break;
        }
        json_value_free(root_value);
    }
    return result;
}

static void cleanup_prov_info(PROV_INSTANCE_INFO* prov_info)
{
    if (prov_info->transport_open)
    {
        prov_info->prov_transport_protocol->prov_transport_close(prov_info->transport_handle);
        prov_info->transport_open = false;
        prov_info->is_connected = false;
    }
    free(prov_info->registration_id);
    prov_info->registration_id = NULL;
    free(prov_info->iothub_info.device_id);
    prov_info->iothub_info.device_id = NULL;
    free(prov_info->iothub_info.iothub_key);
    prov_info->iothub_info.iothub_key = NULL;
    free(prov_info->iothub_info.iothub_url);
    prov_info->iothub_info.iothub_url = NULL;
    prov_info->auth_attempts_made = 0;
}

static void on_transport_registration_data(PROV_DEVICE_TRANSPORT_RESULT transport_result, BUFFER_HANDLE iothub_key, const char* assigned_hub, const char* device_id, void* user_ctx)
{
    if (user_ctx == NULL)
    {
        LogError("user context was unexpectantly NULL");
    }
    else
    {
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)user_ctx;
        if (transport_result == PROV_DEVICE_TRANSPORT_RESULT_OK)
        {
            if (prov_info->hsm_type == PROV_AUTH_TYPE_TPM)
            {
                if (iothub_key == NULL)
                {
                    prov_info->prov_state = CLIENT_STATE_ERROR;
                    if (prov_info->error_reason == PROV_DEVICE_RESULT_OK)
                    {
                        prov_info->error_reason = PROV_DEVICE_RESULT_KEY_ERROR;
                    }
                    LogError("invalid iothub device key");
                }
                else
                {
                    const unsigned char* key_value = BUFFER_u_char(iothub_key);
                    size_t key_len = BUFFER_length(iothub_key);

                    /* Codes_SRS_SECURE_ENCLAVE_CLIENT_07_028: [ prov_auth_import_key shall import the specified key into the tpm using secure_device_import_key secure enclave function. ] */
                    if (prov_auth_import_key(prov_info->prov_auth_handle, key_value, key_len) != 0)
                    {
                        prov_info->prov_state = CLIENT_STATE_ERROR;
                        prov_info->error_reason = PROV_DEVICE_RESULT_KEY_ERROR;
                        LogError("Failure to import the provisioning key");
                    }
                }
            }

            if (prov_info->prov_state != CLIENT_STATE_ERROR)
            {
                prov_info->register_callback(PROV_DEVICE_RESULT_OK, assigned_hub, device_id, prov_info->user_context);
                prov_info->prov_state = CLIENT_STATE_READY;
                cleanup_prov_info(prov_info);
            }
        }
        else if (transport_result == PROV_DEVICE_TRANSPORT_RESULT_UNAUTHORIZED)
        {
            prov_info->prov_state = CLIENT_STATE_ERROR;
            prov_info->error_reason = PROV_DEVICE_RESULT_DEV_AUTH_ERROR;
            LogError("provisioning result is unauthorized");
        }
        else
        {
            prov_info->prov_state = CLIENT_STATE_ERROR;
            if (prov_info->error_reason == PROV_DEVICE_RESULT_OK)
            {
                prov_info->error_reason = PROV_DEVICE_RESULT_TRANSPORT;
            }
            LogError("Failure retrieving data from the provisioning service");
        }
    }
}

static void on_transport_status(PROV_DEVICE_TRANSPORT_STATUS transport_status, uint32_t retry_interval, void* user_ctx)
{
    if (user_ctx == NULL)
    {
        LogError("user_ctx was unexpectatly NULL");
    }
    else
    {
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)user_ctx;

        prov_info->retry_after_ms = (size_t)retry_interval * 1000; // retry_interval is in seconds.

        switch (transport_status)
        {
            case PROV_DEVICE_TRANSPORT_STATUS_CONNECTED:
                prov_info->is_connected = true;
                if (prov_info->register_status_cb != NULL)
                {
                    prov_info->register_status_cb(PROV_DEVICE_REG_STATUS_CONNECTED, prov_info->status_user_ctx);
                }
                break;
            case PROV_DEVICE_TRANSPORT_STATUS_AUTHENTICATED:
            case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING:
            case PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED:
                prov_info->prov_state = CLIENT_STATE_STATUS_SEND;
                if (transport_status == PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED)
                {
                    if (prov_info->register_status_cb != NULL)
                    {
                        prov_info->register_status_cb(PROV_DEVICE_REG_STATUS_REGISTERING, prov_info->status_user_ctx);
                    }
                }
                else if (transport_status == PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING)
                {
                    if (prov_info->register_status_cb != NULL)
                    {
                        prov_info->register_status_cb(PROV_DEVICE_REG_STATUS_ASSIGNING, prov_info->status_user_ctx);
                    }
                }
                break;
            case PROV_DEVICE_TRANSPORT_STATUS_TRANSIENT:
                if (prov_info->prov_state == CLIENT_STATE_REGISTER_SENT)
                {
                    prov_info->prov_state = CLIENT_STATE_REGISTER_SEND;
                }
                else if (prov_info->prov_state == CLIENT_STATE_STATUS_SENT)
                {
                    prov_info->prov_state = CLIENT_STATE_STATUS_SEND;
                }
                else
                {
                    // Ideally this should not happen
                    LogError("State Error: Transient Error occured in the %d state", (int)transport_status);
                }
                break;
            default:
                LogError("Unknown status encountered");
                break;
        }
    }
}

static void destroy_instance(PROV_INSTANCE_INFO* prov_info)
{
    cleanup_prov_info(prov_info);
    // Clean custom request data
    free(prov_info->custom_request_data);
    prov_info->custom_request_data = NULL;
    if (prov_info->custom_response_data != NULL)
    {
        json_free_serialized_string(prov_info->custom_response_data);
        prov_info->custom_response_data = NULL;
    }
    prov_info->prov_transport_protocol->prov_transport_destroy(prov_info->transport_handle);
    prov_info->transport_handle = NULL;
    free(prov_info->scope_id);
    prov_auth_destroy(prov_info->prov_auth_handle);
    tickcounter_destroy(prov_info->tick_counter);
    free(prov_info);
}

PROV_DEVICE_LL_HANDLE Prov_Device_LL_Create(const char* uri, const char* id_scope, PROV_DEVICE_TRANSPORT_PROVIDER_FUNCTION protocol)
{
    PROV_INSTANCE_INFO* result;
    /* Codes_SRS_PROV_CLIENT_07_001: [If uri is NULL Prov_Device_LL_CreateFromUri shall return NULL.] */
    if (uri == NULL || id_scope == NULL || protocol == NULL)
    {
        LogError("Invalid parameter specified uri: %p, id_scope: %p, protocol: %p", uri, id_scope, protocol);
        result = NULL;
    }
    else
    {
        srand((unsigned int)get_time(NULL));

        /* Codes_SRS_PROV_CLIENT_07_002: [ Prov_Device_LL_CreateFromUri shall allocate a PROV_DEVICE_LL_HANDLE and initialize all members. ] */
        result = (PROV_INSTANCE_INFO*)malloc(sizeof(PROV_INSTANCE_INFO));
        if (result == NULL)
        {
            LogError("unable to allocate Instance Info");
        }
        else
        {
            memset(result, 0, sizeof(PROV_INSTANCE_INFO));

            /* Codes_SRS_PROV_CLIENT_07_028: [ CLIENT_STATE_READY is the initial state after the object is created which will send a uhttp_client_open call to the http endpoint. ] */
            result->prov_state = CLIENT_STATE_READY;
            result->retry_after_ms = PROV_GET_THROTTLE_TIME * 1000;
            result->prov_transport_protocol = protocol();
            result->error_reason = PROV_DEVICE_RESULT_OK;

            /* Codes_SRS_PROV_CLIENT_07_034: [ Prov_Device_LL_Create shall construct a id_scope by base64 encoding the uri. ] */
            if (mallocAndStrcpy_s(&result->scope_id, id_scope) != 0)
            {
                /* Codes_SRS_PROV_CLIENT_07_003: [ If any error is encountered, Prov_Device_LL_CreateFromUri shall return NULL. ] */
                LogError("failed to construct id_scope");
                free(result);
                result = NULL;
            }
            else if ((result->prov_auth_handle = prov_auth_create()) == NULL)
            {
                /* Codes_SRS_PROV_CLIENT_07_003: [ If any error is encountered, Prov_Device_LL_CreateFromUri shall return NULL. ] */
                LogError("failed calling prov_auth_create\r\n");
                destroy_instance(result);
                result = NULL;
            }
            else if ((result->tick_counter = tickcounter_create()) == NULL)
            {
                LogError("failure: allocating tickcounter");
                destroy_instance(result);
                result = NULL;
            }
            else
            {
                TRANSPORT_HSM_TYPE hsm_type;
                if ((result->hsm_type = prov_auth_get_type(result->prov_auth_handle)) == PROV_AUTH_TYPE_TPM)
                {
                    hsm_type = TRANSPORT_HSM_TYPE_TPM;
                }
                else if (result->hsm_type == PROV_AUTH_TYPE_KEY)
                {
                    hsm_type = TRANSPORT_HSM_TYPE_SYMM_KEY;
                }
                else
                {
                    hsm_type = TRANSPORT_HSM_TYPE_X509;
                }

                if ((result->transport_handle = result->prov_transport_protocol->prov_transport_create(uri, hsm_type, result->scope_id, PROV_API_VERSION, on_transport_error, result)) == NULL)
                {
                    /* Codes_SRS_PROV_CLIENT_07_003: [ If any error is encountered, Prov_Device_LL_CreateFromUri shall return NULL. ] */
                    LogError("failed calling into transport create");
                    destroy_instance(result);
                    result = NULL;
                }
                else
                {
                    // Ensure that we are passed the throttling time and send on the first send
                    (void)tickcounter_get_current_ms(result->tick_counter, &result->last_send_time_ms);
                    result->last_send_time_ms += result->retry_after_ms;
                }
            }
        }
    }
    return (PROV_DEVICE_LL_HANDLE)result;
}

void Prov_Device_LL_Destroy(PROV_DEVICE_LL_HANDLE handle)
{
    /* Codes_SRS_PROV_CLIENT_07_005: [ If handle is NULL Prov_Device_LL_Destroy shall do nothing. ] */
    if (handle != NULL)
    {
        /* Codes_SRS_PROV_CLIENT_07_006: [ Prov_Device_LL_Destroy shall destroy resources associated with the IoTHub_client ] */
        destroy_instance(handle);
    }
}

PROV_DEVICE_RESULT Prov_Device_LL_Register_Device(PROV_DEVICE_LL_HANDLE handle, PROV_DEVICE_CLIENT_REGISTER_DEVICE_CALLBACK register_callback, void* user_context, PROV_DEVICE_CLIENT_REGISTER_STATUS_CALLBACK reg_status_cb, void* status_ctx)
{
    PROV_DEVICE_RESULT result;
    /* Codes_SRS_PROV_CLIENT_07_007: [ If handle or register_callback is NULL, Prov_Device_LL_Register_Device shall return PROV_CLIENT_INVALID_ARG. ] */
    if (handle == NULL || register_callback == NULL)
    {
        LogError("Invalid parameter specified handle: %p register_callback: %p", handle, register_callback);
        result = PROV_DEVICE_RESULT_INVALID_ARG;
    }
    /* Codes_SRS_PROV_CLIENT_07_035: [ Prov_Device_LL_Create shall store the registration_id from the security module. ] */
    else if (handle->registration_id == NULL && (handle->registration_id = prov_auth_get_registration_id(handle->prov_auth_handle)) == NULL)
    {
        /* Codes_SRS_PROV_CLIENT_07_003: [ If any error is encountered, Prov_Device_LL_CreateFromUri shall return NULL. ] */
        LogError("failure: Unable to retrieve registration Id from device auth.");
        result = PROV_DEVICE_RESULT_ERROR;
    }
    else
    {
        BUFFER_HANDLE ek_value = NULL;
        BUFFER_HANDLE srk_value = NULL;

        if (handle->prov_state != CLIENT_STATE_READY)
        {
            LogError("state is invalid");
            if (!handle->user_supplied_reg_id)
            {
                free(handle->registration_id);
                handle->registration_id = NULL;
            }
            result = PROV_DEVICE_RESULT_ERROR;
        }
        else
        {
            if (handle->hsm_type == PROV_AUTH_TYPE_TPM)
            {
                if ((ek_value = prov_auth_get_endorsement_key(handle->prov_auth_handle)) == NULL)
                {
                    LogError("Could not get endorsement key from tpm");
                    if (!handle->user_supplied_reg_id)
                    {
                        free(handle->registration_id);
                        handle->registration_id = NULL;
                    }
                    result = PROV_DEVICE_RESULT_ERROR;
                }
                else if ((srk_value = prov_auth_get_storage_key(handle->prov_auth_handle)) == NULL)
                {
                    LogError("Could not get storage root key from tpm");
                    if (!handle->user_supplied_reg_id)
                    {
                        free(handle->registration_id);
                        handle->registration_id = NULL;
                    }
                    result = PROV_DEVICE_RESULT_ERROR;
                    BUFFER_delete(ek_value);
                }
                else
                {
                    result = PROV_DEVICE_RESULT_OK;
                }
            }
            else if (handle->hsm_type == PROV_AUTH_TYPE_X509)
            {
                char* x509_cert;
                char* x509_private_key;
                if ((x509_cert = prov_auth_get_certificate(handle->prov_auth_handle)) == NULL)
                {
                    LogError("Could not get the x509 certificate");
                    if (!handle->user_supplied_reg_id)
                    {
                        free(handle->registration_id);
                        handle->registration_id = NULL;
                    }
                    result = PROV_DEVICE_RESULT_ERROR;
                }
                else if ((x509_private_key = prov_auth_get_alias_key(handle->prov_auth_handle)) == NULL)
                {
                    LogError("Could not get the x509 alias key");
                    if (!handle->user_supplied_reg_id)
                    {
                        free(handle->registration_id);
                        handle->registration_id = NULL;
                    }
                    free(x509_cert);
                    result = PROV_DEVICE_RESULT_ERROR;
                }
                else
                {
                    if (handle->prov_transport_protocol->prov_transport_x509_cert(handle->transport_handle, x509_cert, x509_private_key) != 0)
                    {
                        LogError("unable to set the x509 certificate information on transport");
                        if (!handle->user_supplied_reg_id)
                        {
                            free(handle->registration_id);
                            handle->registration_id = NULL;
                        }
                        result = PROV_DEVICE_RESULT_ERROR;
                    }
                    else
                    {
                        result = PROV_DEVICE_RESULT_OK;
                    }
                    free(x509_cert);
                    free(x509_private_key);
                }
            }
            else
            {
                result = PROV_DEVICE_RESULT_OK;
            }
        }
        if (result == PROV_DEVICE_RESULT_OK)
        {
            /* Codes_SRS_PROV_CLIENT_07_008: [ Prov_Device_LL_Register_Device shall set the state to send the registration request to on subsequent DoWork calls. ] */
            handle->register_callback = register_callback;
            handle->user_context = user_context;

            handle->register_status_cb = reg_status_cb;
            handle->status_user_ctx = status_ctx;
            
            // Free the custom data if its been allocated
            if (handle->custom_response_data != NULL)
            {
                json_free_serialized_string(handle->custom_response_data);
                handle->custom_response_data = NULL;
            }

            if (handle->prov_transport_protocol->prov_transport_open(handle->transport_handle, handle->registration_id, ek_value, srk_value, on_transport_registration_data, handle, on_transport_status, handle, prov_transport_challenge_callback, handle) != 0)
            {
                LogError("Failure establishing  connection");
                if (!handle->user_supplied_reg_id)
                {
                    free(handle->registration_id);
                    handle->registration_id = NULL;
                }
                handle->register_callback = NULL;
                handle->user_context = NULL;

                handle->register_status_cb = NULL;
                handle->status_user_ctx = NULL;
                result = PROV_DEVICE_RESULT_ERROR;
            }
            else
            {
                handle->transport_open = true;
                handle->prov_state = CLIENT_STATE_REGISTER_SEND;
                /* Codes_SRS_PROV_CLIENT_07_009: [ Upon success Prov_Device_LL_Register_Device shall return PROV_CLIENT_OK. ] */
                result = PROV_DEVICE_RESULT_OK;
            }
            BUFFER_delete(ek_value);
            BUFFER_delete(srk_value);
        }
    }
    return result;
}

void Prov_Device_LL_DoWork(PROV_DEVICE_LL_HANDLE handle)
{
    /* Codes_SRS_PROV_CLIENT_07_010: [ If handle is NULL, Prov_Device_LL_DoWork shall do nothing. ] */
    if (handle != NULL)
    {
        PROV_INSTANCE_INFO* prov_info = (PROV_INSTANCE_INFO*)handle;
        /* Codes_SRS_PROV_CLIENT_07_011: [ Prov_Device_LL_DoWork shall call the underlying http_client_dowork function ] */
        if (prov_info->prov_state != CLIENT_STATE_ERROR)
        {
            prov_info->prov_transport_protocol->prov_transport_dowork(prov_info->transport_handle);
        }
        if (prov_info->is_connected || prov_info->prov_state == CLIENT_STATE_ERROR)
        {
            tickcounter_ms_t current_time = 0;

            switch (prov_info->prov_state)
            {
                case CLIENT_STATE_REGISTER_SEND:
                    if (tickcounter_get_current_ms(prov_info->tick_counter, &current_time) != 0)
                    {
                        LogError("Failure getting the current time");
                        prov_info->error_reason = PROV_DEVICE_RESULT_ERROR;
                        prov_info->prov_state = CLIENT_STATE_ERROR;
                    }
                    else
                    {
                        if ((current_time - prov_info->last_send_time_ms) > prov_info->retry_after_ms)
                        {
                            /* Codes_SRS_PROV_CLIENT_07_013: [ CLIENT_STATE_REGISTER_SEND which shall construct an initial call to the service with endorsement information ] */
                            if (prov_info->prov_transport_protocol->prov_transport_register(prov_info->transport_handle, prov_transport_process_json_reply, prov_transport_create_json_payload, prov_info) != 0)
                            {
                                LogError("Failure registering device");
                                if (prov_info->error_reason == PROV_DEVICE_RESULT_OK)
                                {
                                    prov_info->error_reason = PROV_DEVICE_RESULT_TRANSPORT;
                                }
                                prov_info->prov_state = CLIENT_STATE_ERROR;
                            }
                            else
                            {
                                (void)tickcounter_get_current_ms(prov_info->tick_counter, &prov_info->timeout_value);
                                prov_info->prov_state = CLIENT_STATE_REGISTER_SENT;
                            }
                            prov_info->last_send_time_ms = current_time;
                        }
                    }
                    break;

                case CLIENT_STATE_STATUS_SEND:
                {
                    if (tickcounter_get_current_ms(prov_info->tick_counter, &current_time) != 0)
                    {
                        LogError("Failure getting the current time");
                        prov_info->error_reason = PROV_DEVICE_RESULT_ERROR;
                        prov_info->prov_state = CLIENT_STATE_ERROR;
                    }
                    else
                    {
                        if ((current_time - prov_info->last_send_time_ms) > prov_info->retry_after_ms)
                        {
                            /* Codes_SRS_PROV_CLIENT_07_026: [ Upon receiving the reply of the CLIENT_STATE_URL_REQ_SEND message from  iothub_client shall process the the reply of the CLIENT_STATE_URL_REQ_SEND state ] */
                            if (prov_info->prov_transport_protocol->prov_transport_get_op_status(prov_info->transport_handle) != 0)
                            {
                                LogError("Failure sending operation status");
                                if (prov_info->error_reason == PROV_DEVICE_RESULT_OK)
                                {
                                    prov_info->error_reason = PROV_DEVICE_RESULT_TRANSPORT;
                                }
                                prov_info->prov_state = CLIENT_STATE_ERROR;
                            }
                            else
                            {
                                prov_info->prov_state = CLIENT_STATE_STATUS_SENT;
                                if (tickcounter_get_current_ms(prov_info->tick_counter, &prov_info->timeout_value) != 0)
                                {
                                    LogError("Failure getting the current time");
                                    prov_info->error_reason = PROV_DEVICE_RESULT_ERROR;
                                    prov_info->prov_state = CLIENT_STATE_ERROR;
                                }
                            }
                            prov_info->last_send_time_ms = current_time;
                        }
                    }
                    break;
                }

                case CLIENT_STATE_REGISTER_SENT:
                case CLIENT_STATE_STATUS_SENT:
                {
                    if (prov_info->prov_timeout > 0)
                    {
                        (void)tickcounter_get_current_ms(prov_info->tick_counter, &current_time);
                        if ((current_time - prov_info->timeout_value) / 1000 > prov_info->prov_timeout)
                        {
                            LogError("Timeout waiting for reply");
                            prov_info->error_reason = PROV_DEVICE_RESULT_TIMEOUT;
                            prov_info->prov_state = CLIENT_STATE_ERROR;
                        }
                    }
                    break;
                }

                case CLIENT_STATE_READY:
                    break;

                case CLIENT_STATE_ERROR:
                default:
                    prov_info->register_callback(prov_info->error_reason, NULL, NULL, prov_info->user_context);
                    prov_info->prov_state = CLIENT_STATE_READY;
                    cleanup_prov_info(prov_info);
                    break;
            }
        }
        else
        {
            // Check the connection
            if ((prov_info->prov_state != CLIENT_STATE_READY) && (prov_info->prov_timeout > 0))
            {
                tickcounter_ms_t current_time = 0;
                (void)tickcounter_get_current_ms(prov_info->tick_counter, &current_time);
                if ((current_time - prov_info->timeout_value) / 1000 > prov_info->prov_timeout)
                {
                    LogError("Timed out connecting to provisioning service");
                    prov_info->error_reason = PROV_DEVICE_RESULT_TIMEOUT;
                    prov_info->prov_state = CLIENT_STATE_ERROR;
                }
            }
        }
    }
}

PROV_DEVICE_RESULT Prov_Device_LL_SetOption(PROV_DEVICE_LL_HANDLE handle, const char* option_name, const void* value)
{
    PROV_DEVICE_RESULT result;
    if (handle == NULL || option_name == NULL)
    {
        LogError("Invalid parameter specified handle: %p option_name: %p", handle, option_name);
        result = PROV_DEVICE_RESULT_INVALID_ARG;
    }
    else
    {
        if (strcmp(option_name, OPTION_TRUSTED_CERT) == 0)
        {
            const char* cert_info = (const char*)value;
            if (handle->prov_transport_protocol->prov_transport_trusted_cert(handle->transport_handle, cert_info) != 0)
            {
                result = PROV_DEVICE_RESULT_ERROR;
                LogError("failure allocating certificate");
            }
            else
            {
                result = PROV_DEVICE_RESULT_OK;
            }
        }
        else if (strcmp(OPTION_LOG_TRACE, option_name) == 0)
        {
            bool log_trace = *((bool*)value);
            if (handle->prov_transport_protocol->prov_transport_set_trace(handle->transport_handle, log_trace) != 0)
            {
                result = PROV_DEVICE_RESULT_ERROR;
                LogError("failure setting trace option");
            }
            else
            {
                result = PROV_DEVICE_RESULT_OK;
            }
        }
        else if (strcmp(OPTION_HTTP_PROXY, option_name) == 0)
        {
            /* Codes_SRS_IOTHUB_TRANSPORT_MQTT_COMMON_01_001: [ If `option` is `proxy_data`, `value` shall be used as an `HTTP_PROXY_OPTIONS*`. ]*/
            HTTP_PROXY_OPTIONS* proxy_options = (HTTP_PROXY_OPTIONS*)value;

            if (handle->prov_transport_protocol->prov_transport_set_proxy(handle->transport_handle, proxy_options) != 0)
            {
                LogError("setting proxy options");
                result = PROV_DEVICE_RESULT_ERROR;
            }
            else
            {
                result = PROV_DEVICE_RESULT_OK;
            }
        }
        else if (strcmp(PROV_OPTION_TIMEOUT, option_name) == 0)
        {
            if (value == NULL)
            {
                LogError("setting PROV_OPTION_TIMEOUT option");
                result = PROV_DEVICE_RESULT_ERROR;
            }
            else
            {
                handle->prov_timeout = *((uint8_t*)value);
                result = PROV_DEVICE_RESULT_OK;
            }
        }
        else if (strcmp(PROV_REGISTRATION_ID, option_name) == 0)
        {
            if (handle->prov_state != CLIENT_STATE_READY)
            {
                LogError("registration id cannot be set after registration has begun");
                result = PROV_DEVICE_RESULT_ERROR;
            }
            else if (value == NULL)
            {
                LogError("value must be set to the correct registration id");
                result = PROV_DEVICE_RESULT_ERROR;
            }
            else
            {
                char* temp_reg;
                if (mallocAndStrcpy_s(&temp_reg, (const char*)value) != 0)
                {
                    LogError("Failure allocating setting registration id");
                    result = PROV_DEVICE_RESULT_ERROR;
                }
                else if (prov_auth_set_registration_id(handle->prov_auth_handle, temp_reg) != 0)
                {
                    LogError("Failure setting registration id");
                    free(temp_reg);
                    result = PROV_DEVICE_RESULT_ERROR;
                }
                else
                {
                    if (handle->registration_id != NULL)
                    {
                        free(handle->registration_id);
                    }
                    handle->registration_id = temp_reg;
                    handle->user_supplied_reg_id = true;
                    result = PROV_DEVICE_RESULT_OK;
                }
            }
        }
        else
        {
            if (handle->prov_transport_protocol->prov_transport_set_option(handle->transport_handle, option_name, value) != 0)
            {
                LogError("Failure in prov transport set option\n");
                result = PROV_DEVICE_RESULT_ERROR;
            }
            else
            {
                result = PROV_DEVICE_RESULT_OK;
            }
        }
    }
    return result;
}

const char* Prov_Device_LL_GetVersionString(void)
{
    return PROV_DEVICE_CLIENT_VERSION;
}

PROV_DEVICE_RESULT Prov_Device_LL_Set_Provisioning_Payload(PROV_DEVICE_LL_HANDLE handle, const char* jsonDataField)
{
    PROV_DEVICE_RESULT result;
    if (handle == NULL)
    {
        LogError("Invalid parameter specified handle: %p", handle);
        result = PROV_DEVICE_RESULT_INVALID_ARG;
    }
    else
    {
        char* temp_data;
        if (mallocAndStrcpy_s(&temp_data, jsonDataField) != 0)
        {
            LogError("Failure setting custom provisioning data");
            result = PROV_DEVICE_RESULT_ERROR;
        }
        else
        {
            if (handle->custom_request_data != NULL)
            {
                free(handle->custom_request_data);
            }
            handle->custom_request_data = temp_data;
            result = PROV_DEVICE_RESULT_OK;
        }
    }
    return result;
}

const char* Prov_Device_LL_Get_Provisioning_Payload(PROV_DEVICE_LL_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        LogError("Invalid parameter specified handle: %p", handle);
        result = NULL;
    }
    else
    {
        result = handle->custom_response_data;
    }
    return result;
}
