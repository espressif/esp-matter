/*******************************************************************************
* @file  rsi_mqtt_client.c
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

#include "rsi_mqtt_client.h"

#include "MQTT_wrappers.h"

#include "MQTTClient.h"

#include "rsi_nwk.h"

/** @addtogroup NETWORK13 
* @{
*/
/*==============================================*/
/**
 * @brief      Allocate memory for the MQTT for a single client. Returns MQTT Client instance pointer, which is used for further MQTT client operations. This is non-blocking API.
 * @pre        \ref rsi_config_ipaddress() API needs to be called before this API
 * @param[in]  buffer     		        - Buffer pointer to allocate  memory for MQTT Client information
 * @param[in]  length     		        - Buffer length
 * @param[in]  server_ip  		        - IPv4 address of the MQTT broker
 * @param[in]  server_port		        - MQTT broker port number
 * @param[in]  client_port		        - MQTT client port number
 * @param[in]  flags      		        - Each bit has its own significance \n
 * 
 *  Flags                         |    Description
 *  :-----------------------------|:--------------------------------------
 *  Bit(0) - Server IP version    |     1      - IPv6 \n
 *  ^                             |     0      - IPv4
 *     
 * @param[in]  keep_alive_interval 	 - MQTT client keep alive interval \n
 *             				                 If there are no transactions between MQTT client and broker with in \n 
 *             				                 this time period, MQTT Broker disconnects the MQTT client \n
 *             				                 If 0 -> Server does not disconnect \n
 * @return     Positive Value        - Returns MQTT client information structure pointer \n
 *             NULL                  - In case of failure
 *
 */

rsi_mqtt_client_info_t *rsi_mqtt_client_init(int8_t *buffer,
                                             uint32_t length,
                                             int8_t *server_ip,
                                             uint32_t server_port,
                                             uint32_t client_port,
                                             uint16_t flags,
                                             uint16_t keep_alive_interval)
{
  rsi_mqtt_client_info_t *rsi_mqtt_client = NULL;
  SL_PRINTF(SL_MQTT_CLIENT_INIT_ENTRY, NETWORK, LOG_INFO);
  // If any invalid parameter is given, return NULL
  if (!(buffer && length && server_port && client_port && server_port)) {
    // Return invalid command error
    SL_PRINTF(SL_MQTT_CLIENT_INIT_COMMAND_ERROR, NETWORK, LOG_ERROR);
    return NULL;
  }
  // Given buffer length for MQTT client information is not sufficient
  if (length < MQTT_CLIENT_INFO_SIZE) {
    SL_PRINTF(SL_MQTT_CLIENT_INIT_INSUFFICIENT_BUFFER_LENGTH, NETWORK, LOG_ERROR);
    return NULL;
  }

  rsi_mqtt_client = (rsi_mqtt_client_info_t *)buffer;

  buffer += sizeof(rsi_mqtt_client_info_t);

  rsi_mqtt_client->mqtt_client.ipstack = (Network *)buffer;

  buffer += sizeof(Network);

  rsi_mqtt_client->server_port = server_port;

  rsi_mqtt_client->client_port = client_port;

  rsi_mqtt_client->keep_alive_interval = keep_alive_interval;

  if (flags & RSI_IPV6) {
    memcpy(&rsi_mqtt_client->server_ip.ipv6[0], server_ip, RSI_IPV6_ADDRESS_LENGTH);
  } else {
    // Fill IP address
    memcpy(&rsi_mqtt_client->server_ip.ipv4[0], server_ip, RSI_IPV4_ADDRESS_LENGTH);
  }

  rsi_mqtt_client->mqtt_tx_buffer = buffer;

  buffer += MQTT_CLIENT_TX_BUFFER_SIZE;

  rsi_mqtt_client->mqtt_rx_buffer = buffer;

  buffer += MQTT_CLIENT_RX_BUFFER_SIZE;

  // Initialize creating new network (initialize network callbacks)
  NewNetwork(rsi_mqtt_client->mqtt_client.ipstack);

  // Initialize buffer to the MQTT client
  MQTTClient((Client *)rsi_mqtt_client,
             rsi_mqtt_client->mqtt_client.ipstack,
             MQTT_CONNECT_TIME_OUT,
             (uint8_t *)rsi_mqtt_client->mqtt_tx_buffer,
             MQTT_CLIENT_TX_BUFFER_SIZE,
             (uint8_t *)rsi_mqtt_client->mqtt_rx_buffer,
             MQTT_CLIENT_RX_BUFFER_SIZE);
  SL_PRINTF(SL_MQTT_CLIENT_INIT_EXIT, NETWORK, LOG_ERROR);
  return (rsi_mqtt_client);
}
/** @} */

/** @addtogroup NETWORK13 
* @{
*/
/*==============================================*/
/**
 * @brief       Establish TCP connection with the given MQTT client port and establish MQTT protocol level connection. If Callback is provided it is non-blocking API otherwise it is blocking API.
 * @param[in]   rsi_mqtt_client - MQTT client information pointer which was returned in rsi_mqtt_client_init() API
 * @param[in]   flags           - Network flags,Each bit has its own significance \n
 * 
 *  Flags                           |    Description
 *  :-------------------------------|:-----------------------------------
 *  BIT(0) - IP version             |     1      - IPv6 \n
 *  ^                               |     0      - IPv4
 *  BIT(1) - SSL flag               |     1      - SSL Enable \n
 *  ^                               |     0      - SSL Disable
 * 
 * @param[in]   client_id 	   - clientID of the MQTT Client and should be unique for each device
 * @param[in]   username 	     - Username for the login credentials of MQTT server
 * @param[in]   password 	     - password for the login credentials of MQTT server
 * @note Need to register callback if ASYNC_MQTT is enable
 * For Asynchronous \n
 * @param[in]	  callback 	     - Callback handler for asynchronous response. \n
 * @param[out]	sock_no 	     - Socket descriptor number \n
 * @param[out]	buffer  	     - Buffer \n
 * @param[out] 	length  	     - length of buffer \n
 *   
 * @return      Zero           - Success \n
 *              Negative value - Failure 
 * @note        Procedure for connecting MQTT over SSL : \n Enable TCP_IP_FEAT_SSL in Opermode parameters as below  \n
 *                                                       #define RSI_TCP_IP_FEATURE_BIT_MAP (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_SSL | TCP_IP_FEAT_DNS_CLIENT)  \n
 *                                                       Load the related SSL Certificates in the module using rsi_wlan_set_certificate() API.           
 *
 */
#ifdef ASYNC_MQTT
int32_t rsi_mqtt_connect(rsi_mqtt_client_info_t *rsi_mqtt_client,
                         uint16_t flags,
                         int8_t *client_id,
                         int8_t *username,
                         int8_t *password,
                         void (*callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length))
#else
/** @cond */

int32_t rsi_mqtt_connect(rsi_mqtt_client_info_t *rsi_mqtt_client,
                         uint16_t flags,
                         int8_t *client_id,
                         int8_t *username,
                         int8_t *password)
/** @endcond */

#endif
{
  SL_PRINTF(SL_MQTT_CLIENT_CONNECT_ENTRY, NETWORK, LOG_INFO);
  int32_t status = 0;

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

  if ((rsi_mqtt_client == NULL) || (client_id == NULL)) {
    // Return invalid parmater error
    SL_PRINTF(SL_MQTT_CLIENT_CONNECT_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  // Connect to the new network
#ifdef ASYNC_MQTT
  status = ConnectNetwork((Network *)&rsi_mqtt_client->mqtt_client,
                          flags,
                          (char *)&(rsi_mqtt_client->server_ip),
                          rsi_mqtt_client->server_port,
                          rsi_mqtt_client->client_port,
                          callback);
#else
  status = ConnectNetwork((Network *)&rsi_mqtt_client->mqtt_client,
                          flags,
                          (char *)&(rsi_mqtt_client->server_ip),
                          rsi_mqtt_client->server_port,
                          rsi_mqtt_client->client_port);
#endif
  if (status != RSI_SUCCESS) {
    SL_PRINTF(SL_MQTT_CLIENT_CONNECT_EXIT_1, NETWORK, LOG_INFO, "status: %4x", status);
    return status;
  }

  data.willFlag = 0;
  // MQTT Version
  data.MQTTVersion = MQTT_VERSION;
  // Assign client ID
  data.clientID.cstring = (char *)client_id;

  // Fill username and password
  if (username != NULL)
    data.username.cstring = (char *)username;

  if (password != NULL)
    data.password.cstring = (char *)password;
  // Keep Alive interval
  data.keepAliveInterval = rsi_mqtt_client->keep_alive_interval;
  // New connection
  data.cleansession = 1;

  // Connect to  the MQTT broker
  status = MQTTConnect(&rsi_mqtt_client->mqtt_client, &data);

  // Shut Down the port
  if (status)
    mqtt_disconnect(rsi_mqtt_client->mqtt_client.ipstack);
  SL_PRINTF(SL_MQTT_CLIENT_CONNECT_EXIT_2, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup NETWORK13 
* @{
*/
/*==============================================*/
/**
 * @brief      Disconnect the client from MQTT broker. This is a blocking API.
 * @pre        \ref rsi_mqtt_connect() API needs to be called before this API 
 * @param[in]  rsi_mqtt_client - MQTT client information structure pointer that was returned in rsi_mqtt_client_init() API
 * @return      Zero           - Success \n
 *              Negative value - Failure
 *              
 *
 *
 */
int32_t rsi_mqtt_disconnect(rsi_mqtt_client_info_t *rsi_mqtt_client)
{
  SL_PRINTF(SL_MQTT_CLIENT_DISCONNECT_ENTRY, NETWORK, LOG_INFO);
  int32_t status = 0;
  // If MQTT info structure is NULL ,throw error
  if (rsi_mqtt_client == NULL) {
    // Return invalid command error
    SL_PRINTF(SL_MQTT_CLIENT_DISCONNECT_INVALID_PARAM, NETWORK, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }
  // Call MQTT disconnect
  status = MQTTDisconnect(&rsi_mqtt_client->mqtt_client);
  // Shut Down the port
  mqtt_disconnect(rsi_mqtt_client->mqtt_client.ipstack);
  SL_PRINTF(SL_MQTT_CLIENT_DISCONNECT_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Publish the given message on the given topic. This is a blocking API.
 * @pre        \ref rsi_mqtt_connect() API needs to be called before this API
 * @param[in]  rsi_mqtt_client  - MQTT client info structure that was returned in rsi_mqtt_client_init() API
 * @param[in]  topic		        - String of topic
 * @param[in]  publish_msg 	    - Message to publish
 * @return     Zero             - Success \n
 *             Negative value 	- Failure
 *             
 * 
 *
 */
int32_t rsi_mqtt_publish(rsi_mqtt_client_info_t *rsi_mqtt_client, int8_t *topic, MQTTMessage *publish_msg)
{
  int32_t status = 0;
  SL_PRINTF(SL_MQTT_PUBLISH_ENTRY, NETWORK, LOG_INFO);
  // If any invalid parameter is received
  if ((rsi_mqtt_client == NULL) || (topic == NULL) || (publish_msg == NULL)) {
    // Return invalid command parameter error
    SL_PRINTF(SL_MQTT_PUBLISH_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  // Publish the message
  status = MQTTPublish(&rsi_mqtt_client->mqtt_client, (const char *)topic, (MQTTMessage *)publish_msg);

  // Return status
  SL_PRINTF(SL_MQTT_PUBLISH_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Subscribe on the specified topic given.If any other client posts any message on the same topic, that message is received if MQTT client is subscribed to that topic.
 *             This is a non-blocking API.
 * @pre        \ref rsi_mqtt_connect() API needs to be called before this API
 * @param[in]  rsi_mqtt_client       - MQTT client structure info pointer that was returned in \ref rsi_mqtt_client_init() API
 * @param[in]  qos                   - Quality of service of the message
 * @param[in]  topic                 - Topic string
 * @param[in]  call_back_handler_ptr - Callback pointer to call when a message is received from MQTT broker
 * @return      Zero                 - Success \n
 *              Negative value       - Failure
 */
int32_t rsi_mqtt_subscribe(rsi_mqtt_client_info_t *rsi_mqtt_client,
                           uint8_t qos,
                           int8_t *topic,
                           void (*call_back_handler_ptr)(MessageData *md))
{
  int32_t status = 0;
  SL_PRINTF(SL_MQTT_SUBSCRIBE_ENTRY, NETWORK, LOG_INFO);
  // If any invalid parameter is received
#ifdef ASYNC_MQTT
  if ((rsi_mqtt_client == NULL) || (topic == NULL))
#else
  if ((rsi_mqtt_client == NULL) || (topic == NULL) || (call_back_handler_ptr == NULL))
#endif
  {
    // Return invalid parameter error
    SL_PRINTF(SL_MQTT_SUBSCRIBE_INVALID_PARAM_1, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  if (qos > 2) {
    // Return invalid parameter error
    SL_PRINTF(SL_MQTT_SUBSCRIBE_INVALID_PARAM_2, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  status = MQTTSubscribe(&rsi_mqtt_client->mqtt_client, (const char *)topic, (enum QoS)qos, call_back_handler_ptr);
  SL_PRINTF(SL_MQTT_SUBSCRIBE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Unsubscribe on the specified topic given. If unsubscribed, any messages on the topic is not received further
 *             This is a non-blocking API.
 * @pre        \ref rsi_mqtt_connect() API needs to be called before this API
 * @param[in]  rsi_mqtt_client - MQTT client instance that was returned in \ref rsi_mqtt_client_init() API
 * @param[in]  topic           - Topic string
 * @return     Zero            - Success \n
 *             Negative value  - Failure
 */
int32_t rsi_mqtt_unsubscribe(rsi_mqtt_client_info_t *rsi_mqtt_client, int8_t *topic)
{
  SL_PRINTF(SL_MQTT_UNSUBSCRIBE_ENTRY, NETWORK, LOG_INFO);
  int32_t status;
  if ((rsi_mqtt_client == NULL) || (topic == NULL)) {
    // Return invalid command error
    SL_PRINTF(SL_MQTT_SUBSCRIBE_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  // Unsubscribe to the topic
  status = MQTTUnsubscribe(&rsi_mqtt_client->mqtt_client, (const char *)topic);
  SL_PRINTF(SL_MQTT_UNSUBSCRIBE_EXIT, NETWORK, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Wait for the MQTT messages to receive on the specific MQTT client. This is a non-blocking API.
 * @pre        \ref rsi_mqtt_connect() API needs to be called before this API
 * @param[in]  rsi_mqtt_client - MQTT client instance that was returned in \ref rsi_mqtt_client_init() API
 * @param[in]  time_out        - Time out in milliseconds.
 * @return     Zero            - Success \n
 *             Negative value  - Failure
 */
int32_t rsi_mqtt_poll_for_recv_data(rsi_mqtt_client_info_t *rsi_mqtt_client, int time_out)
{
  if (rsi_mqtt_client == NULL) {
    // Return invalid command error
    SL_PRINTF(SL_MQTT_POLL_FOR_RECV_DATA_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  SL_PRINTF(SL_MQTT_POLL_FOR_RECV_DATA_EXIT, NETWORK, LOG_INFO);
  return MQTTYield(&rsi_mqtt_client->mqtt_client, time_out);
}
/** @} */
