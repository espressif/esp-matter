// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "event_control/event.h"
#include "hal/hal.h"
#include "app/framework/include/af.h"
#include <pthread.h>
#include "transport-mqtt.h"

// MQTT open-source-module include
#include "util/third_party/paho.mqtt.c/src/MQTTAsync.h"

// MQTT definitions
// The broker address could be different if we connect to the cloud and may
// need paramaterization
#define MQTT_KEEP_ALIVE_INTERVAL_S        20 // seconds
#define MQTT_RETAINED                     0 // not retained
#define MQTT_RECONNECT_RATE_MS            1000 // milliseconds

// QoS definitions - generated in EMBER_AF_PLUGIN_TRANSPORT_MQTT_QOS value
#define QO_S0_ONLY_ONCE                   0
#define QO_S1_AT_LEAST_ONCE               1
#define QO_S2_EXACTLY_ONCE                2

#define EUI64_NULL_TERMINATED_STRING_SIZE 17 // 16 ASCII chars + 1 NULL char

// MQTT objects and state variables
static char mqttClientIdString[
  EMBER_AF_PLUGIN_TRANSPORT_MQTT_CLIENT_ID_PREFIX_LENGTH
  + EUI64_NULL_TERMINATED_STRING_SIZE] = { 0 };
static volatile bool mqttConnected = false;
static pthread_mutex_t mqttConnectedLock;
static MQTTAsync mqttClient;
static MQTTAsync_connectOptions mqttConnectOptions =
  MQTTAsync_connectOptions_initializer;

// MQTT helper functions
static bool mqttConnect(void);

// MQTT protected connection check helper functions
static bool isMqttConnectedProtected(void);
static void updateMqttConnectedProtected(bool connected);
static void acquireMqttConnectedLock(void);
static void releaseMqttConnectedLock(void);

// MQTT client callback definitions
static MQTTAsync_onFailure mqttConnectFailureCallback;
static MQTTAsync_onSuccess mqttConnectSuccessCallback;
static MQTTAsync_connectionLost mqttConnectionLostCallback;
static MQTTAsync_onFailure mqttTopicSubscribeFailureCallack;
static MQTTAsync_onFailure mqttTopicPublishFailureCallack;
static MQTTAsync_messageArrived mqttMessageArrivedCallback;

// Event controls
EmberEventControl emberAfPluginTransportMqttBrokerReconnectEventControl;

void emberAfPluginTransportMqttInitCallback(void)
{
  emberAfCorePrintln("MQTT Client Init");
  EmberEUI64 eui64;
  char euiString[EUI64_NULL_TERMINATED_STRING_SIZE] = { 0 };
  int status;

  status = pthread_mutex_init(&mqttConnectedLock, NULL);
#ifndef EMBER_TEST
  if (status != 0) {
    emberAfCorePrintln("pthread_mutex_init failed, status = 0x%X", status);
    assert(false);
  }
#endif

  // Save our EUI information
  emberAfGetEui64(eui64);
  snprintf(euiString,
           EUI64_NULL_TERMINATED_STRING_SIZE,
           "%02X%02X%02X%02X%02X%02X%02X%02X",
           eui64[7],
           eui64[6],
           eui64[5],
           eui64[4],
           eui64[3],
           eui64[2],
           eui64[1],
           eui64[0]);

  strcat(mqttClientIdString, EMBER_AF_PLUGIN_TRANSPORT_MQTT_CLIENT_ID_PREFIX);
  strcat(mqttClientIdString, euiString);
  emberAfCorePrintln("MQTT Client ID = %s", mqttClientIdString);

  status = MQTTAsync_create(&mqttClient,
                            EMBER_AF_PLUGIN_TRANSPORT_MQTT_BROKER_ADDRESS,
                            mqttClientIdString,
                            MQTTCLIENT_PERSISTENCE_NONE,
                            NULL); // persistence_context is NULL since
                                   // persistence is NONE
  if (status != MQTTASYNC_SUCCESS) {
    emberAfCorePrintln("MQTTAsync_create failed, status = 0x%X", status);
    assert(false);
  }

  status = MQTTAsync_setCallbacks(mqttClient,
                                  NULL, // context is NULL, no app context used
                                        // here
                                  mqttConnectionLostCallback,
                                  mqttMessageArrivedCallback,
                                  NULL); // dc is NULL,
                                         // MQTTAsync_deliveryComplete unusued
  if (status != MQTTASYNC_SUCCESS) {
    emberAfCorePrintln("MQTTAsync_setCallbacks failed, status = 0x%X", status);
    assert(false);
  }

  mqttConnectOptions.keepAliveInterval = MQTT_KEEP_ALIVE_INTERVAL_S;
  mqttConnectOptions.cleansession = 1;
  mqttConnectOptions.onSuccess = mqttConnectSuccessCallback;
  mqttConnectOptions.onFailure = mqttConnectFailureCallback;
  mqttConnectOptions.context = mqttClient;

// Note that this won't try to connect to MQTT if we are in EMBER_TEST mode
// because the simulator doesn't work with this plugin, we will simply not
// start and never fire our connection event
#ifndef EMBER_TEST
  // Start our connection event timer to attempt to connect to the broker
  emberEventControlSetActive(
    emberAfPluginTransportMqttBrokerReconnectEventControl);
#endif
}

void emberAfPluginTransportMqttBrokerReconnectEventHandler(void)
{
  bool currentMqttConnected = false;
  static bool lastMqttConnected = false;

  emberEventControlSetInactive(
    emberAfPluginTransportMqttBrokerReconnectEventControl);

  // Get the current connection state
  currentMqttConnected = isMqttConnectedProtected();

  // If not connected, reconnect to the broker
  if (!currentMqttConnected) {
    mqttConnectOptions.keepAliveInterval = MQTT_KEEP_ALIVE_INTERVAL_S;
    mqttConnectOptions.cleansession = 1;

    emberAfCorePrintln("Attempting to reconnect to broker");
    mqttConnect();
  }

  // Check if the connection state changed
  if (currentMqttConnected != lastMqttConnected) {
    if (currentMqttConnected) {
      emberAfPluginTransportMqttStateChangedCallback(
        EMBER_AF_PLUGIN_TRANSPORT_MQTT_STATE_CONNECTED);
    } else {
      emberAfPluginTransportMqttStateChangedCallback(
        EMBER_AF_PLUGIN_TRANSPORT_MQTT_STATE_DISCONNECTED);
    }
  }

  // Reset our last state for the next execution of this event
  lastMqttConnected = currentMqttConnected;

  emberEventControlSetDelayMS(
    emberAfPluginTransportMqttBrokerReconnectEventControl,
    MQTT_RECONNECT_RATE_MS);
}

static bool mqttConnect(void)
{
  int status = MQTTAsync_connect(mqttClient, &mqttConnectOptions);
  if (status != MQTTASYNC_SUCCESS) {
    emberAfCorePrintln("MQTTAsync_connect failed, status = %d", status);
    return false;
  }
  return true;
}

static void mqttConnectFailureCallback(void                 * context,
                                       MQTTAsync_failureData* response)
{
  emberAfCorePrintln("MQTTAsync_connect failed, returned response = %d",
                     response ? response->code : 0);

  updateMqttConnectedProtected(false);
}

static void mqttConnectSuccessCallback(void                 * context,
                                       MQTTAsync_successData* response)
{
  emberAfCorePrintln("MQTT connected to broker");
  updateMqttConnectedProtected(true);
}

static void mqttConnectionLostCallback(void *context, char *cause)
{
  emberAfCorePrintln("MQTT connection lost, cause = %s", cause);
  updateMqttConnectedProtected(false);
}

bool emberAfPluginTransportMqttSubscribe(const char* topic)
{
  bool returnStatus = true;
  int sendStatus;
  MQTTAsync_responseOptions mqttSubscribeResponseOptions =
    MQTTAsync_responseOptions_initializer;

  if (isMqttConnectedProtected()) {
    emberAfCorePrintln("Subscribing to topic \"%s\" using QoS%d",
                       topic,
                       EMBER_AF_PLUGIN_TRANSPORT_MQTT_QOS);

    mqttSubscribeResponseOptions.onFailure = mqttTopicSubscribeFailureCallack;
    mqttSubscribeResponseOptions.context = mqttClient;

    sendStatus = MQTTAsync_subscribe(mqttClient,
                                     topic,
                                     EMBER_AF_PLUGIN_TRANSPORT_MQTT_QOS,
                                     &mqttSubscribeResponseOptions);
    if (sendStatus != MQTTASYNC_SUCCESS) {
      emberAfCorePrintln("MQTTAsync_subscribe failed, status = %d", sendStatus);
      returnStatus = false;
    }
  } else {
    emberAfCorePrintln("MQTT not connected, cannot subscribe to: %s", topic);
    returnStatus = false;
  }

  return returnStatus;
}

static void mqttTopicSubscribeFailureCallack(void                 * context,
                                             MQTTAsync_failureData* response)
{
  emberAfCorePrintln("MQTTAsync_subscribe failed, returned response = %d",
                     response ? response->code : 0);
}

static int mqttMessageArrivedCallback(void              *context,
                                      char              *topicName,
                                      int               topicLen,
                                      MQTTAsync_message *message)
{
  if (emberAfPluginTransportMqttMessageArrivedCallback(topicName,
                                                       message->payload)) {
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return TRUE; // Return TRUE to let the MQTT library know we handled the
                 // message
  }
  return FALSE; // Return FALSE to let the MQTT library know we didn't handle
                // the message
}

bool emberAfPluginTransportMqttPublish(const char* topic, const char* payload)
{
  bool returnStatus = true;
  int sendStatus;
  MQTTAsync_message mqttMessage = MQTTAsync_message_initializer;
  MQTTAsync_responseOptions mqttPublishResponseOptions =
    MQTTAsync_responseOptions_initializer;

  mqttMessage.payload = (char*)payload;
  mqttMessage.payloadlen = strlen(payload);
  mqttMessage.qos = EMBER_AF_PLUGIN_TRANSPORT_MQTT_QOS;
  mqttMessage.retained = MQTT_RETAINED;

  if (isMqttConnectedProtected()) {
    mqttPublishResponseOptions.onFailure = mqttTopicPublishFailureCallack;
    mqttPublishResponseOptions.context = mqttClient;

    sendStatus = MQTTAsync_sendMessage(mqttClient,
                                       topic,
                                       &mqttMessage,
                                       &mqttPublishResponseOptions);
    if (sendStatus != MQTTASYNC_SUCCESS) {
      emberAfCorePrintln("MQTTAsync_sendMessage failed, status = %d",
                         sendStatus);
      returnStatus = false;
    }
  } else {
    emberAfCorePrintln("MQTT not connected, message not sent: %s - %s",
                       topic,
                       payload);
    returnStatus = false;
  }

  return returnStatus;
}

static void mqttTopicPublishFailureCallack(void                 * context,
                                           MQTTAsync_failureData* response)
{
  emberAfCorePrintln("MQTTAsync_sendMessage failed, returned response = %d",
                     response ? response->code : 0);
}

static bool isMqttConnectedProtected(void)
{
  bool mqttConnectedProtected;
  acquireMqttConnectedLock();
  mqttConnectedProtected = mqttConnected;
  releaseMqttConnectedLock();
  return mqttConnectedProtected;
}

static void updateMqttConnectedProtected(bool connected)
{
  acquireMqttConnectedLock();
  mqttConnected = connected;
  releaseMqttConnectedLock();
}

static void acquireMqttConnectedLock(void)
{
#ifndef EMBER_TEST
  int status;

  // Locking the mutex shouldn't fail, so we simply assert if it does
  status = pthread_mutex_lock(&mqttConnectedLock);
  if (status != 0) {
    emberAfCorePrintln("pthread_mutex_lock failed, status = 0x%X", status);
  }
#endif
}

static void releaseMqttConnectedLock(void)
{
#ifndef EMBER_TEST
  int status;

  // Unlocking the mutex shouldn't fail, so we simply assert if it does
  status = pthread_mutex_unlock(&mqttConnectedLock);
  if (status != 0) {
    emberAfCorePrintln("pthread_mutex_lock failed, status = 0x%X", status);
  }
#endif
}
