// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <inttypes.h>

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/http_proxy_io.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/http_proxy_io.h"

#include "azure_prov_client/internal/prov_transport_mqtt_common.h"
#include "azure_umqtt_c/mqtt_client.h"

#include "azure_prov_client/prov_client_const.h"

#define SUBSCRIBE_TOPIC_COUNT       1

static const char* const MQTT_SUBSCRIBE_TOPIC = "$dps/registrations/res/#";
static const char* const MQTT_USERNAME_FMT = "%s/registrations/%s/api-version=%s&ClientVersion=%s";
static const char* const MQTT_REGISTER_MESSAGE_FMT = "$dps/registrations/PUT/iotdps-register/?$rid=%d";
static const char* const MQTT_STATUS_MESSAGE_FMT = "$dps/registrations/GET/iotdps-get-operationstatus/?$rid=%d&operationId=%s";
static const char* const MQTT_TOPIC_STATUS_PREFIX = "$dps/registrations/res/";
static const char* const KEY_NAME_VALUE = "registration";

typedef enum MQTT_TRANSPORT_STATE_TAG
{
    MQTT_STATE_IDLE,
    MQTT_STATE_DISCONNECTED,
    MQTT_STATE_CONNECTING,
    MQTT_STATE_CONNECTED,

    MQTT_STATE_SUBSCRIBING,
    MQTT_STATE_SUBSCRIBED,

    MQTT_STATE_ERROR
} MQTT_TRANSPORT_STATE;

typedef enum PROV_TRANSPORT_STATE_TAG
{
    TRANSPORT_CLIENT_STATE_IDLE,

    TRANSPORT_CLIENT_STATE_REG_SEND,
    TRANSPORT_CLIENT_STATE_REG_SENT,
    TRANSPORT_CLIENT_STATE_REG_RECV,

    TRANSPORT_CLIENT_STATE_STATUS_SEND,
    TRANSPORT_CLIENT_STATE_STATUS_SENT,
    TRANSPORT_CLIENT_STATE_STATUS_RECV,

    TRANSPORT_CLIENT_STATE_TRANSIENT,
    TRANSPORT_CLIENT_STATE_ERROR
} PROV_TRANSPORT_STATE;

typedef struct PROV_TRANSPORT_MQTT_INFO_TAG
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

    MQTT_CLIENT_HANDLE mqtt_client;

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

    uint16_t packet_id;

    TRANSPORT_HSM_TYPE hsm_type;

    PROV_MQTT_TRANSPORT_IO transport_io_cb;

    PROV_TRANSPORT_STATE transport_state;
    MQTT_TRANSPORT_STATE mqtt_state;

    XIO_HANDLE transport_io;
    uint32_t retry_after_value;

    PROV_TRANSPORT_ERROR_CALLBACK error_cb;
    void* error_ctx;
} PROV_TRANSPORT_MQTT_INFO;

static uint16_t get_next_packet_id(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    if (mqtt_info->packet_id + 1 >= USHRT_MAX)
    {
        mqtt_info->packet_id = 1;
    }
    else
    {
        mqtt_info->packet_id++;
    }
    return mqtt_info->packet_id;
}

static void mqtt_error_callback(MQTT_CLIENT_HANDLE handle, MQTT_CLIENT_EVENT_ERROR error, void* user_ctx)
{
    (void)handle;
    if (user_ctx != NULL)
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)user_ctx;
        switch (error)
        {
            case MQTT_CLIENT_CONNECTION_ERROR:
            case MQTT_CLIENT_COMMUNICATION_ERROR:
                LogError("MQTT communication error");
                break;
            case MQTT_CLIENT_NO_PING_RESPONSE:
                LogError("Mqtt Ping Response was not encountered.  Reconnecting device...");
                break;

            case MQTT_CLIENT_PARSE_ERROR:
            case MQTT_CLIENT_MEMORY_ERROR:
            case MQTT_CLIENT_UNKNOWN_ERROR:
            default:
            {
                LogError("INTERNAL ERROR: unexpected error value received %d", error);
                break;
            }
        }
        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
        mqtt_info->mqtt_state = MQTT_STATE_ERROR;
    }
    else
    {
        LogError("mqtt_error_callback was invoked with a NULL context");
    }
}

#ifndef NO_LOGGING
static const char* retrieve_mqtt_return_codes(CONNECT_RETURN_CODE rtn_code)
{
    switch (rtn_code)
    {
    case CONNECTION_ACCEPTED:
        return "Accepted";
    case CONN_REFUSED_UNACCEPTABLE_VERSION:
        return "Unacceptable Version";
    case CONN_REFUSED_ID_REJECTED:
        return "Id Rejected";
    case CONN_REFUSED_SERVER_UNAVAIL:
        return "Server Unavailable";
    case CONN_REFUSED_BAD_USERNAME_PASSWORD:
        return "Bad Username/Password";
    case CONN_REFUSED_NOT_AUTHORIZED:
        return "Not Authorized";
    case CONN_REFUSED_UNKNOWN:
    default:
        return "Unknown";
    }
}
#endif // NO_LOGGING

static void mqtt_operation_complete_callback(MQTT_CLIENT_HANDLE handle, MQTT_CLIENT_EVENT_RESULT event_result, const void* msg_info, void* user_ctx)
{
    (void)handle;
    (void)msg_info;
    if (user_ctx != NULL)
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)user_ctx;
        switch (event_result)
        {
            case MQTT_CLIENT_ON_CONNACK:
            {
                const CONNECT_ACK* connack = (const CONNECT_ACK*)msg_info;
                if (connack != NULL)
                {
                    if (connack->returnCode == CONNECTION_ACCEPTED)
                    {
                        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_052: [ Once the mqtt CONNACK is recieved prov_transport_common_mqtt_dowork shall set mqtt_state to MQTT_STATE_CONNECTED ] */
                        mqtt_info->mqtt_state = MQTT_STATE_CONNECTED;
                    }
                    else
                    {
                        LogError("Connection Not Accepted: 0x%x: %s", connack->returnCode, retrieve_mqtt_return_codes(connack->returnCode));
                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                        mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                        if (mqtt_info->error_cb != NULL)
                        {
                            mqtt_info->error_cb(PROV_DEVICE_ERROR_KEY_UNAUTHORIZED, mqtt_info->error_ctx);
                        }
                    }
                }
                else
                {
                    mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                    mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                    LogError("CONNECT_ACK packet is NULL");
                }
                break;
            }
            case MQTT_CLIENT_ON_SUBSCRIBE_ACK:
            {
                const SUBSCRIBE_ACK* suback = (const SUBSCRIBE_ACK*)msg_info;
                if (suback != NULL)
                {
                    size_t index = 0;
                    for (index = 0; index < suback->qosCount; index++)
                    {
                        if (suback->qosReturn[index] == DELIVER_FAILURE)
                        {
                            LogError("Subscribe delivery failure of subscribe %lu", (unsigned long)index);
                            break;
                        }
                    }

                    if (index == suback->qosCount)
                    {
                        mqtt_info->mqtt_state = MQTT_STATE_SUBSCRIBED;
                    }
                    else
                    {
                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                        mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                    }
                }
                else
                {
                    mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                    mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                    LogError("SUBSCRIBE_ACK packet is NULL");
                }
                break;
            }
            case MQTT_CLIENT_ON_PUBLISH_ACK:
            case MQTT_CLIENT_ON_PUBLISH_COMP:
            case MQTT_CLIENT_ON_PUBLISH_RECV:
            case MQTT_CLIENT_ON_PUBLISH_REL:
            case MQTT_CLIENT_ON_DISCONNECT:
            case MQTT_CLIENT_ON_UNSUBSCRIBE_ACK:
                break;
            default:
                LogError("Unknown MQTT_CLIENT_EVENT_RESULT item %d", (int)event_result);
                break;
        }
    }
    else
    {
        LogError("mqtt_operation_complete_callback was invoked with a NULL context");
    }
}

static int get_retry_after_property(const char* topic_name, PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result = MU_FAILURE;

    const char* iterator = topic_name;

    size_t topic_len = strlen(iterator);
    size_t retry_len = strlen(RETRY_AFTER_KEY_VALUE);
    while (iterator != NULL && *iterator != '\0')
    {
        if (topic_len > retry_len)
        {
            if (memcmp(iterator, RETRY_AFTER_KEY_VALUE, retry_len) == 0)
            {
                // send the retry-after value to parse
                mqtt_info->retry_after_value = parse_retry_after_value(iterator + retry_len + 1);
                result = 0;
                break;
            }
        }
        else
        {
            // Topic string is not there
            result = MU_FAILURE;
            break;
        }
        iterator++;
        topic_len--;
    }
    return result;
}

static void mqtt_notification_callback(MQTT_MESSAGE_HANDLE handle, void* user_ctx)
{
    if (user_ctx != NULL)
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)user_ctx;

        bool is_transient_error = false;
        const char* topic_resp = mqttmessage_getTopicName(handle);
        if (topic_resp != NULL)
        {
            // Extract the registration status
            size_t status_pos = strlen(MQTT_TOPIC_STATUS_PREFIX);
            if (memcmp(MQTT_TOPIC_STATUS_PREFIX, topic_resp, status_pos) == 0)
            {
                // If the status code is >= 429 then this is a transient error
                long status_code = atol(topic_resp + status_pos);
                if (status_code >= PROV_STATUS_CODE_TRANSIENT_ERROR)
                {
                    // On transient error reset the transport to send state
                    mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_TRANSIENT;
                    is_transient_error = true;
                }
            }

            // Get the retry after field on failures this value will
            // be set to the default value
            (void)get_retry_after_property(topic_resp, mqtt_info);
        }
        else
        {
            LogError("failure topic name is NULL");
            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            mqtt_info->mqtt_state = MQTT_STATE_ERROR;
        }

        if (!is_transient_error)
        {
            const APP_PAYLOAD* payload = mqttmessage_getApplicationMsg(handle);
            if (payload != NULL)
            {
                if (mqtt_info->payload_data != NULL)
                {
                    free(mqtt_info->payload_data);
                    mqtt_info->payload_data = NULL;
                }

                if ((mqtt_info->payload_data = malloc(payload->length + 1)) == NULL)
                {
                    LogError("failure allocating payload data");
                    mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                    mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                }
                else
                {
                    memset(mqtt_info->payload_data, 0, payload->length + 1);
                    memcpy(mqtt_info->payload_data, payload->message, payload->length);
                    if (mqtt_info->transport_state == TRANSPORT_CLIENT_STATE_REG_SENT)
                    {
                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_REG_RECV;
                    }
                    else
                    {
                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_STATUS_RECV;
                    }
                }
            }
            else
            {
                LogError("failure NULL message encountered from umqtt");
                mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                mqtt_info->mqtt_state = MQTT_STATE_ERROR;
            }
        }
    }
    else
    {
        LogError("mqtt_notification_callback was invoked with a NULL context");
    }
}

static int send_mqtt_message(PROV_TRANSPORT_MQTT_INFO* mqtt_info, const char* msg_topic)
{
    int result;
    MQTT_MESSAGE_HANDLE msg_handle = NULL;
    char* prov_payload;

    if ((prov_payload = mqtt_info->json_create_cb(NULL, NULL, mqtt_info->json_ctx)) == NULL)
    {
        LogError("Failed creating json mqtt payload");
        result = MU_FAILURE;
    }
    else if ((msg_handle = mqttmessage_create_in_place(get_next_packet_id(mqtt_info), msg_topic, DELIVER_AT_MOST_ONCE, (const uint8_t*)prov_payload, strlen(prov_payload))) == NULL)
    {
        LogError("Failed creating mqtt message");
        result = MU_FAILURE;
        free(prov_payload);
    }
    else
    {
        if (mqtt_client_publish(mqtt_info->mqtt_client, msg_handle) != 0)
        {
            LogError("Failed publishing client message");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
        mqttmessage_destroy(msg_handle);
        free(prov_payload);
    }
    return result;
}

static int send_register_message(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result;
    char* msg_topic;

    size_t length = strlen(MQTT_REGISTER_MESSAGE_FMT) + 8;
    if ((msg_topic = malloc(length + 1)) == NULL)
    {
        LogError("Failed allocating mqtt registration message");
        result = MU_FAILURE;
    }
    else if (sprintf(msg_topic, MQTT_REGISTER_MESSAGE_FMT, mqtt_info->packet_id) <= 0)
    {
        LogError("Failed setting registration message");
        free(msg_topic);
        result = MU_FAILURE;
    }
    else
    {
        result = send_mqtt_message(mqtt_info, msg_topic);
        free(msg_topic);
    }
    return result;
}

static int send_operation_status_message(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result;
    char* msg_topic;

    size_t length = strlen(MQTT_STATUS_MESSAGE_FMT) + strlen(mqtt_info->operation_id) + 8;
    if ((msg_topic = malloc(length + 1)) == NULL)
    {
        LogError("Failed allocating mqtt status message");
        result = MU_FAILURE;
    }
    else if (sprintf(msg_topic, MQTT_STATUS_MESSAGE_FMT, mqtt_info->packet_id, mqtt_info->operation_id) <= 0)
    {
        LogError("Failed creating mqtt status message");
        free(msg_topic);
        result = MU_FAILURE;
    }
    else
    {
        result = send_mqtt_message(mqtt_info, msg_topic);
        free(msg_topic);
    }
    return result;
}

static int subscribe_to_topic(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result;
    SUBSCRIBE_PAYLOAD subscribe[SUBSCRIBE_TOPIC_COUNT];
    subscribe[0].subscribeTopic = MQTT_SUBSCRIBE_TOPIC;
    subscribe[0].qosReturn = DELIVER_AT_LEAST_ONCE;

    if (mqtt_client_subscribe(mqtt_info->mqtt_client, get_next_packet_id(mqtt_info), subscribe, SUBSCRIBE_TOPIC_COUNT) != 0)
    {
        LogError("Failed subscribing to topic.");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

static char* construct_username(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    char* result;
    size_t length;

    length = strlen(MQTT_USERNAME_FMT) + strlen(mqtt_info->registration_id) + strlen(mqtt_info->scope_id) + strlen(mqtt_info->api_version) + strlen(PROV_DEVICE_CLIENT_VERSION);
    if ((result = malloc(length + 1)) == NULL)
    {
        LogError("Failure allocating username");
        result = NULL;
    }
    else if (sprintf(result, MQTT_USERNAME_FMT, mqtt_info->scope_id, mqtt_info->registration_id, mqtt_info->api_version, PROV_DEVICE_CLIENT_VERSION) <= 0)
    {
        LogError("Failure creating mqtt username");
        free(result);
        result = NULL;
    }
    return result;
}

static int create_transport_io_object(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result;
    if (mqtt_info->transport_io == NULL)
    {
        HTTP_PROXY_OPTIONS* transport_proxy;
        if (mqtt_info->proxy_option.host_address != NULL)
        {
            transport_proxy = &mqtt_info->proxy_option;
        }
        else
        {
            transport_proxy = NULL;
        }

        if ((mqtt_info->transport_io = mqtt_info->transport_io_cb(mqtt_info->hostname, transport_proxy)) == NULL)
        {
            LogError("Failure calling transport_io callback");
            result = MU_FAILURE;
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
    return result;
}

static int construct_transport(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result;

    if (create_transport_io_object(mqtt_info) != 0)
    {
        LogError("Failed constructing transport io");
        result = MU_FAILURE;
    }
    else
    {
        if (mqtt_info->certificate != NULL && xio_setoption(mqtt_info->transport_io, OPTION_TRUSTED_CERT, mqtt_info->certificate) != 0)
        {
            LogError("Failure setting trusted certs");
            result = MU_FAILURE;
            xio_destroy(mqtt_info->transport_io);
            mqtt_info->transport_io = NULL;
        }
        else if (mqtt_info->hsm_type == TRANSPORT_HSM_TYPE_X509)
        {
            if (mqtt_info->x509_cert != NULL && mqtt_info->private_key != NULL)
            {
                if (xio_setoption(mqtt_info->transport_io, OPTION_X509_ECC_CERT, mqtt_info->x509_cert) != 0)
                {
                    LogError("Failure setting x509 cert on xio");
                    xio_destroy(mqtt_info->transport_io);
                    mqtt_info->transport_io = NULL;
                    result = MU_FAILURE;
                }
                else if (xio_setoption(mqtt_info->transport_io, OPTION_X509_ECC_KEY, mqtt_info->private_key) != 0)
                {
                    LogError("Failure setting x509 key on xio");
                    if (mqtt_info->error_cb != NULL)
                    {
                        mqtt_info->error_cb(PROV_DEVICE_ERROR_KEY_FAIL, mqtt_info->error_ctx);
                    }
                    xio_destroy(mqtt_info->transport_io);
                    mqtt_info->transport_io = NULL;
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                LogError("x509 certificate is NULL");
                xio_destroy(mqtt_info->transport_io);
                mqtt_info->transport_io = NULL;
                result = MU_FAILURE;
            }
        }
        else
        {
            result = 0;
        }
    }
    return result;
}

static int create_connection(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    int result;
    MQTT_CLIENT_OPTIONS options;
    memset(&options, 0, sizeof(MQTT_CLIENT_OPTIONS));

    char* username_info;

    if ((username_info = construct_username(mqtt_info)) == NULL)
    {
        LogError("Failure creating username info");
        result = MU_FAILURE;
    }
    else if (construct_transport(mqtt_info) != 0)
    {
        LogError("Failure constructing transport");
        free(username_info);
        result = MU_FAILURE;
    }
    else if ((mqtt_info->hsm_type == TRANSPORT_HSM_TYPE_SYMM_KEY) && (options.password = mqtt_info->challenge_cb(NULL, 0, KEY_NAME_VALUE, mqtt_info->challenge_ctx)) == NULL)
    {
        LogError("Failure retrieving sas token from key");
        xio_destroy(mqtt_info->transport_io);
        mqtt_info->transport_io = NULL;
        free(username_info);
        result = MU_FAILURE;
    }
    else
    {
        (void)mqtt_client_set_trace(mqtt_info->mqtt_client, mqtt_info->log_trace, false);

        options.username = username_info;
        options.clientId = mqtt_info->registration_id;
        options.useCleanSession = 1;
        options.log_trace = mqtt_info->log_trace;
        options.qualityOfServiceValue = DELIVER_AT_LEAST_ONCE;
        if (mqtt_client_connect(mqtt_info->mqtt_client, mqtt_info->transport_io, &options) != 0)
        {
            xio_destroy(mqtt_info->transport_io);
            mqtt_info->transport_io = NULL;
            LogError("Failure connecting to mqtt server");
            result = MU_FAILURE;
        }
        else
        {
            result = 0;
        }
        if (options.password != NULL)
        {
            free(options.password);
        }
        free(username_info);
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

void cleanup_mqtt_data(PROV_TRANSPORT_MQTT_INFO* mqtt_info)
{
    free(mqtt_info->hostname);
    free(mqtt_info->registration_id);
    free(mqtt_info->operation_id);
    free(mqtt_info->api_version);
    free(mqtt_info->scope_id);
    free(mqtt_info->certificate);
    free((char*)mqtt_info->proxy_option.host_address);
    free((char*)mqtt_info->proxy_option.username);
    free((char*)mqtt_info->proxy_option.password);
    free(mqtt_info->x509_cert);
    free(mqtt_info->private_key);
    free(mqtt_info->sas_token);
    free(mqtt_info->payload_data);
    if (mqtt_info->transport_io != NULL)
    {
        xio_destroy(mqtt_info->transport_io);
    }
    free(mqtt_info);
}

PROV_DEVICE_TRANSPORT_HANDLE prov_transport_common_mqtt_create(const char* uri, TRANSPORT_HSM_TYPE type, const char* scope_id, const char* api_version, PROV_MQTT_TRANSPORT_IO transport_io, PROV_TRANSPORT_ERROR_CALLBACK error_cb, void* error_ctx)
{
    PROV_TRANSPORT_MQTT_INFO* result;
    if (uri == NULL || scope_id == NULL || api_version == NULL || transport_io == NULL)
    {
        /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_001: [ If uri, scope_id, registration_id, api_version, or transport_io is NULL, prov_transport_common_mqtt_create shall return NULL. ] */
        LogError("Invalid parameter specified uri: %p, scope_id: %p, api_version: %p, transport_io: %p", uri, scope_id, api_version, transport_io);
        result = NULL;
    }
    else if (type == TRANSPORT_HSM_TYPE_TPM)
    {
        /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_062: [ If TRANSPORT_HSM_TYPE is TRANSPORT_HSM_TYPE_TPM prov_transport_common_mqtt_create shall return NULL (currently TPM is not supported). ] */
        LogError("HSM type of TPM is not supported");
        result = NULL;
    }
    else
    {
        /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_003: [ prov_transport_common_mqtt_create shall allocate a PROV_TRANSPORT_MQTT_INFO and initialize the containing fields. ] */
        result = malloc(sizeof(PROV_TRANSPORT_MQTT_INFO));
        if (result == NULL)
        {
            /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_002: [ If any error is encountered, prov_transport_common_mqtt_create shall return NULL. ] */
            LogError("Unable to allocate PROV_TRANSPORT_MQTT_INFO");
        }
        else
        {
            memset(result, 0, sizeof(PROV_TRANSPORT_MQTT_INFO));
            if (mallocAndStrcpy_s(&result->hostname, uri) != 0)
            {
                /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_002: [ If any error is encountered, prov_transport_common_mqtt_create shall return NULL. ] */
                LogError("Failure allocating hostname");
                free(result);
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&result->api_version, api_version) != 0)
            {
                /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_002: [ If any error is encountered, prov_transport_common_mqtt_create shall return NULL. ] */
                LogError("Failure allocating api_version");
                cleanup_mqtt_data(result);
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&result->scope_id, scope_id) != 0)
            {
                /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_002: [ If any error is encountered, prov_transport_common_mqtt_create shall return NULL. ] */
                LogError("Failure allocating scope_id");
                cleanup_mqtt_data(result);
                result = NULL;
            }
            else if ((result->mqtt_client = mqtt_client_init(mqtt_notification_callback, mqtt_operation_complete_callback, result, mqtt_error_callback, result)) == NULL)
            {
                /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_002: [ If any error is encountered, prov_transport_common_mqtt_create shall return NULL. ] */
                LogError("Failed initializing mqtt client.");
                cleanup_mqtt_data(result);
                result = NULL;
            }
            else
            {
                result->transport_io_cb = transport_io;
                result->hsm_type = type;
                result->error_cb = error_cb;
                result->error_ctx = error_ctx;
                result->retry_after_value = PROV_GET_THROTTLE_TIME;
            }
        }
    }
    /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_004: [ On success prov_transport_common_mqtt_create shall return a new instance of PROV_DEVICE_TRANSPORT_HANDLE. ] */
    return result;
}

void prov_transport_common_mqtt_destroy(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_005: [ If handle is NULL, prov_transport_common_mqtt_destroy shall do nothing. ] */
    if (handle != NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_006: [ prov_transport_common_mqtt_destroy shall free all resources used in this module. ] */
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        mqtt_client_deinit(mqtt_info->mqtt_client);
        mqtt_info->mqtt_client = NULL;
        cleanup_mqtt_data(mqtt_info);
    }
}

int prov_transport_common_mqtt_open(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* registration_id, BUFFER_HANDLE ek, BUFFER_HANDLE srk, PROV_DEVICE_TRANSPORT_REGISTER_CALLBACK data_callback, void* user_ctx, PROV_DEVICE_TRANSPORT_STATUS_CALLBACK status_cb, void* status_ctx, PROV_TRANSPORT_CHALLENGE_CALLBACK reg_challenge_cb, void* challenge_ctx)
{
    int result;
    PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
    if (mqtt_info == NULL || data_callback == NULL || status_cb == NULL || registration_id == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_007: [ If handle, data_callback, or status_cb is NULL, prov_transport_common_mqtt_open shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, data_callback: %p, status_cb: %p, registration_id: %p", handle, data_callback, status_cb, registration_id);
        result = MU_FAILURE;
    }
    else if ((mqtt_info->hsm_type == TRANSPORT_HSM_TYPE_TPM || mqtt_info->hsm_type == TRANSPORT_HSM_TYPE_SYMM_KEY) && reg_challenge_cb == NULL)
    {
        LogError("registration challenge callback must be set");
        result = MU_FAILURE;
    }
    // Should never be here since TPM is not supported, so I'm going to check to ensure compliance
    else if (ek != NULL || srk != NULL)
    {
        /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_008: [ If hsm_type is TRANSPORT_HSM_TYPE_TPM and ek or srk is NULL, prov_transport_common_mqtt_open shall return a non-zero value. ] */
        LogError("Invalid parameter specified ek: %p, srk: %p", ek, srk);
        result = MU_FAILURE;
    }
    else if (mallocAndStrcpy_s(&mqtt_info->registration_id, registration_id) != 0)
    {
        /* Codes_PROV_TRANSPORT_HTTP_CLIENT_07_003: [ If any error is encountered prov_transport_http_create shall return NULL. ] */
        LogError("failure constructing registration Id");
        result = MU_FAILURE;
    }
    else
    {
        mqtt_info->register_data_cb = data_callback;
        mqtt_info->user_ctx = user_ctx;
        mqtt_info->status_cb = status_cb;
        mqtt_info->status_ctx = status_ctx;
        mqtt_info->mqtt_state = MQTT_STATE_DISCONNECTED;
        // Must add a false connect here due to the protocol quirk
        //mqtt_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_CONNECTED, mqtt_info->status_ctx);
        mqtt_info->challenge_cb = reg_challenge_cb;
        mqtt_info->challenge_ctx = challenge_ctx;

        result = 0;
    }
    return result;
}

int prov_transport_common_mqtt_close(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_011: [ If handle is NULL, prov_transport_common_mqtt_close shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        BUFFER_delete(mqtt_info->ek);
        mqtt_info->ek = NULL;
        BUFFER_delete(mqtt_info->srk);
        mqtt_info->srk = NULL;
        free(mqtt_info->registration_id);
        mqtt_info->registration_id = NULL;

        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_012: [ prov_transport_common_mqtt_close shall close all connection associated with mqtt communication. ] */
        if (mqtt_client_disconnect(mqtt_info->mqtt_client, NULL, NULL) == 0)
        {
            mqtt_client_dowork(mqtt_info->mqtt_client);
        }
        xio_destroy(mqtt_info->transport_io);
        mqtt_info->transport_io = NULL;

        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_013: [ On success prov_transport_common_mqtt_close shall return a zero value. ] */
        mqtt_info->mqtt_state = MQTT_STATE_IDLE;
        result = 0;
    }
    return result;
}

int prov_transport_common_mqtt_register_device(PROV_DEVICE_TRANSPORT_HANDLE handle, PROV_TRANSPORT_JSON_PARSE json_parse_cb, PROV_TRANSPORT_CREATE_JSON_PAYLOAD json_create_cb, void* json_ctx)
{
    int result;
    PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
    if (mqtt_info == NULL || json_parse_cb == NULL || json_create_cb == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_014: [ If handle is NULL, prov_transport_common_mqtt_register_device shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, json_parse_cb: %p", handle, json_parse_cb);
        result = MU_FAILURE;
    }
    /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_061: [ If the transport_state is TRANSPORT_CLIENT_STATE_REG_SEND or the the operation_id is NULL, prov_transport_common_mqtt_register_device shall return a non-zero value. ] */
    else if (mqtt_info->transport_state == TRANSPORT_CLIENT_STATE_REG_SEND || mqtt_info->operation_id != NULL)
    {
        LogError("Failure: device is currently in the registration process");
        result = MU_FAILURE;
    }
    else if (mqtt_info->transport_state == TRANSPORT_CLIENT_STATE_ERROR)
    {
        LogError("Provisioning is in an error state, close the connection and try again.");
        result = MU_FAILURE;
    }
    else
    {
        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_REG_SEND;
        mqtt_info->json_parse_cb = json_parse_cb;
        mqtt_info->json_create_cb = json_create_cb;
        mqtt_info->json_ctx = json_ctx;

        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_017: [ On success prov_transport_common_mqtt_register_device shall return a zero value. ] */
        result = 0;
    }
    return result;
}

int prov_transport_common_mqtt_get_operation_status(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    int result;
    if (handle == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_018: [ If handle is NULL, prov_transport_common_mqtt_get_operation_status shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        if (mqtt_info->operation_id == NULL)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_019: [ If the operation_id is NULL, prov_transport_common_mqtt_get_operation_status shall return a non-zero value. ] */
            LogError("operation_id was not previously set in the challenge method");
            result = MU_FAILURE;
        }
        else if (mqtt_info->transport_state == TRANSPORT_CLIENT_STATE_ERROR)
        {
            LogError("Provisioning is in an error state, close the connection and try again.");
            result = MU_FAILURE;
        }
        else
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_021: [ prov_transport_common_mqtt_get_operation_status shall set the transport_state to TRANSPORT_CLIENT_STATE_STATUS_SEND. ] */
            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_STATUS_SEND;
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_022: [ On success prov_transport_common_mqtt_get_operation_status shall return a zero value. ] */
            result = 0;
        }
    }
    return result;
}

void prov_transport_common_mqtt_dowork(PROV_DEVICE_TRANSPORT_HANDLE handle)
{
    /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_046: [ If handle is NULL, prov_transport_common_mqtt_dowork shall do nothing. ] */
    if (handle != NULL)
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        if (mqtt_info->mqtt_state == MQTT_STATE_DISCONNECTED)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_047: [ If the mqtt_state is MQTT_STATE_DISCONNECTED prov_transport_common_mqtt_dowork shall attempt to connect the mqtt connections. ] */
            if (create_connection(mqtt_info) != 0)
            {
                /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                LogError("unable to create mqtt connection");
                mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            }
            else
            {
                mqtt_info->mqtt_state = MQTT_STATE_CONNECTING;
            }
        }
        else if (mqtt_info->mqtt_state == MQTT_STATE_CONNECTED)
        {
            mqtt_client_dowork(mqtt_info->mqtt_client);
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_050: [ When the mqtt_state is MQTT_STATE_CONNECTED, prov_transport_common_mqtt_dowork shall subscribe to the topic $dps/registrations/res/# ] */
            if (subscribe_to_topic(mqtt_info) != 0)
            {
                /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                LogError("Failure subscribing to topic");
                mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
            }
            else
            {
                mqtt_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_CONNECTED, mqtt_info->retry_after_value, mqtt_info->status_ctx);
                mqtt_info->mqtt_state = MQTT_STATE_SUBSCRIBING;
            }
        }
        else if (mqtt_info->mqtt_state != MQTT_STATE_IDLE)
        {
            mqtt_client_dowork(mqtt_info->mqtt_client);
            if (mqtt_info->mqtt_state == MQTT_STATE_SUBSCRIBED || mqtt_info->mqtt_state == MQTT_STATE_ERROR)
            {
                switch (mqtt_info->transport_state)
                {
                    case TRANSPORT_CLIENT_STATE_REG_SEND:
                        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_053: [ When then transport_state is set to TRANSPORT_CLIENT_STATE_REG_SEND, prov_transport_common_mqtt_dowork shall send a REGISTER_ME message ] */
                        if (send_register_message(mqtt_info) != 0)
                        {
                            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                            mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                        }
                        else
                        {
                            /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_054: [ Upon successful sending of a TRANSPORT_CLIENT_STATE_REG_SEND message, prov_transport_common_mqtt_dowork shall set the transport_state to TRANSPORT_CLIENT_STATE_REG_SENT ] */
                            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_REG_SENT;
                        }
                        break;
                    case TRANSPORT_CLIENT_STATE_STATUS_SEND:
                        /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_055: [ When then transport_state is set to TRANSPORT_CLIENT_STATE_STATUS_SEND, prov_transport_common_mqtt_dowork shall send a AMQP_OPERATION_STATUS message ] */
                        if (send_operation_status_message(mqtt_info) != 0)
                        {
                            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                            mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                        }
                        else
                        {
                            /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_056: [ Upon successful sending of a AMQP_OPERATION_STATUS message, prov_transport_common_mqtt_dowork shall set the transport_state to TRANSPORT_CLIENT_STATE_STATUS_SENT ] */
                            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_STATUS_SENT;
                        }
                        break;

                    case TRANSPORT_CLIENT_STATE_REG_RECV:
                    case TRANSPORT_CLIENT_STATE_STATUS_RECV:
                    {
                        PROV_JSON_INFO* parse_info = mqtt_info->json_parse_cb(mqtt_info->payload_data, mqtt_info->json_ctx);
                        if (parse_info == NULL)
                        {
                            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                            LogError("Unable to process registration reply.");
                            mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                            mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                        }
                        else
                        {
                            switch (parse_info->prov_status)
                            {
                                case PROV_DEVICE_TRANSPORT_STATUS_UNASSIGNED:
                                case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNING:
                                    if (parse_info->operation_id == NULL)
                                    {
                                        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                                        LogError("Failure operation Id invalid");
                                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                                        mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                                    }
                                    else if (mqtt_info->operation_id == NULL && mallocAndStrcpy_s(&mqtt_info->operation_id, parse_info->operation_id) != 0)
                                    {
                                        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                                        LogError("Failure copying operation Id");
                                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                                        mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                                    }
                                    else
                                    {
                                        if (mqtt_info->status_cb != NULL)
                                        {
                                            mqtt_info->status_cb(parse_info->prov_status, mqtt_info->retry_after_value, mqtt_info->status_ctx);
                                        }
                                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                                    }
                                    break;
                                case PROV_DEVICE_TRANSPORT_STATUS_ASSIGNED:
                                    mqtt_info->register_data_cb(PROV_DEVICE_TRANSPORT_RESULT_OK, parse_info->authorization_key, parse_info->iothub_uri, parse_info->device_id, mqtt_info->user_ctx);
                                    mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                                    break;
                                case PROV_DEVICE_TRANSPORT_STATUS_TRANSIENT:
                                    break;
                                case PROV_DEVICE_TRANSPORT_STATUS_ERROR:
                                default:
                                    /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_049: [ If any error is encountered prov_transport_common_mqtt_dowork shall set the mqtt_state to MQTT_STATE_ERROR and the transport_state to TRANSPORT_CLIENT_STATE_ERROR. ] */
                                    LogError("Unable to process message reply.");
                                    mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_ERROR;
                                    mqtt_info->mqtt_state = MQTT_STATE_ERROR;
                                    break;

                            }
                            free_json_parse_info(parse_info);
                        }
                        break;
                    }
                    case TRANSPORT_CLIENT_STATE_TRANSIENT:
                        if (mqtt_info->status_cb != NULL)
                        {
                            mqtt_info->status_cb(PROV_DEVICE_TRANSPORT_STATUS_TRANSIENT, mqtt_info->retry_after_value, mqtt_info->status_ctx);
                        }
                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                        break;

                    case TRANSPORT_CLIENT_STATE_ERROR:
                        /* Codes_PROV_TRANSPORT_MQTT_COMMON_07_057: [ If transport_state is set to TRANSPORT_CLIENT_STATE_ERROR, prov_transport_common_mqtt_dowork shall call the register_data_cb function with PROV_DEVICE_TRANSPORT_RESULT_ERROR setting the transport_state to TRANSPORT_CLIENT_STATE_IDLE ] */
                        mqtt_info->register_data_cb(PROV_DEVICE_TRANSPORT_RESULT_ERROR, NULL, NULL, NULL, mqtt_info->user_ctx);
                        mqtt_info->transport_state = TRANSPORT_CLIENT_STATE_IDLE;
                        mqtt_info->mqtt_state = MQTT_STATE_IDLE;
                        break;
                    case TRANSPORT_CLIENT_STATE_REG_SENT:
                    case TRANSPORT_CLIENT_STATE_STATUS_SENT:
                        break;

                    case TRANSPORT_CLIENT_STATE_IDLE:
                    default:
                        break;
                }
            }
        }
    }
}

int prov_transport_common_mqtt_set_trace(PROV_DEVICE_TRANSPORT_HANDLE handle, bool trace_on)
{
    int result;
    if (handle == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_023: [ If handle is NULL, prov_transport_common_mqtt_set_trace shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p", handle);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_024: [ prov_transport_common_mqtt_set_trace shall set the log_trace variable to trace_on. ]*/
        mqtt_info->log_trace = trace_on;
        if (mqtt_info->mqtt_client != NULL)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_059: [ If the umqtt connection is not NULL, prov_transport_common_mqtt_set_trace shall set the trace option on that connection. ] */
            mqtt_client_set_trace(mqtt_info->mqtt_client, mqtt_info->log_trace, false);
        }
        result = 0;
    }
    return result;
}

int prov_transport_common_mqtt_x509_cert(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* certificate, const char* private_key)
{
    int result;
    if (handle == NULL || certificate == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_026: [ If handle or certificate is NULL, prov_transport_common_mqtt_x509_cert shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, certificate: %p", handle, certificate);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;

        if (mqtt_info->x509_cert != NULL)
        {
            free(mqtt_info->x509_cert);
            mqtt_info->x509_cert = NULL;
        }
        if (mqtt_info->private_key != NULL)
        {
            free(mqtt_info->private_key);
            mqtt_info->private_key = NULL;
        }

        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_027: [ prov_transport_common_mqtt_x509_cert shall copy the certificate and private_key values. ] */
        if (mallocAndStrcpy_s(&mqtt_info->x509_cert, certificate) != 0)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_028: [ On any failure prov_transport_common_mqtt_x509_cert, shall return a non-zero value. ] */
            result = MU_FAILURE;
            LogError("failure allocating certificate");
        }
        else if (mallocAndStrcpy_s(&mqtt_info->private_key, private_key) != 0)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_028: [ On any failure prov_transport_common_mqtt_x509_cert, shall return a non-zero value. ] */
            LogError("failure allocating certificate");
            free(mqtt_info->x509_cert);
            mqtt_info->x509_cert = NULL;
            result = MU_FAILURE;
        }
        else
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_029: [ On success prov_transport_common_mqtt_x509_cert shall return a zero value. ] */
            result = 0;
        }
    }
    return result;
}

int prov_transport_common_mqtt_set_trusted_cert(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* certificate)
{
    int result;
    if (handle == NULL || certificate == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_030: [ If handle or certificate is NULL, prov_transport_common_mqtt_set_trusted_cert shall return a non-zero value. ] */
        LogError("Invalid parameter specified handle: %p, certificate: %p", handle, certificate);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;

        if (mqtt_info->certificate != NULL)
        {
            free(mqtt_info->certificate);
            mqtt_info->certificate = NULL;
        }

        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_031: [ prov_transport_common_mqtt_set_trusted_cert shall copy the certificate value. ] */
        if (mallocAndStrcpy_s(&mqtt_info->certificate, certificate) != 0)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_032: [ On any failure prov_transport_common_mqtt_set_trusted_cert, shall return a non-zero value. ] */
            result = MU_FAILURE;
            LogError("failure allocating certificate");
        }
        else
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_033: [ On success prov_transport_common_mqtt_set_trusted_cert shall return a zero value. ] */
            result = 0;
        }
    }
    return result;
}

int prov_transport_common_mqtt_set_proxy(PROV_DEVICE_TRANSPORT_HANDLE handle, const HTTP_PROXY_OPTIONS* proxy_options)
{
    int result;
    if (handle == NULL || proxy_options == NULL)
    {
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_034: [ If handle or proxy_options is NULL, prov_transport_common_mqtt_set_proxy shall return a non-zero value. ]*/
        LogError("Invalid parameter specified handle: %p, proxy_options: %p", handle, proxy_options);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        if (proxy_options->host_address == NULL)
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_035: [ If HTTP_PROXY_OPTIONS host_address is NULL, prov_transport_common_mqtt_set_proxy shall return a non-zero value. ] */
            LogError("NULL host_address in proxy options");
            result = MU_FAILURE;
        }
        /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_036: [ If HTTP_PROXY_OPTIONS password is not NULL and username is NULL, prov_transport_common_mqtt_set_proxy shall return a non-zero value. ] */
        else if (((proxy_options->username == NULL) || (proxy_options->password == NULL)) &&
            (proxy_options->username != proxy_options->password))
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_039: [ On any failure prov_transport_common_mqtt_set_proxy, shall return a non-zero value. ] */
            LogError("Only one of username and password for proxy settings was NULL");
            result = MU_FAILURE;
        }
        else
        {
            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_037: [ If any of the host_addess, username, or password variables are non-NULL, prov_transport_common_mqtt_set_proxy shall free the memory. ] */
            if (mqtt_info->proxy_option.host_address != NULL)
            {
                free((char*)mqtt_info->proxy_option.host_address);
                mqtt_info->proxy_option.host_address = NULL;
            }
            if (mqtt_info->proxy_option.username != NULL)
            {
                free((char*)mqtt_info->proxy_option.username);
                mqtt_info->proxy_option.username = NULL;
            }
            if (mqtt_info->proxy_option.password != NULL)
            {
                free((char*)mqtt_info->proxy_option.password);
                mqtt_info->proxy_option.password = NULL;
            }

            /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_038: [ prov_transport_common_mqtt_set_proxy shall copy the host_addess, username, or password variables ] */
            mqtt_info->proxy_option.port = proxy_options->port;
            if (mallocAndStrcpy_s((char**)&mqtt_info->proxy_option.host_address, proxy_options->host_address) != 0)
            {
                /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_039: [ On any failure prov_transport_common_mqtt_set_proxy, shall return a non-zero value. ] */
                LogError("Failure setting proxy_host name");
                result = MU_FAILURE;
            }
            else if (proxy_options->username != NULL && mallocAndStrcpy_s((char**)&mqtt_info->proxy_option.username, proxy_options->username) != 0)
            {
                /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_039: [ On any failure prov_transport_common_mqtt_set_proxy, shall return a non-zero value. ] */
                LogError("Failure setting proxy username");
                free((char*)mqtt_info->proxy_option.host_address);
                mqtt_info->proxy_option.host_address = NULL;
                result = MU_FAILURE;
            }
            else if (proxy_options->password != NULL && mallocAndStrcpy_s((char**)&mqtt_info->proxy_option.password, proxy_options->password) != 0)
            {
                /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_039: [ On any failure prov_transport_common_mqtt_set_proxy, shall return a non-zero value. ] */
                LogError("Failure setting proxy password");
                free((char*)mqtt_info->proxy_option.host_address);
                mqtt_info->proxy_option.host_address = NULL;
                free((char*)mqtt_info->proxy_option.username);
                mqtt_info->proxy_option.username = NULL;
                result = MU_FAILURE;
            }
            else
            {
                /* Tests_PROV_TRANSPORT_MQTT_COMMON_07_040: [ On success prov_transport_common_mqtt_set_proxy shall return a zero value. ] */
                result = 0;
            }
        }
    }
    return result;
}

int prov_transport_common_mqtt_set_option(PROV_DEVICE_TRANSPORT_HANDLE handle, const char* option, const void* value)
{
    int result;
    if (handle == NULL || option == NULL)
    {
        LogError("Invalid parameter specified handle: %p, option: %p", handle, option);
        result = MU_FAILURE;
    }
    else
    {
        PROV_TRANSPORT_MQTT_INFO* mqtt_info = (PROV_TRANSPORT_MQTT_INFO*)handle;
        if (mqtt_info->transport_io == NULL && create_transport_io_object(mqtt_info) != 0)
        {
            LogError("Failure creating transport io object");
            result = MU_FAILURE;
        }
        else
        {
            result = xio_setoption(mqtt_info->transport_io, option, value);
        }
    }
    return result;
}
