/*******************************************************************************
* @file  rsi_mqtt_client.h
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef RSI_MQTT_CLIENT_H
#define RSI_MQTT_CLIENT_H

#include "MQTTClient.h"
/******************************************************
 * *                      Macros
 * ******************************************************/

// MQTT version
#define MQTT_VERSION 3

// MQTT Connect timeout
#define MQTT_CONNECT_TIME_OUT 20000

// Rx buffer size
#define MQTT_CLIENT_RX_BUFFER_SIZE 1500

// Tx buffer size
#define MQTT_CLIENT_TX_BUFFER_SIZE 1500

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

typedef struct rsi_mqtt_client_info_s {
  // MQTT Client structure
  Client mqtt_client;
  // MQTT broker port
  uint32_t server_port;
  // MQTT client port
  uint32_t client_port;

  // Server IP address
  rsi_ip_addr_t server_ip;

  // Keep alive interval
  uint16_t keep_alive_interval;

  // MQTT client TX buffer
  int8_t *mqtt_tx_buffer;

  // MQTT client RX buffer
  int8_t *mqtt_rx_buffer;

} rsi_mqtt_client_info_t;

// Total MQTT initialization buffer required for MQTT client info storage
#define MQTT_CLIENT_INFO_SIZE \
  (sizeof(rsi_mqtt_client_info_t) + sizeof(Network) + MQTT_CLIENT_TX_BUFFER_SIZE + MQTT_CLIENT_RX_BUFFER_SIZE)

/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
rsi_mqtt_client_info_t *rsi_mqtt_client_init(int8_t *buffer,
                                             uint32_t length,
                                             int8_t *server_ip,
                                             uint32_t server_port,
                                             uint32_t client_port,
                                             uint16_t flags,
                                             uint16_t keep_alive_interval);
#ifdef ASYNC_MQTT
int32_t rsi_mqtt_connect(rsi_mqtt_client_info_t *rsi_mqtt_client,
                         uint16_t flags,
                         int8_t *client_id,
                         int8_t *username,
                         int8_t *password,
                         void (*callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length));
#else
int32_t rsi_mqtt_connect(rsi_mqtt_client_info_t *rsi_mqtt_client,
                         uint16_t flags,
                         int8_t *client_id,
                         int8_t *username,
                         int8_t *password);
#endif
int32_t rsi_mqtt_disconnect(rsi_mqtt_client_info_t *rsi_mqtt_client);
int32_t rsi_mqtt_publish(rsi_mqtt_client_info_t *rsi_mqtt_client, int8_t *topic, MQTTMessage *publish_msg);
int32_t rsi_mqtt_subscribe(rsi_mqtt_client_info_t *rsi_mqtt_client,
                           uint8_t qos,
                           int8_t *topic,
                           void (*call_back_handler_ptr)(MessageData *md));
int32_t rsi_mqtt_unsubscribe(rsi_mqtt_client_info_t *rsi_mqtt_client, int8_t *topic);
int32_t rsi_mqtt_poll_for_recv_data(rsi_mqtt_client_info_t *rsi_mqtt_client, int time_out);
void mqtt_disconnect(Network *n);

#endif
