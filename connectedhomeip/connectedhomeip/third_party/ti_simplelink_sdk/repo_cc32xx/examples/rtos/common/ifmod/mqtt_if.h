/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MQTT_IF_H_
#define MQTT_IF_H_

#include <ti/net/mqtt/mqttclient.h>

#define MQTTCLIENT_MAX_SIMULTANEOUS_SUB_TOPICS 4

// MQTT events for event callback
enum
{
    MQTT_EVENT_CONNACK,
    MQTT_EVENT_SUBACK,
    MQTT_EVENT_PUBACK,
    MQTT_EVENT_UNSUBACK,
    MQTT_EVENT_SERVER_DISCONNECT,
    MQTT_EVENT_CLIENT_DISCONNECT,
    MQTT_EVENT_DESTROY,
    MQTT_EVENT_MAX
};

typedef void (*MQTT_IF_TopicCallback_f)(char* topic, char* payload, uint8_t qos);
typedef void (*MQTT_IF_EventCallback_f)(int32_t event);

typedef struct MQTT_IF_ClientParams
{
    char                *clientID;
    char                *username;
    char                *password;
    uint16_t            keepaliveTime;
    bool                cleanConnect;
    bool                mqttMode31;     // false 3.1.1 (default) : true 3.1
    bool                blockingSend;
    MQTTClient_Will     *willParams;
} MQTT_IF_ClientParams_t;

typedef struct MQTT_IF_initParams
{
    unsigned int stackSize;
    uint8_t threadPriority;
} MQTT_IF_InitParams_t;

/**
 \brief  Create the infrastructure for the MQTT_IF (mqtt interface) module

 \param[in] initParams: parameters to set stack size and thread priority for module

 \return Success 0 or Failure -1

 \sa MQTT_IF_Deinit()
 */
int MQTT_IF_Init(MQTT_IF_InitParams_t initParams);

/**
 \brief  Destroys the infrastructure created from calling MQTT_IF_Init

 \param[in] mqttClientHandle: handle for the mqtt client module instance

 \return Success 0 or Failure -1

 \sa MQTT_IF_Init()
 */
int MQTT_IF_Deinit();

/**
 \brief  Connect will set all client and connection parameters and initiate a connection to the broker.
         It will also register the event callback defined in the user's application and the create an
         internal context thread for the internal MQTT library

 \param[in] mqttClientParams: params for the mqtt client parameters
 \param[in] mqttConnParams: params for the mqtt connection parameters
 \param[in] mqttCB: the callback that is registered when for MQTT operation events (e.g. CONNACK, SUBACK...)

 \return Success 0 or Failure -1

 \sa MQTT_IF_Disconnect()
 */
MQTTClient_Handle MQTT_IF_Connect(MQTT_IF_ClientParams_t mqttClientParams, MQTTClient_ConnParams mqttConnParams, MQTT_IF_EventCallback_f mqttCB);

/**
 \brief  Instructs the internal MQTT library to close the MQTT connection to the broker

 \param[in] mqttClientHandle: handle for the mqtt client module instance

 \return Success 0 or Failure -1

 \sa MQTT_IF_Connect()
 */
int MQTT_IF_Disconnect(MQTTClient_Handle mqttClientHandle);

/**
 \brief  Subscribes to the topics specified by the caller in subscriptionInfo. Topic subscriptions are agnostic to the
 \ connection status. Meaning if you subscribe to topics then disconnect, the MQTT_IF module will still hold the topics
 \ so on re-connect the original topics are subscribed to again automatically.

 \param[in] mqttClientHandle: handle for the mqtt client module instance
 \param[in] subscriptionInfo: data structure containing all the data required to subscribe
 \param[in] numOfTopics: number of topics stored in subscriptionInfo

 \return Success 0 or Failure -1

 \sa MQTT_IF_Unsubscribe()
 */
int MQTT_IF_Subscribe(MQTTClient_Handle mqttClientHandle, char* topic, unsigned int qos, MQTT_IF_TopicCallback_f topicCB);

/**
 \brief  Unsubscribes to the topics specified by the caller in subscriptionInfo

 \param[in] mqttClientHandle: handle for the mqtt client module instance
 \param[in] subscriptionInfo: data structure containing all the data required to subscribe
 \param[in] numOfTopics: number of topics stored in subscriptionInfo

 \return Success 0 or Failure -1

 \sa MQTT_IF_Subscribe()
 */
int MQTT_IF_Unsubscribe(MQTTClient_Handle mqttClientHandle, char* topic);

/**
 \brief  Publishes to the topic specified by the user

 \param[in] mqttClientHandle: handle for the mqtt client module instance
 \param[in] topic: topic user wants to publish to
 \param[in] payload: payload to publish to topic
 \param[in] payloadLen: length of payload passed by the user
 \param[in] flags QOS define MQTT_PUBLISH_QOS_0, MQTT_PUBLISH_QOS_1 or MQTT_PUBLISH_QOS_2
             use MQTT_PUBLISH_RETAIN is message should be retained

 \return Success 0 or Failure -1

 */
//\param[in] flags QOS define MQTT_PUBLISH_QOS_0, MQTT_PUBLISH_QOS_1 or MQTT_PUBLISH_QOS_2
//use MQTT_PUBLISH_RETAIN if message should be retained
int MQTT_IF_Publish(MQTTClient_Handle mqttClientHandle, char* topic, char* payload, unsigned short payloadLen, int flags);

#endif /* MQTT_IF_H_ */
