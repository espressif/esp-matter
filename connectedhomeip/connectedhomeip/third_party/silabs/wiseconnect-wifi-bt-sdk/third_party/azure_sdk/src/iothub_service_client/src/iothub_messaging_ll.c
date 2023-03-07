// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <ctype.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/platform.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/shared_util_options.h"

#include "azure_uamqp_c/connection.h"
#include "azure_uamqp_c/message_receiver.h"
#include "azure_uamqp_c/message_sender.h"
#include "azure_uamqp_c/messaging.h"
#include "azure_uamqp_c/sasl_mechanism.h"
#include "azure_uamqp_c/saslclientio.h"
#include "azure_uamqp_c/sasl_plain.h"
#include "azure_uamqp_c/cbs.h"

#include "parson.h"

#include "iothub_messaging_ll.h"
#include "iothub_sc_version.h"

#define SIZE_OF_PERCENT_S_IN_FMT_STRING 2

MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_FEEDBACK_STATUS_CODE, IOTHUB_FEEDBACK_STATUS_CODE_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_MESSAGE_SEND_STATE, IOTHUB_MESSAGE_SEND_STATE_VALUES);
MU_DEFINE_ENUM_STRINGS_WITHOUT_INVALID(IOTHUB_MESSAGING_RESULT, IOTHUB_MESSAGING_RESULT_VALUES);

typedef struct CALLBACK_DATA_TAG
{
    IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCompleteCallback;
    IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback;
    IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageCallback;
    void* openUserContext;
    void* sendUserContext;
    void* feedbackUserContext;
} CALLBACK_DATA;

typedef struct IOTHUB_MESSAGING_TAG
{
    int isOpened;
    char* hostname;
    char* iothubName;
    char* iothubSuffix;
    char* sharedAccessKey;
    char* keyName;
    char* trusted_cert;

    MESSAGE_SENDER_HANDLE message_sender;
    MESSAGE_RECEIVER_HANDLE message_receiver;
    CONNECTION_HANDLE connection;
    SESSION_HANDLE session;
    LINK_HANDLE sender_link;
    LINK_HANDLE receiver_link;
    SASL_MECHANISM_HANDLE sasl_mechanism_handle;
    SASL_PLAIN_CONFIG sasl_plain_config;
    XIO_HANDLE tls_io;
    XIO_HANDLE sasl_io;

    MESSAGE_SENDER_STATE message_sender_state;
    MESSAGE_RECEIVER_STATE message_receiver_state;

    CALLBACK_DATA* callback_data;

} IOTHUB_MESSAGING;


static const char* const FEEDBACK_RECORD_KEY_DEVICE_ID = "deviceId";
static const char* const FEEDBACK_RECORD_KEY_DEVICE_GENERATION_ID = "deviceGenerationId";
static const char* const FEEDBACK_RECORD_KEY_DESCRIPTION = "description";
static const char* const FEEDBACK_RECORD_KEY_ENQUED_TIME_UTC = "enqueuedTimeUtc";
static const char* const FEEDBACK_RECORD_KEY_ORIGINAL_MESSAGE_ID = "originalMessageId";
static const char* const AMQP_ADDRESS_PATH_FMT = "/devices/%s/messages/deviceBound";
static const char* const AMQP_ADDRESS_PATH_MODULE_FMT = "/devices/%s/modules/%s/messages/deviceBound";

static int setMessageId(IOTHUB_MESSAGE_HANDLE iothub_message_handle, PROPERTIES_HANDLE uamqp_message_properties)
{
    int result;
    const char* messageId;

    if ((messageId = IoTHubMessage_GetMessageId(iothub_message_handle)) != NULL)
    {
        AMQP_VALUE uamqp_message_id;
        if ((uamqp_message_id = amqpvalue_create_string(messageId)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Failed to create an AMQP_VALUE for the messageId property value.");
            result = MU_FAILURE;
        }
        else
        {
            int api_call_result;

            /* Codes_SRS_IOTHUBMESSAGING_12_083: [ The message-id AMQP_VALUE shall be set on the uAMQP message using properties_set_message_id ] */
            if ((api_call_result = properties_set_message_id(uamqp_message_properties, uamqp_message_id)) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogInfo("Failed to set value of uAMQP message 'message-id' property (%d).", api_call_result);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
            amqpvalue_destroy(uamqp_message_id);
        }
    }
    else
    {
        result = 0;
    }

    return result;
}

static int setCorrelationId(IOTHUB_MESSAGE_HANDLE iothub_message_handle, PROPERTIES_HANDLE uamqp_message_properties)
{
    int result;
    const char* correlationId;

    if ((correlationId = IoTHubMessage_GetCorrelationId(iothub_message_handle)) != NULL)
    {
        AMQP_VALUE uamqp_correlation_id;
        if ((uamqp_correlation_id = amqpvalue_create_string(correlationId)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Failed to create an AMQP_VALUE for the messageId property value.");
            result = MU_FAILURE;
        }
        else
        {
            int api_call_result;

            /*Codes_SRS_IOTHUBMESSAGING_12_086: [ The correlation-id AMQP_VALUE shall be set on the uAMQP message using properties_set_correlation_id ] */
            if ((api_call_result = properties_set_correlation_id(uamqp_message_properties, uamqp_correlation_id)) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogInfo("Failed to set value of uAMQP message 'message-id' property (%d).", api_call_result);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
            amqpvalue_destroy(uamqp_correlation_id);
        }
    }
    else
    {
        result = 0;
    }

    return result;
}

static int addPropertiesToAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message, AMQP_VALUE to_amqp_value)
{
    int result;
    PROPERTIES_HANDLE uamqp_message_properties = NULL; /* This initialization is forced by Valgrind */
    int api_call_result;

    /*Codes_SRS_IOTHUBMESSAGING_12_079: [ The uAMQP message properties shall be retrieved using message_get_properties ] */
    if ((api_call_result = message_get_properties(uamqp_message, &uamqp_message_properties)) != 0)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Failed to get properties map from uAMQP message (error code %d).", api_call_result);
        result = MU_FAILURE;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_080: [ If UAMQP message properties were not present then a new properties container shall be created using properties_create ] */
    else if (uamqp_message_properties == NULL &&
        (uamqp_message_properties = properties_create()) == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Failed to create properties map for uAMQP message (error code %d).", api_call_result);
        result = MU_FAILURE;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_081: [ Message-id from the IOTHUB_MESSAGE shall be read using IoTHubMessage_GetMessageId ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_082: [ As message-id is optional field, if it is not set on the IOTHUB_MESSAGE, message_create_from_iothub_message shall ignore it and continue normally ] */
        if (setMessageId(iothub_message_handle, uamqp_message_properties) != 0)
        {
            LogError("Failed to set uampq messageId.");
            result = MU_FAILURE;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_084: [ Correlation-id from the IOTHUB_MESSAGE shall be read using IoTHubMessage_GetCorrelationId ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_085: [ As correlation-id is optional field, if it is not set on the IOTHUB_MESSAGE, message_create_from_iothub_message() shall ignore it and continue normally ] */
        else if (setCorrelationId(iothub_message_handle, uamqp_message_properties) != 0)
        {
            LogError("Failed to set uampq correlationId.");
            result = MU_FAILURE;
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_87: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message TO property to the given message properties by calling properties_set_to ] */
            if ((properties_set_to(uamqp_message_properties, to_amqp_value)) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not create properties for message - properties_set_to failed");
                result = MU_FAILURE;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
            else if ((api_call_result = message_set_properties(uamqp_message, uamqp_message_properties)) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Failed to set properties map on uAMQP message (error code %d).", api_call_result);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        properties_destroy(uamqp_message_properties);
    }
    return result;
}

static int addApplicationPropertiesToAMQPMessage(IOTHUB_MESSAGE_HANDLE iothub_message_handle, MESSAGE_HANDLE uamqp_message)
{
    int result;
    MAP_HANDLE properties_map;
    const char* const* propertyKeys;
    const char* const* propertyValues;
    size_t propertyCount = 0;

    /*Codes_SRS_IOTHUBMESSAGING_12_088: [ The IOTHUB_MESSAGE_HANDLE properties shall be obtained by calling IoTHubMessage_Properties ] */
    properties_map = IoTHubMessage_Properties(iothub_message_handle);

    /*Codes_SRS_IOTHUBMESSAGING_12_089: [ The actual keys and values, as well as the number of properties shall be obtained by calling Map_GetInternals on the handle obtained from IoTHubMessage_Properties ] */
    if (Map_GetInternals(properties_map, &propertyKeys, &propertyValues, &propertyCount) != MAP_OK)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Failed to get the internals of the property map.");
        result = MU_FAILURE;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_090: [ If the number of properties is greater than 0, message_create_from_iothub_message() shall iterate through all the properties and add them to the uAMQP message ] */
        if (propertyCount != 0)
        {
            AMQP_VALUE uamqp_map;

            /*Codes_SRS_IOTHUBMESSAGING_12_091: [ A uAMQP property map shall be created by calling amqpvalue_create_map ] */
            if ((uamqp_map = amqpvalue_create_map()) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Failed to create uAMQP map for the properties.");
                result = MU_FAILURE;
            }
            else
            {
                size_t i = 0;
                for (i = 0; i < propertyCount; i++)
                {
                    AMQP_VALUE map_key_value = NULL;
                    AMQP_VALUE map_value_value = NULL;

                    /*Codes_SRS_IOTHUBMESSAGING_12_092: [ An AMQP_VALUE instance shall be created using amqpvalue_create_string() to hold each uAMQP property name ] */
                    if ((map_key_value = amqpvalue_create_string(propertyKeys[i])) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Failed to create uAMQP property key name.");
                        break;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_093: [ An AMQP_VALUE instance shall be created using amqpvalue_create_string() to hold each uAMQP property value ] */
                        if ((map_value_value = amqpvalue_create_string(propertyValues[i])) == NULL)
                        {
                            /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                            LogError("Failed to create uAMQP property key value.");
                            break;
                        }
                        else
                        {
                            /*Codes_SRS_IOTHUBMESSAGING_12_094: [ The property name and value (AMQP_VALUE instances) shall be added to the uAMQP property map by calling amqpvalue_map_set_value ] */
                            if (amqpvalue_set_map_value(uamqp_map, map_key_value, map_value_value) != 0)
                            {
                                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                                LogError("Failed to set key/value into the the uAMQP property map.");
                                break;
                            }
                            amqpvalue_destroy(map_value_value);
                        }
                        /*Codes_SRS_IOTHUBMESSAGING_12_095: [ After adding the property name and value to the uAMQP property map, both AMQP_VALUE instances shall be destroyed using amqpvalue_destroy ] */
                        amqpvalue_destroy(map_key_value);
                    }
                }

                if (i == propertyCount)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_096: [ If no errors occurred processing the properties, the uAMQP properties map shall be set on the uAMQP message by calling message_set_application_properties ] */
                    if (message_set_application_properties(uamqp_message, uamqp_map) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Failed to transfer the message properties to the uAMQP message.");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
                else
                {
                    LogError("Failed to set application property into the the uAMQP property map.");
                    result = MU_FAILURE;
                }
                amqpvalue_destroy(uamqp_map);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_097: [ If the number of properties is 0, no application properties shall be set on the uAMQP message and message_create_from_iothub_message() shall return with success ] */
            result = 0;
        }
    }
    return result;
}

static int getMessageContentAndSize(IOTHUB_MESSAGE_HANDLE message, unsigned const char** messageContent, size_t* messageContentSize)
{
    int result;
    unsigned const char* contentByteArr;
    const char* contentStr;
    size_t contentSize;

    IOTHUBMESSAGE_CONTENT_TYPE contentType = IoTHubMessage_GetContentType(message);

    switch (contentType)
    {
    case IOTHUBMESSAGE_BYTEARRAY:
        if (IoTHubMessage_GetByteArray(message, &contentByteArr, &contentSize) != IOTHUB_MESSAGE_OK)
        {
            LogError("Failed getting the BYTE array representation of the IOTHUB_MESSAGE_HANDLE instance.");
            result = MU_FAILURE;
        }
        else
        {
            *messageContent = contentByteArr;
            *messageContentSize = contentSize;
            result = 0;
        }
        break;
    case IOTHUBMESSAGE_STRING:
        if ((contentStr = IoTHubMessage_GetString(message)) == NULL)
        {
            LogError("Failed getting the STRING representation of the IOTHUB_MESSAGE_HANDLE instance.");
            result = MU_FAILURE;
        }
        else
        {
            contentSize = strlen(contentStr);
            *messageContent = (unsigned const char*)contentStr;
            *messageContentSize = contentSize;
            result = 0;
        }
        break;
    default:
        LogError("Cannot parse IOTHUB_MESSAGE_HANDLE with content type IOTHUBMESSAGE_UNKNOWN.");
        result = MU_FAILURE;
        break;
    }
    return result;
}

static char* createSasToken(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->sharedAccessKey == NULL)
    {
        LogError("createSasPlainConfig failed - sharedAccessKey cannot be NULL");
        result = NULL;
    }
    else if (messagingHandle->hostname == NULL)
    {
        LogError("createSasPlainConfig failed - hostname cannot be NULL");
        result = NULL;
    }
    else if (messagingHandle->keyName == NULL)
    {
        LogError("createSasPlainConfig failed - keyName cannot be NULL");
        result = NULL;
    }
    else
    {
        STRING_HANDLE hostName = NULL;
        STRING_HANDLE sharedAccessKey = NULL;
        STRING_HANDLE keyName = NULL;

        if ((hostName = STRING_construct(messagingHandle->hostname)) == NULL)
        {
            LogError("STRING_construct failed for hostName");
            result = NULL;
        }
        else if ((sharedAccessKey = STRING_construct(messagingHandle->sharedAccessKey)) == NULL)
        {
            LogError("STRING_construct failed for sharedAccessKey");
            result = NULL;
        }
        else if ((keyName = STRING_construct(messagingHandle->keyName)) == NULL)
        {
            LogError("STRING_construct failed for keyName");
            result = NULL;
        }
        else
        {
            time_t currentTime = time(NULL);
            size_t expiry_time = (size_t)(currentTime + (365 * 24 * 60 * 60));
            const char* c_buffer = NULL;

            STRING_HANDLE sasHandle = SASToken_Create(sharedAccessKey, hostName, keyName, expiry_time);
            if (sasHandle == NULL)
            {
                LogError("SASToken_Create failed");
                result = NULL;
            }
            else if ((c_buffer = (const char*)STRING_c_str(sasHandle)) == NULL)
            {
                LogError("STRING_c_str returned NULL");
                result = NULL;
            }
            else if (mallocAndStrcpy_s(&buffer, c_buffer) != 0)
            {
                LogError("mallocAndStrcpy_s failed for sharedAccessToken");
                result = NULL;
            }
            else
            {
                result = buffer;
            }
            STRING_delete(sasHandle);
        }
        STRING_delete(keyName);
        STRING_delete(sharedAccessKey);
        STRING_delete(hostName);
    }
    return result;
}

static char* createAuthCid(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->iothubName == NULL)
    {
        LogError("createSasPlainConfig failed - iothubName cannot be NULL");
        result = NULL;
    }
    else
    {
        const char AMQP_SEND_AUTHCID_FMT[] = "%s@sas.root.%s";
        const int AMQP_SEND_AUTHCID_FMT_LENGTH = sizeof(AMQP_SEND_AUTHCID_FMT) - 2 * SIZE_OF_PERCENT_S_IN_FMT_STRING;

        size_t authCidLen = strlen(messagingHandle->keyName) + AMQP_SEND_AUTHCID_FMT_LENGTH + strlen(messagingHandle->iothubName);

        if ((buffer = (char*)malloc(authCidLen + 1)) == NULL)
        {
            LogError("Malloc failed for authCid.");
            result = NULL;
        }
        else if ((snprintf(buffer, authCidLen + 1, AMQP_SEND_AUTHCID_FMT, messagingHandle->keyName, messagingHandle->iothubName)) < 0)
        {
            LogError("sprintf_s failed for authCid.");
            free(buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static char* createReceiveTargetAddress(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->hostname == NULL)
    {
        LogError("createSendTargetAddress failed - hostname cannot be NULL");
        result = NULL;
    }
    else
    {
        const char* AMQP_SEND_TARGET_ADDRESS_FMT = "amqps://%s/messages/servicebound/feedback";
        size_t addressLen = strlen(AMQP_SEND_TARGET_ADDRESS_FMT) + strlen(messagingHandle->hostname);

        if ((buffer = (char*)malloc(addressLen + 1)) == NULL)
        {
            LogError("Malloc failed for receiveTargetAddress");
            result = NULL;
        }
        else if ((snprintf(buffer, addressLen + 1, AMQP_SEND_TARGET_ADDRESS_FMT, messagingHandle->hostname)) < 0)
        {
            LogError("sprintf_s failed for receiveTargetAddress.");
            free((char*)buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static char* createSendTargetAddress(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    char* result;

    char* buffer = NULL;
    if (messagingHandle->hostname == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_077: [ If the messagingHandle->hostname input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_INVALID_ARG ] */
        LogError("createSendTargetAddress failed - hostname cannot be NULL");
        result = NULL;
    }
    else
    {
        const char* AMQP_SEND_TARGET_ADDRESS_FMT = "amqps://%s/messages/deviceBound";
        size_t addressLen = strlen(AMQP_SEND_TARGET_ADDRESS_FMT) + strlen(messagingHandle->hostname);

        if ((buffer = (char*)malloc(addressLen + 1)) == NULL)
        {
            LogError("Malloc failed for sendTargetAddress");
            result = NULL;
        }
        else if ((snprintf(buffer, addressLen + 1, AMQP_SEND_TARGET_ADDRESS_FMT, messagingHandle->hostname)) < 0)
        {
            LogError("sprintf_s failed for sendTargetAddress.");
            free((char*)buffer);
            result = NULL;
        }
        else
        {
            result = buffer;
        }
    }
    return result;
}

static char* createDeviceDestinationString(const char* deviceId, const char* moduleId)
{
    char* result;

    if (deviceId == NULL)
    {
        LogError("createDeviceDestinationString failed - deviceId cannot be NULL");
        result = NULL;
    }
    else
    {
        size_t deviceDestLen = strlen(AMQP_ADDRESS_PATH_MODULE_FMT) + strlen(deviceId) + (moduleId == NULL ? 0 : strlen(moduleId)) + 1;

        char* buffer = (char*)malloc(deviceDestLen);
        if (buffer == NULL)
        {
            LogError("Could not create device destination string.");
            result = NULL;
        }
        else
        {
            if ((moduleId == NULL) && (snprintf(buffer, deviceDestLen, AMQP_ADDRESS_PATH_FMT, deviceId)) < 0)
            {
                LogError("sprintf_s failed for deviceDestinationString.");
                free((char*)buffer);
                result = NULL;
            }
            else if ((moduleId != NULL) && (snprintf(buffer, deviceDestLen, AMQP_ADDRESS_PATH_MODULE_FMT, deviceId, moduleId)) < 0)
            {
                LogError("sprintf_s failed for deviceDestinationString for module.");
                free((char*)buffer);
                result = NULL;
            }
            else
            {
                result = buffer;
            }
        }
    }
    return result;
}

static void IoTHubMessaging_LL_SenderStateChanged(void* context, MESSAGE_SENDER_STATE new_state, MESSAGE_SENDER_STATE previous_state)
{
    (void)previous_state;
    if (context != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_049: [ IoTHubMessaging_LL_SenderStateChanged shall save the new_state to local variable ] */
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;
        messagingData->message_sender_state = new_state;

        if ((messagingData->message_sender_state == MESSAGE_SENDER_STATE_OPEN) && (messagingData->message_receiver_state == MESSAGE_RECEIVER_STATE_OPEN))
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_050: [ If both sender and receiver state is open IoTHubMessaging_LL_SenderStateChanged shall set the isOpened local variable to true ] */
            messagingData->isOpened = true;
            if (messagingData->callback_data->openCompleteCompleteCallback != NULL)
            {
                (messagingData->callback_data->openCompleteCompleteCallback)(messagingData->callback_data->openUserContext);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_051: [ If neither sender_state nor receiver_state is open IoTHubMessaging_LL_SenderStateChanged shall set the local isOpened variable to false ] */
            messagingData->isOpened = false;
        }
    }
}

static void IoTHubMessaging_LL_ReceiverStateChanged(const void* context, MESSAGE_RECEIVER_STATE new_state, MESSAGE_RECEIVER_STATE previous_state)
{
    (void)previous_state;
    if (context != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_052: [ IoTHubMessaging_LL_ReceiverStateChanged shall save the new_state to local variable ] */
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;
        messagingData->message_receiver_state = new_state;

        if ((messagingData->message_sender_state == MESSAGE_SENDER_STATE_OPEN) && (messagingData->message_receiver_state == MESSAGE_RECEIVER_STATE_OPEN))
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_053: [ If both sender and receiver state is open IoTHubMessaging_LL_ReceiverStateChanged shall set the isOpened local variable to true ] */
            messagingData->isOpened = true;
            if (messagingData->callback_data->openCompleteCompleteCallback != NULL)
            {
                (messagingData->callback_data->openCompleteCompleteCallback)(messagingData->callback_data->openUserContext);
            }
        }
        else
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_054: [ If neither sender_state nor receiver_state is open IoTHubMessaging_LL_ReceiverStateChanged shall set the local isOpened variable to false ] */
            messagingData->isOpened = false;
        }
    }
}

static void IoTHubMessaging_LL_SendMessageComplete(void* context, MESSAGE_SEND_RESULT send_result, AMQP_VALUE delivery_state)
{
    (void)delivery_state;
    /*Codes_SRS_IOTHUBMESSAGING_12_056: [ If context is NULL IoTHubMessaging_LL_SendMessageComplete shall return ] */
    if (context != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_055: [ If context is not NULL and IoTHubMessaging_LL_SendMessageComplete shall call user callback with user context and messaging result ] */
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;
        if (messagingData->callback_data->sendCompleteCallback != NULL)
        {
            // Convert a send result to an
            IOTHUB_MESSAGING_RESULT msg_result;
            switch (send_result)
            {
                case MESSAGE_SEND_OK:
                    msg_result = IOTHUB_MESSAGING_OK;
                    break;
                case MESSAGE_SEND_ERROR:
                case MESSAGE_SEND_TIMEOUT:
                case MESSAGE_SEND_CANCELLED:
                default:
                    msg_result = IOTHUB_MESSAGING_ERROR;
                    break;
            }
            (messagingData->callback_data->sendCompleteCallback)(messagingData->callback_data->sendUserContext, msg_result);
        }
    }
}

static AMQP_VALUE IoTHubMessaging_LL_FeedbackMessageReceived(const void* context, MESSAGE_HANDLE message)
{
    AMQP_VALUE result;

    /*Codes_SRS_IOTHUBMESSAGING_12_057: [ If context is NULL IoTHubMessaging_LL_FeedbackMessageReceived shall do nothing and return delivery_accepted ] */
    if (context == NULL)
    {
        result = messaging_delivery_accepted();
    }
    else
    {
        IOTHUB_MESSAGING* messagingData = (IOTHUB_MESSAGING*)context;

        BINARY_DATA binary_data;
        JSON_Value* root_value = NULL;
        JSON_Object* feedback_object = NULL;
        JSON_Array* feedback_array = NULL;

        /*Codes_SRS_IOTHUBMESSAGING_12_058: [ If context is not NULL IoTHubMessaging_LL_FeedbackMessageReceived shall get the content string of the message by calling message_get_body_amqp_data ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_059: [ IoTHubMessaging_LL_FeedbackMessageReceived shall parse the response JSON to IOTHUB_SERVICE_FEEDBACK_BATCH struct ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_060: [ IoTHubMessaging_LL_FeedbackMessageReceived shall use the following parson APIs to parse the response string: json_parse_string, json_value_get_object, json_object_get_string, json_object_dotget_string  ] */
        if (message_get_body_amqp_data_in_place(message, 0, &binary_data) != 0)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("Cannot get message data");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed reading message body");
        }
        else if ((root_value = json_parse_string((const char*)binary_data.bytes)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("json_parse_string failed");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed parsing json root");
        }
        else if ((feedback_array = json_value_get_array(root_value)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("json_parse_string failed");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed parsing json array");
        }
        else if (json_array_get_count(feedback_array) == 0)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
            LogError("json_array_get_count failed");
            result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "json_array_get_count failed");
        }
        else
        {
            IOTHUB_SERVICE_FEEDBACK_BATCH* feedbackBatch;

            if ((feedbackBatch = (IOTHUB_SERVICE_FEEDBACK_BATCH*)malloc(sizeof(IOTHUB_SERVICE_FEEDBACK_BATCH))) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
                LogError("json_parse_string failed");
                result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed to allocate memory for feedback batch");
            }
            else
            {
                size_t array_count = 0;
                if ((array_count = json_array_get_count(feedback_array)) <= 0)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
                    LogError("json_array_get_count failed");
                    free(feedbackBatch);
                    result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "json_array_get_count failed");
                }
                else if ((feedbackBatch->feedbackRecordList = singlylinkedlist_create()) == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_061: [ If any of the parson API fails, IoTHubMessaging_LL_FeedbackMessageReceived shall return IOTHUB_MESSAGING_INVALID_JSON ] */
                    LogError("singlylinkedlist_create failed");
                    free(feedbackBatch);
                    result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "singlylinkedlist_create failed");
                }
                else
                {
                    bool isLoopFailed = false;
                    for (size_t i = 0; i < array_count; i++)
                    {
                        if ((feedback_object = json_array_get_object(feedback_array, i)) == NULL)
                        {
                            isLoopFailed = true;
                            break;
                        }
                        else
                        {
                            IOTHUB_SERVICE_FEEDBACK_RECORD* feedbackRecord;
                            if ((feedbackRecord = (IOTHUB_SERVICE_FEEDBACK_RECORD*)malloc(sizeof(IOTHUB_SERVICE_FEEDBACK_RECORD))) == NULL)
                            {
                                isLoopFailed = true;
                                break;
                            }
                            else
                            {
                                feedbackRecord->deviceId = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_DEVICE_ID);
                                feedbackRecord->generationId = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_DEVICE_GENERATION_ID);
                                feedbackRecord->description = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_DESCRIPTION);
                                feedbackRecord->enqueuedTimeUtc = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_ENQUED_TIME_UTC);
                                feedbackRecord->originalMessageId = (char*)json_object_get_string(feedback_object, FEEDBACK_RECORD_KEY_ORIGINAL_MESSAGE_ID);
                                feedbackRecord->correlationId = "";

                                if (feedbackRecord->description == NULL)
                                {
                                    feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN;
                                }
                                else
                                {
                                    size_t j;
                                    for (j = 0; feedbackRecord->description[j]; j++)
                                    {
                                        feedbackRecord->description[j] = (char)tolower(feedbackRecord->description[j]);
                                    }

                                    if (strcmp(feedbackRecord->description, "success") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_SUCCESS;
                                    }
                                    else if (strcmp(feedbackRecord->description, "expired") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_EXPIRED;
                                    }
                                    else if (strcmp(feedbackRecord->description, "deliverycountexceeded") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_DELIVER_COUNT_EXCEEDED;
                                    }
                                    else if (strcmp(feedbackRecord->description, "rejected") == 0)
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_REJECTED;
                                    }
                                    else
                                    {
                                        feedbackRecord->statusCode = IOTHUB_FEEDBACK_STATUS_CODE_UNKNOWN;
                                    }
                                }
                                if (singlylinkedlist_add(feedbackBatch->feedbackRecordList, feedbackRecord) == NULL)
                                {
                                    LogError("singlylinkedlist_add failed");
                                    free(feedbackRecord);
                                }
                            }
                        }
                    }
                    feedbackBatch->lockToken = "";
                    feedbackBatch->userId = "";

                    if (isLoopFailed)
                    {
                        LogError("Failed to read feedback records");
                        result = messaging_delivery_rejected("Rejected due to failure reading AMQP message", "Failed to read feedback records");
                    }
                    else
                    {
                        if (messagingData->callback_data->feedbackMessageCallback != NULL)
                        {
                            /*Codes_SRS_IOTHUBMESSAGING_12_062: [ If context is not NULL IoTHubMessaging_LL_FeedbackMessageReceived shall call IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK with the received IOTHUB_SERVICE_FEEDBACK_BATCH ] */
                            (messagingData->callback_data->feedbackMessageCallback)(messagingData->callback_data->feedbackUserContext, feedbackBatch);
                        }
                        result = messaging_delivery_accepted();
                    }

                    /*Codes_SRS_IOTHUBMESSAGING_12_078: [** IoTHubMessaging_LL_FeedbackMessageReceived shall do clean up before exits ] */
                    LIST_ITEM_HANDLE feedbackRecord = singlylinkedlist_get_head_item(feedbackBatch->feedbackRecordList);
                    while (feedbackRecord != NULL)
                    {
                        IOTHUB_SERVICE_FEEDBACK_RECORD* feedback = (IOTHUB_SERVICE_FEEDBACK_RECORD*)singlylinkedlist_item_get_value(feedbackRecord);
                        feedbackRecord = singlylinkedlist_get_next_item(feedbackRecord);
                        free(feedback);
                    }
                    singlylinkedlist_destroy(feedbackBatch->feedbackRecordList);
                    free(feedbackBatch);
                }
            }
        }
        json_array_clear(feedback_array);
        json_value_free(root_value);
    }
    return result;
}

IOTHUB_MESSAGING_HANDLE IoTHubMessaging_LL_Create(IOTHUB_SERVICE_CLIENT_AUTH_HANDLE serviceClientHandle)
{
    IOTHUB_MESSAGING_HANDLE result;
    CALLBACK_DATA* callback_data;

    /*Codes_SRS_IOTHUBMESSAGING_12_001: [ If the serviceClientHandle input parameter is NULL IoTHubMessaging_LL_Create shall return NULL ] */
    if (serviceClientHandle == NULL)
    {
        LogError("serviceClientHandle input parameter cannot be NULL");
        result = NULL;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_064: [ If any member of the serviceClientHandle input parameter is NULL IoTHubMessaging_LL_Create shall return NULL ] */
        IOTHUB_SERVICE_CLIENT_AUTH* serviceClientAuth = (IOTHUB_SERVICE_CLIENT_AUTH*)serviceClientHandle;

        if (serviceClientAuth->hostname == NULL)
        {
            LogError("authInfo->hostName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubName == NULL)
        {
            LogError("authInfo->iothubName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->iothubSuffix == NULL)
        {
            LogError("authInfo->iothubSuffix input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->keyName == NULL)
        {
            LogError("authInfo->keyName input parameter cannot be NULL");
            result = NULL;
        }
        else if (serviceClientAuth->sharedAccessKey == NULL)
        {
            LogError("authInfo->sharedAccessKey input parameter cannot be NULL");
            result = NULL;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_002: [ IoTHubMessaging_LL_Create shall allocate memory for a new messaging instance ] */
        else if ((result = (IOTHUB_MESSAGING*)malloc(sizeof(IOTHUB_MESSAGING))) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_003: [ If the allocation failed, IoTHubMessaging_LL_Create shall return NULL ] */
            LogError("Malloc failed for IOTHUB_REGISTRYMANAGER");
        }
        else
        {
            memset(result, 0, sizeof(IOTHUB_MESSAGING) );

            /*Codes_SRS_IOTHUBMESSAGING_12_004: [ If the allocation and creation is successful, IoTHubMessaging_LL_Create shall return with the messaging instance, a non-NULL value ] */
            /*Codes_SRS_IOTHUBMESSAGING_12_065: [ IoTHubMessaging_LL_Create shall allocate memory and copy hostName to result->hostName by calling mallocAndStrcpy_s ] */
            if (mallocAndStrcpy_s(&result->hostname, serviceClientAuth->hostname) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_066: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for hostName");
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_067: [ IoTHubMessaging_LL_Create shall allocate memory and copy iothubName to result->iothubName by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->iothubName, serviceClientAuth->iothubName) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_068: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for iothubName");
                free(result->hostname);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_069: [ IoTHubMessaging_LL_Create shall allocate memory and copy iothubSuffix to result->iothubSuffix by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->iothubSuffix, serviceClientAuth->iothubSuffix) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_070: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for iothubSuffix");
                free(result->hostname);
                free(result->iothubName);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_071: [ IoTHubMessaging_LL_Create shall allocate memory and copy sharedAccessKey to result->sharedAccessKey by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->sharedAccessKey, serviceClientAuth->sharedAccessKey) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_072: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for sharedAccessKey");
                free(result->hostname);
                free(result->iothubName);
                free(result->iothubSuffix);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_073: [ IoTHubMessaging_LL_Create shall allocate memory and copy keyName to result->keyName by calling mallocAndStrcpy_s ] */
            else if (mallocAndStrcpy_s(&result->keyName, serviceClientAuth->keyName) != 0)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_074: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("mallocAndStrcpy_s failed for keyName");
                free(result->hostname);
                free(result->iothubName);
                free(result->iothubSuffix);
                free(result->sharedAccessKey);
                free(result);
                result = NULL;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_075: [ IoTHubMessaging_LL_Create shall set messaging isOpened flag to false ] */
            else if ((callback_data = (CALLBACK_DATA*)malloc(sizeof(CALLBACK_DATA))) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_074: [ If the mallocAndStrcpy_s fails, IoTHubMessaging_LL_Create shall do clean up and return NULL ] */
                LogError("Malloc failed for callback_data");
                free(result->hostname);
                free(result->iothubName);
                free(result->iothubSuffix);
                free(result->sharedAccessKey);
                free(result->keyName);
                free(result);
                result = NULL;
            }
            else
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_076: [ If create successfull IoTHubMessaging_LL_Create shall save the callback data return the valid messaging handle ] */
                callback_data->openCompleteCompleteCallback = NULL;
                callback_data->sendCompleteCallback = NULL;
                callback_data->feedbackMessageCallback = NULL;
                callback_data->openUserContext = NULL;
                callback_data->sendUserContext = NULL;
                callback_data->feedbackUserContext = NULL;

                result->callback_data = callback_data;
            }
        }
    }
    return result;
}

void IoTHubMessaging_LL_Destroy(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_005: [ If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Destroy shall return ] */
    if (messagingHandle != NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_006: [ If the messagingHandle input parameter is not NULL IoTHubMessaging_LL_Destroy shall free all resources (memory) allocated by IoTHubMessaging_LL_Create ] */
        IOTHUB_MESSAGING* messHandle = (IOTHUB_MESSAGING*)messagingHandle;

        free(messHandle->callback_data);
        free(messHandle->hostname);
        free(messHandle->iothubName);
        free(messHandle->iothubSuffix);
        free(messHandle->sharedAccessKey);
        free(messHandle->keyName);
        free(messHandle->trusted_cert);
        free(messHandle);
    }
}

static int attachServiceClientTypeToLink(LINK_HANDLE link)
{
    fields attach_properties;
    AMQP_VALUE serviceClientTypeKeyName;
    AMQP_VALUE serviceClientTypeValue;
    int result;

    if ((attach_properties = amqpvalue_create_map()) == NULL)
    {
        LogError("Failed to create the map for service client type.");
        result = MU_FAILURE;
    }
    else
    {
        if ((serviceClientTypeKeyName = amqpvalue_create_symbol("com.microsoft:client-version")) == NULL)
        {
            LogError("Failed to create the key name for the service client type.");
            result = MU_FAILURE;
        }
        else
        {
            if ((serviceClientTypeValue = amqpvalue_create_string(IOTHUB_SERVICE_CLIENT_TYPE_PREFIX IOTHUB_SERVICE_CLIENT_BACKSLASH IOTHUB_SERVICE_CLIENT_VERSION)) == NULL)
            {
                LogError("Failed to create the key value for the service client type.");
                result = MU_FAILURE;
            }
            else
            {
                if ((result = amqpvalue_set_map_value(attach_properties, serviceClientTypeKeyName, serviceClientTypeValue)) != 0)
                {
                    LogError("Failed to set the property map for the service client type.  Error code is: %d", result);
                }
                else if ((result = link_set_attach_properties(link, attach_properties)) != 0)
                {
                    LogError("Unable to attach the service client type to the link properties. Error code is: %d", result);
                }
                else
                {
                    result = 0;
                }

                amqpvalue_destroy(serviceClientTypeValue);
            }

            amqpvalue_destroy(serviceClientTypeKeyName);
        }

        amqpvalue_destroy(attach_properties);
    }
    return result;
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Open(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_OPEN_COMPLETE_CALLBACK openCompleteCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    char* send_target_address = NULL;
    char* receive_target_address = NULL;

    TLSIO_CONFIG tls_io_config;
    SASLCLIENTIO_CONFIG sasl_io_config;

    AMQP_VALUE sendSource = NULL;
    AMQP_VALUE sendTarget = NULL;

    AMQP_VALUE receiveSource = NULL;
    AMQP_VALUE receiveTarget = NULL;

    /*Codes_SRS_IOTHUBMESSAGING_12_007: [ If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_INVALID_ARG ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_008: [ If messaging is already opened IoTHubMessaging_LL_Open return shall IOTHUB_MESSAGING_OK ] */
    else if (messagingHandle->isOpened != 0)
    {
        LogError("Messaging is already opened");
        result = IOTHUB_MESSAGING_OK;
    }
    else
    {
        messagingHandle->message_sender = NULL;
        messagingHandle->connection = NULL;
        messagingHandle->session = NULL;
        messagingHandle->sender_link = NULL;
        messagingHandle->sasl_plain_config.authzid = NULL;
        messagingHandle->sasl_mechanism_handle = NULL;
        messagingHandle->tls_io = NULL;
        messagingHandle->sasl_io = NULL;

        /*Codes_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_LL_Open shall create uAMQP messaging target for sender by calling the messaging_create_target ] */
        if ((send_target_address = createSendTargetAddress(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_077: [ If the messagingHandle->hostname input parameter is NULL IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create sendTargetAddress");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_027: [ IoTHubMessaging_LL_Open shall create uAMQP messaging source for receiver by calling the messaging_create_source ] */
        else if ((receive_target_address = createReceiveTargetAddress(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create receiveTargetAddress");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
        else if ((messagingHandle->sasl_plain_config.authcid = createAuthCid(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create authCid");
            result = IOTHUB_MESSAGING_ERROR;
        }
        /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
        else if ((messagingHandle->sasl_plain_config.passwd = createSasToken(messagingHandle)) == NULL)
        {
            /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
            LogError("Could not create sasToken");
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            const SASL_MECHANISM_INTERFACE_DESCRIPTION* sasl_mechanism_interface;

            /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
            if ((sasl_mechanism_interface = saslplain_get_interface()) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not get SASL plain mechanism interface.");
                result = IOTHUB_MESSAGING_ERROR;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_010: [ IoTHubMessaging_LL_Open shall create uAMQP PLAIN SASL mechanism by calling saslmechanism_create with the sasl plain interface ] */
            else if ((messagingHandle->sasl_mechanism_handle = saslmechanism_create(sasl_mechanism_interface, &messagingHandle->sasl_plain_config)) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not create SASL plain mechanism.");
                result = IOTHUB_MESSAGING_ERROR;
            }
            else
            {
                tls_io_config.hostname = messagingHandle->hostname;
                tls_io_config.port = 5671;
                tls_io_config.underlying_io_interface = NULL;
                tls_io_config.underlying_io_parameters = NULL;

                const IO_INTERFACE_DESCRIPTION* tlsio_interface;

                /*Codes_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_LL_Open shall create uAMQP TLSIO by calling the xio_create ] */
                if ((tlsio_interface = platform_get_default_tlsio()) == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                    LogError("Could not get default TLS IO interface.");
                    result = IOTHUB_MESSAGING_ERROR;
                }
                /*Codes_SRS_IOTHUBMESSAGING_12_011: [ IoTHubMessaging_LL_Open shall create uAMQP TLSIO by calling the xio_create ] */
                else if ((messagingHandle->tls_io = xio_create(tlsio_interface, &tls_io_config)) == NULL)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                    LogError("Could not create TLS IO.");
                    result = IOTHUB_MESSAGING_ERROR;
                }
                else if (messagingHandle->trusted_cert != NULL && xio_setoption(messagingHandle->tls_io, OPTION_TRUSTED_CERT, messagingHandle->trusted_cert) != 0)
                {
                    LogError("Could set tlsio trusted certificate.");
                    xio_destroy(messagingHandle->tls_io);
                    messagingHandle->tls_io = NULL;
                    result = IOTHUB_MESSAGING_ERROR;
                }
                else
                {
                    messagingHandle->callback_data->openCompleteCompleteCallback = openCompleteCallback;
                    messagingHandle->callback_data->openUserContext = userContextCallback;

                    sasl_io_config.sasl_mechanism = messagingHandle->sasl_mechanism_handle;
                    sasl_io_config.underlying_io = messagingHandle->tls_io;

                    const IO_INTERFACE_DESCRIPTION* saslclientio_interface;

                    /*Codes_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_LL_Open shall create uAMQP SASL IO by calling the xio_create with the previously created SASL mechanism and TLSIO] */
                    if ((saslclientio_interface = saslclientio_get_interface_description()) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create get SASL IO interface description.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_012: [ IoTHubMessaging_LL_Open shall create uAMQP SASL IO by calling the xio_create with the previously created SASL mechanism and TLSIO] */
                    else if ((messagingHandle->sasl_io = xio_create(saslclientio_interface, &sasl_io_config)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create SASL IO.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_013: [ IoTHubMessaging_LL_Open shall create uAMQP connection by calling the connection_create with the previously created SASL IO ] */
                    else if ((messagingHandle->connection = connection_create(messagingHandle->sasl_io, messagingHandle->hostname, "some", NULL, NULL)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create connection.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_014: [ IoTHubMessaging_LL_Open shall create uAMQP session by calling the session_create ] */
                    else if ((messagingHandle->session = session_create(messagingHandle->connection, NULL, NULL)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create session.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_015: [ IoTHubMessaging_LL_Open shall set the AMQP incoming window to UINT32 maximum value by calling session_set_incoming_window ] */
                    else if (session_set_incoming_window(messagingHandle->session, 2147483647) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set incoming window.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_016: [ IoTHubMessaging_LL_Open shall set the AMQP outgoing window to UINT32 maximum value by calling session_set_outgoing_window ] */
                    else if (session_set_outgoing_window(messagingHandle->session, 255 * 1024) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set outgoing window.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_021: [ IoTHubMessaging_LL_Open shall create uAMQP messaging source for sender by calling the messaging_create_source ] */
                    else if ((sendSource = messaging_create_source("ingress")) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create source for link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_022: [ IoTHubMessaging_LL_Open shall create uAMQP messaging target for sender by calling the messaging_create_target ] */
                    else if ((sendTarget = messaging_create_target(send_target_address)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create target for link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_018: [ IoTHubMessaging_LL_Open shall create uAMQP sender link by calling the link_create ] */
                    else if ((messagingHandle->sender_link = link_create(messagingHandle->session, "sender-link", role_sender, sendSource, sendTarget)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_06_001: [ IoTHubMessaging_LL_Open shall add the version property to the sender link by calling the link_set_attach_properties ] */
                    else if (attachServiceClientTypeToLink(messagingHandle->sender_link) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set the sender attach properties.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_019: [ IoTHubMessaging_LL_Open shall set the AMQP sender link settle mode to sender_settle_mode_unsettled  by calling link_set_snd_settle_mode ] */
                    else if (link_set_snd_settle_mode(messagingHandle->sender_link, sender_settle_mode_unsettled) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set the sender settle mode.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_023: [ IoTHubMessaging_LL_Open shall create uAMQP message sender by calling the messagesender_create with the created sender link and the local IoTHubMessaging_LL_SenderStateChanged callback ] */
                    else if ((messagingHandle->message_sender = messagesender_create(messagingHandle->sender_link, IoTHubMessaging_LL_SenderStateChanged, messagingHandle)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create message sender.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_023: [ IoTHubMessaging_LL_Open shall create uAMQP message sender by calling the messagesender_create with the created sender link and the local IoTHubMessaging_LL_SenderStateChanged callback ] */
                    else if (messagesender_open(messagingHandle->message_sender) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not open the message sender.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_027: [ IoTHubMessaging_LL_Open shall create uAMQP messaging source for receiver by calling the messaging_create_source ] */
                    else if ((receiveSource = messaging_create_source(receive_target_address)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create source for link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_028: [ IoTHubMessaging_LL_Open shall create uAMQP messaging target for receiver by calling the messaging_create_target ] */
                    else if ((receiveTarget = messaging_create_target("receiver_001")) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create target for link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_024: [ IoTHubMessaging_LL_Open shall create uAMQP receiver link by calling the link_create ] */
                    else if ((messagingHandle->receiver_link = link_create(messagingHandle->session, "receiver-link", role_receiver, receiveSource, receiveTarget)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_06_002: [ IoTHubMessaging_LL_Open shall add the version property to the receiver by calling the link_set_attach_properties ] */
                    else if (attachServiceClientTypeToLink(messagingHandle->receiver_link) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create link.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_025: [ IoTHubMessaging_LL_Open shall set the AMQP receiver link settle mode to receiver_settle_mode_first by calling link_set_rcv_settle_mode ] */
                    else if (link_set_rcv_settle_mode(messagingHandle->receiver_link, receiver_settle_mode_first) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set the sender settle mode.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_029: [ IoTHubMessaging_LL_Open shall create uAMQP message receiver by calling the messagereceiver_create with the created sender link and the local IoTHubMessaging_LL_ReceiverStateChanged callback ] */
                    else if ((messagingHandle->message_receiver = messagereceiver_create(messagingHandle->receiver_link, IoTHubMessaging_LL_ReceiverStateChanged, messagingHandle)) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not create message receiver.");
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    /*Codes_SRS_IOTHUBMESSAGING_12_029: [ IoTHubMessaging_LL_Open shall create uAMQP message receiver by calling the messagereceiver_create with the created sender link and the local IoTHubMessaging_LL_ReceiverStateChanged callback ] */
                    else if (messagereceiver_open(messagingHandle->message_receiver, IoTHubMessaging_LL_FeedbackMessageReceived, messagingHandle) != 0)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_030: [ If any of the uAMQP call fails IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not open the message receiver.");
                        messagereceiver_destroy(messagingHandle->message_receiver);
                        messagingHandle->message_receiver = NULL;
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_031: [ If all of the uAMQP call return 0 (success) IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_OK ] */
                        messagingHandle->isOpened = true;
                        result = IOTHUB_MESSAGING_OK;
                    }
                }
            }
            if (result != IOTHUB_MESSAGING_OK)
            {
                if (messagingHandle->message_sender != NULL)
                {
                    messagesender_destroy(messagingHandle->message_sender);
                    messagingHandle->message_sender = NULL;
                }
                if (messagingHandle->tls_io != NULL)
                {
                    xio_destroy(messagingHandle->tls_io);
                    messagingHandle->tls_io = NULL;
                }
                if (messagingHandle->sasl_io != NULL)
                {
                    xio_destroy(messagingHandle->sasl_io);
                    messagingHandle->sasl_io = NULL;
                }
                if (messagingHandle->session != NULL)
                {
                    session_destroy(messagingHandle->session);
                    messagingHandle->session = NULL;
                }
                if (messagingHandle->connection != NULL)
                {
                    connection_destroy(messagingHandle->connection);
                    messagingHandle->connection = NULL;
                }
                if (messagingHandle->receiver_link != NULL)
                {
                    link_destroy(messagingHandle->receiver_link);
                    messagingHandle->receiver_link = NULL;
                }
            }
        }
        if (result != IOTHUB_MESSAGING_OK)
        {
            if (messagingHandle->sasl_plain_config.authcid != NULL)
            {
                free((char*)messagingHandle->sasl_plain_config.authcid);
                messagingHandle->sasl_plain_config.authcid = NULL;
            }
            if (messagingHandle->sasl_plain_config.passwd != NULL)
            {
                free((char*)messagingHandle->sasl_plain_config.passwd);
                messagingHandle->sasl_plain_config.passwd = NULL;
            }
        }
    }
    amqpvalue_destroy(sendSource);
    amqpvalue_destroy(sendTarget);
    amqpvalue_destroy(receiveSource);
    amqpvalue_destroy(receiveTarget);

    if (send_target_address != NULL)
    {
        free(send_target_address);
    }
    if (receive_target_address != NULL)
    {
        free(receive_target_address);
    }
    return result;
}

void IoTHubMessaging_LL_Close(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_032: [ If the messagingHandle input parameter is NULL IoTHubMessaging_LL_Close shall return IOTHUB_MESSAGING_INVALID_ARG ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_033: [ IoTHubMessaging_LL_Close destroy the AMQP transportconnection by calling link_destroy, session_destroy, connection_destroy, xio_destroy, saslmechanism_destroy ] */
    else
    {
        messagesender_destroy(messagingHandle->message_sender);
        messagereceiver_destroy(messagingHandle->message_receiver);

        link_destroy(messagingHandle->sender_link);
        link_destroy(messagingHandle->receiver_link);

        session_destroy(messagingHandle->session);
        connection_destroy(messagingHandle->connection);
        xio_destroy(messagingHandle->sasl_io);
        xio_destroy(messagingHandle->tls_io);
        saslmechanism_destroy(messagingHandle->sasl_mechanism_handle);

        if (messagingHandle->sasl_plain_config.authcid != NULL)
        {
            free((char*)messagingHandle->sasl_plain_config.authcid);
        }
        if (messagingHandle->sasl_plain_config.passwd != NULL)
        {
            free((char*)messagingHandle->sasl_plain_config.passwd);
        }
        if (messagingHandle->sasl_plain_config.authzid != NULL)
        {
            free((char*)messagingHandle->sasl_plain_config.authzid);
        }
        messagingHandle->isOpened = false;
    }
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_SetFeedbackMessageCallback(IOTHUB_MESSAGING_HANDLE messagingHandle, IOTHUB_FEEDBACK_MESSAGE_RECEIVED_CALLBACK feedbackMessageReceivedCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    /*Codes_SRS_IOTHUBMESSAGING_12_042: [ IoTHubMessaging_LL_SetCallbacks shall verify the messagingHandle input parameter and if it is NULL then return NULL ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_043: [ IoTHubMessaging_LL_SetCallbacks shall save the given feedbackMessageReceivedCallback to use them in local callbacks ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_044: [ IoTHubMessaging_LL_Open shall return IOTHUB_MESSAGING_OK after the callbacks have been set ] */
        messagingHandle->callback_data->feedbackMessageCallback = feedbackMessageReceivedCallback;
        messagingHandle->callback_data->feedbackUserContext = userContextCallback;
        result = IOTHUB_MESSAGING_OK;
    }
    return result;
}


IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_Send(IOTHUB_MESSAGING_HANDLE messagingHandle, const char* deviceId, IOTHUB_MESSAGE_HANDLE message, IOTHUB_SEND_COMPLETE_CALLBACK sendCompleteCallback, void* userContextCallback)
{
    IOTHUB_MESSAGING_RESULT result;

    // There is no support for module sending message for callers, but most of plumbing is available should this be enabled via a new API.
    const char* moduleId = NULL;

    char* deviceDestinationString;

    /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL ] */
    if (messagingHandle == NULL)
    {
        LogError("Input parameter messagingHandle cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL ] */
    else if (deviceId == NULL)
    {
        LogError("Input parameter deviceId cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_034: [ IoTHubMessaging_LL_SendMessage shall verify the messagingHandle, deviceId, message input parameters and if any of them are NULL then return NULL ] */
    else if (message == NULL)
    {
        LogError("Input parameter message cannot be NULL");
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_035: [ IoTHubMessaging_LL_SendMessage shall verify if the AMQP messaging has been established by a successfull call to _Open and if it is not then return IOTHUB_MESSAGING_ERROR ] */
    else if (messagingHandle->isOpened == 0)
    {
        LogError("Messaging is not opened - call IoTHubMessaging_LL_Open to open");
        result = IOTHUB_MESSAGING_ERROR;
    }
    /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
    else if ((deviceDestinationString = createDeviceDestinationString(deviceId, moduleId)) == NULL)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
        LogError("Could not create a message.");
        result = IOTHUB_MESSAGING_ERROR;
    }
    else
    {
        unsigned const char* messageContent;
        size_t messageContentSize;

        if (getMessageContentAndSize(message, &messageContent, &messageContentSize) != 0)
        {
            LogError("Failed getting the message content and message size from IOTHUB_MESSAGE_HANDLE instance.");
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            MESSAGE_HANDLE amqpMessage;
            AMQP_VALUE to_amqp_value;

            /*Codes_SRS_IOTHUBMESSAGING_12_036: [ IoTHubMessaging_LL_SendMessage shall create a uAMQP message by calling message_create ] */
            if ((amqpMessage = message_create()) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not create a message.");
                result = IOTHUB_MESSAGING_ERROR;
            }
            /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
            else if ((to_amqp_value = amqpvalue_create_string(deviceDestinationString)) == NULL)
            {
                /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                LogError("Could not create properties for message - amqpvalue_create_string");
                message_destroy(amqpMessage);
                result = IOTHUB_MESSAGING_ERROR;
            }
            else
            {
                BINARY_DATA binary_data;

                binary_data.bytes = messageContent;
                binary_data.length = messageContentSize;

                /*Codes_SRS_IOTHUBMESSAGING_12_037: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message body to the given message content by calling message_add_body_amqp_data ] */
                if (message_add_body_amqp_data(amqpMessage, binary_data) != 0)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                    LogError("Failed setting the body of the uAMQP message.");
                    result = IOTHUB_MESSAGING_ERROR;
                }
                /*Codes_SRS_IOTHUBMESSAGING_12_038: [ IoTHubMessaging_LL_SendMessage shall set the uAMQP message properties to the given message properties by calling message_set_properties ] */
                else if (addPropertiesToAMQPMessage(message, amqpMessage, to_amqp_value) != 0)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                    message_destroy(amqpMessage);
                    LogError("Failed setting properties of the uAMQP message.");
                    result = IOTHUB_MESSAGING_ERROR;
                }
                else if (addApplicationPropertiesToAMQPMessage(message, amqpMessage) != 0)
                {
                    /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                    message_destroy(amqpMessage);
                    LogError("Failed setting application properties of the uAMQP message.");
                    result = IOTHUB_MESSAGING_ERROR;
                }
                else
                {
                    messagingHandle->callback_data->sendCompleteCallback = sendCompleteCallback;
                    messagingHandle->callback_data->sendUserContext = userContextCallback;

                    /*Codes_SRS_IOTHUBMESSAGING_12_039: [ IoTHubMessaging_LL_SendMessage shall call uAMQP messagesender_send with the created message with IoTHubMessaging_LL_SendMessageComplete callback by which IoTHubMessaging is notified of completion of send ] */
                    if (messagesender_send_async(messagingHandle->message_sender, amqpMessage, IoTHubMessaging_LL_SendMessageComplete, messagingHandle, 0) == NULL)
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_040: [ If any of the uAMQP call fails IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_ERROR ] */
                        LogError("Could not set outgoing window.");
                        message_destroy(amqpMessage);
                        result = IOTHUB_MESSAGING_ERROR;
                    }
                    else
                    {
                        /*Codes_SRS_IOTHUBMESSAGING_12_041: [ If all uAMQP call return 0 then IoTHubMessaging_LL_SendMessage shall return IOTHUB_MESSAGING_OK  ] */
                        result = IOTHUB_MESSAGING_OK;
                    }
                }
                message_destroy(amqpMessage);
                amqpvalue_destroy(to_amqp_value);
            }
        }
        free(deviceDestinationString);
    }
    return result;
}


void IoTHubMessaging_LL_DoWork(IOTHUB_MESSAGING_HANDLE messagingHandle)
{
    /*Codes_SRS_IOTHUBMESSAGING_12_045: [ IoTHubMessaging_LL_DoWork shall verify if uAMQP transport has been initialized and if it is not then return immediately ] */
    if (messagingHandle != 0)
    {
        /*Codes_SRS_IOTHUBMESSAGING_12_046: [ IoTHubMessaging_LL_DoWork shall call uAMQP connection_dowork ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_047: [ IoTHubMessaging_LL_SendMessageComplete callback given to messagesender_send will be called with MESSAGE_SEND_RESULT ] */
        /*Codes_SRS_IOTHUBMESSAGING_12_048: [ If message has been received the IoTHubMessaging_LL_FeedbackMessageReceived callback given to messagesender_receive will be called with the received MESSAGE_HANDLE ] */
        connection_dowork(messagingHandle->connection);
    }
}

IOTHUB_MESSAGING_RESULT IoTHubMessaging_LL_SetTrustedCert(IOTHUB_MESSAGING_HANDLE messagingHandle, const char* trusted_cert)
{
    IOTHUB_MESSAGING_RESULT result;
    if (messagingHandle == NULL || trusted_cert == NULL)
    {
        LogError("Invalid argument messagingHandle: %p trusted_cert: %p", messagingHandle, trusted_cert);
        result = IOTHUB_MESSAGING_INVALID_ARG;
    }
    else
    {
        char* temp_cert;
        if (mallocAndStrcpy_s(&temp_cert, trusted_cert) != 0)
        {
            result = IOTHUB_MESSAGING_ERROR;
        }
        else
        {
            if (messagingHandle->trusted_cert != NULL)
            {
                free(messagingHandle->trusted_cert);
            }
            messagingHandle->trusted_cert = temp_cert;
            result = IOTHUB_MESSAGING_OK;
        }
    }
    return result;
}

