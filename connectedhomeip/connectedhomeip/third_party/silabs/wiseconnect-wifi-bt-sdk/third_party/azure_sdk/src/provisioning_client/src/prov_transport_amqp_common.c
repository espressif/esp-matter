// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/azure_base64.h"

#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_uamqp_c/sasl_plain.h"
#include "azure_uamqp_c/amqp_definitions_application_properties.h"

#include "azure_prov_client/internal/prov_sasl_tpm.h"
#include "azure_prov_client/prov_client_const.h"
#include "azure_prov_client/internal/prov_transport_amqp_common.h"

#define AMQP_MAX_SENDER_MSG_SIZE    UINT64_MAX
#define AMQP_MAX_RECV_MSG_SIZE      65536

static const char* const AMQP_ADDRESS_FMT = "amqps://%s/%s/registrations/%s";
static const char* const AMQP_SASL_USERNAME_FMT = "%s/registrations/%s";
static const char* const KEY_NAME_VALUE = "registration";

static const char* const AMQP_REGISTER_ME = "iotdps-register";
static const char* const AMQP_OPERATION_STATUS = "iotdps-get-operationstatus";

static const char* const AMQP_API_VERSION_KEY = "com.microsoft:api-version";

static const char* const AMQP_OP_TYPE_PROPERTY = "iotdps-operation-type";
static const char* const AMQP_OPERATION_ID = "iotdps-operation-id";

typedef enum AMQP_TRANSPORT_STATE_TAG
{
    AMQP_STATE_IDLE,
    AMQP_STATE_DISCONNECTED,
    AMQP_STATE_CONNECTING,
    AMQP_STATE_CONNECTED,
    AMQP_STATE_ERROR
} AMQP_TRANSPORT_STATE;

typedef enum TRANSPORT_CLIENT_STATE_TAG
{
    TRANSPORT_CLIENT_STATE_IDLE,

    TRANSPORT_CLIENT_STATE_REG_SEND,
    TRANSPORT_CLIENT_STATE_REG_SENT,
    TRANSPORT_CLIENT_STATE_REG_RECV,

    TRANSPORT_CLIENT_STATE_STATUS_SEND,
    TRANSPORT_CLIENT_STATE_STATUS_SENT,
    TRANSPORT_CLIENT_STATE_STATUS_RECV,


    TRANSPORT_CLIENT_STATE_ERROR
} TRANSPORT_CLIENT_STATE;

typedef struct PROV_TRANSPORT_AMQP_INFO_TAG
{
    PROV_DEVICE_TRANSPORT_REGISTER_CALLBACK register_data_cb;
    void* user_ctx;
    PROV_DEVICE_TRANSPORT_STATUS_CALLBACK status_cb;
    void* status_ctx;
    PROV_TRANSPORT_CHALLENGE_CALLBACK challenge_cb;
    void* challenge_ctx;
    PROV_TRANSPORT_JSON_PARSE json_parse_cb;
    PROV_TRANSPORT_CREATE_JSON_PAYLOAD json_create_cb;
    void* json_ctx;

    char* hostname;

    HTTP_PROXY_OPTIONS proxy_option;

    char* x509_cert;
    char* private_key;

    char* certificate;

    BUFFER_HANDLE ek;
    BUFFER_HANDLE srk;
    char* registration_id;
    char* scope_id;
    char* sas_token;
    char* operation_id;

    char* api_version;

    char* payload_data;

    bool log_trace;
    uint32_t retry_after_value;

    TRANSPORT_HSM_TYPE hsm_type;

    PROV_AMQP_TRANSPORT_IO transport_io_cb;

    TRANSPORT_CLIENT_STATE transport_state;
    AMQP_TRANSPORT_STATE amqp_state;

    MESSAGE_RECEIVER_STATE msg_recv_state;
    MESSAGE_SENDER_STATE msg_send_state;

    SASL_MECHANISM_HANDLE sasl_handler;
    SASLCLIENTIO_CONFIG sasl_io_config;
    CONNECTION_HANDLE connection;
    XIO_HANDLE transport_io;
    XIO_HANDLE underlying_io;
    SESSION_HANDLE session;
    LINK_HANDLE sender_link;
    LINK_HANDLE receiver_link;
    MESSAGE_SENDER_HANDLE msg_sender;
    MESSAGE_RECEIVER_HANDLE msg_receiver;

    PROV_TRANSPORT_ERROR_CALLBACK error_cb;
    void* error_ctx;
} PROV_TRANSPORT_AMQP_INFO;

static char* on_sasl_tpm_challenge_cb(BUFFER_HANDLE data_handle, void* user_ctx)
{
    char* result;
    PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)user_ctx;
    if (amqp_info == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_042: [ If user_ctx is NULL, on_sasl_tpm_challenge_cb shall return NULL. ] */
        LogError("Bad argument user_ctx NULL");
        result = NULL;
    }
    else if (amqp_info->challenge_cb == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_043: [ If the challenge_cb function is NULL, on_sasl_tpm_challenge_cb shall return NULL. ] */
        LogError("challenge callback is NULL, device needs to register");
        result = NULL;
    }
    else
    {
        const unsigned char* data = BUFFER_u_char(data_handle);
        size_t length = BUFFER_length(data_handle);

        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_045: [ on_sasl_tpm_challenge_cb shall call the challenge_cb returning the resulting value. ] */
        result = amqp_info->challenge_cb(data, length, KEY_NAME_VALUE, amqp_info->challenge_ctx);
        if (result == NULL)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_044: [ If any failure is encountered on_sasl_tpm_challenge_cb shall return NULL. ] */
            LogError("Failure challenge_cb");
            amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            amqp_info->amqp_state = AMQP_STATE_ERROR;
        }
    }
    return result;
}

static void on_message_sender_state_changed_callback(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    if (context == NULL)
    {
        LogError("on_message_sender_state_changed_callback was invoked with a NULL context");
    }
    else
    {
        if (new_state != previous_state)
        {
            PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)context;
            amqp_info->msg_send_state = new_state;
            switch (amqp_info->msg_send_state)
            {
                case MESSAGE_SENDER_STATE_IDLE:
                case MESSAGE_SENDER_STATE_OPENING:
                case MESSAGE_SENDER_STATE_CLOSING:
                    break;
                case MESSAGE_SENDER_STATE_OPEN:
                    if (amqp_info->msg_recv_state == MESSAGE_RECEIVER_STATE_OPEN)
                    {
                        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_052: [ Once the uamqp reciever and sender link are connected the amqp_state shall be set to AMQP_STATE_CONNECTED ] */
                        amqp_info->amqp_state = AMQP_STATE_CONNECTED;
                        if (amqp_info->status_cb != NULL)
                        {
                            amqp_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_CONNECTED, amqp_info->retry_after_value, amqp_info->status_ctx);
                        }
                    }
                    break;
                case MESSAGE_SENDER_STATE_ERROR:
                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                    amqp_info->amqp_state = AMQP_STATE_ERROR;
                    break;
                default:
                    LogError("on_message_sender_state_changed_callback received unexpected state %d", amqp_info->msg_recv_state);

                    amqp_info->amqp_state = AMQP_STATE_ERROR;
                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                    if (amqp_info->status_cb != NULL)
                    {
                        amqp_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_ERROR, amqp_info->retry_after_value, amqp_info->status_ctx);
                    }
            }
        }
    }
}

static void on_message_receiver_state_changed_callback(const void* user_ctx, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    if (user_ctx == NULL)
    {
        LogError("on_message_receiver_state_changed_callback was invoked with a NULL context");
    }
    else
    {
        if (new_state != previous_state)
        {
            PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)user_ctx;
            amqp_info->msg_recv_state = new_state;
            switch (amqp_info->msg_recv_state)
            {
            case MESSAGE_RECEIVER_STATE_IDLE:
            case MESSAGE_RECEIVER_STATE_OPENING:
            case MESSAGE_RECEIVER_STATE_CLOSING:
                break;
            case MESSAGE_RECEIVER_STATE_OPEN:
                if (amqp_info->msg_send_state == MESSAGE_SENDER_STATE_OPEN)
                {
                    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_052: [ Once the uamqp reciever and sender link are connected the amqp_state shall be set to AMQP_STATE_CONNECTED ] */
                    amqp_info->amqp_state = AMQP_STATE_CONNECTED;
                    if (amqp_info->status_cb != NULL)
                    {
                        amqp_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_CONNECTED, amqp_info->retry_after_value, amqp_info->status_ctx);
                    }
                }
                break;
            case MESSAGE_RECEIVER_STATE_ERROR:
                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                amqp_info->amqp_state = AMQP_STATE_ERROR;
                break;
            default:
                LogError("on_message_receiver_state_changed_callback received unexpected state %d", amqp_info->msg_recv_state);

                amqp_info->amqp_state = AMQP_STATE_ERROR;
                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                if (amqp_info->status_cb != NULL)
                {
                    amqp_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_ERROR, amqp_info->retry_after_value, amqp_info->status_ctx);
                }
            }
        }
    }
}

static void get_error_retry_after(PROV_TRANSPORT_AMQP_INFO* amqp_info, AMQP_VALUE delivery_state)
{
    if (amqpvalue_get_type(delivery_state) == AMQP_TYPE_LIST)
    {
        uint32_t list_count = 0;
        if (amqpvalue_get_list_item_count(delivery_state, &list_count) == 0)
        {
            for (uint32_t index = 0; index < list_count; index++)
            {
                AMQP_VALUE list_item_value = amqpvalue_get_list_item(delivery_state, index);
                if (list_item_value != NULL && amqpvalue_get_type(list_item_value) == AMQP_TYPE_DESCRIBED)
                {
                    // Currently this comes back as ulong from the amqp value
                    AMQP_VALUE desc = amqpvalue_get_inplace_descriptor(list_item_value);
                    if (desc != NULL && amqpvalue_get_type(desc) == AMQP_TYPE_ULONG)
                    {
                        uint64_t desc_value;
                        if (amqpvalue_get_ulong(desc, &desc_value) == 0)
                        {
                            // If the value is greater than the max throttle time (5 min)
                            // then set it to max throttle time
                            if (desc_value > MAX_PROV_GET_THROTTLE_TIME)
                            {
                                amqp_info->retry_after_value = MAX_PROV_GET_THROTTLE_TIME;
                            }
                            else
                            {
                                amqp_info->retry_after_value = (uint32_t)desc_value;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}

static void on_amqp_send_complete(void* user_ctx, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    if (user_ctx == NULL)
    {
        LogError("on_amqp_send_complete was invoked with a NULL context");
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)user_ctx;
        if (send_result != MESSAGE_SEND_OK)
        {
            (void)delivery_state;
            // To be included when a issue in the service is fixed.
            amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            amqp_info->amqp_state = AMQP_STATE_ERROR;
        }
    }
}

static STRING_HANDLE construct_link_address(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    STRING_HANDLE result;
    result = STRING_construct_sprintf(AMQP_ADDRESS_FMT, amqp_info->hostname, amqp_info->scope_id, amqp_info->registration_id);
    if (result == NULL)
    {
        LogError("Failure constructing event address");
    }
    return result;
}

static int get_retry_after_property(PROV_TRANSPORT_AMQP_INFO* amqp_info, MESSAGE_HANDLE message)
{
    int result;
    AMQP_VALUE app_prop = NULL;
    AMQP_VALUE prop_desc = NULL;
    uint32_t prop_count = 0;

    if (message_get_application_properties(message, &app_prop) != 0)
    {
        LogError("Failure getting application property");
        result = MU_FAILURE;
    }
    else if ((prop_desc = amqpvalue_get_inplace_described_value(app_prop)) == NULL)
    {
        LogError("Failure getting application property description");
        application_properties_destroy(app_prop);
        result = MU_FAILURE;
    }
    else if (amqpvalue_get_map_pair_count(prop_desc, &prop_count) != 0)
    {
        LogError("Failure getting application property count");
        application_properties_destroy(app_prop);
        result = MU_FAILURE;
    }
    else
    {
        bool found_value = false;
        result = MU_FAILURE;
        for (uint32_t index = 0; index < prop_count; index++)
        {
            AMQP_VALUE map_key_name = NULL;
            AMQP_VALUE map_key_value = NULL;
            const char *key_name;

            if ((amqpvalue_get_map_key_value_pair(prop_desc, index, &map_key_name, &map_key_value)) != 0)
            {
                LogError("Failed reading the key/value pair from the uAMQP property map.");
                result = MU_FAILURE;
            }
            else
            {
                if ((result = amqpvalue_get_string(map_key_name, &key_name)) != 0)
                {
                    LogError("Failed parsing the uAMQP property name.");
                    result = MU_FAILURE;
                }
                else if (key_name != NULL && strcmp(key_name, RETRY_AFTER_KEY_VALUE) == 0)
                {
                    const char* key_value;
                    int32_t val_int;

                    // The AMQP type is either int or string, for more information
                    // on this see prov_transport.h:parse_retry_after_value function.
                    AMQP_TYPE type = amqpvalue_get_type(map_key_value);
                    if (type == AMQP_TYPE_INT)
                    {
                        if (amqpvalue_get_int(map_key_value, &val_int) != 0)
                        {
                            LogError("Failed parsing the uAMQP property value.");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            amqp_info->retry_after_value = (uint32_t)val_int;
                            found_value = true;
                            result = 0;
                        }
                    }
                    else if (type == AMQP_TYPE_STRING)
                    {
                        if (amqpvalue_get_string(map_key_value, &key_value) != 0)
                        {
                            LogError("Failed parsing the uAMQP property value.");
                            result = MU_FAILURE;
                        }
                        else
                        {
                            amqp_info->retry_after_value = parse_retry_after_value(key_value);
                            found_value = true;
                            result = 0;
                        }
                    }
                    else
                    {
                        LogError("Failed parsing the uAMQP property value.");
                        result = MU_FAILURE;
                    }
                }
                amqpvalue_destroy(map_key_value);
                amqpvalue_destroy(map_key_name);
            }
            if (found_value)
            {
                // If the message retry-after only got through it once
                break;
            }
        }
        application_properties_destroy(app_prop);
    }
    return result;
}

static AMQP_VALUE on_message_recv_callback(const void* user_ctx, MESSAGE_HANDLE message)
{
    AMQP_VALUE result;
    if (user_ctx == NULL)
    {
        LogError("on_message_recv_callback was invoked with a NULL context");
        result = messaging_delivery_rejected("SDK error", "user context NULL");
    }
    else
    {
        MESSAGE_BODY_TYPE body_type;
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)user_ctx;

        if (message_get_body_type(message, &body_type) != 0)
        {
            LogError("message body type has failed");
            amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            amqp_info->amqp_state = AMQP_STATE_ERROR;
        }
        else if (body_type != MESSAGE_BODY_TYPE_DATA)
        {
            LogError("invalid message type of %d", body_type);
            amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            amqp_info->amqp_state = AMQP_STATE_ERROR;
        }
        else
        {
            BINARY_DATA binary_data;
            if (amqp_info->payload_data != NULL)
            {
                free(amqp_info->payload_data);
                amqp_info->payload_data = NULL;
            }

            if (message_get_body_amqp_data_in_place(message, 0, &binary_data) != 0)
            {
                LogError("failure getting message from amqp body");
                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                amqp_info->amqp_state = AMQP_STATE_ERROR;
            }
            else if ((amqp_info->payload_data = malloc(binary_data.length + 1)) == NULL)
            {
                LogError("failure allocating payload data");
                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                amqp_info->amqp_state = AMQP_STATE_ERROR;
            }
            else
            {
                // Get the retry after field on failures this value will
                // be set to the default value
                (void)get_retry_after_property(amqp_info, message);

                memset(amqp_info->payload_data, 0, binary_data.length + 1);
                memcpy(amqp_info->payload_data, binary_data.bytes, binary_data.length);
                if (amqp_info->transport_state == TRANSPORT_CLIENT_STATE_REG_SENT)
                {
                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_REG_RECV;
                }
                else
                {
                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_STATUS_RECV;
                }
            }
        }
        result = messaging_delivery_accepted();
    }
    return result;
}

static int set_amqp_msg_property(AMQP_VALUE uamqp_prop_map, const char* key, const char* value)
{
    int result;
    AMQP_VALUE uamqp_key;
    AMQP_VALUE uamqp_value;
    if ((uamqp_key = amqpvalue_create_string(key)) == NULL)
    {
        LogError("Failed to create property key string.");
        result = MU_FAILURE;
    }
    else if ((uamqp_value = amqpvalue_create_string(value)) == NULL)
    {
        amqpvalue_destroy(uamqp_key);
        LogError("Failed to create property value string.");
        result = MU_FAILURE;
    }
    else
    {
        if (amqpvalue_set_map_value(uamqp_prop_map, uamqp_key, uamqp_value) != 0)
        {
            LogError("Failed to set map value.");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
        amqpvalue_destroy(uamqp_key);
        amqpvalue_destroy(uamqp_value);
    }
    return result;
}

static char* construct_json_data(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    char* result;
    bool data_success = true;
    STRING_HANDLE encoded_srk = NULL;
    STRING_HANDLE encoded_ek = NULL;
    const char* ek_value = NULL;
    const char* srk_value = NULL;

    // For TPM we need to add tpm encoded values
    if (amqp_info->hsm_type == TRANSPORT_HSM_TYPE_TPM)
    {
        if ((encoded_ek = Azure_Base64_Encode(amqp_info->ek)) == NULL)
        {
            LogError("Failure encoding ek");
            data_success = false;
        }
        else if ((encoded_srk = Azure_Base64_Encode(amqp_info->srk)) == NULL)
        {
            LogError("Failure encoding srk");
            data_success = false;
        }
        else
        {
            ek_value = STRING_c_str(encoded_ek);
            srk_value = STRING_c_str(encoded_srk);
        }
    }

    if (data_success)
    {
        result = amqp_info->json_create_cb(ek_value, srk_value, amqp_info->json_ctx);
    }
    else
    {
        result = NULL;
    }

    if (encoded_srk != NULL)
    {
        STRING_delete(encoded_srk);
    }
    if (encoded_ek != NULL)
    {
        STRING_delete(encoded_ek);
    }
    return result;
}

static int send_amqp_message(PROV_TRANSPORT_AMQP_INFO* amqp_info, const char* msg_type)
{
    int result;
    MESSAGE_HANDLE uamqp_message;
    char* json_data;

    if ((uamqp_message = message_create()) == NULL)
    {
        LogError("Failed allocating the uAMQP message.");
        result = MU_FAILURE;
    }
    else if ((json_data = construct_json_data(amqp_info)) == NULL)
    {
        LogError("Failed constructing json payload");
        message_destroy(uamqp_message);
        result = MU_FAILURE;
    }
    else
    {
        AMQP_VALUE uamqp_prop_map;

        BINARY_DATA binary_data;
        binary_data.bytes = (const unsigned char*)json_data;
        binary_data.length = strlen(json_data);

        if ((uamqp_prop_map = amqpvalue_create_map()) == NULL)
        {
            LogError("Failed to create uAMQP map for the properties.");
            result = MU_FAILURE;
        }
        else if (message_add_body_amqp_data(uamqp_message, binary_data) != 0)
        {
            LogError("Failed to adding amqp msg body.");
            amqpvalue_destroy(uamqp_prop_map);
            result = MU_FAILURE;
        }
        else if (set_amqp_msg_property(uamqp_prop_map, AMQP_OP_TYPE_PROPERTY, msg_type) != 0)
        {
            LogError("Failed to adding amqp type property to message.");
            result = MU_FAILURE;
            message_destroy(uamqp_message);
            amqpvalue_destroy(uamqp_prop_map);
        }
        else
        {
            if (amqp_info->operation_id != NULL)
            {
                if (set_amqp_msg_property(uamqp_prop_map, AMQP_OPERATION_ID, amqp_info->operation_id) != 0)
                {
                    LogError("Failed to adding operation status property to message.");
                    result = MU_FAILURE;
                    message_destroy(uamqp_message);
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                result = 0;
            }

            if (result == 0)
            {
                if (message_set_application_properties(uamqp_message, uamqp_prop_map) != 0)
                {
                    LogError("Failed to set map value.");
                    result = MU_FAILURE;
                }
                else if (messagesender_send_async(amqp_info->msg_sender, uamqp_message, on_amqp_send_complete, amqp_info, 0) == NULL)
                {
                    LogError("Failed to send message.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
        }
        amqpvalue_destroy(uamqp_prop_map);
        message_destroy(uamqp_message);
        free(json_data);
    }
    return result;
}

static int add_link_properties(LINK_HANDLE amqp_link, const char* key, const char* value)
{
    fields attach_properties;
    AMQP_VALUE device_client_type_key_name;
    AMQP_VALUE device_client_type_value;
    int result;

    if ((attach_properties = amqpvalue_create_map()) == NULL)
    {
        LogError("Failed to create the map for device client type.");
        result = MU_FAILURE;
    }
    else
    {
        if ((device_client_type_key_name = amqpvalue_create_symbol(key)) == NULL)
        {
            LogError("Failed to create the key name for the device client type.");
            result = MU_FAILURE;
        }
        else
        {
            if ((device_client_type_value = amqpvalue_create_string(value)) == NULL)
            {
                LogError("Failed to create the key value for the device client type.");
                result = MU_FAILURE;
            }
            else
            {
                if ((result = amqpvalue_set_map_value(attach_properties, device_client_type_key_name, device_client_type_value)) != 0)
                {
                    LogError("Failed to set the property map for the device client type (error code is: %d)", result);
                    result = MU_FAILURE;
                }
                else if ((result = link_set_attach_properties(amqp_link, attach_properties)) != 0)
                {
                    LogError("Unable to attach the device client type to the link properties (error code is: %d)", result);
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
                amqpvalue_destroy(device_client_type_value);
            }
            amqpvalue_destroy(device_client_type_key_name);
        }
        amqpvalue_destroy(attach_properties);
    }
    return result;
}

static int create_sender_link(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    int result;
    AMQP_VALUE msg_source;
    AMQP_VALUE msg_target;
    STRING_HANDLE event_address;

    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_050: [ The reciever and sender endpoints addresses shall be constructed in the following manner: amqps://[hostname]/[scope_id]/registrations/[registration_id] ] */
    if ((event_address = construct_link_address(amqp_info)) == NULL)
    {
        LogError("Failure constructing amqp link address");
        result = MU_FAILURE;
    }
    else
    {
        if ((msg_source = messaging_create_source("remote_endpoint")) == NULL)
        {
            LogError("Failure retrieving messaging create source");
            result = MU_FAILURE;
        }
        else if ((msg_target = messaging_create_target(STRING_c_str(event_address))) == NULL)
        {
            LogError("Failure creating messaging target");
            amqpvalue_destroy(msg_source);
            result = MU_FAILURE;
        }
        else if ((amqp_info->sender_link = link_create(amqp_info->session, "sender_link", role_sender, msg_source, msg_target)) == NULL)
        {
            LogError("Failure creating link");
            amqpvalue_destroy(msg_source);
            amqpvalue_destroy(msg_target);
            result = MU_FAILURE;
        }
        else
        {
            if (add_link_properties(amqp_info->sender_link, AMQP_API_VERSION_KEY, PROV_API_VERSION))
            {
                LogError("Failure adding link property");
                link_destroy(amqp_info->sender_link);
                amqp_info->sender_link = NULL;
                result = MU_FAILURE;
            }
            else if (link_set_max_message_size(amqp_info->sender_link, AMQP_MAX_SENDER_MSG_SIZE) != 0)
            {
                LogError("Failure setting sender link max size");
                link_destroy(amqp_info->sender_link);
                amqp_info->sender_link = NULL;
                result = MU_FAILURE;
            }
            else if ((amqp_info->msg_sender = messagesender_create(amqp_info->sender_link, on_message_sender_state_changed_callback, amqp_info)) == NULL)
            {
                LogError("Failure creating message sender");
                link_destroy(amqp_info->sender_link);
                amqp_info->sender_link = NULL;
                result = MU_FAILURE;
            }
            else if (messagesender_open(amqp_info->msg_sender) != 0)
            {
                LogError("Failure opening message sender");
                messagesender_destroy(amqp_info->msg_sender);
                amqp_info->msg_sender = NULL;
                link_destroy(amqp_info->sender_link);
                amqp_info->sender_link = NULL;
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
            amqpvalue_destroy(msg_target);
            amqpvalue_destroy(msg_source);
        }
        STRING_delete(event_address);
    }
    return result;
}

static int create_receiver_link(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    int result;
    AMQP_VALUE msg_source;
    AMQP_VALUE msg_target;
    STRING_HANDLE event_address;

    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_050: [ The reciever and sender endpoints addresses shall be constructed in the following manner: amqps://[hostname]/[scope_id]/registrations/[registration_id] ] */
    if ((event_address = construct_link_address(amqp_info)) == NULL)
    {
        LogError("Failure constructing amqp link address");
        result = MU_FAILURE;
    }
    else
    {
        if ((msg_source = messaging_create_source(STRING_c_str(event_address))) == NULL)
        {
            LogError("Failure retrieving messaging create source");
            result = MU_FAILURE;
        }
        else if ((msg_target = messaging_create_target("local_endpoint")) == NULL)
        {
            LogError("Failure creating messaging target");
            amqpvalue_destroy(msg_source);
            result = MU_FAILURE;
        }
        else if ((amqp_info->receiver_link = link_create(amqp_info->session, "recv_link", role_receiver, msg_source, msg_target)) == NULL)
        {
            LogError("Failure creating link");
            amqpvalue_destroy(msg_source);
            amqpvalue_destroy(msg_target);
            result = MU_FAILURE;
        }
        else
        {
            link_set_rcv_settle_mode(amqp_info->receiver_link, receiver_settle_mode_first);
            if (add_link_properties(amqp_info->receiver_link, AMQP_API_VERSION_KEY, PROV_API_VERSION))
            {
                LogError("Failure adding link property");
                link_destroy(amqp_info->sender_link);
                amqp_info->sender_link = NULL;
                result = MU_FAILURE;
            }
            else if (link_set_max_message_size(amqp_info->receiver_link, AMQP_MAX_RECV_MSG_SIZE) != 0)
            {
                LogError("Failure setting max message size");
                link_destroy(amqp_info->receiver_link);
                amqp_info->receiver_link = NULL;
                result = MU_FAILURE;
            }
            else if ((amqp_info->msg_receiver = messagereceiver_create(amqp_info->receiver_link, on_message_receiver_state_changed_callback, (void*)amqp_info)) == NULL)
            {
                LogError("Failure creating message receiver");
                link_destroy(amqp_info->receiver_link);
                amqp_info->receiver_link = NULL;
                result = MU_FAILURE;
            }
            else if (messagereceiver_open(amqp_info->msg_receiver, on_message_recv_callback, (void*)amqp_info) != 0)
            {
                LogError("Failure opening message receiver");
                link_destroy(amqp_info->receiver_link);
                amqp_info->receiver_link = NULL;
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            amqpvalue_destroy(msg_source);
            amqpvalue_destroy(msg_target);
        }
        STRING_delete(event_address);
    }
    return result;
}

static int create_transport_io_object(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    int result;
    if (amqp_info->underlying_io == NULL)
    {
        HTTP_PROXY_OPTIONS* transport_proxy;
        SASL_MECHANISM_HANDLE* sasl_mechanism = NULL;
        PROV_TRANSPORT_IO_INFO* trans_info;

        // Set the proxy options if neccessary
        if (amqp_info->proxy_option.host_address != NULL)
        {
            transport_proxy = &amqp_info->proxy_option;
        }
        else
        {
            transport_proxy = NULL;
        }

        if (amqp_info->hsm_type == TRANSPORT_HSM_TYPE_TPM || amqp_info->hsm_type == TRANSPORT_HSM_TYPE_SYMM_KEY)
        {
            sasl_mechanism = &amqp_info->sasl_handler;
        }

        if ((trans_info = amqp_info->transport_io_cb(amqp_info->hostname, sasl_mechanism, transport_proxy)) == NULL)
        {
            LogError("Failure calling transport_io callback");
            result = MU_FAILURE;
        }
        else
        {
            if (trans_info->sasl_handle == NULL)
            {
                amqp_info->underlying_io = trans_info->transport_handle;
            }
            else
            {
                amqp_info->transport_io = trans_info->transport_handle;
                amqp_info->underlying_io = trans_info->sasl_handle;
            }
            free(trans_info);
            result = 0;
        }
    }
    else
    {
        result = 0;
    }
    return result;
}

static int create_amqp_connection(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    int result;

    if (create_transport_io_object(amqp_info) != 0)
    {
        LogError("Failure setting transport object");
        result = MU_FAILURE;
    }
    else
    {
        if (amqp_info->hsm_type == TRANSPORT_HSM_TYPE_X509)
        {
            if (amqp_info->x509_cert != NULL && amqp_info->private_key != NULL)
            {
                if (xio_setoption(amqp_info->underlying_io, OPTION_X509_ECC_CERT, amqp_info->x509_cert) != 0)
                {
                    LogError("Failure setting x509 cert on xio");
                    result = MU_FAILURE;
                    xio_destroy(amqp_info->underlying_io);
                    amqp_info->underlying_io = NULL;
                }
                else if (xio_setoption(amqp_info->underlying_io, OPTION_X509_ECC_KEY, amqp_info->private_key) != 0)
                {
                    LogError("Failure setting x509 key on xio");
                    if (amqp_info->error_cb != NULL)
                    {
                        amqp_info->error_cb(PROV_DEVICE_ERROR_KEY_FAIL, amqp_info->error_ctx);
                    }
                    result = MU_FAILURE;
                    xio_destroy(amqp_info->underlying_io);
                    amqp_info->underlying_io = NULL;
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                LogError("x509 certificate is NULL");
                result = MU_FAILURE;
                xio_destroy(amqp_info->underlying_io);
                amqp_info->underlying_io = NULL;
            }
        }
        else
        {
            result = 0;
        }

        if (result == 0)
        {
            if (amqp_info->certificate != NULL && xio_setoption(amqp_info->underlying_io, OPTION_TRUSTED_CERT, amqp_info->certificate) != 0)
            {
                LogError("Failure setting trusted certs");
                result = MU_FAILURE;
                xio_destroy(amqp_info->underlying_io);
                amqp_info->underlying_io = NULL;
            }
            else if ((amqp_info->connection = connection_create(amqp_info->underlying_io, amqp_info->hostname, "prov_connection", NULL, NULL)) == NULL)
            {
                LogError("failed creating amqp connection");
                xio_destroy(amqp_info->underlying_io);
                amqp_info->underlying_io = NULL;
                result = MU_FAILURE;
            }
            else
            {
                connection_set_trace(amqp_info->connection, amqp_info->log_trace);
                result = 0;
            }
        }
    }
    return result;
}

static char* construct_username(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    char* result;
    size_t length;

    length = strlen(AMQP_SASL_USERNAME_FMT) + strlen(amqp_info->scope_id) + strlen(amqp_info->registration_id);
    if ((result = malloc(length + 1)) == NULL)
    {
        LogError("Failure allocating username");
    }
    else if (sprintf(result, AMQP_SASL_USERNAME_FMT, amqp_info->scope_id, amqp_info->registration_id) <= 0)
    {
        LogError("Failure creating mqtt username");
        free(result);
        result = NULL;
    }
    return result;
}


static int create_sasl_handler(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    int result;
    const SASL_MECHANISM_INTERFACE_DESCRIPTION* sasl_interface;

    if (amqp_info->hsm_type == TRANSPORT_HSM_TYPE_TPM)
    {
        SASL_TPM_CONFIG_INFO sasl_config;
        sasl_config.registration_id = amqp_info->registration_id;
        sasl_config.scope_id = amqp_info->scope_id;
        sasl_config.storage_root_key = amqp_info->srk;
        sasl_config.endorsement_key = amqp_info->ek;
        sasl_config.hostname = amqp_info->hostname;
        sasl_config.challenge_cb = on_sasl_tpm_challenge_cb;
        sasl_config.user_ctx = amqp_info;

        // Create TPM SASL handler
        if ((sasl_interface = prov_sasltpm_get_interface()) == NULL)
        {
            LogError("prov_sasltpm_get_interface was NULL");
            result = MU_FAILURE;
        }
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_048: [ If the hsm_type is TRANSPORT_HSM_TYPE_TPM create_connection shall create a tpm_saslmechanism. ] */
        else if ((amqp_info->sasl_handler = saslmechanism_create(sasl_interface, &sasl_config)) == NULL)
        {
            LogError("failed creating tpm sasl mechanism");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
    }
    else if (amqp_info->hsm_type == TRANSPORT_HSM_TYPE_SYMM_KEY)
    {
        SASL_PLAIN_CONFIG sasl_config;
        char* sasl_username;
        char* sas_token;

        if ((sasl_username = construct_username(amqp_info)) == NULL)
        {
            LogError("failed creating symmetical sasl username");
            result = MU_FAILURE;
        }
        else if ((sas_token = amqp_info->challenge_cb(NULL, 0, KEY_NAME_VALUE, amqp_info->challenge_ctx)) == NULL)
        {
            LogError("failed creating symmetical sasl token");
            result = MU_FAILURE;
            free(sasl_username);
        }
        else
        {
            sasl_config.authcid = sasl_username;
            sasl_config.passwd = sas_token;
            sasl_config.authzid = NULL;

            if ((sasl_interface = saslplain_get_interface()) == NULL)
            {
                LogError("prov_saslplain_get_interface was NULL");
                result = MU_FAILURE;
            }
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_048: [ If the hsm_type is TRANSPORT_HSM_TYPE_TPM create_connection shall create a tpm_saslmechanism. ] */
            else if ((amqp_info->sasl_handler = saslmechanism_create(sasl_interface, &sasl_config)) == NULL)
            {
                LogError("failed creating plain sasl mechanism");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
            free(sasl_username);
            free(sas_token);
        }
    }
    else
    {
        result = 0;
    }
    return result;
}

static int create_connection(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    int result;

    if (create_sasl_handler(amqp_info) != 0)
    {
        LogError("failed creating amqp Sasl handler");
        result = MU_FAILURE;
    }
    else if (create_amqp_connection(amqp_info) != 0)
    {
        LogError("failed creating amqp connection");
        saslmechanism_destroy(amqp_info->sasl_handler);
        amqp_info->sasl_handler = NULL;
        result = MU_FAILURE;
    }
    else
    {
        if ((amqp_info->session = session_create(amqp_info->connection, NULL, NULL)) == NULL)
        {
            LogError("failed creating amqp session");
            saslmechanism_destroy(amqp_info->sasl_handler);
            amqp_info->sasl_handler = NULL;
            result = MU_FAILURE;
        }
        else
        {
            (void)session_set_incoming_window(amqp_info->session, 2147483647);
            (void)session_set_outgoing_window(amqp_info->session, 65536);

            if (create_receiver_link(amqp_info) != 0)
            {
                LogError("failure creating amqp receiver link");
                saslmechanism_destroy(amqp_info->sasl_handler);
                amqp_info->sasl_handler = NULL;
                session_destroy(amqp_info->session);
                amqp_info->session = NULL;
                result = MU_FAILURE;
            }
            else if (create_sender_link(amqp_info) != 0)
            {
                LogError("failure creating amqp sender link");
                saslmechanism_destroy(amqp_info->sasl_handler);
                amqp_info->sasl_handler = NULL;
                session_destroy(amqp_info->session);
                amqp_info->session = NULL;
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

static void free_json_parse_info(PROV_JSON_INFO* parse_info)
{
    switch (parse_info->prov_status)
    {
        case PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED:
            BUFFER_delete(parse_info->authorization_key);
            free(parse_info->key_name);
            break;
        case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNED:
            BUFFER_delete(parse_info->authorization_key);
            free(parse_info->iothub_uri);
            free(parse_info->device_id);
            break;
        case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING:
            free(parse_info->operation_id);
            break;
        default:
            break;
    }
    free(parse_info);
}

void cleanup_amqp_data(PROV_TRANSPORT_AMQP_INFO* amqp_info)
{
    free(amqp_info->hostname);
    free(amqp_info->registration_id);
    free(amqp_info->operation_id);
    free(amqp_info->api_version);
    free(amqp_info->scope_id);
    free(amqp_info->certificate);
    free((char*)amqp_info->proxy_option.host_address);
    free((char*)amqp_info->proxy_option.username);
    free((char*)amqp_info->proxy_option.password);
    free(amqp_info->x509_cert);
    free(amqp_info->private_key);
    free(amqp_info->sas_token);
    free(amqp_info->payload_data);
    if (amqp_info->transport_io != NULL)
    {
        xio_destroy(amqp_info->transport_io);
    }
    if (amqp_info->underlying_io != NULL)
    {
        xio_destroy(amqp_info->underlying_io);
    }
    free(amqp_info);
}

PROV_DEVICE_TRANSPORT_HANDLE prov_transport_common_amqp_create(const char* uri, TRANSPORT_HSM_TYPE type, const char* scope_id, const char* api_version, PROV_AMQP_TRANSPORT_IO transport_io, PROV_TRANSPORT_ERROR_CALLBACK error_cb, void* error_ctx)
{
    PROV_TRANSPORT_AMQP_INFO* result;
    if (uri == NULL || scope_id == NULL || api_version == NULL || transport_io == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_001: [ If uri, scope_id, registration_id, api_version, or transport_io is NULL, prov_transport_common_amqp_create shall return NULL. ] */
        LogError("Invalid parameter specified uri: %p, scope_id: %p, api_version: %p, transport_io: %p", uri, scope_id, api_version, transport_io);
        result = NULL;
    }
    else
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_003: [ prov_transport_common_amqp_create shall allocate a PROV_TRANSPORT_AMQP_INFO structure ] */
        result = malloc(sizeof(PROV_TRANSPORT_AMQP_INFO));
        if (result == NULL)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_002: [ If any error is encountered, prov_transport_common_amqp_create shall return NULL. ] */
            LogError("Unable to allocate PROV_TRANSPORT_AMQP_INFO");
        }
        else
        {
            memset(result, 0, sizeof(PROV_TRANSPORT_AMQP_INFO));
            if (mallocAndStrcpy_s(&result->hostname, uri) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_002: [ If any error is encountered, prov_transport_common_amqp_create shall return NULL. ] */
                LogError("Failure allocating hostname");
                free(result);
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&result->api_version, api_version) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_002: [ If any error is encountered, prov_transport_common_amqp_create shall return NULL. ] */
                LogError("Failure allocating api_version");
                cleanup_amqp_data(result);
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&result->scope_id, scope_id) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_002: [ If any error is encountered, prov_transport_common_amqp_create shall return NULL. ] */
                LogError("Failure allocating scope id");
                cleanup_amqp_data(result);
                result = NULL;
            }
            else
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_004: [ On success prov_transport_common_amqp_create shall allocate a new instance of PROV_DEVICE_TRANSPORT_HANDLE. ] */
                result->transport_io_cb = transport_io;
                result->hsm_type = type;
                result->error_cb = error_cb;
                result->error_ctx = error_ctx;
                result->retry_after_value = PROV_GET_THROTTLE_TIME;
            }
        }
    }
    return result;
}

void prov_transport_common_amqp_destroy(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_005: [ If handle is NULL, prov_transport_common_amqp_destroy shall do nothing. ] */
    if (handle != NULL)
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_006: [ prov_transport_common_amqp_destroy shall free all resources used in this module. ] */
        cleanup_amqp_data(amqp_info);
    }
}

int prov_transport_common_amqp_open(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* registration_id, BUFFER_HANDLE ek, BUFFER_HANDLE srk, PROV_DEVICE_TRANSPORT_REGISTER_CALLBACK data_callback, void* user_ctx, PROV_DEVICE_TRANSPORT_STATUS_CALLBACK status_cb, void* status_ctx, PROV_TRANSPORT_CHALLENGE_CALLBACK reg_challenge_cb, void* challenge_ctx)
{
    int result;
    PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
    if (amqp_info == NULL || data_callback == NULL || status_cb == NULL || registration_id == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_007: [ If handle, data_callback, or status_cb is NULL, prov_transport_common_amqp_open shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, data_callback: %p, status_cb: %p, registration_id: %p", handle, data_callback, status_cb, registration_id);
        result = MU_FAILURE;
    }
    else if ((amqp_info->hsm_type == TRANSPORT_HSM_TYPE_TPM || amqp_info->hsm_type == TRANSPORT_HSM_TYPE_SYMM_KEY) && reg_challenge_cb == NULL)
    {
        LogError("registration challenge callback must be set");
        result = MU_FAILURE;
    }
    else if ((amqp_info->hsm_type == TRANSPORT_HSM_TYPE_TPM) && (ek == NULL || srk == NULL) )
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_008: [ If hsm_type is TRANSPORT_HSM_TYPE_TPM and ek or srk is NULL, prov_transport_common_amqp_open shall return a non-zero value. ] */
        LogError("Invalid parameter specified ek: %p, srk: %p", ek, srk);
        result = MU_FAILURE;
    }
    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_009: [ prov_transport_common_amqp_open shall clone the ek and srk values.] */
    else if (ek != NULL && (amqp_info->ek = BUFFER_clone(ek)) == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_041: [ If a failure is encountered, prov_transport_common_amqp_open shall return a non-zero value. ] */
        LogError("Unable to allocate endorsement key");
        result = MU_FAILURE;
    }
    else if (srk != NULL && (amqp_info->srk = BUFFER_clone(srk)) == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_041: [ If a failure is encountered, prov_transport_common_amqp_open shall return a non-zero value. ] */
        LogError("Unable to allocate storage root key");
        BUFFER_delete(amqp_info->ek);
        amqp_info->ek = NULL;
        result = MU_FAILURE;
    }
    else if (mallocAndStrcpy_s(&amqp_info->registration_id, registration_id) != 0)
    {
        /* Codes_PROV_TRANSPORT_HTTP_CLIENT_07_003: [ If any error is encountered prov_transport_http_create shall return NULL. ] */
        LogError("failure constructing registration Id");
        BUFFER_delete(amqp_info->ek);
        amqp_info->ek = NULL;
        BUFFER_delete(amqp_info->srk);
        amqp_info->srk = NULL;
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_010: [ When complete prov_transport_common_amqp_open shall send a status callback of PROV_DEVICE_TRANSPORT_STATUS_CONNECTED.] */
        amqp_info->register_data_cb = data_callback;
        amqp_info->user_ctx = user_ctx;
        amqp_info->status_cb = status_cb;
        amqp_info->status_ctx = status_ctx;
        amqp_info->challenge_cb = reg_challenge_cb;
        amqp_info->challenge_ctx = challenge_ctx;
        amqp_info->amqp_state = AMQP_STATE_DISCONNECTED;
        result = 0;
    }
    return result;
}

int prov_transport_common_amqp_close(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_011: [ If handle is NULL, prov_transport_common_amqp_close shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        BUFFER_delete(amqp_info->ek);
        amqp_info->ek = NULL;
        BUFFER_delete(amqp_info->srk);
        amqp_info->srk = NULL;

        free(amqp_info->registration_id);
        amqp_info->registration_id = NULL;

        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_012: [ prov_transport_common_amqp_close shall close all links and connection associated with amqp communication. ] */
        if (amqp_info->msg_sender != NULL)
        {
            messagesender_close(amqp_info->msg_sender);
        }
        if (amqp_info->msg_receiver != NULL)
        {
            messagereceiver_close(amqp_info->msg_receiver);
        }
        if (amqp_info->msg_sender != NULL)
        {
            messagesender_destroy(amqp_info->msg_sender);
            amqp_info->msg_sender = NULL;
        }
        if (amqp_info->msg_receiver != NULL)
        {
            messagereceiver_destroy(amqp_info->msg_receiver);
            amqp_info->msg_receiver = NULL;
        }

        // Link
        if (amqp_info->receiver_link != NULL)
        {
            link_destroy(amqp_info->receiver_link);
            amqp_info->receiver_link = NULL;
        }
        if (amqp_info->sender_link != NULL)
        {
            link_destroy(amqp_info->sender_link);
            amqp_info->sender_link = NULL;
        }
        if (amqp_info->sasl_handler != NULL)
        {
            saslmechanism_destroy(amqp_info->sasl_handler);
            amqp_info->sasl_handler = NULL;
        }
        if (amqp_info->session != NULL)
        {
            session_destroy(amqp_info->session);
            amqp_info->session = NULL;
        }
        if (amqp_info->connection != NULL)
        {
            connection_destroy(amqp_info->connection);
            amqp_info->connection = NULL;
        }
        xio_destroy(amqp_info->transport_io);
        amqp_info->transport_io = NULL;
        xio_destroy(amqp_info->underlying_io);
        amqp_info->underlying_io = NULL;

        amqp_info->amqp_state = AMQP_STATE_IDLE;
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_013: [ On success prov_transport_common_amqp_close shall return a zero value. ] */
        result = 0;
    }
    return result;
}

int prov_transport_common_amqp_register_device(PROV_DEVICE_TRANSPORT_HANDLE handle, PROV_TRANSPORT_JSON_PARSE json_parse_cb, PROV_TRANSPORT_CREATE_JSON_PAYLOAD json_create_cb, void* json_ctx)
{
    int result;
    PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
    if (amqp_info == NULL || json_parse_cb == NULL || json_create_cb == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_014: [ If handle is NULL, prov_transport_common_amqp_register_device shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, json_parse_cb: %p", handle, json_parse_cb);
        result = MU_FAILURE;
    }
    else if (amqp_info->transport_state == TRANSPORT_CLIENT_STATE_REG_SEND || amqp_info->operation_id != NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_061: [ If the transport_state is TRANSPORT_CLIENT_STATE_REG_SEND or the the operation_id is NULL, prov_transport_common_amqp_register_device shall return a non-zero value. ] */
        LogError("Failure: device is currently in the registration process");
        result = MU_FAILURE;
    }
    else if (amqp_info->transport_state == TRANSPORT_CLIENT_STATE_ERROR)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_016: [ If the transport_state is set to TRANSPORT_CLIENT_STATE_ERROR shall, prov_transport_common_amqp_register_device shall return a non-zero value. ] */
        LogError("Provisioning is in an error state, close the connection and try again.");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_017: [ On success prov_transport_common_amqp_register_device shall return a zero value. ] */
        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_REG_SEND;
        amqp_info->json_parse_cb = json_parse_cb;
        amqp_info->json_create_cb = json_create_cb;
        amqp_info->json_ctx = json_ctx;

        result = 0;
    }
    return result;
}

int prov_transport_common_amqp_get_operation_status(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_018: [ If handle is NULL, prov_transport_common_amqp_get_operation_status shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        if (amqp_info->operation_id == NULL)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_019: [ If the operation_id is NULL, prov_transport_common_amqp_get_operation_status shall return a non-zero value. ] */
            LogError("operation_id was not previously set in the challenge method");
            result = MU_FAILURE;
        }
        else if (amqp_info->transport_state == TRANSPORT_CLIENT_STATE_ERROR)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_020: [ If the transport_state is set to TRANSPORT_CLIENT_STATE_ERROR shall, prov_transport_common_amqp_get_operation_status shall return a non-zero value. ] */
            LogError("provisioning is in an error state, close the connection and try again.");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_021: [ prov_transport_common_amqp_get_operation_status shall set the transport_state to TRANSPORT_CLIENT_STATE_STATUS_SEND. ] */
            amqp_info->transport_state = TRANSPORT_CLIENT_STATE_STATUS_SEND;
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_022: [ On success prov_transport_common_amqp_get_operation_status shall return a zero value. ] */
            result = 0;
        }
    }
    return result;
}

void prov_transport_common_amqp_dowork(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_046: [ If handle is NULL, prov_transport_common_amqp_dowork shall do nothing. ] */
    if (handle != NULL)
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        if (amqp_info->amqp_state == AMQP_STATE_DISCONNECTED)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_047: [ If the amqp_state is AMQP_STATE_DISCONNECTED prov_transport_common_amqp_dowork shall attempt to connect the amqp connections and links. ] */
            if (create_connection(amqp_info) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_049: [ If any error is encountered prov_transport_common_amqp_dowork shall set the amqp_state to AMQP_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                LogError("unable to create amqp connection");
                amqp_info->amqp_state = AMQP_STATE_ERROR;
                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            }
            else
            {
                amqp_info->amqp_state = AMQP_STATE_CONNECTING;
            }
        }
        else if (amqp_info->amqp_state != AMQP_STATE_IDLE)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_051: [ Once connected prov_transport_common_amqp_dowork shall call uamqp connection dowork function and check the `transport_state` ] */
            connection_dowork(amqp_info->connection);
            if (amqp_info->amqp_state == AMQP_STATE_CONNECTED || amqp_info->amqp_state == AMQP_STATE_ERROR)
            {
                switch (amqp_info->transport_state)
                {
                case TRANSPORT_CLIENT_STATE_REG_SEND:
                    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_053: [ When then transport_state is set to TRANSPORT_CLIENT_STATE_REG_SEND, prov_transport_common_amqp_dowork shall send a AMQP_REGISTER_ME message ] */
                    if (send_amqp_message(amqp_info, AMQP_REGISTER_ME) != 0)
                    {
                        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_049: [ If any error is encountered prov_transport_common_amqp_dowork shall set the amqp_state to AMQP_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                        amqp_info->amqp_state = AMQP_STATE_ERROR;
                    }
                    else
                    {
                        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_054: [ Upon successful sending of a TRANSPORT_CLIENT_STATE_REG_SEND message, prov_transport_common_amqp_dowork shall set the transport_state to TRANSPORT_CLIENT_STATE_REG_SENT ] */
                        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_REG_SENT;
                    }
                    break;

                case TRANSPORT_CLIENT_STATE_REG_RECV:
                case TRANSPORT_CLIENT_STATE_STATUS_RECV:
                {
                    PROV_JSON_INFO* parse_info = amqp_info->json_parse_cb(amqp_info->payload_data, amqp_info->json_ctx);
                    if (parse_info == NULL)
                    {
                        LogError("Unable to process registration reply.");
                        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                        amqp_info->amqp_state = AMQP_STATE_ERROR;
                    }
                    else
                    {
                        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                        switch (parse_info->prov_status)
                        {
                            case PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED:
                            case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING:
                                if (parse_info->operation_id == NULL)
                                {
                                    LogError("Failure operation Id invalid");
                                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                                    amqp_info->amqp_state = AMQP_STATE_ERROR;
                                }
                                else if (amqp_info->operation_id == NULL && mallocAndStrcpy_s(&amqp_info->operation_id, parse_info->operation_id) != 0)
                                {
                                    LogError("Failure copying operation Id");
                                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                                    amqp_info->amqp_state = AMQP_STATE_ERROR;
                                }
                                else
                                {
                                    if (amqp_info->status_cb != NULL)
                                    {
                                        amqp_info->status_cb(parse_info->prov_status, amqp_info->retry_after_value, amqp_info->status_ctx);
                                    }
                                }
                                break;

                            case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNED:
                                amqp_info->register_data_cb(PROV_DEVICE_TRANSPORT_RESULT_OK, parse_info->authorization_key, parse_info->iothub_uri, parse_info->device_id, amqp_info->user_ctx);
                                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                                break;
                            case PROV_DEVICE_TRANSPORT_STATUS_ERROR:
                            default:
                                LogError("Unable to process message reply.");
                                amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                                amqp_info->amqp_state = AMQP_STATE_ERROR;
                                break;

                        }
                        free_json_parse_info(parse_info);
                    }
                    break;
                }

                case TRANSPORT_CLIENT_STATE_STATUS_SEND:
                    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_055: [ When then transport_state is set to TRANSPORT_CLIENT_STATE_STATUS_SEND, prov_transport_common_amqp_dowork shall send a AMQP_OPERATION_STATUS message ] */
                    if (send_amqp_message(amqp_info, AMQP_OPERATION_STATUS) != 0)
                    {
                        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_049: [ If any error is encountered prov_transport_common_amqp_dowork shall set the amqp_state to AMQP_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                        amqp_info->amqp_state = AMQP_STATE_ERROR;
                    }
                    else
                    {
                        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_056: [ Upon successful sending of a AMQP_OPERATION_STATUS message, prov_transport_common_amqp_dowork shall set the transport_state to TRANSPORT_CLIENT_STATE_STATUS_SENT ] */
                        amqp_info->transport_state = TRANSPORT_CLIENT_STATE_STATUS_SENT;
                    }
                    break;

                case TRANSPORT_CLIENT_STATE_ERROR:
                    /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_057: [ If transport_state is set to TRANSPORT_CLIENT_STATE_ERROR, prov_transport_common_amqp_dowork shall call the register_data_cb function with PROV_DEVICE_TRANSPORT_RESULT_ERROR setting the transport_state to TRANSPORT_CLIENT_STATE_IDLE. ] */
                    amqp_info->register_data_cb(PROV_DEVICE_TRANSPORT_RESULT_ERROR, NULL, NULL, NULL, amqp_info->user_ctx);
                    amqp_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                    amqp_info->amqp_state = AMQP_STATE_IDLE;
                    break;
                case TRANSPORT_CLIENT_STATE_REG_SENT:
                case TRANSPORT_CLIENT_STATE_STATUS_SENT:
                    // Check timout
                    break;
                case TRANSPORT_CLIENT_STATE_IDLE:
                default:
                    break;
                }
            }
        }
    }
}

int prov_transport_common_amqp_set_trace(PROV_DEVICE_TRANSPORT_HANDLE handle, bool trace_on)
{
    int result;
    if (handle == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_023: [ If handle is NULL, prov_transport_common_amqp_set_trace shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_024: [ prov_transport_common_amqp_set_trace shall set the log_trace variable to trace_on. ]*/
        amqp_info->log_trace = trace_on;

        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_059: [ If the uamqp connection is not NULL, prov_transport_common_amqp_set_trace shall set the connection trace option on that connection. ] */
        if (amqp_info->connection != NULL)
        {
            connection_set_trace(amqp_info->connection, amqp_info->log_trace);
        }

        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_025: [ On success prov_transport_common_amqp_set_trace shall return a zero value. ] */
        result = 0;
    }
    return result;
}

int prov_transport_common_amqp_x509_cert(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* certificate, const char* private_key)
{
    int result;
    if (handle == NULL || certificate == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_026: [ If handle or certificate is NULL, prov_transport_common_amqp_x509_cert shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, certificate: %p", handle, certificate);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;

        if (amqp_info->x509_cert != NULL)
        {
            free(amqp_info->x509_cert);
            amqp_info->x509_cert = NULL;
        }
        if (amqp_info->private_key != NULL)
        {
            free(amqp_info->private_key);
            amqp_info->private_key = NULL;
        }

        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_027: [ prov_transport_common_amqp_x509_cert shall copy the certificate and private_key values. ] */
        if (mallocAndStrcpy_s(&amqp_info->x509_cert, certificate) != 0)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_028: [ On any failure prov_transport_common_amqp_x509_cert, shall return a non-zero value. ] */
            result = MU_FAILURE;
            LogError("failure allocating certificate");
        }
        else if (mallocAndStrcpy_s(&amqp_info->private_key, private_key) != 0)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_028: [ On any failure prov_transport_common_amqp_x509_cert, shall return a non-zero value. ] */
            LogError("failure allocating certificate");
            free(amqp_info->x509_cert);
            amqp_info->x509_cert = NULL;
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_029: [ On success prov_transport_common_amqp_x509_cert shall return a zero value. ] */
            result = 0;
        }
    }
    return result;
}

int prov_transport_common_amqp_set_trusted_cert(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* certificate)
{
    int result;
    if (handle == NULL || certificate == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_030: [ If handle or certificate is NULL, prov_transport_common_amqp_set_trusted_cert shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, certificate: %p", handle, certificate);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;

        if (amqp_info->certificate != NULL)
        {
            free(amqp_info->certificate);
            amqp_info->certificate = NULL;
        }

        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_031: [ prov_transport_common_amqp_set_trusted_cert shall copy the certificate value. ] */
        if (mallocAndStrcpy_s(&amqp_info->certificate, certificate) != 0)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_032: [ On any failure prov_transport_common_amqp_set_trusted_cert, shall return a non-zero value. ] */
            result = MU_FAILURE;
            LogError("failure allocating certificate");
        }
        else
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_033: [ On success prov_transport_common_amqp_set_trusted_cert shall return a zero value. ] */
            result = 0;
        }
    }
    return result;
}

int prov_transport_common_amqp_set_proxy(PROV_DEVICE_TRANSPORT_HANDLE handle, const HTTP_PROXY_OPTIONS* proxy_options)
{
    int result;
    if (handle == NULL || proxy_options == NULL)
    {
        /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_034: [ If handle or proxy_options is NULL, prov_transport_common_amqp_set_proxy shall return a non-zero value. ]*/
        LogError("Invalid parameter specified handle: %p, proxy_options: %p", handle, proxy_options);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        if (proxy_options->host_address == NULL)
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_035: [ If HTTP_PROXY_OPTIONS host_address is NULL, prov_transport_common_amqp_set_proxy shall return a non-zero value. ] */
            LogError("NULL host_address in proxy options");
            result = MU_FAILURE;
        }
        else if (((proxy_options->username == NULL) || (proxy_options->password == NULL)) &&
            (proxy_options->username != proxy_options->password))
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_036: [ If HTTP_PROXY_OPTIONS password is not NULL and password is NULL, prov_transport_common_amqp_set_proxy shall return a non-zero value. ] */
            LogError("Only one of username and password for proxy settings was NULL");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_037: [ If any of the host_addess, username, or password variables are non-NULL, prov_transport_common_amqp_set_proxy shall free the memory. ] */
            if (amqp_info->proxy_option.host_address != NULL)
            {
                free((char*)amqp_info->proxy_option.host_address);
                amqp_info->proxy_option.host_address = NULL;
            }
            if (amqp_info->proxy_option.username != NULL)
            {
                free((char*)amqp_info->proxy_option.username);
                amqp_info->proxy_option.username = NULL;
            }
            if (amqp_info->proxy_option.password != NULL)
            {
                free((char*)amqp_info->proxy_option.password);
                amqp_info->proxy_option.password = NULL;
            }

            /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_038: [ prov_transport_common_amqp_set_proxy shall copy the host_addess, username, or password variables ] */
            amqp_info->proxy_option.port = proxy_options->port;
            if (mallocAndStrcpy_s((char**)&amqp_info->proxy_option.host_address, proxy_options->host_address) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_039: [ On any failure prov_transport_common_amqp_set_proxy, shall return a non-zero value. ] */
                LogError("Failure setting proxy_host name");
                result = MU_FAILURE;
            }
            else if (proxy_options->username != NULL && mallocAndStrcpy_s((char**)&amqp_info->proxy_option.username, proxy_options->username) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_039: [ On any failure prov_transport_common_amqp_set_proxy, shall return a non-zero value. ] */
                LogError("Failure setting proxy username");
                free((char*)amqp_info->proxy_option.host_address);
                amqp_info->proxy_option.host_address = NULL;
                result = MU_FAILURE;
            }
            else if (proxy_options->password != NULL && mallocAndStrcpy_s((char**)&amqp_info->proxy_option.password, proxy_options->password) != 0)
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_039: [ On any failure prov_transport_common_amqp_set_proxy, shall return a non-zero value. ] */
                LogError("Failure setting proxy password");
                free((char*)amqp_info->proxy_option.host_address);
                amqp_info->proxy_option.host_address = NULL;
                free((char*)amqp_info->proxy_option.username);
                amqp_info->proxy_option.username = NULL;
                result = MU_FAILURE;
            }
            else
            {
                /* Codes_PROV_TRANSPORT_AMQP_COMMON_07_040: [ On success prov_transport_common_amqp_set_proxy shall return a zero value. ] */
                result = 0;
            }
        }
    }
    return result;
}

int prov_transport_common_amqp_set_option(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    int result;
    if (handle == NULL || option == NULL)
    {
        LogError("Invalid parameter specified handle: %p, option: %p", handle, option);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_AMQP_INFO* amqp_info = (PROV_TRANSPORT_AMQP_INFO*)handle;
        if (amqp_info->underlying_io == NULL && create_transport_io_object(amqp_info) != 0)
        {
            LogError("Failure creating transport io object");
            result = MU_FAILURE;
        }
        else
        {
            result = xio_setoption(amqp_info->underlying_io, option, value);
        }
    }
    return result;
}
