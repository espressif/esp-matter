// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_umqtt_c/mqtt_message.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/string_token.h"
#include "azure_macro_utils/macro_utils.h"

typedef struct MQTT_MESSAGE_TAG
{
    uint16_t packetId;
    QOS_VALUE qosInfo;

    char* topicName;
    APP_PAYLOAD appPayload;

    const char* const_topic_name;
    APP_PAYLOAD const_payload;

    bool isDuplicateMsg;
    bool isMessageRetained;
} MQTT_MESSAGE;

static MQTT_MESSAGE* create_msg_object(uint16_t packetId, QOS_VALUE qosValue)
{
    MQTT_MESSAGE* result;
    result = (MQTT_MESSAGE*)malloc(sizeof(MQTT_MESSAGE));
    if (result != NULL)
    {
        memset(result, 0, sizeof(MQTT_MESSAGE));
        result->packetId = packetId;
        result->isDuplicateMsg = false;
        result->isMessageRetained = false;
        result->qosInfo = qosValue;
    }
    else
    {
        LogError("Failure unable to allocate MQTT Message.");
    }
    return result;
}

MQTT_MESSAGE_HANDLE mqttmessage_create_in_place(uint16_t packetId, const char* topicName, QOS_VALUE qosValue, const uint8_t* appMsg, size_t appMsgLength)
{
    /* Codes_SRS_MQTTMESSAGE_07_026: [If the parameters topicName is NULL then mqttmessage_create_in_place shall return NULL.].] */
    MQTT_MESSAGE* result;
    if (topicName == NULL)
    {
        LogError("Invalid Parameter topicName: %p, packetId: %d.", topicName, packetId);
        result = NULL;
    }
    else
    {
        result = create_msg_object(packetId, qosValue);
        if (result == NULL)
        {
            /* Codes_SRS_MQTTMESSAGE_07_028: [If any memory allocation fails mqttmessage_create_in_place shall free any allocated memory and return NULL.] */
            LogError("Failure creating message object");
        }
        else
        {
            /* Codes_SRS_MQTTMESSAGE_07_027: [mqttmessage_create_in_place shall use the a pointer to topicName or appMsg .] */
            result->const_topic_name = topicName;
            result->const_payload.length = appMsgLength;
            if (result->const_payload.length > 0)
            {
                result->const_payload.message = (uint8_t*)appMsg;
            }
        }
    }
    /* Codes_SRS_MQTTMESSAGE_07_029: [ Upon success, mqttmessage_create_in_place shall return a NON-NULL MQTT_MESSAGE_HANDLE value.] */
    return (MQTT_MESSAGE_HANDLE)result;
}

MQTT_MESSAGE_HANDLE mqttmessage_create(uint16_t packetId, const char* topicName, QOS_VALUE qosValue, const uint8_t* appMsg, size_t appMsgLength)
{
    /* Codes_SRS_MQTTMESSAGE_07_001:[If the parameters topicName is NULL is zero then mqttmessage_create shall return NULL.] */
    MQTT_MESSAGE* result;
    if (topicName == NULL)
    {
        LogError("Invalid Parameter topicName: %p, packetId: %d.", topicName, packetId);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_002: [mqttmessage_create shall allocate and copy the topicName and appMsg parameters.] */
        result = create_msg_object(packetId, qosValue);
        if (result == NULL)
        {
            /* Codes_SRS_MQTTMESSAGE_07_028: [If any memory allocation fails mqttmessage_create_in_place shall free any allocated memory and return NULL.] */
            LogError("Failure creating message object");
        }
        else
        {
            if (mallocAndStrcpy_s(&result->topicName, topicName) != 0)
            {
                /* Codes_SRS_MQTTMESSAGE_07_003: [If any memory allocation fails mqttmessage_create shall free any allocated memory and return NULL.] */
                LogError("Failure allocating topic name");
                free(result);
                result = NULL;
            }
            else
            {
                /* Codes_SRS_MQTTMESSAGE_07_002: [mqttmessage_create shall allocate and copy the topicName and appMsg parameters.] */
                result->appPayload.length = appMsgLength;
                if (result->appPayload.length > 0)
                {
                    result->appPayload.message = malloc(appMsgLength);
                    if (result->appPayload.message == NULL)
                    {
                        /* Codes_SRS_MQTTMESSAGE_07_003: [If any memory allocation fails mqttmessage_create shall free any allocated memory and return NULL.] */
                        LogError("Failure allocating message value of %lu", (unsigned long)appMsgLength);
                        free(result->topicName);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        (void)memcpy(result->appPayload.message, appMsg, appMsgLength);
                    }
                }
                else
                {
                    result->appPayload.message = NULL;
                }
            }
        }
    }
    /* Codes_SRS_MQTTMESSAGE_07_004: [If mqttmessage_createMessage succeeds the it shall return a NON-NULL MQTT_MESSAGE_HANDLE value.] */
    return (MQTT_MESSAGE_HANDLE)result;
}

void mqttmessage_destroy(MQTT_MESSAGE_HANDLE handle)
{
    /* Codes_SRS_MQTTMESSAGE_07_005: [If the handle parameter is NULL then mqttmessage_destroyMessage shall do nothing] */
    if (handle != NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_006: [mqttmessage_destroyMessage shall free all resources associated with the MQTT_MESSAGE_HANDLE value] */
        if (handle->topicName != NULL)
        {
            free(handle->topicName);
        }
        if (handle->appPayload.message != NULL)
        {
            free(handle->appPayload.message);
        }
        free(handle);
    }
}

MQTT_MESSAGE_HANDLE mqttmessage_clone(MQTT_MESSAGE_HANDLE handle)
{
    MQTT_MESSAGE_HANDLE result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_007: [If handle parameter is NULL then mqttmessage_clone shall return NULL.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_008: [mqttmessage_clone shall create a new MQTT_MESSAGE_HANDLE with data content identical of the handle value.] */
        result = mqttmessage_create(handle->packetId, handle->topicName, handle->qosInfo, handle->appPayload.message, handle->appPayload.length);
        if (result != NULL)
        {
            result->isDuplicateMsg = handle->isDuplicateMsg;
            result->isMessageRetained = handle->isMessageRetained;
        }
    }
    return result;
}

uint16_t mqttmessage_getPacketId(MQTT_MESSAGE_HANDLE handle)
{
    uint16_t result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_010: [If handle is NULL then mqttmessage_getPacketId shall return 0.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = 0;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_011: [mqttmessage_getPacketId shall return the packetId value contained in MQTT_MESSAGE_HANDLE handle.] */
        result = handle->packetId;
    }
    return result;
}

const char* mqttmessage_getTopicName(MQTT_MESSAGE_HANDLE handle)
{
    const char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_012: [If handle is NULL then mqttmessage_getTopicName shall return a NULL string.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_013: [mqttmessage_getTopicName shall return the topicName contained in MQTT_MESSAGE_HANDLE handle.] */
        if (handle->topicName == NULL)
        {
            result = handle->const_topic_name;
        }
        else
        {
            result = handle->topicName;
        }
    }
    return result;
}

int mqttmessage_getTopicLevels(MQTT_MESSAGE_HANDLE handle, char*** levels, size_t* count)
{
    int result;

    // Codes_SRS_MQTTMESSAGE_09_001: [ If `handle`, `levels` or `count` are NULL the function shall return a non-zero value. ]
    if (handle == NULL || levels == NULL || count == NULL)
    {
        LogError("Invalid Parameter handle: %p, levels: %p, count: %p", handle, levels, count);
        result = MU_FAILURE;
    }
    else
    {
        MQTT_MESSAGE* msgInfo = (MQTT_MESSAGE*)handle;

        const char* topic_name = msgInfo->topicName != NULL ? msgInfo->topicName : msgInfo->const_topic_name;

        if (topic_name == NULL)
        {
            LogError("Topic name is NULL");
            result = MU_FAILURE;
        }
        else
        {
            const char* delimiters[1];

            delimiters[0] = "/";

            // Codes_SRS_MQTTMESSAGE_09_002: [ The topic name, excluding the property bag, shall be split into individual tokens using "/" as separator ]
            // Codes_SRS_MQTTMESSAGE_09_004: [ The split tokens shall be stored in `levels` and its count in `count` ]
            if (StringToken_Split(topic_name, strlen(topic_name), delimiters, 1, false, levels, count) != 0)
            {
                // Codes_SRS_MQTTMESSAGE_09_003: [ If splitting fails the function shall return a non-zero value. ]
                LogError("Failed splitting topic levels");
                result = MU_FAILURE;
            }
            else
            {
                // Codes_SRS_MQTTMESSAGE_09_005: [ If no failures occur the function shall return zero. ]
                result = 0;
            }
        }
    }

    return result;
}

QOS_VALUE mqttmessage_getQosType(MQTT_MESSAGE_HANDLE handle)
{
    QOS_VALUE result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_014: [If handle is NULL then mqttmessage_getQosType shall return the default DELIVER_AT_MOST_ONCE value.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = DELIVER_AT_MOST_ONCE;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_015: [mqttmessage_getQosType shall return the QOS Type value contained in MQTT_MESSAGE_HANDLE handle.] */
        result = handle->qosInfo;
    }
    return result;
}

bool mqttmessage_getIsDuplicateMsg(MQTT_MESSAGE_HANDLE handle)
{
    bool result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_016: [If handle is NULL then mqttmessage_getIsDuplicateMsg shall return false.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = false;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_017: [mqttmessage_getIsDuplicateMsg shall return the isDuplicateMsg value contained in MQTT_MESSAGE_HANDLE handle.] */
        result = handle->isDuplicateMsg;
    }
    return result;
}

bool mqttmessage_getIsRetained(MQTT_MESSAGE_HANDLE handle)
{
    bool result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_018: [If handle is NULL then mqttmessage_getIsRetained shall return false.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = false;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_019: [mqttmessage_getIsRetained shall return the isRetained value contained in MQTT_MESSAGE_HANDLE handle.] */
        result = handle->isMessageRetained;
    }
    return result;
}

int mqttmessage_setIsDuplicateMsg(MQTT_MESSAGE_HANDLE handle, bool duplicateMsg)
{
    int result;
    /* Codes_SRS_MQTTMESSAGE_07_022: [If handle is NULL then mqttmessage_setIsDuplicateMsg shall return a non-zero value.] */
    if (handle == NULL)
    {
        LogError("Invalid Parameter handle: %p.", handle);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_023: [mqttmessage_setIsDuplicateMsg shall store the duplicateMsg value in the MQTT_MESSAGE_HANDLE handle.] */
        handle->isDuplicateMsg = duplicateMsg;
        result = 0;
    }
    return result;
}

int mqttmessage_setIsRetained(MQTT_MESSAGE_HANDLE handle, bool retainMsg)
{
    int result;
    /* Codes_SRS_MQTTMESSAGE_07_024: [If handle is NULL then mqttmessage_setIsRetained shall return a non-zero value.] */
    if (handle == NULL)
    {
        LogError("Invalid Parameter handle: %p.", handle);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_025: [mqttmessage_setIsRetained shall store the retainMsg value in the MQTT_MESSAGE_HANDLE handle.] */
        handle->isMessageRetained = retainMsg;
        result = 0;
    }
    return result;
}

const APP_PAYLOAD* mqttmessage_getApplicationMsg(MQTT_MESSAGE_HANDLE handle)
{
    const APP_PAYLOAD* result;
    if (handle == NULL)
    {
        /* Codes_SRS_MQTTMESSAGE_07_020: [If handle is NULL or if msgLen is 0 then mqttmessage_getApplicationMsg shall return NULL.] */
        LogError("Invalid Parameter handle: %p.", handle);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTTMESSAGE_07_021: [mqttmessage_getApplicationMsg shall return the applicationMsg value contained in MQTT_MESSAGE_HANDLE handle and the length of the appMsg in the msgLen parameter.] */
        if (handle->const_payload.length > 0)
        {
            result = &handle->const_payload;
        }
        else
        {
            result = &handle->appPayload;
        }
    }
    return result;
}
