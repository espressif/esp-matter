/*******************************************************************************
* @file  rsi_sntp_client.h
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

#ifndef RSI_SNTP_CLIENT_H
#define RSI_SNTP_CLIENT_H
/******************************************************
 * *                      Macros
 * ******************************************************/
#ifndef RSI_IPV6
#define RSI_IPV6 1
#endif

//SNTP Method to USe
#define RSI_SNTP_BROADCAST_MODE 1

//SNTP Method to USe
#define RSI_SNTP_UNICAST_MODE 2

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
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
int32_t rsi_sntp_client_create_async(uint8_t flags,
                                     uint8_t *server_ip,
                                     uint8_t sntp_method,
                                     uint16_t sntp_timeout,
                                     void (*rsi_sntp_client_create_response_handler)(uint16_t status,
                                                                                     const uint8_t cmd_type,
                                                                                     const uint8_t *buffer));
int32_t rsi_sntp_client_gettime(uint16_t length, uint8_t *sntp_time_rsp);
int32_t rsi_sntp_client_gettime_date(uint16_t length, uint8_t *sntp_time_date_rsp);
int32_t rsi_sntp_client_server_info(uint16_t length, uint8_t *sntp_server_response);
int32_t rsi_sntp_client_delete_async(void);
#endif
