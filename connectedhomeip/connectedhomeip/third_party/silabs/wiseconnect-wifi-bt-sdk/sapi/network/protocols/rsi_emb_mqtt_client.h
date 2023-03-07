/*******************************************************************************
* @file  rsi_emb_mqtt_client.h
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

#ifndef RSI_EMB_MQTT_CLIENT_H
#define RSI_EMB_MQTT_CLIENT_H

/******************************************************
 * *                      Macros
 * ******************************************************/

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
` * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

int32_t rsi_emb_mqtt_client_init(int8_t *server_ip,
                                 uint32_t server_port,
                                 uint32_t client_port,
                                 uint16_t flags,
                                 uint16_t keep_alive_interval,
                                 int8_t *clientid,
                                 int8_t *username,
                                 int8_t *password);
int32_t rsi_emb_mqtt_connect(uint8_t mqtt_flags, int8_t *will_topic, uint16_t will_message_len, int8_t *will_message);
int32_t rsi_emb_mqtt_publish(int8_t *topic, rsi_mqtt_pubmsg_t *publish_msg);
int32_t rsi_emb_mqtt_subscribe(uint8_t qos, int8_t *topic);
int32_t rsi_emb_mqtt_unsubscribe(int8_t *topic);
int32_t rsi_emb_mqtt_disconnect(void);
int32_t rsi_emb_mqtt_destroy(void);
int32_t rsi_emb_mqtt_register_call_back(uint32_t callback_id,
                                        void (*call_back_handler_ptr)(uint16_t status,
                                                                      uint8_t *buffer,
                                                                      const uint32_t length));
int32_t rsi_cal_mqtt_packet_len(int32_t rem_len);
#endif
