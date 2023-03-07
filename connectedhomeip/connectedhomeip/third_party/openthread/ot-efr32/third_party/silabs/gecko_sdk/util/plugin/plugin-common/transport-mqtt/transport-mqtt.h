// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#ifndef __TRANSPORT_MQTT_H
#define __TRANSPORT_MQTT_H

typedef enum {
  EMBER_AF_PLUGIN_TRANSPORT_MQTT_STATE_CONNECTED,
  EMBER_AF_PLUGIN_TRANSPORT_MQTT_STATE_DISCONNECTED
} EmberAfPluginTransportMqttState;

/** @brief MQTT Subscribe
 *
 * This function should be called to subscribe to a specific topic.  If the
 * publish succeeds then true should be returned, if the publish fails or
 * there is no connection to a broker false should be returned.
 *
 * @param topic String contains the topic for a message subscription
 */
bool emberAfPluginTransportMqttSubscribe(const char* topic);

/** @brief MQTT Publish
 *
 * This function should be called to publish to a specific topic. If the publish
 * succeeds then true should be returned, if the publish fails or there is no
 * connection to a broker false should be returned.
 *
 * @param topic String contains the topic for the message to be published
 * @param content String contains the payload for the message to be published
 */
bool emberAfPluginTransportMqttPublish(const char* topic, const char* paylaod);

#endif // __TRANSPORT_MQTT_H
