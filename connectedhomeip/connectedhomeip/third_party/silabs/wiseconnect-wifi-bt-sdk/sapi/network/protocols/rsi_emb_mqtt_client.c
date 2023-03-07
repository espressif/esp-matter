/*******************************************************************************
* @file  rsi_emb_mqtt_client.c
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

#include "rsi_driver.h"
#include "rsi_emb_mqtt_client.h"

#define RSI_LENGTH_ADJ 2
/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief     Create MQTT objects. TCP level connection happens in this API. This is a blocking API.
 * @pre  \ref rsi_config_ipaddress() API needs to be called before this API.
 * @param[in]  server_ip           - MQTT broker IP address to connect
 * @param[in]  server_port         - Port number of MQTT broker
 * @param[in]  client_port         - Port number of MQTT client (local port)
 * @param[in]  flags               - Network flags. Each bit in the flag has its own significance
 * 
 *  Flags                           |    Description
 *  :-------------------------------|:----------------------------------------------------------------------
 *  BIT(0) - Clean Session          |     1      - Enable clean session for clearing the historic data \n
 *  ^                               |     0      - Disable clean session
 *  BIT(1) - SSL flag               |     1      - SSL Enable \n
 *  ^                               |     0      - SSL Disable
 *  BIT(2) - Server IP version      |     1      - IPV6 \n
 *  ^                               |     0      - IPV4
 
 * @param[in]  keep_alive_interval - MQTT client keep alive interval
 * @param[in]  clientid            - client_id of MQTT_client which should be unique for different clients.
 * @param[in]  username            - user_name of the MQTT_client which is a credential for logging to MQTT_server as an authentication
 * @param[in]  password            - Password of the MQTT_client which is also credential for MQTT_server as an authentication
 * @return     Zero                - Success  \n
 *	           Negative Value      - Failure \n
 *				                           -2     - Invalid parameters \n
 *				                           -3     - Command given in wrong state \n
 *				                           -4     - Packet allocation failure \n
 *		                   		         -5     - Command not supported \n
 *				                           -32    - Network command in progress \n
 *				                           -44    - Parameter length exceeds maximum value
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 *
 */

int32_t rsi_emb_mqtt_client_init(int8_t *server_ip,
                                 uint32_t server_port,
                                 uint32_t client_port,
                                 uint16_t flags,
                                 uint16_t keep_alive_interval,
                                 int8_t *clientid,
                                 int8_t *username,
                                 int8_t *password)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_ENTRY, NETWORK, LOG_INFO);

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_client_init_t *mqtt_ops = NULL;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if ((clientid == NULL) || (server_ip == NULL) || (!client_port) || (!server_port)) {
    SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  // If any of the parameter is valid and exceeds max allowed length, then return error
  if ((rsi_strlen(clientid) > (RSI_EMB_MQTT_CLIENTID_MAX_LEN - RSI_LENGTH_ADJ))
      || ((username != NULL) && (rsi_strlen(username) > (RSI_EMB_MQTT_USERNAME_MAX_LEN - RSI_LENGTH_ADJ)))
      || ((password != NULL) && (rsi_strlen(password) > (RSI_EMB_MQTT_PASSWORD_MAX_LEN - RSI_LENGTH_ADJ)))) {
    SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_PARAMTER_LENGTH_EXCEEDS_MAX_VAL, NETWORK, LOG_ERROR);
    return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_emb_mqtt_client_init_t *)pkt->data;

    // Memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_client_init_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_CLIENT_INIT);

    // Copy MQTT Server port
    rsi_uint32_to_4bytes(mqtt_ops->server_port, server_port);

    // Copy MQTT Client port
    rsi_uint32_to_4bytes(mqtt_ops->client_port, client_port);

    // Copy MQTT Keep Alive period
    rsi_uint16_to_2bytes(mqtt_ops->keep_alive_interval, keep_alive_interval);

    if (!(flags & RSI_EMB_MQTT_IPV6_ENABLE)) {
      // Fill IP version
      rsi_uint32_to_4bytes(mqtt_ops->server_ip.ip_version, RSI_IP_VERSION_4);
      // Fill IP address
      memcpy(mqtt_ops->server_ip.server_ip_address.ipv4_address, server_ip, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Fill IP version
      rsi_uint32_to_4bytes(mqtt_ops->server_ip.ip_version, RSI_IP_VERSION_6);
      // Fill IPv6 address
      memcpy(mqtt_ops->server_ip.server_ip_address.ipv6_address, server_ip, RSI_IPV6_ADDRESS_LENGTH);
    }

    // Copy Client id length
    mqtt_ops->clientID_len = rsi_strlen(clientid);

    // Copy Client ID
    rsi_strcpy(&mqtt_ops->client_id, clientid);

    // Copy username length
    mqtt_ops->username_len = rsi_strlen(username);

    if (username) {
      // Copy username if username is not NULL
      rsi_strcpy(&mqtt_ops->user_name, username);
    }
    // Copy password length
    mqtt_ops->password_len = rsi_strlen(password);
    if (password) {
      // Copy password if password is not NULL
      rsi_strcpy(&mqtt_ops->password, password);
    }

    if (flags & RSI_EMB_MQTT_CLEAN_SESSION) {
      mqtt_ops->clean = 1;
    }
    if (flags & RSI_EMB_MQTT_SSL_ENABLE) {
      rsi_wlan_cb_non_rom->emb_mqtt_ssl_enable = 1;

      mqtt_ops->encrypt = 1;
    }

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_CLIENT_INIT_EXIT, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief       Connect to MQTT Server/Broker. MQTT level connection happens in this API. This is a blocking API.
 * @pre         \ref rsi_emb_mqtt_client_init() API needs to be called before this API.
 * @param[in]   mqtt_flags       - Network flags. Each bit in the flag has its own significance \n
 * 
 *  Flags                           |    Description
 *  :-------------------------------|:--------------------------------------
 *  BIT(6) - pwdFlag                |     1      - Enable pwdFlag \n
 *  ^                               |     0      - Disable pwdFlag
 *  BIT(7) - usrFlag                |     1      - Enable usrFlag \n
 *  ^                               |     0      - Disable usrFlag
 * 
 * @param[in]   will_topic       - Will topic that the MQTT client wants the MQTT Server to publish when disconnected unexpectedly.
 * @param[in]   will_message_len -  Length of will_message
 * @param[in]   will_message     - Will message issued by the MQTT_Server for a broken client.
 * @note 	will_topic and will_message are not supported and should be NULL.
 * @return      Zero             - Success  \n
 *              Negative Value   - Failure \n
 *				                         -3     - Command given in wrong state \n
 *				                         -4     - Packet allocation failure \n
 *			                	         -5     - Command not supported \n
 *			                  	       -32    - Network command in progress
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes. \n
 * @note        For connecting Embedded_MQTT(emb_mqtt) over SSL : \n Enable TCP_IP_FEAT_SSL in Opermode parameters as below \n 
 *                                                  #define RSI_TCP_IP_FEATURE_BIT_MAP (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT) \n
 *                                                  Load the related SSL Certificates in the module using rsi_wlan_set_certificate() API.
 *
 */

int32_t rsi_emb_mqtt_connect(uint8_t mqtt_flags, int8_t *will_topic, uint16_t will_message_len, int8_t *will_message)
{
  // Added to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(will_message_len);
  UNUSED_PARAMETER(will_topic);
  UNUSED_PARAMETER(will_message);

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_CONNECT_ENTRY, NETWORK, LOG_INFO);

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_connect_t *mqtt_ops = NULL;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_CONNECT_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_CONNECT_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_CONNECT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_emb_mqtt_connect_t *)pkt->data;

    // Memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_connect_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_CONNECT);

    if (mqtt_flags & RSI_EMB_MQTT_USER_FLAG) {
      mqtt_ops->usrFlag = 1;
    }
    if (mqtt_flags & RSI_EMB_MQTT_PWD_FLAG) {
      mqtt_ops->pwdFlag = 1;
    }
    // Will messages are not supported

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set MQTT connect command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_CONNECT_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_CONNECT_EXIT, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief      Publish the given message on the topic specified. This is a blocking API.
 * @pre   \ref rsi_emb_mqtt_connect() API needs to be called before this API.
 * @param[in]  topic          - Topic string on which MQTT client wants to publish data
 * @param[in]  publish_msg    - Publish message
 * @return     Zero           -  Success  \n
 *             Negative Value - Failure \n
 *                              -5  - Command not supported \n
 *                              -2  - Invalid parameters \n
 *                              -3  - Command given in wrong state \n
 *                              -4  - Packet allocation failure \n
 *                              -44 - Parameter length exceeds maximum value \n
 *                              -32 - Network command in progress
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_emb_mqtt_publish(int8_t *topic, rsi_mqtt_pubmsg_t *publish_msg)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_PUBLISH_ENTRY, NETWORK, LOG_INFO);

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_snd_pub_t *mqtt_ops = NULL;

  uint16_t max_payload_size;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_PUBLISH_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_PUBLISH_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if ((topic == NULL) || (publish_msg == NULL) || (publish_msg->qos > 2)) {
    SL_PRINTF(SL_EMB_MQTT_PUBLISH_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_PUBLISH_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_emb_mqtt_snd_pub_t *)pkt->data;

    // Memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_snd_pub_t));

    // Length of TOPIC
    mqtt_ops->topic_len = rsi_strlen(topic);

    // Strlen
    if (mqtt_ops->topic_len > (RSI_EMB_MQTT_TOPIC_MAX_LEN - RSI_LENGTH_ADJ)) {
      SL_PRINTF(SL_EMB_MQTT_PUBLISH_PARAMTER_LENGTH_EXCEEDS_MAX_VAL_1, NETWORK, LOG_ERROR);
      return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
    }

    if (rsi_wlan_cb_non_rom->emb_mqtt_ssl_enable) {
      max_payload_size = RSI_EMB_MQTT_SSL_PUB_MAX_LEN;
    } else {
      max_payload_size = RSI_EMB_MQTT_PUB_MAX_LEN;
    }

    if (max_payload_size < rsi_cal_mqtt_packet_len((2 + mqtt_ops->topic_len + publish_msg->payloadlen))) {
      SL_PRINTF(SL_EMB_MQTT_PUBLISH_PARAMTER_LENGTH_EXCEEDS_MAX_VAL_2, NETWORK, LOG_ERROR);
      return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
    }

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_SND_PUB_PKT);

    // Copying TOPIC
    rsi_strcpy(&mqtt_ops->topic, topic);

    mqtt_ops->qos = publish_msg->qos;

    mqtt_ops->retained = publish_msg->retained;

    mqtt_ops->dup = publish_msg->dup;

    rsi_uint16_to_2bytes(mqtt_ops->msg_len, publish_msg->payloadlen);

    mqtt_ops->msg = (int8_t *)(pkt->data + sizeof(rsi_emb_mqtt_snd_pub_t));
    if (publish_msg->payloadlen) {

      memcpy(mqtt_ops->msg, publish_msg->payload, publish_msg->payloadlen);
    }
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_PUBLISH_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_PUBLISH_COMMAND_SEND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief      Subscribe to the topic specified. Thus, MQTT client will receive any data that is published on this topic. This is a blocking API.
 * @pre        \ref rsi_emb_mqtt_connect() API needs to be called before this API.
 * @param[in]  qos   	        - Quality of service of message at MQTT protocol level. Valid values are 0, 1, 2.
 * @param[in]  topic 	        - Topic string on which MQTT client wants to subscribe.
 * @return     Zero           -  Success  \n
 *             Negative Value - Failure \n
 *                              -5  - Command not supported \n
 *                              -2  - Invalid parameters \n
 *                              -3  - Command given in wrong state \n
 *                              -4  - Packet allocation failure \n
 *                              -44 - Parameter length exceeds maximum value \n
 *                              -32 - Network command in progress
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */
int32_t rsi_emb_mqtt_subscribe(uint8_t qos, int8_t *topic)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_ENTRY, NETWORK, LOG_INFO);

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_sub_t *mqtt_ops = NULL;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if (topic == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }
  // Strlen
  if (rsi_strlen(topic) > (RSI_EMB_MQTT_TOPIC_MAX_LEN - RSI_LENGTH_ADJ)) {
    SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_PARAMTER_LENGTH_EXCEEDS_MAX_VAL, NETWORK, LOG_ERROR);
    return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_emb_mqtt_sub_t *)pkt->data;

    // Memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_sub_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_SUBSCRIBE);

    // Length of topic
    mqtt_ops->topic_len = rsi_strlen(topic);

    rsi_strcpy(&mqtt_ops->topic, topic);

    mqtt_ops->qos = qos;

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set subscribe command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_SUBSCRIBE_COMMAND_SEND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief      Unsubscribe to the topic specified.Thus, MQTT client will not receive any data published on this topic. This is a blocking API.
 * @pre        \ref rsi_emb_mqtt_connect() API needs to be called before this API.
 * @param[in]  topic 	        - Topic string to which MQTT client wants to unsubscribe.
 * @return     Zero           - Success  \n
 *             Negative Value - Failure \n
 *                         	    -5  - Command not supported \n
 *                         	    -2  - Invalid parameters \n
 *                        	    -3  - Command given in wrong state \n
 *                        	    -4  - Packet allocation failure \n
 *                        	    -44 - Parameter length exceeds maximum value \n
 *                       	      -32 - Network command in progress
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_emb_mqtt_unsubscribe(int8_t *topic)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_ENTRY, NETWORK, LOG_INFO);

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_emb_mqtt_unsub_t *mqtt_ops = NULL;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if (topic == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // Strlen
  if (rsi_strlen(topic) > (RSI_EMB_MQTT_TOPIC_MAX_LEN - RSI_LENGTH_ADJ)) {
    SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_PARAMTER_LENGTH_EXCEEDS_MAX_VAL, NETWORK, LOG_ERROR);
    return RSI_ERROR_PARAMTER_LENGTH_EXCEEDS_MAX_VAL;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_emb_mqtt_unsub_t *)pkt->data;

    // Memset
    memset(mqtt_ops, 0, sizeof(rsi_emb_mqtt_unsub_t));

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_UNSUBSCRIBE);

    // Length of TOPIC
    mqtt_ops->topic_len = rsi_strlen(topic);

    // Copy topic
    rsi_strcpy(&mqtt_ops->topic, topic);
#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send MQTT cmd
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_UNSUBSCRIBE_COMMAND_SEND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief      Disconnect the client from MQTT Server/Broker. TCP and MQTT level disconnection take place here. This is a blocking API.
 * @pre        \ref rsi_emb_mqtt_connect()  API needs to be called before this API.
 * @param[in]  Void
 * @return     Zero             -  Success \n
 *             Negative Value   - Failure \n
 *                         	      -5  - Command not supported \n
 *                         	      -3  - Command given in wrong state \n
 *                         	      -4  - Packet allocation failure \n
 *                         	      -32 - Network command in progress
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_emb_mqtt_disconnect()
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_DISCONNECT_ENTRY, NETWORK, LOG_INFO);

  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_emb_mqtt_command_t *mqtt_ops = NULL;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_DISCONNECT_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_DISCONNECT_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_DISCONNECT_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_req_emb_mqtt_command_t *)pkt->data;

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_DISCONNECT);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_DISCONNECT_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_DISCONNECT_COMMAND_SEND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup NETWORK2
* @{
*/
/*==============================================*/
/**
 * @brief      Delete MQTT clients profile configuration. TCP level disconnection happens here, if required. This is a blocking API.
 * @pre        \ref rsi_emb_mqtt_client_init() API needs to be called before this API, based on requirement.This API can also be issued after \ref rsi_emb_mqtt_disconnect() API.
 * @param[in]  Void
 * @return      Zero 	         -  Success  \n
 * 	   	        Negative Value - Failure \n
 *             			             -5  - Command not supported \n
 *             			             -3  - Command given in wrong state \n
 *             			             -4  - Packet allocation failure \n
 *             			             -32 - Network command in progress
 * @note        Refer to Error Codes section for the description of the above error codes \ref error-codes.
 */

int32_t rsi_emb_mqtt_destroy()
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EMB_MQTT_DESTROY_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_req_emb_mqtt_command_t *mqtt_ops = NULL;

  if (wlan_cb->opermode != RSI_WLAN_CLIENT_MODE) {
    // Command not supported
    SL_PRINTF(SL_EMB_MQTT_DESTROY_COMMAND_NOT_SUPPORTED, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_NOT_SUPPORTED;
  } else {
    // If state is not in ipconfig done state
    if ((wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE)) {
      // Command given in wrong state
      SL_PRINTF(SL_EMB_MQTT_DESTROY_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change NWK state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EMB_MQTT_DESTROY_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mqtt_ops = (rsi_req_emb_mqtt_command_t *)pkt->data;

    rsi_uint32_to_4bytes(mqtt_ops->command_type, RSI_EMB_MQTT_COMMAND_DESTROY);

#ifndef RSI_NWK_SEM_BITMAP
    rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    // Send set FTP create command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EMB_MQTT_CLIENT, pkt);

    // Wait on NWK semaphore
    rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_EMB_MQTT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_nwk_status();
    // Change NWK state to allow
    rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_EMB_MQTT_DESTROY_NWK_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_EMB_MQTT_DESTROY_COMMAND_SEN_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Register callbacks for MQTT Asynchronous messages. This is a non-blocking API.
 * @param[in]  callback_id           - Callback id for MQTT responses
 * @param[in]  call_back_handler_ptr - Callback function pointer
 * @param[out] status                - Success - RSI_SUCCESS\n
 *                                     Failure - Possible error codes are : 0x0030, 0x0036, 0x0065, 0xBBF1, 0xBBF2, 0xBBF3, 0xFFF6
 * @param[out] buffer                - Pointer to buffer which hold data \n
 * @param[out] length                - Length of the buffer \n
 * @return     Status of the call_back_handler_ptr is returned
 */

int32_t rsi_emb_mqtt_register_call_back(uint32_t callback_id,
                                        void (*call_back_handler_ptr)(uint16_t status,
                                                                      uint8_t *buffer,
                                                                      const uint32_t length))
{

  SL_PRINTF(SL_EMB_MQTT_REGISTER_CALL_BACK_ENTRY, NETWORK, LOG_INFO);
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return, if the callback number exceeds the RSI_MAX_NUM_CALLBACKS, or
     * the callback is already registered
     */
    SL_PRINTF(SL_EMB_MQTT_REGISTER_CALL_BACK_EXCEEDS_MAX_CALLBACKS, NETWORK, LOG_ERROR);
    return RSI_ERROR_EXCEEDS_MAX_CALLBACKS;
  }

  if (callback_id == RSI_WLAN_NWK_EMB_MQTT_REMOTE_TERMINATE_CB) {
    // Remote terminate of Embedded MQTT socket callback handler
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_remote_terminate_handler = call_back_handler_ptr;
  } else if (callback_id == RSI_WLAN_NWK_EMB_MQTT_PUB_MSG_CB) {
    //  MQTT callback for publish message
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_publish_message_callback = call_back_handler_ptr;
  } else if (callback_id == RSI_WLAN_NWK_EMB_MQTT_KEEPALIVE_TIMEOUT_CB) {
    // MQTT keep alive timeout callback handler
    rsi_wlan_cb_non_rom->nwk_callbacks.rsi_emb_mqtt_keep_alive_timeout_callback = call_back_handler_ptr;
  } else {
    SL_PRINTF(SL_EMB_MQTT_REGISTER_CALL_BACK_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  SL_PRINTF(SL_EMB_MQTT_REGISTER_CALL_BACK_EXIT, NETWORK, LOG_ERROR);
  return RSI_SUCCESS;
}
/*==============================================*/
/**
 * @brief      Adjusted length of MQTT packet. This is a non-blocking API.
 * @param[in]  rem_len 		      -  Length
 * @return     adjusted rem_len -  Success \n
 */
/// @private
int32_t rsi_cal_mqtt_packet_len(int32_t rem_len)
{
  SL_PRINTF(SL_CAL_MQTT_PACKET_LEN_ENTRY, NETWORK, LOG_INFO);
  rem_len += 1; /* Header byte */

  /* Now remaining_length field */
  if (rem_len < 128)
    rem_len += 1;
  else if (rem_len < 16384)
    rem_len += 2;
  else if (rem_len < 2097151)
    rem_len += 3;
  else
    rem_len += 4;
  SL_PRINTF(SL_CAL_MQTT_PACKET_LEN_EXIT, NETWORK, LOG_INFO, "remlen: %4x", rem_len);
  return rem_len;
}
/** @} */
