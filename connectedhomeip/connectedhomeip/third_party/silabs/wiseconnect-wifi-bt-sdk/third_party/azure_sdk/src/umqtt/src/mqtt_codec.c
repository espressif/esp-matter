// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <limits.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_macro_utils/macro_utils.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_umqtt_c/mqtt_codec.h"
#include <inttypes.h>

#define PAYLOAD_OFFSET                      5
#define PACKET_TYPE_BYTE(p)                 (CONTROL_PACKET_TYPE)((uint8_t)(((uint8_t)(p)) & 0xf0))
#define FLAG_VALUE_BYTE(p)                  ((uint8_t)(((uint8_t)(p)) & 0xf))

#define USERNAME_FLAG                       0x80
#define PASSWORD_FLAG                       0x40
#define WILL_RETAIN_FLAG                    0x20
#define WILL_QOS_FLAG_                      0x18
#define WILL_FLAG_FLAG                      0x04
#define CLEAN_SESSION_FLAG                  0x02

#define NEXT_128_CHUNK                      0x80
#define PUBLISH_DUP_FLAG                    0x8
#define PUBLISH_QOS_EXACTLY_ONCE            0x4
#define PUBLISH_QOS_AT_LEAST_ONCE           0x2
#define PUBLISH_QOS_RETAIN                  0x1

#define PROTOCOL_NUMBER                     4
#define CONN_FLAG_BYTE_OFFSET               7

#define CONNECT_FIXED_HEADER_SIZE           2
#define CONNECT_VARIABLE_HEADER_SIZE        10
#define SUBSCRIBE_FIXED_HEADER_FLAG         0x2
#define UNSUBSCRIBE_FIXED_HEADER_FLAG       0x2

#define MAX_SEND_SIZE                       0xFFFFFF7F // 268435455

// This captures the maximum packet size for 3 digits.
// If it's above this value then we bail out of the loop
#define MAX_3_DIGIT_PACKET_SIZE             2097152

#define CODEC_STATE_VALUES      \
    CODEC_STATE_FIXED_HEADER,   \
    CODEC_STATE_VAR_HEADER,     \
    CODEC_STATE_PAYLOAD

static const char* const TRUE_CONST = "true";
static const char* const FALSE_CONST = "false";

MU_DEFINE_ENUM(CODEC_STATE_RESULT, CODEC_STATE_VALUES);

typedef struct MQTTCODEC_INSTANCE_TAG
{
    CONTROL_PACKET_TYPE currPacket;
    CODEC_STATE_RESULT codecState;
    size_t bufferOffset;
    int headerFlags;
    BUFFER_HANDLE headerData;
    ON_PACKET_COMPLETE_CALLBACK packetComplete;
    void* callContext;
    uint8_t storeRemainLen[4];
    size_t remainLenIndex;
} MQTTCODEC_INSTANCE;

typedef struct PUBLISH_HEADER_INFO_TAG
{
    const char* topicName;
    uint16_t packetId;
    const char* msgBuffer;
    QOS_VALUE qualityOfServiceValue;
} PUBLISH_HEADER_INFO;

static const char* retrieve_qos_value(QOS_VALUE value)
{
    switch (value)
    {
        case DELIVER_AT_MOST_ONCE:
            return "DELIVER_AT_MOST_ONCE";
        case DELIVER_AT_LEAST_ONCE:
            return "DELIVER_AT_LEAST_ONCE";
        case DELIVER_EXACTLY_ONCE:
        default:
            return "DELIVER_EXACTLY_ONCE";
    }
}

static void byteutil_writeByte(uint8_t** buffer, uint8_t value)
{
    if (buffer != NULL)
    {
        **buffer = value;
        (*buffer)++;
    }
}

static void byteutil_writeInt(uint8_t** buffer, uint16_t value)
{
    if (buffer != NULL)
    {
        **buffer = (char)(value / 256);
        (*buffer)++;
        **buffer = (char)(value % 256);
        (*buffer)++;
    }
}

static void byteutil_writeUTF(uint8_t** buffer, const char* stringData, uint16_t len)
{
    if (buffer != NULL)
    {
        byteutil_writeInt(buffer, len);
        (void)memcpy(*buffer, stringData, len);
        *buffer += len;
    }
}

static CONTROL_PACKET_TYPE processControlPacketType(uint8_t pktByte, int* flags)
{
    CONTROL_PACKET_TYPE result;
    result = PACKET_TYPE_BYTE(pktByte);
    if (flags != NULL)
    {
        *flags = FLAG_VALUE_BYTE(pktByte);
    }
    return result;
}

static int addListItemsToUnsubscribePacket(BUFFER_HANDLE ctrlPacket, const char** payloadList, size_t payloadCount, STRING_HANDLE trace_log)
{
    int result = 0;
    size_t index = 0;
    for (index = 0; index < payloadCount && result == 0; index++)
    {
        // Add the Payload
        size_t offsetLen = BUFFER_length(ctrlPacket);
        size_t topicLen = strlen(payloadList[index]);
        if (topicLen > USHRT_MAX)
        {
            result = MU_FAILURE;
        }
        else if (BUFFER_enlarge(ctrlPacket, topicLen + 2) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            uint8_t* iterator = BUFFER_u_char(ctrlPacket);
            iterator += offsetLen;
            byteutil_writeUTF(&iterator, payloadList[index], (uint16_t)topicLen);
        }
        if (trace_log != NULL)
        {
            STRING_sprintf(trace_log, " | TOPIC_NAME: %s", payloadList[index]);
        }
    }
    return result;
}

static int addListItemsToSubscribePacket(BUFFER_HANDLE ctrlPacket, SUBSCRIBE_PAYLOAD* payloadList, size_t payloadCount, STRING_HANDLE trace_log)
{
    int result = 0;
    size_t index = 0;
    for (index = 0; index < payloadCount && result == 0; index++)
    {
        // Add the Payload
        size_t offsetLen = BUFFER_length(ctrlPacket);
        size_t topicLen = strlen(payloadList[index].subscribeTopic);
        if (topicLen > USHRT_MAX)
        {
            result = MU_FAILURE;
        }
        else if (BUFFER_enlarge(ctrlPacket, topicLen + 2 + 1) != 0)
        {
            result = MU_FAILURE;
        }
        else
        {
            uint8_t* iterator = BUFFER_u_char(ctrlPacket);
            iterator += offsetLen;
            byteutil_writeUTF(&iterator, payloadList[index].subscribeTopic, (uint16_t)topicLen);
            *iterator = payloadList[index].qosReturn;

            if (trace_log != NULL)
            {
                STRING_sprintf(trace_log, " | TOPIC_NAME: %s | QOS: %d", payloadList[index].subscribeTopic, (int)payloadList[index].qosReturn);
            }
        }
    }
    return result;
}

static int constructConnectVariableHeader(BUFFER_HANDLE ctrlPacket, const MQTT_CLIENT_OPTIONS* mqttOptions, STRING_HANDLE trace_log)
{
    int result = 0;
    if (BUFFER_enlarge(ctrlPacket, CONNECT_VARIABLE_HEADER_SIZE) != 0)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint8_t* iterator = BUFFER_u_char(ctrlPacket);
        if (iterator == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (trace_log != NULL)
            {
                STRING_sprintf(trace_log, " | VER: %d | KEEPALIVE: %d | FLAGS:", PROTOCOL_NUMBER, mqttOptions->keepAliveInterval);
            }
            byteutil_writeUTF(&iterator, "MQTT", 4);
            byteutil_writeByte(&iterator, PROTOCOL_NUMBER);
            byteutil_writeByte(&iterator, 0); // Flags will be entered later
            byteutil_writeInt(&iterator, mqttOptions->keepAliveInterval);
            result = 0;
        }
    }
    return result;
}

static int constructPublishVariableHeader(BUFFER_HANDLE ctrlPacket, const PUBLISH_HEADER_INFO* publishHeader, STRING_HANDLE trace_log)
{
    int result = 0;
    size_t topicLen = 0;
    size_t spaceLen = 0;
    size_t idLen = 0;

    size_t currLen = BUFFER_length(ctrlPacket);

    topicLen = strlen(publishHeader->topicName);
    spaceLen += 2;

    if (publishHeader->qualityOfServiceValue != DELIVER_AT_MOST_ONCE)
    {
        // Packet Id is only set if the QOS is not 0
        idLen = 2;
    }

    if (topicLen > USHRT_MAX)
    {
        result = MU_FAILURE;
    }
    else if (BUFFER_enlarge(ctrlPacket, topicLen + idLen + spaceLen) != 0)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint8_t* iterator = BUFFER_u_char(ctrlPacket);
        if (iterator == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            iterator += currLen;
            /* The Topic Name MUST be present as the first field in the PUBLISH Packet Variable header.It MUST be 792 a UTF-8 encoded string [MQTT-3.3.2-1] as defined in section 1.5.3.*/
            byteutil_writeUTF(&iterator, publishHeader->topicName, (uint16_t)topicLen);
            if (trace_log != NULL)
            {
                STRING_sprintf(trace_log, " | TOPIC_NAME: %s", publishHeader->topicName);
            }
            if (idLen > 0)
            {
                if (trace_log != NULL)
                {
                    STRING_sprintf(trace_log, " | PACKET_ID: %"PRIu16, publishHeader->packetId);
                }
                byteutil_writeInt(&iterator, publishHeader->packetId);
            }
            result = 0;
        }
    }
    return result;
}

static int constructSubscibeTypeVariableHeader(BUFFER_HANDLE ctrlPacket, uint16_t packetId)
{
    int result = 0;
    if (BUFFER_enlarge(ctrlPacket, 2) != 0)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint8_t* iterator = BUFFER_u_char(ctrlPacket);
        if (iterator == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            byteutil_writeInt(&iterator, packetId);
            result = 0;
        }
    }
    return result;
}

static BUFFER_HANDLE constructPublishReply(CONTROL_PACKET_TYPE type, uint8_t flags, uint16_t packetId)
{
    BUFFER_HANDLE result = BUFFER_new();
    if (result != NULL)
    {
        if (BUFFER_pre_build(result, 4) != 0)
        {
            BUFFER_delete(result);
            result = NULL;
        }
        else
        {
            uint8_t* iterator = BUFFER_u_char(result);
            if (iterator == NULL)
            {
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                *iterator = (uint8_t)type | flags;
                iterator++;
                *iterator = 0x2;
                iterator++;
                byteutil_writeInt(&iterator, packetId);
            }
        }
    }
    return result;
}

static int constructFixedHeader(BUFFER_HANDLE ctrlPacket, CONTROL_PACKET_TYPE packetType, uint8_t flags)
{
    int result;
    size_t packetLen = BUFFER_length(ctrlPacket);
    uint8_t remainSize[4] ={ 0 };
    size_t index = 0;

    // Calculate the length of packet
    do
    {
        uint8_t encode = packetLen % 128;
        packetLen /= 128;
        // if there are more data to encode, set the top bit of this byte
        if (packetLen > 0)
        {
            encode |= NEXT_128_CHUNK;
        }
        remainSize[index++] = encode;
    } while (packetLen > 0);

    BUFFER_HANDLE fixedHeader = BUFFER_new();
    if (fixedHeader == NULL)
    {
        result = MU_FAILURE;
    }
    else if (BUFFER_pre_build(fixedHeader, index + 1) != 0)
    {
        BUFFER_delete(fixedHeader);
        result = MU_FAILURE;
    }
    else
    {
        uint8_t* iterator = BUFFER_u_char(fixedHeader);
        *iterator = (uint8_t)packetType | flags;
        iterator++;
        (void)memcpy(iterator, remainSize, index);

        result = BUFFER_prepend(ctrlPacket, fixedHeader);
        BUFFER_delete(fixedHeader);
    }
    return result;
}

static int constructConnPayload(BUFFER_HANDLE ctrlPacket, const MQTT_CLIENT_OPTIONS* mqttOptions, STRING_HANDLE trace_log)
{
    int result = 0;
    size_t clientLen = 0;
    size_t usernameLen = 0;
    size_t passwordLen = 0;
    size_t willMessageLen = 0;
    size_t willTopicLen = 0;
    size_t spaceLen = 0;
    size_t currLen = 0;
    size_t totalLen = 0;

    if (mqttOptions->clientId != NULL)
    {
        spaceLen += 2;
        clientLen = strlen(mqttOptions->clientId);
    }
    if (mqttOptions->username != NULL)
    {
        spaceLen += 2;
        usernameLen = strlen(mqttOptions->username);
    }
    if (mqttOptions->password != NULL)
    {
        spaceLen += 2;
        passwordLen = strlen(mqttOptions->password);
    }
    if (mqttOptions->willMessage != NULL)
    {
        spaceLen += 2;
        willMessageLen = strlen(mqttOptions->willMessage);
    }
    if (mqttOptions->willTopic != NULL)
    {
        spaceLen += 2;
        willTopicLen = strlen(mqttOptions->willTopic);
    }

    currLen = ctrlPacket == NULL ? 0 : BUFFER_length(ctrlPacket);
    totalLen = clientLen + usernameLen + passwordLen + willMessageLen + willTopicLen + spaceLen;

    if (ctrlPacket == NULL)
    {
        result = MU_FAILURE;
    }
    // Validate the Username & Password
    else if (clientLen > USHRT_MAX)
    {
        result = MU_FAILURE;
    }
    else if (usernameLen == 0 && passwordLen > 0)
    {
        result = MU_FAILURE;
    }
    else if ((willMessageLen > 0 && willTopicLen == 0) || (willTopicLen > 0 && willMessageLen == 0))
    {
        result = MU_FAILURE;
    }
    else if (BUFFER_enlarge(ctrlPacket, totalLen) != 0)
    {
        result = MU_FAILURE;
    }
    else
    {
        uint8_t* packet = BUFFER_u_char(ctrlPacket);
        uint8_t* iterator = packet;

        iterator += currLen;
        byteutil_writeUTF(&iterator, mqttOptions->clientId, (uint16_t)clientLen);

        // TODO: Read on the Will Topic
        if (willMessageLen > USHRT_MAX || willTopicLen > USHRT_MAX || usernameLen > USHRT_MAX || passwordLen > USHRT_MAX)
        {
            result = MU_FAILURE;
        }
        else
        {
            STRING_HANDLE connect_payload_trace = NULL;
            if (trace_log != NULL)
            {
                connect_payload_trace = STRING_new();
            }
            if (willMessageLen > 0 && willTopicLen > 0)
            {
                if (trace_log != NULL)
                {
                    (void)STRING_sprintf(connect_payload_trace, " | WILL_TOPIC: %s", mqttOptions->willTopic);
                }
                packet[CONN_FLAG_BYTE_OFFSET] |= WILL_FLAG_FLAG;
                byteutil_writeUTF(&iterator, mqttOptions->willTopic, (uint16_t)willTopicLen);
                packet[CONN_FLAG_BYTE_OFFSET] |= (mqttOptions->qualityOfServiceValue << 3);
                if (mqttOptions->messageRetain)
                {
                    packet[CONN_FLAG_BYTE_OFFSET] |= WILL_RETAIN_FLAG;
                }
                byteutil_writeUTF(&iterator, mqttOptions->willMessage, (uint16_t)willMessageLen);
            }
            if (usernameLen > 0)
            {
                packet[CONN_FLAG_BYTE_OFFSET] |= USERNAME_FLAG;
                byteutil_writeUTF(&iterator, mqttOptions->username, (uint16_t)usernameLen);
                if (trace_log != NULL)
                {
                    (void)STRING_sprintf(connect_payload_trace, " | USERNAME: %s", mqttOptions->username);
                }
            }
            if (passwordLen > 0)
            {
                packet[CONN_FLAG_BYTE_OFFSET] |= PASSWORD_FLAG;
                byteutil_writeUTF(&iterator, mqttOptions->password, (uint16_t)passwordLen);
                if (trace_log != NULL)
                {
                    (void)STRING_sprintf(connect_payload_trace, " | PWD: XXXX");
                }
            }
            // TODO: Get the rest of the flags
            if (trace_log != NULL)
            {
                (void)STRING_sprintf(connect_payload_trace, " | CLEAN: %s", mqttOptions->useCleanSession ? "1" : "0");
            }
            if (mqttOptions->useCleanSession)
            {
                packet[CONN_FLAG_BYTE_OFFSET] |= CLEAN_SESSION_FLAG;
            }
            if (trace_log != NULL)
            {
                (void)STRING_sprintf(trace_log, " %lu", packet[CONN_FLAG_BYTE_OFFSET]);
                (void)STRING_concat_with_STRING(trace_log, connect_payload_trace);
                STRING_delete(connect_payload_trace);
            }
            result = 0;
        }
    }
    return result;
}

static int prepareheaderDataInfo(MQTTCODEC_INSTANCE* codecData, uint8_t remainLen)
{
    int result;
    result = 0;
    codecData->storeRemainLen[codecData->remainLenIndex++] = remainLen;
    if (remainLen <= 0x7f)
    {
        int multiplier = 1;
        int totalLen = 0;
        size_t index = 0;
        uint8_t encodeByte = 0;
        do
        {
            encodeByte = codecData->storeRemainLen[index++];
            totalLen += (encodeByte & 127) * multiplier;
            multiplier *= NEXT_128_CHUNK;

            if (multiplier > MAX_3_DIGIT_PACKET_SIZE)
            {
                result = MU_FAILURE;
                break;
            }
        } while ((encodeByte & NEXT_128_CHUNK) != 0);

        if (result != 0 || totalLen > MAX_SEND_SIZE)
        {
            LogError("Receive buffer too large for MQTT packet");
            result = MU_FAILURE;
        }
        else
        {
            codecData->codecState = CODEC_STATE_VAR_HEADER;

            // Reset remainLen Index
            codecData->remainLenIndex = 0;
            memset(codecData->storeRemainLen, 0, 4 * sizeof(uint8_t));

            if (totalLen > 0)
            {
                codecData->bufferOffset = 0;
                codecData->headerData = BUFFER_new();
                if (codecData->headerData == NULL)
                {
                    /* Codes_SRS_MQTT_CODEC_07_035: [ If any error is encountered then the packet state will be marked as error and mqtt_codec_bytesReceived shall return a non-zero value. ] */
                    LogError("Failed BUFFER_new");
                    result = MU_FAILURE;
                }
                else
                {
                    if (BUFFER_pre_build(codecData->headerData, totalLen) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_035: [ If any error is encountered then the packet state will be marked as error and mqtt_codec_bytesReceived shall return a non-zero value. ] */
                        LogError("Failed BUFFER_pre_build");
                        result = MU_FAILURE;
                    }

                }
            }
        }
    }
    else if (codecData->remainLenIndex == (sizeof(codecData->storeRemainLen) / sizeof(codecData->storeRemainLen[0])))
    {
        // The maximum number of bytes in the Remaining Length field is four
        // This allows applications to send Control Packets of size up to 268,435,455 (256 MB). 
        // The representation of this number on the wire is: 0xFF, 0xFF, 0xFF, 0x7F.

        // The last byte has exceed the max value of 0x7F
        LogError("MQTT packet len is invalid");
        result = MU_FAILURE;
    }
    return result;
}

static void completePacketData(MQTTCODEC_INSTANCE* codecData)
{
    if (codecData->packetComplete != NULL)
    {
        codecData->packetComplete(codecData->callContext, codecData->currPacket, codecData->headerFlags, codecData->headerData);
    }

    // Clean up data
    codecData->currPacket = UNKNOWN_TYPE;
    codecData->codecState = CODEC_STATE_FIXED_HEADER;
    codecData->headerFlags = 0;
    BUFFER_delete(codecData->headerData);
    codecData->headerData = NULL;
}

static void clear_codec_data(MQTTCODEC_INSTANCE* codec_data)
{
    // Clear the code instance data
    codec_data->currPacket = UNKNOWN_TYPE;
    codec_data->codecState = CODEC_STATE_FIXED_HEADER;
    codec_data->headerFlags = 0;
    codec_data->bufferOffset = 0;
    codec_data->headerData = NULL;
    memset(codec_data->storeRemainLen, 0, 4 * sizeof(uint8_t));
    codec_data->remainLenIndex = 0;
}

void mqtt_codec_reset(MQTTCODEC_HANDLE handle)
{
    if (handle != NULL)
    {
        clear_codec_data(handle);
    }
}

MQTTCODEC_HANDLE mqtt_codec_create(ON_PACKET_COMPLETE_CALLBACK packetComplete, void* callbackCtx)
{
    MQTTCODEC_HANDLE result;
    result = malloc(sizeof(MQTTCODEC_INSTANCE));
    /* Codes_SRS_MQTT_CODEC_07_001: [If a failure is encountered then mqtt_codec_create shall return NULL.] */
    if (result != NULL)
    {
        /* Codes_SRS_MQTT_CODEC_07_002: [On success mqtt_codec_create shall return a MQTTCODEC_HANDLE value.] */
        clear_codec_data(result);
        result->packetComplete = packetComplete;
        result->callContext = callbackCtx;
    }
    return result;
}

void mqtt_codec_destroy(MQTTCODEC_HANDLE handle)
{
    /* Codes_SRS_MQTT_CODEC_07_003: [If the handle parameter is NULL then mqtt_codec_destroy shall do nothing.] */
    if (handle != NULL)
    {
        MQTTCODEC_INSTANCE* codecData = (MQTTCODEC_INSTANCE*)handle;
        /* Codes_SRS_MQTT_CODEC_07_004: [mqtt_codec_destroy shall deallocate all memory that has been allocated by this object.] */
        BUFFER_delete(codecData->headerData);
        free(codecData);
    }
}

BUFFER_HANDLE mqtt_codec_connect(const MQTT_CLIENT_OPTIONS* mqttOptions, STRING_HANDLE trace_log)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_MQTT_CODEC_07_008: [If the parameters mqttOptions is NULL then mqtt_codec_connect shall return a null value.] */
    if (mqttOptions == NULL)
    {
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTT_CODEC_07_009: [mqtt_codec_connect shall construct a BUFFER_HANDLE that represents a MQTT CONNECT packet.] */
        result = BUFFER_new();
        if (result != NULL)
        {
            STRING_HANDLE varible_header_log = NULL;
            if (trace_log != NULL)
            {
                varible_header_log = STRING_new();
            }
            // Add Variable Header Information
            if (constructConnectVariableHeader(result, mqttOptions, varible_header_log) != 0)
            {
                /* Codes_SRS_MQTT_CODEC_07_010: [If any error is encountered then mqtt_codec_connect shall return NULL.] */
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                if (constructConnPayload(result, mqttOptions, varible_header_log) != 0)
                {
                    /* Codes_SRS_MQTT_CODEC_07_010: [If any error is encountered then mqtt_codec_connect shall return NULL.] */
                    BUFFER_delete(result);
                    result = NULL;
                }
                else
                {
                    if (trace_log != NULL)
                    {
                        (void)STRING_copy(trace_log, "CONNECT");
                    }
                    if (constructFixedHeader(result, CONNECT_TYPE, 0) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_010: [If any error is encountered then mqtt_codec_connect shall return NULL.] */
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        if (trace_log != NULL)
                        {
                            (void)STRING_concat_with_STRING(trace_log, varible_header_log);
                        }
                    }
                }
                if (varible_header_log != NULL)
                {
                    STRING_delete(varible_header_log);
                }
            }
        }
    }
    return result;
}

BUFFER_HANDLE mqtt_codec_disconnect()
{
    /* Codes_SRS_MQTT_CODEC_07_011: [On success mqtt_codec_disconnect shall construct a BUFFER_HANDLE that represents a MQTT DISCONNECT packet.] */
    BUFFER_HANDLE result = BUFFER_new();
    if (result != NULL)
    {
        if (BUFFER_enlarge(result, 2) != 0)
        {
            /* Codes_SRS_MQTT_CODEC_07_012: [If any error is encountered mqtt_codec_disconnect shall return NULL.] */
            BUFFER_delete(result);
            result = NULL;
        }
        else
        {
            uint8_t* iterator = BUFFER_u_char(result);
            if (iterator == NULL)
            {
                /* Codes_SRS_MQTT_CODEC_07_012: [If any error is encountered mqtt_codec_disconnect shall return NULL.] */
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                iterator[0] = DISCONNECT_TYPE;
                iterator[1] = 0;
            }
        }
    }
    return result;
}

BUFFER_HANDLE mqtt_codec_publish(QOS_VALUE qosValue, bool duplicateMsg, bool serverRetain, uint16_t packetId, const char* topicName, const uint8_t* msgBuffer, size_t buffLen, STRING_HANDLE trace_log)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_MQTT_CODEC_07_005: [If the parameters topicName is NULL then mqtt_codec_publish shall return NULL.] */
    if (topicName == NULL)
    {
        result = NULL;
    }
    /* Codes_SRS_MQTT_CODEC_07_036: [mqtt_codec_publish shall return NULL if the buffLen variable is greater than the MAX_SEND_SIZE (0xFFFFFF7F).] */
    else if (buffLen > MAX_SEND_SIZE)
    {
        /* Codes_SRS_MQTT_CODEC_07_006: [If any error is encountered then mqtt_codec_publish shall return NULL.] */
        result = NULL;
    }
    else
    {
        PUBLISH_HEADER_INFO publishInfo ={ 0 };
        publishInfo.topicName = topicName;
        publishInfo.packetId = packetId;
        publishInfo.qualityOfServiceValue = qosValue;

        uint8_t headerFlags = 0;
        if (duplicateMsg) headerFlags |= PUBLISH_DUP_FLAG;
        if (serverRetain) headerFlags |= PUBLISH_QOS_RETAIN;
        if (qosValue != DELIVER_AT_MOST_ONCE)
        {
            if (qosValue == DELIVER_AT_LEAST_ONCE)
            {
                headerFlags |= PUBLISH_QOS_AT_LEAST_ONCE;
            }
            else
            {
                headerFlags |= PUBLISH_QOS_EXACTLY_ONCE;
            }
        }

        /* Codes_SRS_MQTT_CODEC_07_007: [mqtt_codec_publish shall return a BUFFER_HANDLE that represents a MQTT PUBLISH message.] */
        result = BUFFER_new();
        if (result != NULL)
        {
            STRING_HANDLE varible_header_log = NULL;
            if (trace_log != NULL)
            {
                varible_header_log = STRING_construct_sprintf(" | IS_DUP: %s | RETAIN: %d | QOS: %s", duplicateMsg ? TRUE_CONST : FALSE_CONST,
                    serverRetain ? 1 : 0,
                    retrieve_qos_value(publishInfo.qualityOfServiceValue) );
            }

            if (constructPublishVariableHeader(result, &publishInfo, varible_header_log) != 0)
            {
                /* Codes_SRS_MQTT_CODEC_07_006: [If any error is encountered then mqtt_codec_publish shall return NULL.] */
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                size_t payloadOffset = BUFFER_length(result);
                if (buffLen > 0)
                {
                    if (BUFFER_enlarge(result, buffLen) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_006: [If any error is encountered then mqtt_codec_publish shall return NULL.] */
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        uint8_t* iterator = BUFFER_u_char(result);
                        if (iterator == NULL)
                        {
                            /* Codes_SRS_MQTT_CODEC_07_006: [If any error is encountered then mqtt_codec_publish shall return NULL.] */
                            BUFFER_delete(result);
                            result = NULL;
                        }
                        else
                        {
                            iterator += payloadOffset;
                            // Write Message
                            (void)memcpy(iterator, msgBuffer, buffLen);
                            if (trace_log)
                            {
                                STRING_sprintf(varible_header_log, " | PAYLOAD_LEN: %lu", (unsigned long)buffLen);
                            }
                        }
                    }
                }

                if (result != NULL)
                {
                    if (trace_log != NULL)
                    {
                        (void)STRING_copy(trace_log, "PUBLISH");
                    }
                    if (constructFixedHeader(result, PUBLISH_TYPE, headerFlags) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_006: [If any error is encountered then mqtt_codec_publish shall return NULL.] */
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        if (trace_log != NULL)
                        {
                            (void)STRING_concat_with_STRING(trace_log, varible_header_log);
                        }
                    }
                }
            }
            if (varible_header_log != NULL)
            {
                STRING_delete(varible_header_log);
            }
        }
    }
    return result;
}

BUFFER_HANDLE mqtt_codec_publishAck(uint16_t packetId)
{
    /* Codes_SRS_MQTT_CODEC_07_013: [On success mqtt_codec_publishAck shall return a BUFFER_HANDLE representation of a MQTT PUBACK packet.] */
    /* Codes_SRS_MQTT_CODEC_07_014 : [If any error is encountered then mqtt_codec_publishAck shall return NULL.] */
    BUFFER_HANDLE result = constructPublishReply(PUBACK_TYPE, 0, packetId);
    return result;
}

BUFFER_HANDLE mqtt_codec_publishReceived(uint16_t packetId)
{
    /* Codes_SRS_MQTT_CODEC_07_015: [On success mqtt_codec_publishRecieved shall return a BUFFER_HANDLE representation of a MQTT PUBREC packet.] */
    /* Codes_SRS_MQTT_CODEC_07_016 : [If any error is encountered then mqtt_codec_publishRecieved shall return NULL.] */
    BUFFER_HANDLE result = constructPublishReply(PUBREC_TYPE, 0, packetId);
    return result;
}

BUFFER_HANDLE mqtt_codec_publishRelease(uint16_t packetId)
{
    /* Codes_SRS_MQTT_CODEC_07_017: [On success mqtt_codec_publishRelease shall return a BUFFER_HANDLE representation of a MQTT PUBREL packet.] */
    /* Codes_SRS_MQTT_CODEC_07_018 : [If any error is encountered then mqtt_codec_publishRelease shall return NULL.] */
    BUFFER_HANDLE result = constructPublishReply(PUBREL_TYPE, 2, packetId);
    return result;
}

BUFFER_HANDLE mqtt_codec_publishComplete(uint16_t packetId)
{
    /* Codes_SRS_MQTT_CODEC_07_019: [On success mqtt_codec_publishComplete shall return a BUFFER_HANDLE representation of a MQTT PUBCOMP packet.] */
    /* Codes_SRS_MQTT_CODEC_07_020 : [If any error is encountered then mqtt_codec_publishComplete shall return NULL.] */
    BUFFER_HANDLE result = constructPublishReply(PUBCOMP_TYPE, 0, packetId);
    return result;
}

BUFFER_HANDLE mqtt_codec_ping()
{
    /* Codes_SRS_MQTT_CODEC_07_021: [On success mqtt_codec_ping shall construct a BUFFER_HANDLE that represents a MQTT PINGREQ packet.] */
    BUFFER_HANDLE result = BUFFER_new();
    if (result != NULL)
    {
        if (BUFFER_enlarge(result, 2) != 0)
        {
            /* Codes_SRS_MQTT_CODEC_07_022: [If any error is encountered mqtt_codec_ping shall return NULL.] */
            BUFFER_delete(result);
            result = NULL;
        }
        else
        {
            uint8_t* iterator = BUFFER_u_char(result);
            if (iterator == NULL)
            {
                /* Codes_SRS_MQTT_CODEC_07_022: [If any error is encountered mqtt_codec_ping shall return NULL.] */
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                iterator[0] = PINGREQ_TYPE;
                iterator[1] = 0;
            }
        }
    }
    return result;
}

BUFFER_HANDLE mqtt_codec_subscribe(uint16_t packetId, SUBSCRIBE_PAYLOAD* subscribeList, size_t count, STRING_HANDLE trace_log)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_MQTT_CODEC_07_023: [If the parameters subscribeList is NULL or if count is 0 then mqtt_codec_subscribe shall return NULL.] */
    if (subscribeList == NULL || count == 0)
    {
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTT_CODEC_07_026: [mqtt_codec_subscribe shall return a BUFFER_HANDLE that represents a MQTT SUBSCRIBE message.]*/
        result = BUFFER_new();
        if (result != NULL)
        {
            if (constructSubscibeTypeVariableHeader(result, packetId) != 0)
            {
                /* Codes_SRS_MQTT_CODEC_07_025: [If any error is encountered then mqtt_codec_subscribe shall return NULL.] */
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                STRING_HANDLE sub_trace = NULL;
                if (trace_log != NULL)
                {
                    sub_trace = STRING_construct_sprintf(" | PACKET_ID: %"PRIu16, packetId);
                }
                /* Codes_SRS_MQTT_CODEC_07_024: [mqtt_codec_subscribe shall iterate through count items in the subscribeList.] */
                if (addListItemsToSubscribePacket(result, subscribeList, count, sub_trace) != 0)
                {
                    /* Codes_SRS_MQTT_CODEC_07_025: [If any error is encountered then mqtt_codec_subscribe shall return NULL.] */
                    BUFFER_delete(result);
                    result = NULL;
                }
                else
                {

                    if (trace_log != NULL)
                    {
                        STRING_concat(trace_log, "SUBSCRIBE");
                    }
                    if (constructFixedHeader(result, SUBSCRIBE_TYPE, SUBSCRIBE_FIXED_HEADER_FLAG) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_025: [If any error is encountered then mqtt_codec_subscribe shall return NULL.] */
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        if (trace_log != NULL)
                        {
                            (void)STRING_concat_with_STRING(trace_log, sub_trace);
                        }
                    }
                }
                if (sub_trace != NULL)
                {
                    STRING_delete(sub_trace);
                }
            }
        }
    }
    return result;
}

BUFFER_HANDLE mqtt_codec_unsubscribe(uint16_t packetId, const char** unsubscribeList, size_t count, STRING_HANDLE trace_log)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_MQTT_CODEC_07_027: [If the parameters unsubscribeList is NULL or if count is 0 then mqtt_codec_unsubscribe shall return NULL.] */
    if (unsubscribeList == NULL || count == 0)
    {
        result = NULL;
    }
    else
    {
        /* Codes_SRS_MQTT_CODEC_07_030: [mqtt_codec_unsubscribe shall return a BUFFER_HANDLE that represents a MQTT SUBSCRIBE message.] */
        result = BUFFER_new();
        if (result != NULL)
        {
            if (constructSubscibeTypeVariableHeader(result, packetId) != 0)
            {
                /* Codes_SRS_MQTT_CODEC_07_029: [If any error is encountered then mqtt_codec_unsubscribe shall return NULL.] */
                BUFFER_delete(result);
                result = NULL;
            }
            else
            {
                STRING_HANDLE unsub_trace = NULL;
                if (trace_log != NULL)
                {
                    unsub_trace = STRING_construct_sprintf(" | PACKET_ID: %"PRIu16, packetId);
                }
                /* Codes_SRS_MQTT_CODEC_07_028: [mqtt_codec_unsubscribe shall iterate through count items in the unsubscribeList.] */
                if (addListItemsToUnsubscribePacket(result, unsubscribeList, count, unsub_trace) != 0)
                {
                    /* Codes_SRS_MQTT_CODEC_07_029: [If any error is encountered then mqtt_codec_unsubscribe shall return NULL.] */
                    BUFFER_delete(result);
                    result = NULL;
                }
                else
                {
                    if (trace_log != NULL)
                    {
                        (void)STRING_copy(trace_log, "UNSUBSCRIBE");
                    }
                    if (constructFixedHeader(result, UNSUBSCRIBE_TYPE, UNSUBSCRIBE_FIXED_HEADER_FLAG) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_029: [If any error is encountered then mqtt_codec_unsubscribe shall return NULL.] */
                        BUFFER_delete(result);
                        result = NULL;
                    }
                    else
                    {
                        if (trace_log != NULL)
                        {
                            (void)STRING_concat_with_STRING(trace_log, unsub_trace);
                        }
                    }
                }
                if (unsub_trace != NULL)
                {
                    STRING_delete(unsub_trace);
                }
            }
        }
    }
    return result;
}

int mqtt_codec_bytesReceived(MQTTCODEC_HANDLE handle, const unsigned char* buffer, size_t size)
{
    int result;
    MQTTCODEC_INSTANCE* codec_Data = (MQTTCODEC_INSTANCE*)handle;
    /* Codes_SRS_MQTT_CODEC_07_031: [If the parameters handle or buffer is NULL then mqtt_codec_bytesReceived shall return a non-zero value.] */
    if (codec_Data == NULL)
    {
        result = MU_FAILURE;
    }
    /* Codes_SRS_MQTT_CODEC_07_031: [If the parameters handle or buffer is NULL then mqtt_codec_bytesReceived shall return a non-zero value.] */
    /* Codes_SRS_MQTT_CODEC_07_032: [If the parameters size is zero then mqtt_codec_bytesReceived shall return a non-zero value.] */
    else if (buffer == NULL || size == 0)
    {
        codec_Data->currPacket = PACKET_TYPE_ERROR;
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_MQTT_CODEC_07_033: [mqtt_codec_bytesReceived constructs a sequence of bytes into the corresponding MQTT packets and on success returns zero.] */
        result = 0;
        size_t index = 0;
        for (index = 0; index < size && result == 0; index++)
        {
            uint8_t iterator = ((int8_t*)buffer)[index];
            if (codec_Data->codecState == CODEC_STATE_FIXED_HEADER)
            {
                if (codec_Data->currPacket == UNKNOWN_TYPE)
                {
                    codec_Data->currPacket = processControlPacketType(iterator, &codec_Data->headerFlags);

                    // validate packet type and invalid reserved header flags
                    switch (codec_Data->currPacket)
                    {
                        case PACKET_INVALID1_TYPE:
                        case PACKET_INVALID2_TYPE:
                            codec_Data->currPacket = PACKET_TYPE_ERROR;
                            result = MU_FAILURE;
                            break;

                        case CONNECT_TYPE:
                        case CONNACK_TYPE:
                        case PUBACK_TYPE:
                        case PUBREC_TYPE:
                        case PUBCOMP_TYPE:
                        case SUBACK_TYPE:
                        case UNSUBACK_TYPE:
                        case PINGREQ_TYPE:
                        case PINGRESP_TYPE:
                        case DISCONNECT_TYPE:
                            if (codec_Data->headerFlags & 0x0F) // flags must be all zeros
                            {
                                codec_Data->currPacket = PACKET_TYPE_ERROR;
                                result = MU_FAILURE;
                            }
                            break;

                        case PUBREL_TYPE:
                        case SUBSCRIBE_TYPE:
                        case UNSUBSCRIBE_TYPE:
                            if ((codec_Data->headerFlags & 0x0F) != 0x02) // only bit 1 must be set
                            {
                                codec_Data->currPacket = PACKET_TYPE_ERROR;
                                result = MU_FAILURE;
                            }
                            break;

                        case PUBLISH_TYPE:
                        case CONTROL_PACKET_TYPE_INVALID: 
                        case PACKET_TYPE_ERROR:
                        case UNKNOWN_TYPE:
                            break;
                    }
                }
                else
                {
                    if (prepareheaderDataInfo(codec_Data, iterator) != 0)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_035: [If any error is encountered then the packet state will be marked as error and mqtt_codec_bytesReceived shall return a non-zero value.] */
                        codec_Data->currPacket = PACKET_TYPE_ERROR;
                        result = MU_FAILURE;
                    }
                    else if (codec_Data->currPacket == PINGRESP_TYPE)
                    {
                        // PINGRESP must not have a payload
                        if (((int8_t*)buffer)[index] == 0)
                        {
                            /* Codes_SRS_MQTT_CODEC_07_034: [Upon a constructing a complete MQTT packet mqtt_codec_bytesReceived shall call the ON_PACKET_COMPLETE_CALLBACK function.] */
                            completePacketData(codec_Data);
                        }
                        else
                        {
                            codec_Data->currPacket = PACKET_TYPE_ERROR;
                            result = MU_FAILURE;
                        }
                    }
                }
            }
            else if (codec_Data->codecState == CODEC_STATE_VAR_HEADER)
            {
                if (codec_Data->headerData == NULL)
                {
                    codec_Data->codecState = CODEC_STATE_PAYLOAD;
                }
                else
                {
                    uint8_t* dataBytes = BUFFER_u_char(codec_Data->headerData);
                    if (dataBytes == NULL)
                    {
                        /* Codes_SRS_MQTT_CODEC_07_035: [If any error is encountered then the packet state will be marked as error and mqtt_codec_bytesReceived shall return a non-zero value.] */
                        codec_Data->currPacket = PACKET_TYPE_ERROR;
                        result = MU_FAILURE;
                    }
                    else
                    {
                        // Increment the data
                        dataBytes += codec_Data->bufferOffset++;
                        *dataBytes = iterator;

                        size_t totalLen = BUFFER_length(codec_Data->headerData);
                        if (codec_Data->bufferOffset >= totalLen)
                        {
                            /* Codes_SRS_MQTT_CODEC_07_034: [Upon a constructing a complete MQTT packet mqtt_codec_bytesReceived shall call the ON_PACKET_COMPLETE_CALLBACK function.] */
                            completePacketData(codec_Data);
                        }
                    }
                }
            }
            else
            {
                /* Codes_SRS_MQTT_CODEC_07_035: [If any error is encountered then the packet state will be marked as error and mqtt_codec_bytesReceived shall return a non-zero value.] */
                codec_Data->currPacket = PACKET_TYPE_ERROR;
                result = MU_FAILURE;
            }
        }
    }
    return result;
}
